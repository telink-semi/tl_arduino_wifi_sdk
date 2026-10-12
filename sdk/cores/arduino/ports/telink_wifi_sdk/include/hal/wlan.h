/*******************************************************************************************************
 *
 * @file    wlan.h
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
 
#ifndef _WLAN_H_
#define _WLAN_H_

#include <hal/types.h>
#include <hal/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WLAN_DEV_F_DUAL_VIF_EN	0x1

#define IF_SUPPORT_DUAL_VIF(_f) ((_f) & WLAN_DEV_F_DUAL_VIF_EN)

struct ieee80211vap;

/*
 * struct wlan_ops - Driver model wlan operations
 *
 * The driver interface is implemented by all wlan devices which use
 * driver model.
 */
struct wlan_ops {
	/*
	 * Start the driver
	 * @dev: wlan device
	 * @return: 0 if OK, -ve on error
	 */
	int (*start)(struct device *dev);

	/*
	 * Resume the driver
	 *
	 * @dev: wlan device
 	 * @flags: Driver specific flags.
	 * @return: 0 if OK, -ve on error
	 */
	int (*resume)(struct device *dev, uint32_t flags);

	/*
	 * Suspend the driver
	 *
	 * @dev: wlan device
 	 * @flags: Driver specific flags.
	 * @return: 0 if OK, -ve on error
	 */
	int (*suspend)(struct device *dev, uint32_t flags);

	/*
	 * Reset the driver, typically restoring statistical counters
	 * @dev: wlan device
	 * @return: 0 if OK, -ve on error
	 */
	int (*reset)(struct device *dev);

	/*
	 * Shutdown the driver
	 * @dev: wlan device
	 * @return: 0 if OK, -ve on error
	 */
	int (*stop)(struct device *dev);

	/*
	 * Get the maximum number of vaps
	 * @dev: wlan device
	 * @return: the maximum number of vaps that can be created
	 */
	int (*num_max_vaps)(struct device *dev);

	/*
	 * Create a vap instance
	 * @dev: wlan device
	 * @idx: vap index
	 * @return: pointer to vap if OK, NULL on error
	 */
	struct ieee80211vap *(*create_vap)(struct device *dev, int idx);

	/*
	 * Get vap instance
	 * @dev: wlan device
	 * @idx: vap index
	 * @return: pointer to vap if OK, NULL on error
	 */
	struct ieee80211vap *(*get_vap)(struct device *dev, int idx);

	/*
	 * Destroy a vap instance
	 * @dev: wlan device
	 * @vap: pointer to the vap to remove
	 * @return: 0 if OK, -ve on error
	 */
	int (*remove_vap)(struct device *dev, struct ieee80211vap *);

	/*
	 * Control a vap instance
	 * @dev: wlan device
	 * @vap: pointer to the vap to configure
	 * @return: 0 if OK, -ve on error
	 */
	int (*ctl_vap)(struct device *dev, struct ieee80211vap *,
			u_long cmd, caddr_t data);

};

#define wlan_ops(x)	((struct wlan_ops *)(x)->driver->ops)

static __inline__ int wlan_start(struct device *dev)
{
	if (!dev)
		return -ENODEV;
	if (!wlan_ops(dev)->start)
		return -ENOSYS;

	return wlan_ops(dev)->start(dev);

}

static __inline__ int wlan_resume(struct device *dev, u32 flags)
{
	if (!dev)
		return -ENODEV;
	if (!wlan_ops(dev)->resume)
		return -ENOSYS;

	return wlan_ops(dev)->resume(dev, flags);
}

static __inline__ int wlan_suspend(struct device *dev, u32 flags)
{
	if (!dev)
		return -ENODEV;
	if (!wlan_ops(dev)->suspend)
		return -ENOSYS;

	return wlan_ops(dev)->suspend(dev, flags);
}

static __inline__ int wlan_reset(struct device *dev)
{
	if (!dev)
		return -ENODEV;
	if (!wlan_ops(dev)->reset)
		return -ENOSYS;

	return wlan_ops(dev)->reset(dev);
}

static __inline__ int wlan_stop(struct device *dev)
{
	if (!dev)
		return -ENODEV;
	if (!wlan_ops(dev)->stop)
		return -ENOSYS;

	return wlan_ops(dev)->stop(dev);
}

static __inline__ int wlan_num_max_vaps(struct device *dev)
{
	if (!dev)
		return 0;
	if (!wlan_ops(dev)->num_max_vaps)
		return 0;

	return wlan_ops(dev)->num_max_vaps(dev);
}

static __inline__ struct ieee80211vap *wlan_create_vap(struct device *dev, int idx)
{
	if (!dev)
		return NULL;
	if (!wlan_ops(dev)->create_vap)
		return NULL;

	return wlan_ops(dev)->create_vap(dev, idx);
}

__ilm__ static __inline__ struct ieee80211vap *wlan_get_vap(struct device *dev, int idx)
{
	if (!dev)
		return NULL;
	if (!wlan_ops(dev)->get_vap)
		return NULL;

	return wlan_ops(dev)->get_vap(dev, idx);
}

static __inline__ int wlan_remove_vap(struct device *dev, struct ieee80211vap *vap)
{
	if (!dev)
		return -ENODEV;

	if (!wlan_ops(dev)->remove_vap)
		return -EINVAL;

	return wlan_ops(dev)->remove_vap(dev, vap);
}

static __inline__ int wlan_ctl_vap(struct device *dev, struct ieee80211vap *vap,
		u_long cmd, caddr_t data)
{
	if (!dev)
		return -ENODEV;

	if (!wlan_ops(dev)->ctl_vap)
		return -EINVAL;

	return wlan_ops(dev)->ctl_vap(dev, vap, cmd, data);
}

/* 0 ~ (n-1): okay, -1: invalid vap */
__ilm__ static inline int get_vap_idx(struct device *dev,
		struct ieee80211vap *vap)
{
  struct ieee80211vap *cmp;
  int idx = 0;

  while ((cmp = wlan_get_vap(dev, idx)) != NULL && cmp != vap)
    idx++;

  return cmp != NULL ? idx : -1;
}

extern struct device *(*wlandev)(int inst);

#ifdef __cplusplus
}
#endif

#endif  /* _WLAN_H_ */
