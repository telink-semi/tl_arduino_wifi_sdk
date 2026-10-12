/*******************************************************************************************************
 *
 * @file    efuse.h
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
 
#ifndef __EFUSE_H__
#define __EFUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * High-level device driver
 */

#define IOCTL_EFUSE_READ_ROW		(1)
#define IOCTL_EFUSE_WRITE_ROW		(2)

#define IOCTL_EFUSE_SET_MODE		(3)
#define IOCTL_EFUSE_GET_MODE		(4)
#define IOCTL_EFUSE_SYNC			(5)
#define IOCTL_EFUSE_LOAD			(6)

enum efuse_mode {
	EFUSE_MODE_RAW 			= 0,
	EFUSE_MODE_RAM_BUFFER	= 1,
	EFUSE_MODE_FLASH_BUFFER	= 2,
};

struct efuse_rw_data
{
	int row;
	u32 *val;
};

/*
 * Low-level device driver
 */

struct efuse_ops {
    int (*read_row)(struct device *dev, int row, u32 *val);
    int (*write_row)(struct device *dev, int row, u32 val);
#ifdef CONFIG_EFUSE_BUFFER_MODE
	int (*set_mode)(struct device *dev, u8 mode);
	int (*get_mode)(struct device *dev, u8 *mode);
	int (*sync)(struct device *dev);
	int (*load)(struct device *dev);
#endif
};

/* Access the operations for an eFuse device */
#define efuse_get_ops(dev)	((struct efuse_ops *)(dev)->driver->ops)

static __inline__ u32 efuse_read(struct device *dev,
        int row, u32 *val)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->read_row)
		return -ENOSYS;

	return efuse_get_ops(dev)->read_row(dev, row, val);

}

static __inline__ int efuse_write(struct device *dev,
		int row, u32 val)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->write_row)
		return -ENOSYS;

	return efuse_get_ops(dev)->write_row(dev, row, val);
}

#ifdef CONFIG_EFUSE_BUFFER_MODE
static __inline__ int efuse_set_mode(struct device *dev, u8 mode)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->set_mode)
		return -ENOSYS;

	return efuse_get_ops(dev)->set_mode(dev, mode);
}

static __inline__ int efuse_get_mode(struct device *dev, u8 *mode)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->get_mode)
		return -ENOSYS;

	return efuse_get_ops(dev)->get_mode(dev, mode);
}

static __inline__ int efuse_sync(struct device *dev, u8 mode)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->sync)
		return -ENOSYS;

	return efuse_get_ops(dev)->sync(dev);
}

static __inline__ int efuse_load(struct device *dev)
{
	if (!dev)
		return -ENODEV;

	if (!efuse_get_ops(dev)->load)
		return -ENOSYS;

	return efuse_get_ops(dev)->load(dev);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
