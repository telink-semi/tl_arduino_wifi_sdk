/********************************************************************************************************
 * @file    ipc_based_tflm.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ipc_based_tflm.h"
#include "ipc.h"
#include <string.h>

typedef enum {
    IPC_BASED_DRIVER_TFLM_CMD_RESET = 0x01,
    IPC_BASED_DRIVER_TFLM_CMD_INIT_FEATURE = 0x02,
    IPC_BASED_DRIVER_TFLM_CMD_CAL_FEATURE = 0x03,
    IPC_BASED_DRIVER_TFLM_CMD_DETECT = 0x04,
} ipc_based_drvier_tflm_cmd_t;

#pragma pack(push, 1)
typedef struct {
    const unsigned char* model_data;
    uint32_t len;
    uint8_t data_labels[];
} para_reset_cmd_t;

typedef struct {
} para_initFeature_cmd_t;

typedef struct {
    uint32_t len;
    uint8_t data[];
} para_calFeature_cmd_t;

typedef struct {
    int expected_label_index;
} para_detect_cmd_t;

typedef union {
    para_reset_cmd_t reset_cmd_para;
    para_initFeature_cmd_t initFeature_cmd_para;
    para_calFeature_cmd_t calFeature_cmd_para;
    para_detect_cmd_t detect_cmd_para;
} para_tflm_t;

typedef struct {
    bool result;
} para_reset_rsp_t;

typedef para_reset_rsp_t para_initFeature_rsp_t;
typedef para_reset_rsp_t para_calFeature_rsp_t;
typedef para_reset_rsp_t para_detect_rsp_t;

#pragma pack(pop)

bool tflm_reset(const char ** catergroy_labels, int category_labels_count, const unsigned char * model_data) {
    bool ret = false;
    do {
        if(!catergroy_labels || category_labels_count <= 0) {
            break;
        }

        para_reset_cmd_t *cmd = (para_reset_cmd_t*)ipc_param_buffer();
        cmd->model_data = model_data;
        cmd->len = 0;
        uint8_t *ptr = cmd->data_labels;
        for(int i = 0; i < category_labels_count; i++) {
            size_t str_len = strlen(catergroy_labels[i]);
            memcpy(ptr, catergroy_labels[i], str_len);
            ptr[str_len] = '\0';
            ptr += str_len + 1;
            cmd->len += str_len + 1;
        }
        
        ipc_response_t rsp;
        ret = ipc_send(IPC_BASED_DRIVER_ID_TFLM, IPC_BASED_DRIVER_TFLM_CMD_RESET, cmd, sizeof(para_reset_cmd_t) + cmd->len, &rsp);
        ret = ret && ((para_reset_rsp_t*)rsp.packet->param)->result;

    } while(0);

    return ret;
}

bool tflm_init_feature(void) {
    bool ret = false;
    do {

        para_initFeature_cmd_t cmd = {
        };
        ipc_response_t rsp;
        ret = ipc_send(IPC_BASED_DRIVER_ID_TFLM, IPC_BASED_DRIVER_TFLM_CMD_INIT_FEATURE, &cmd, sizeof(cmd), &rsp);
        ret = ret && ((para_initFeature_rsp_t*)rsp.packet->param)->result;

    } while(0);

    return ret;
}

bool tflm_cal_feature(uint8_t * data, uint32_t len) {
    bool ret = false;
    do {
        if(!data || len == 0) {
            break;
        }

        para_calFeature_cmd_t *cmd = (para_calFeature_cmd_t*)ipc_param_buffer();
        cmd->len = len;
        memcpy(cmd->data, data, len);
        ipc_response_t rsp;
        ret = ipc_send(IPC_BASED_DRIVER_ID_TFLM, IPC_BASED_DRIVER_TFLM_CMD_CAL_FEATURE, cmd, sizeof(para_calFeature_cmd_t) + len, &rsp);
        ret = ret && ((para_calFeature_rsp_t*)rsp.packet->param)->result;

    } while(0);

    return ret;
}

bool tflm_detect(int expected_label_index) {
    bool ret = false;
    do {
        if(expected_label_index < 0) {
            break;
        }

        para_detect_cmd_t cmd = {
            .expected_label_index = expected_label_index,
        };
        ipc_response_t rsp;
        ret = ipc_send(IPC_BASED_DRIVER_ID_TFLM, IPC_BASED_DRIVER_TFLM_CMD_DETECT, &cmd, sizeof(cmd), &rsp);
        ret = ret && ((para_detect_rsp_t*)rsp.packet->param)->result;

    } while(0);

    return ret;
}
#endif