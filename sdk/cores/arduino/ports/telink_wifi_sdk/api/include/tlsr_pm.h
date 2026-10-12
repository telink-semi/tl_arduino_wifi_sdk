/*******************************************************************************************************
 *
 * @file    tlsr_pm.h
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
 
#ifndef __TLSR_PM_H__
#define __TLSR_PM_H__

#include <stdint.h>
#include <string.h>
#include <u-boot/list.h>

#include "hal/pm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * - Any lowpower state maintains WIFI connection.
 * - The lowpower state is automatically selected by the OS based on next idle duration.
 */
enum tlsr_pm_state {
	TLSR_PM_STATE_ACTIVE			= 0,	/**< ACTIVE - all system components are active */
	TLSR_PM_STATE_LIGHT_SLEEP	= 1,	/**< LIGHT SLEEP - cores are clock gated */
	TLSR_PM_STATE_DEEP_SLEEP		= 2,	/**< DEEP SLEEP - most components are powered down, XTAL/PLL */
	TLSR_PM_STATE_HIBERNATION	= 3,	/**< HIBERNATION - most power efficient */
};

/** Maximum duration for the low power duration */
#define TLSR_PM_DURATION_MAX	(60 * 60 * 24 * 1000 - 1)

/**
 * PM notification callback
 */
typedef void (*tlsr_pm_notify)(enum tlsr_pm_state state);

/**
 * @brief Enter the lowest power down state (hibernation) unconditionally
 *
 * The device would lose all the contexts and any network connectivity is lost.
 * If the duration is 0, device can wake up by GPIO only.
 * This function will not return if successfully put into power down state.
 * When the device wakes up, it starts from the reset vector as if it is a POR.
 *
 * @param[in] duration the duration of the power down period in millisecond
 */
int tlsr_pm_power_down(uint32_t duration);

/**
 * @brief Enable a GPIO pin to be used for the wakeup
 *
 * The GPIO must be configured to trigger the interrupt from
 * the application before calling this function.
 */
int tlsr_pm_enable_wakeup_io(uint8_t pin);

/**
 * @brief Disable a GPIO pin being used as a wakeup event.
 *
 * If the GPIO was not configured for the wakeup, it has no effect.
 */
int tlsr_pm_disable_wakeup_io(uint8_t pin);

/**
 * @brief Enable lowpower state
 *
 * Calling this function indicates that the caller is finished
 * with the work and it is now in idle state. The device will
 * enter the lowpower state when it is ready.
 */
int tlsr_pm_enable_lowpower(void);

/**
 * @brief Disable lowpower state
 *
 * Calling this function indicates that the caller is performning
 * meaningful activities and it does not want the device to enter
 * lowpower states.
 */
int tlsr_pm_disable_lowpower(void);

/**
 * @brief Disable lowpower state for the give duration
 *
 * The device will stay in PM ACTIVE for the duration requested starting
 * from the moment this function is called. If there is a previous call
 * to this function, and the time has not elapsed, the longer call with
 * the longest active duration will take effect.
 *
 * @param[in] duration the duration of the active period until the next lowpower
 */
int tlsr_pm_disable_lowpower_timeout(uint32_t duration);

/**
 * @brief Enable specific PM states
 *
 * The state parameter indicates the PM states to be enabled.
 *
 * @param[in] state state to be enabled
 */
int tlsr_pm_enable_state(enum tlsr_pm_state state);

/**
 * @brief Disable specific PM states
 *
 * The state parameter indicates the PM states to be disabled.
 *
 * @param[in] state state to be disabled
 */
int tlsr_pm_disable_state(enum tlsr_pm_state state);

/**
 * @brief Register a application specific PM callback
 *
 * This function can be used to get notified of the PM state changes.
 *
 * @param[in] pm_cb the callback context to be registered
 */
int tlsr_pm_register_handler(tlsr_pm_notify callback);

/**
 * @brief Unregister the application PM callback
 *
 * This function removes the previously registered callback.
 *
 * @param[in] pm_cb the callback context to be unregistered
 */
int tlsr_pm_unregister_handler(tlsr_pm_notify callback);

/**
 * @brief Set max count to enter hibernation mode
 *
 * This function set max count to enter hibernation mode.
 *
 * @param[in] hib_max hibernation mode max count
 */
int tlsr_pm_set_hib_max_count(uint32_t hib_max);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_PM_H__
