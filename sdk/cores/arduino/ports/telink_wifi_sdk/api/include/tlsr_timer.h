/*******************************************************************************************************
 *
 * @file    tlsr_timer.h
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
 
#ifndef __TLSR_TIMER_H__
#define __TLSR_TIMER_H__

#include <stdint.h>

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TIMER device index
 */
enum tlsr_timer_idx {
	TLSR_TIMER_IDX_0,
	TLSR_TIMER_IDX_1,
	TLSR_TIMER_IDX_MAX,
};

/**
 * TIMER channels
 */
enum tlsr_timer_ch {
	TLSR_TIMER_CH_0,
	TLSR_TIMER_CH_1,
	TLSR_TIMER_CH_2,
	TLSR_TIMER_CH_3,
	TLSR_TIMER_CH_MAX,
};

/**
 * TIMER modes
 */
enum tlsr_timer_mode {
	TLSR_TIMER_MODE_PERIODIC,
	TLSR_TIMER_MODE_ONESHOT,
	TLSR_TIMER_MODE_FREERUN,
	TLSR_TIMER_MODE_PWM,
};

/**
 * TIMER parameter for TLSR_TIMER_MODE_PERIODIC
 */
struct tlsr_timer_cfg_periodic {
	uint32_t duration; /* usec, up to ~107 sec (UINT32_MAX / 40MHz) */
};

/**
 * TIMER parameter for TLSR_TIMER_MODE_ONESHOT
 */
struct tlsr_timer_cfg_oneshot {
	uint32_t duration; /* usec, up to ~107 sec (UINT16_MAX / 40MHz) */
};

/**
 * TIMER parameter for TLSR_TIMER_MODE_FREERUN
 */
struct tlsr_timer_cfg_freerun {
	uint32_t freq; /* freq, 1~1MHz */
};

/**
 * TIMER parameter for TLSR_TIMER_MODE_PWM
 */
struct tlsr_timer_cfg_pwm {
    /* XXX: duty of 0 or 100 can't be achieved by configuring high, low.
     *      Use park instead.
     */
	uint16_t high; /* usec, valid range: [1 - 1638] usec (1638 = UINT16_MAX / 40MHz) */
	uint16_t low;  /* usec, valid range: [1 - 1638] usec (1638 = UINT16_MAX / 40MHz) */
	uint8_t park;  /* level of PWM signal when the timer is stopped */
};

/**
 * TIMER configuration
 */
struct tlsr_timer_cfg {
	enum tlsr_timer_mode mode;
	uint8_t intr_en;
	union {
		struct tlsr_timer_cfg_periodic periodic;
		struct tlsr_timer_cfg_oneshot oneshot;
		struct tlsr_timer_cfg_freerun freerun;
		struct tlsr_timer_cfg_pwm pwm;
	} data;
};

/**
 * TIMER event
 */
enum tlsr_timer_event_type {
	TLSR_TIMER_EVENT_EXPIRE,
};

/**
 * TIMER notification callback
 */
typedef int (*tlsr_timer_notify)(enum tlsr_timer_event_type type, void *ctx);

/**
 * @brief Configure TIMER
 *
 * @param[in] idx index of the TIMER
 * @param[in] ch channel number
 * @param[in] cfg mode of the timer
 * @param[in] notify notification callback
 * @param[in] ctx user context associated with the notification event
 */
int tlsr_timer_configure(enum tlsr_timer_idx idx, enum tlsr_timer_ch ch,
						struct tlsr_timer_cfg *cfg, tlsr_timer_notify notify, void *ctx);

/**
 * @brief Start TIMER
 *
 * @param[in] idx index of the TIMER
 * @param[in] ch channel number
 */
int tlsr_timer_start(enum tlsr_timer_idx idx, enum tlsr_timer_ch ch);

/**
 * @brief Stop TIMER
 *
 * @param[in] idx index of the TIMER
 * @param[in] ch channel number
 */
int tlsr_timer_stop(enum tlsr_timer_idx idx, enum tlsr_timer_ch ch);

/**
 * @brief Start multiple TIMER
 *
 * @param[in] idx index of the TIMER
 * @param[in] chs channel numbers ORed in bit position
 */
int tlsr_timer_start_multi(enum tlsr_timer_idx idx, uint8_t chs);

/**
 * @brief Stop multiple TIMER
 *
 * @param[in] idx index of the TIMER
 * @param[in] chs channel numbers ORed in bit position
 */
int tlsr_timer_stop_multi(enum tlsr_timer_idx idx, uint8_t chs);

/**
 * @brief Get TIMER value
 *
 * @param[in] idx index of the TIMER
 * @param[in] ch channel number
 * @param[in] value pointer to the value to receive
 *
 * If the timer was configured to be freeruning, the function returns the current counter.
 * It is software accumulated value from the start of the timer.
 *
 * If the timer was configured to be periodic or onetime, the function returns the remaining
 * time until the expiration. When the timer stops, the counter is reset to the initial reload value.
 * The hardware is down counter start from the reload value.
 */
int tlsr_timer_value(enum tlsr_timer_idx idx, enum tlsr_timer_ch ch, uint32_t *value);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_TIMER_H__
