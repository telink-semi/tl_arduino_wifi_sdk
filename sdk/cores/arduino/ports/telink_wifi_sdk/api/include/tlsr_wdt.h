/*******************************************************************************************************
 *
 * @file    tlsr_wdt.h
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
 
#ifndef _TLSR_WDT_H_
#define _TLSR_WDT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief watchdog timer start and expire after ms
 *
 * @param[in] ms expire time of watchdog timer
 */
int tlsr_wdt_start(uint32_t ms);

/**
 * @brief watchdog timer stop
 */
int tlsr_wdt_stop(void);

/**
 * @brief watchdog timer feed
 */
int tlsr_wdt_feed(void);

#ifdef __cplusplus
}
#endif

#endif //_TLSR_WDT_H_
