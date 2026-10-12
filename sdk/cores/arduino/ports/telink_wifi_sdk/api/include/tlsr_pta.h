/*******************************************************************************************************
 *
 * @file    tlsr_pta.h
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
 
#ifndef __TLSR_PTA_H__
#define __TLSR_PTA_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PTA force mode
 */
enum tlsr_pta_force_mode {
	TLSR_PTA_FORCE_MODE_OFF,
	TLSR_PTA_FORCE_MODE_WIFI,
	TLSR_PTA_FORCE_MODE_BLE,
};

/**
 * @brief Get current PTA force mode value
 *
 * @param[out] force_mode current PTA force mode
 */
int tlsr_pta_get_force_mode(enum tlsr_pta_force_mode *force_mode);

/**
 * @brief Set PTA force mode value
 *
 * @param[in] force_mode PTA force mode
 */
int tlsr_pta_set_force_mode(enum tlsr_pta_force_mode force_mode);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_PTA_H__
