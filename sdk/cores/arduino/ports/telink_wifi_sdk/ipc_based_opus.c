/********************************************************************************************************
 * @file    ipc_based_opus.c
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
#include "ipc_based_opus.h"
#include "ipc.h"
#include <string.h>

typedef enum {
    IPC_BASED_DRIVER_OPUS_CMD_VERSION = 0x01,
    IPC_BASED_DRIVER_OPUS_CMD_PREPARE = 0x02,
    IPC_BASED_DRIVER_OPUS_CMD_ENCODE = 0x03,
    IPC_BASED_DRIVER_OPUS_CMD_DECODE = 0x04,
} ipc_based_drvier_opus_cmd_t;

typedef enum {
    OPUS_CMD_DECODER = 0x01,
    OPUS_CMD_ENCODER = 0x02,
} opus_prepare_type_t;

#pragma pack(push, 1)

/*  OPUS command parameter */

typedef struct {
} para_version_cmd_t;

typedef struct {
    opus_prepare_type_t type;

    union {
        struct {
            uint8_t channels;
            uint16_t sample_rate;
            uint16_t samples_per_frame;
        } dec;

        struct {
           	uint8_t channels;
            uint16_t sample_rate;
            uint16_t samples_per_frame;
            uint8_t application;
            uint32_t bit_rate;
	        uint8_t complexity;
            bool use_vbr;
        } enc;
    } param;

} para_prepare_cmd_t;

typedef struct {
    uint32_t len;
    uint8_t data[];
} para_encode_cmd_t;

typedef struct {
    uint32_t len;
    uint8_t data[];
} para_decode_cmd_t;

typedef union {
    para_version_cmd_t version;
    para_prepare_cmd_t prepare;
    para_encode_cmd_t encode;
    para_decode_cmd_t decode;
} para_opus_t;

/*  OPUS response parameter */
typedef struct {
    bool success;
    int version;
} para_version_rsp_t;

typedef struct {
    bool success;
} para_prepare_rsp_t;

typedef struct {
    uint32_t success:1;
    uint32_t len:31;
    uint8_t data[];
} para_encode_rsp_t;

typedef struct {
    uint32_t success:1;
    uint32_t len:31;
    uint8_t data[];
} para_decode_rsp_t;


#pragma pack(pop)


typedef struct {
    uint32_t cmd;
    void (*func)(para_opus_t *param, size_t len);
} ipc_based_driver_opus_item_t;

/* OPUS Data*/
typedef struct {
	uint8_t *data;
	size_t length;
} opus_data_t;


int opus_get_version(void) {
    ipc_response_t rsp;
    bool ret = ipc_send(IPC_BASED_DRIVER_ID_OPUS, IPC_BASED_DRIVER_OPUS_CMD_VERSION, NULL, 0, &rsp);
    return ret? ((para_version_rsp_t*)rsp.packet->param)->version: -1;
}

bool opus_prepare_encoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame, uint8_t application,
                          uint32_t bit_rate, uint8_t complexity, bool use_vbr) {
    ipc_response_t rsp;
    para_prepare_cmd_t cmd = {
        .type = OPUS_CMD_ENCODER,
        .param = {
            .enc = {
                .channels = channels,
                .sample_rate = sample_rate,
                .samples_per_frame = samples_per_frame,
                .application = application,
                .bit_rate = bit_rate,
                .complexity = complexity,
                .use_vbr = use_vbr,
            }
        }
    };
    bool ret = ipc_send(IPC_BASED_DRIVER_ID_OPUS, IPC_BASED_DRIVER_OPUS_CMD_PREPARE, &cmd, sizeof(cmd), &rsp);
    return ret && ((para_prepare_rsp_t*)rsp.packet->param)->success;

}

bool opus_prepare_decoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame) {
    ipc_response_t rsp;
    para_prepare_cmd_t cmd = {
        .type = OPUS_CMD_DECODER,
        .param = {
            .dec = {
                .channels = channels,
                .sample_rate = sample_rate,
                .samples_per_frame = samples_per_frame,
            }
        }
    };
    bool ret = ipc_send(IPC_BASED_DRIVER_ID_OPUS, IPC_BASED_DRIVER_OPUS_CMD_PREPARE, &cmd, sizeof(cmd), &rsp);
    return ret && ((para_prepare_rsp_t*)rsp.packet->param)->success;
}

bool opus_encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    ipc_response_t rsp;
    para_encode_cmd_t *cmd = (para_encode_cmd_t*)ipc_param_buffer();
    cmd->len = input_len;
    memcpy(cmd->data, input, input_len);
    bool ret = ipc_send(IPC_BASED_DRIVER_ID_OPUS, IPC_BASED_DRIVER_OPUS_CMD_ENCODE, NULL, sizeof(para_encode_cmd_t) + input_len, &rsp);
    para_encode_rsp_t *rsp_param = (para_encode_rsp_t*)rsp.packet->param;
    if(ret && rsp_param->success && output_len && output) {
        *output_len = rsp_param->len;

        if(*output_len > max_output_len) {
            *output_len = max_output_len;
        }

        memcpy(output, rsp_param->data, *output_len);
        
    }

    return ret && rsp_param->success;
}

bool opus_decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    ipc_response_t rsp;
    para_decode_cmd_t *cmd = (para_decode_cmd_t*)ipc_param_buffer();
    cmd->len = input_len;
    memcpy(cmd->data, input, input_len);
    bool ret = ipc_send(IPC_BASED_DRIVER_ID_OPUS, IPC_BASED_DRIVER_OPUS_CMD_DECODE, NULL, sizeof(para_decode_cmd_t) + input_len, &rsp);
    para_decode_rsp_t *rsp_param = (para_decode_rsp_t*)rsp.packet->param;
    if(ret && rsp_param->success && output_len && output) {
        *output_len = rsp_param->len;

        if(*output_len > max_output_len) {
            *output_len = max_output_len;
        }
        
        memcpy(output, rsp_param->data, *output_len);
    }

    return ret && rsp_param->success;
}

#endif