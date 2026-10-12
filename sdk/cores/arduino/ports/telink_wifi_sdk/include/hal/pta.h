/*******************************************************************************************************
 *
 * @file    pta.h
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
 
#ifndef _PTA_H_
#define _PTA_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * High-level device driver
 */

#define IOCTL_PTA_GET_FORCE_MODE		1
#define IOCTL_PTA_SET_FORCE_MODE		2

enum pta_force_mode {
	PTA_FORCE_MODE_OFF,
	PTA_FORCE_MODE_WIFI,
	PTA_FORCE_MODE_BLE,
};

struct pta_force_mode_val {
	uint32_t val;
};

#ifdef __cplusplus
}
#endif

#endif //_PTA_H_
