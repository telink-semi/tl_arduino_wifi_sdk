/********************************************************************************************************
 * @file    ipc_based_driver.h
 *
 * @brief   This is the header file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "ipc_based_opus.h"
#include "ipc_based_tflm.h"

void ipc_driver_d25_wfi(void);
bool ipc_driver_d25_prepareVad(uint16_t frame_size);
bool ipc_driver_d25_doVad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max);
void ipc_driver_d25_log_level(uint8_t level);

#ifdef __cplusplus
}
#endif