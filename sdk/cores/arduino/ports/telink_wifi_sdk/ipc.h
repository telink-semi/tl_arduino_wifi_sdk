/********************************************************************************************************
 * @file    ipc.h
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
#include <stddef.h>
typedef enum {
    IPC_BASED_DRIVER_ID_SYSTEM = 0,
    IPC_BASED_DRIVER_ID_OPUS,
    IPC_BASED_DRIVER_ID_TFLM,
    IPC_BASED_DRIVER_ID_MAX,
} ipc_based_driver_id_t;

typedef struct {
    uint32_t id:8;
    uint32_t cmd:24;
    uint8_t param[];
} ipc_packet_t;

typedef struct {
    ipc_packet_t *packet;
    size_t param_len;
} ipc_response_t;

void ipc_init(void);
void * ipc_param_buffer(void);
bool ipc_send(ipc_based_driver_id_t driver_id, uint32_t cmd, void *param, size_t len, ipc_response_t *response);

#ifdef __cplusplus
}//extern "C"
#endif
