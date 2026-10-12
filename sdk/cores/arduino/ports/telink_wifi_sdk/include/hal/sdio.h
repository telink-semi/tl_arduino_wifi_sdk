/*******************************************************************************************************
 *
 * @file    sdio.h
 *
 * @brief   This is the source file for TLSR9118
 *
 * @author  
 * @par     Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * @author  
 * @par     Copyright 2023-2025 Senscomm Semiconductor Co., Ltd.
 * 
 * @author  Telink WIFI GROUP
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 * 
 *******************************************************************************************************/
 
#ifndef _SDIO_H_
#define _SDIO_H_

#include <hal/kernel.h>
#include <hal/device.h>
#include <hal/device.h>
#include <hal/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDIO_BLOCK_SIZE 512

struct sdio_req {
	struct list_head entry;
	u8 fn; /* sdio function number */
	u32 len;
	void *priv;
	/*
	 * actually we will link the mbuf->data with this buf,
	 * rather than a continuous fixed memory like SDIO BOOT does
	 */
	u8 *buf;
};

typedef void (*filled_cb)(struct sdio_req *req);
typedef int (*getbuf_cb)(struct sdio_req *req);
typedef void (*resetrxbuf_cb)();

struct sdio_ops {
	int (*start)(struct device *dev, u8 fn_rx, u16 depth_rx, u16 min_linear, u16 mbufext_num);
	int (*stop)(struct device *dev);
	int (*tx)(struct device *dev, u8 fn, u8 *buf, u32 len);
	int (*register_cb)(struct device *dev, filled_cb filled,
			getbuf_cb getbuf, resetrxbuf_cb resetrxbuf);
	void (*release_rx)(struct device *dev, struct sdio_req *req);
	void (*write_flowctrl_info)(struct device *dev, u16 rd_idx, u16 mbuf_num);
#ifdef CONFIG_SDIO_TXCHAIN
	void (*acquire_tx)(struct device *dev);
	void (*release_tx)(struct device *dev);
	void (*txchain_addelem)(struct device *dev, u8 *buf, u16 len, u8 idx);
	void (*txchain_kick)(struct device *dev, u8 fn, u8 cnt);
#endif
#ifdef CONFIG_SDIO_RECOVERY
	u8 (*recover_info_proc)(struct device *dev, bool write, u8 val);
	int (*buffered_rx)(struct device *dev);
#endif
	void (*reenum_host)(void);
};

#define sdio_get_ops(dev) ((struct sdio_ops *)(dev)->driver->ops)

static __inline__ int sdio_start(struct device *dev, u8 fn_rx, u16 depth_rx, u16 min_linear, u16 mbufext_num)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->start) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->start(dev, fn_rx, depth_rx, min_linear, mbufext_num);
}

static __inline__ int sdio_stop(struct device *dev)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->stop) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->stop(dev);
}

static __inline__ int sdio_tx(struct device *dev, u8 fn, u8 *buf, u32 len)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->tx) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->tx(dev, fn, buf, len);
}

#ifdef CONFIG_SDIO_TXCHAIN
static __inline__ int sdio_acquire_tx(struct device *dev)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->acquire_tx) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->acquire_tx(dev);
	return 0;
}

static __inline__ int sdio_release_tx(struct device *dev)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->release_tx) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->release_tx(dev);
	return 0;
}

static __inline__ int sdio_txchain_addelem(struct device *dev, u8 *buf, u16 len, u8 idx)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->txchain_addelem) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->txchain_addelem(dev, buf, len, idx);
	return 0;
}

static __inline__ int sdio_txchain_kick(struct device *dev, u8 fn, u8 cnt)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->txchain_kick) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->txchain_kick(dev, fn, cnt);
	return 0;
}
#endif

static __inline__ int sdio_release_rx_req(struct device *dev, struct sdio_req *req)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->release_rx) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->release_rx(dev, req);
	return 0;
}

static __inline__ int sdio_register_cb(struct device *dev,
		filled_cb filled, getbuf_cb getbuf, resetrxbuf_cb resetrxbuf)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->register_cb) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->register_cb(dev, filled, getbuf, resetrxbuf);
}

static __inline__ int sdio_write_flowctrl_info(struct device *dev, u16 rd_idx, u16 mbuf_num)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->write_flowctrl_info) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->write_flowctrl_info(dev, rd_idx, mbuf_num);
	return 0;
}

#ifdef CONFIG_SDIO_RECOVERY
static __inline__ int sdio_recover_info_proc(struct device *dev, bool write, u8 val)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->recover_info_proc) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->recover_info_proc(dev, write, val);
}

static __inline__ int sdio_buffered_rx(struct device *dev)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->buffered_rx) {
		return -ENOSYS;
	}

	return sdio_get_ops(dev)->buffered_rx(dev);
}
#endif

static __inline__ int sdio_reenum_host(struct device *dev)
{
	if (!dev) {
		return -ENODEV;
	}

	if (!sdio_get_ops(dev)->reenum_host) {
		return -ENOSYS;
	}

	sdio_get_ops(dev)->reenum_host();
	return 0;
}

/* SDIO Debugging log */
//#define CONFIG_SDIO_DBG_LOG

#ifdef CONFIG_SDIO_DBG_LOG
#define sdio_dbg_log printk
#else
#define sdio_dbg_log(args...)
#endif
#define sdio_err_log printk

#ifdef __cplusplus
}
#endif

#endif //_SDIO_H_
