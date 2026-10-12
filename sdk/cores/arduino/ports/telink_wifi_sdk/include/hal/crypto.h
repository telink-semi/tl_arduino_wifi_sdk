/*******************************************************************************************************
 *
 * @file    crypto.h
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
 
#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdint.h>
#include <hal/types.h>
#include <hal/device.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * High-level device driver
 */

#define IOCTL_TRNG_READ				(1)

struct trng_read_value
{
	int len;
	uint8_t *val;
};

/*
 * Low-level device driver
 */

struct pke_ops {
    int (*pke_eccp_point_mul)(struct device *dev, uint32_t *k, uint32_t *px, uint32_t *py,
        uint32_t *qx, uint32_t *qy);
    int (*pke_eccp_point_add)(struct device *dev, uint32_t *p1x, uint32_t *p1y,
        uint32_t *p2x, uint32_t *p2y, uint32_t *qx, uint32_t *qy);
    int (*pke_eccp_point_verify)(struct device *dev, uint32_t *px, uint32_t *py);
};

struct trng_ops {
    int (*get_rand_fast)(struct device *dev, uint8_t *rand, uint32_t bytes);
};

#define crypto_pke_get_ops(dev)		((struct pke_ops *)(dev)->driver->ops)
#define crypto_trng_get_ops(dev)	((struct trng_ops *)(dev)->driver->ops)

static __inline__ int crypto_eccp_point_mul(struct device *dev, uint32_t *k, uint32_t *px, uint32_t *py,
        uint32_t *qx, uint32_t *qy)
{
	if (!dev)
		return -ENODEV;
	if (!crypto_pke_get_ops(dev)->pke_eccp_point_mul)
		return -ENOSYS;

	return crypto_pke_get_ops(dev)->pke_eccp_point_mul(dev, k, px, py, qx, qy);
}

static __inline__ int crypto_eccp_point_add(struct device *dev, uint32_t *p1x, uint32_t *p1y,
        uint32_t *p2x, uint32_t *p2y, uint32_t *qx, uint32_t *qy)
{
	if (!dev)
		return -ENODEV;
	if (!crypto_pke_get_ops(dev)->pke_eccp_point_add)
		return -ENOSYS;

	return crypto_pke_get_ops(dev)->pke_eccp_point_add(dev, p1x, p1y, p2x, p2y, qx, qy);
}

static __inline__ int crypto_eccp_point_verify(struct device *dev, uint32_t *px, uint32_t *py)
{
	if (!dev)
		return -ENODEV;
	if (!crypto_pke_get_ops(dev)->pke_eccp_point_verify)
		return -ENOSYS;

	return crypto_pke_get_ops(dev)->pke_eccp_point_verify(dev, px, py);
}

static __inline__ int crypto_trng_get_rand(struct device *dev, uint8_t *rand, uint32_t bytes)
{
	if (!dev)
		return -ENODEV;
	if (!crypto_trng_get_ops(dev)->get_rand_fast)
		return -ENOSYS;

	return crypto_trng_get_ops(dev)->get_rand_fast(dev, rand, bytes);
}

#ifdef __cplusplus
}
#endif

#endif //_CRYPTO_H_
