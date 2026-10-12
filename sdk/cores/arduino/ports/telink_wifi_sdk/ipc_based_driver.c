/********************************************************************************************************
 * @file    ipc_based_driver.c
 *
 * @brief   This is the source file for Arduino SDK
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
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK 

#include "port_interface.h"
#include "core.h"

typedef enum {
     IPC_BASED_DRIVER_SYSTEM_CMD_WFI = 0x01,
     IPC_BASED_DRIVER_SYSTEM_CMD_PREPARE_VAD,
     IPC_BASED_DRIVER_SYSTEM_CMD_DO_VAD,
     IPC_BASED_DRIVER_SYSTEM_CMD_LOG_LEVEL,
} ipc_based_drvier_system_cmd_t;

#pragma pack(push, 1)

typedef struct {
    uint32_t stall:1;
} para_wfi_cmd_t;

typedef struct {
    uint16_t rms_enable:1;
    uint16_t zcr_enable:1;
    uint16_t rms_threshold;
    uint16_t zcr_threshold_min;
    uint16_t zcr_threshold_max;
    uint32_t len;
    uint8_t data[];
} para_doVad_cmd_t;

typedef struct {
    uint16_t frame_size;
} para_prepareVad_cmd_t;

typedef struct {
    uint8_t level;
} para_logLevel_cmd_t;

typedef union {
    para_wfi_cmd_t wfi_cmd_para;
    para_doVad_cmd_t doVad_cmd_para;
    para_prepareVad_cmd_t prepareVad_cmd_para;
    para_logLevel_cmd_t log_level_cmd_para;
} para_system_t;

typedef struct {
    bool result;
} para_doVad_rsp_t;

typedef para_doVad_rsp_t para_prepareVad_rsp_t;

#pragma pack(pop)

void ipc_driver_d25_wfi(void) {
    para_system_t param = {0};
    param.wfi_cmd_para.stall = 0;

    ipc_send(IPC_BASED_DRIVER_ID_SYSTEM, IPC_BASED_DRIVER_SYSTEM_CMD_WFI, &param, sizeof(para_wfi_cmd_t), NULL);
}

bool ipc_driver_d25_prepareVad(uint16_t frame_size) {
    ipc_response_t rsp = {0};
    para_system_t *param = (para_system_t *)ipc_param_buffer();
    param->prepareVad_cmd_para.frame_size = frame_size;

    bool ret = ipc_send(IPC_BASED_DRIVER_ID_SYSTEM, IPC_BASED_DRIVER_SYSTEM_CMD_PREPARE_VAD, param, sizeof(para_prepareVad_cmd_t), &rsp);

    return ret && ((para_prepareVad_rsp_t *)(rsp.packet->param))->result;
}

bool ipc_driver_d25_doVad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max) {
    ipc_response_t rsp = {0};
    para_system_t *param = (para_system_t *)ipc_param_buffer();
    param->doVad_cmd_para.rms_enable = rms_enable;
    param->doVad_cmd_para.zcr_enable = zcr_enable;
    param->doVad_cmd_para.rms_threshold = rms_threshold;
    param->doVad_cmd_para.zcr_threshold_min = zcr_threshold_min;
    param->doVad_cmd_para.zcr_threshold_max = zcr_threshold_max;
    param->doVad_cmd_para.len = len;
    memcpy(param->doVad_cmd_para.data, data, len);

    bool ret = ipc_send(IPC_BASED_DRIVER_ID_SYSTEM, IPC_BASED_DRIVER_SYSTEM_CMD_DO_VAD, param, sizeof(para_doVad_cmd_t)+len, &rsp);

    return ret && ((para_doVad_rsp_t *)(rsp.packet->param))->result;
}

void ipc_driver_d25_log_level(uint8_t level) {
    para_system_t param = {0};
    param.log_level_cmd_para.level = level;

    ipc_send(IPC_BASED_DRIVER_ID_SYSTEM, IPC_BASED_DRIVER_SYSTEM_CMD_LOG_LEVEL, &param, sizeof(para_logLevel_cmd_t), NULL);
}

#endif