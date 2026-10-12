/*******************************************************************************************************
 *
 * @file    wits_task_wdt.h
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
 
#ifndef _WITS_TASK_WDT_H_
#define _WITS_TASK_WDT_H_

#include <stdint.h>

#include "cmsis_os.h"
#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize task watchdog
 *
 * @param[in] timeout_s expire time of task watchdog
 */
wits_err_t wits_task_wdt_init(uint32_t timeout_s);

/**
 * @brief Susbscribe a task to the task watchdog
 *
 * @param task_id ID of the task. Input NULL to subscribe the current
 *                running task to the task watchdog
 */
wits_err_t wits_task_wdt_add(osThreadId_t task_id);

/**
 * @brief Unsusbscribe a task to the task watchdog
 *
 * @param task_id ID of the task. Input NULL to unsubscribe the current
 *                running task to the task watchdog
 */
wits_err_t wits_task_wdt_delete(osThreadId_t task_id);

/**
 * @brief Reset the task watchdog.
 *
 * @param task_id ID of the task. Input NULL to reset the current
 *                running task to the task watchdog
 */
wits_err_t wits_task_wdt_reset(osThreadId_t task_id);

#ifdef __cplusplus
}
#endif

#endif //_WITS_TASK_WDT_H_
