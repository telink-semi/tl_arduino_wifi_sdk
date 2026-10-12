/*******************************************************************************************************
 *
 * @file    adc.h
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
 
#ifndef __ADC_H__
#define __ADC_H__

#include <hal/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * High-level device driver
 */

#define IOCTL_ADC_READ		0
#define IOCTL_ADC_SET_CB	1
#define IOCTL_ADC_RESET		2

struct adc_read_arg {
	uint8_t ch;
	uint16_t *data;
	uint32_t len;
};

struct adc_set_cb_arg {
	void (*adc_cb)(void *);
	void *ctx;
};

/*
 * Low-level device driver
 */

enum adc_channel {
	ADC_SINGLE_CH_0		= 0,
	ADC_SINGLE_CH_1		= 1,
	ADC_SINGLE_CH_2		= 2,
	ADC_SINGLE_CH_3		= 3,
	ADC_SINGLE_CH_4		= 4,
	ADC_SINGLE_CH_5		= 5,
	ADC_SINGLE_CH_6		= 6,
	ADC_SINGLE_CH_7		= 7,
	ADC_DIFFER_CH_0_1	= 8,
	ADC_DIFFER_CH_2_3	= 9,
	ADC_DIFFER_CH_4_5	= 10,
	ADC_DIFFER_CH_6_7	= 11,
};

typedef void (*adc_cb)(void *ctx);

struct adc_ops {
	int (*get_value)(struct device *dev, enum adc_channel ch, uint16_t *data, uint32_t len);
	int (*get_value_poll)(struct device *dev, enum adc_channel ch, uint16_t *data, uint32_t len);
	int (*reset)(struct device *dev);
	int (*set_cb)(struct device *dev, adc_cb cb, void *ctx);
};

#define adc_ops(x)		((struct adc_ops *)(x)->driver->ops)

static __inline__ int adc_get_value(struct device *dev, enum adc_channel ch, uint16_t *data, uint32_t len)
{
	if (!dev)
		return -ENODEV;

	return adc_ops(dev)->get_value(dev, ch, data, len);
}

static __inline__ int adc_get_value_poll(struct device *dev, enum adc_channel ch, uint16_t *data, uint32_t len)
{
	if (!dev)
		return -ENODEV;

	return adc_ops(dev)->get_value_poll(dev, ch, data, len);
}

static __inline__ int adc_reset(struct device *dev)
{
	if (!dev)
		return -ENODEV;

	return adc_ops(dev)->reset(dev);
}

static __inline__ int adc_set_cb(struct device *dev, adc_cb cb, void *ctx)
{
	if (!dev)
		return -ENODEV;

	return adc_ops(dev)->set_cb(dev, cb, ctx);
}

#ifdef __cplusplus
}
#endif

#endif //__ADC_H__
