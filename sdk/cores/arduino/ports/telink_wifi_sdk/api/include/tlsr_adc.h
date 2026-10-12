/*******************************************************************************************************
 *
 * @file    tlsr_adc.h
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
 
#ifndef __TLSR_ADC_H__
#define __TLSR_ADC_H__

#include <stdint.h>

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

enum tlsr_adc_channel {
	TLSR_ADC_SINGLE_CH_0		= 0,
	TLSR_ADC_SINGLE_CH_1		= 1,
	TLSR_ADC_SINGLE_CH_2		= 2,
	TLSR_ADC_SINGLE_CH_3		= 3,
	TLSR_ADC_SINGLE_CH_4		= 4,
	TLSR_ADC_SINGLE_CH_5		= 5,
	TLSR_ADC_SINGLE_CH_6		= 6,
	TLSR_ADC_SINGLE_CH_7		= 7,
	TLSR_ADC_DIFFER_CH_0_1	= 8,
	TLSR_ADC_DIFFER_CH_2_3	= 9,
	TLSR_ADC_DIFFER_CH_4_5	= 10,
	TLSR_ADC_DIFFER_CH_6_7	= 11,
};

typedef void (*tlsr_adc_cb)(void *ctx);

/**
 * @brief ADC read data
 *
 * @param[in] ch  ADC channel
 * @param[in] buf buffer for ADC value
 * @param[in] len length of the buffer
 */
int tlsr_adc_read(enum tlsr_adc_channel ch, uint16_t *buf, uint32_t len);

/**
 * @brief ADC read data
 *
 * @param[in] ch  ADC channel
 * @param[in] buf buffer for ADC value
 * @param[in] len length of the buffer
 */
int tlsr_adc_read_async(enum tlsr_adc_channel ch, uint16_t *buf, uint32_t len, tlsr_adc_cb cb, void *ctx);

/**
 * @brief reset ADC peripheral
 *
 */
int tlsr_adc_reset(void);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_ADC_H__
