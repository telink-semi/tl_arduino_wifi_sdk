/********************************************************************************************************
 * @file    ipc.c
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
#include <stddef.h>
#include "ipc_dispatcher.h"
#include "port_interface.h"
#include "sdk_common.h"
#include "ipc.h"

#define IPC_TIMEOUT_MS (1000)

typedef struct {
    SemaphoreHandle_t xBinarySemaphore;
    uint32_t data_len;
    uint8_t buffer[CONFIG_IPC_ICMSG_W91_CB_RX_BUF_SIZE];
    bool init_flag;
} ipc_ctx_t;

static ipc_ctx_t ipc_ctx = {0};


void ipc_init(void) {
    ipc_dispatcher_init();
    ipc_dispatcher_start();
    if(ipc_ctx.xBinarySemaphore == NULL) {
        ipc_ctx.xBinarySemaphore = xSemaphoreCreateBinary();
        if(ipc_ctx.xBinarySemaphore == NULL) {
            log_e("ipc_init: xSemaphoreCreateBinary failed");
            return;
        }
    }
    ipc_ctx.init_flag = true;
}


void ipc_dispatcher_cb(const void *data, size_t len, void *param) {
    ipc_ctx_t *ctx = (ipc_ctx_t*)param;
    if (ctx) {
        if (len > sizeof(ctx->buffer)) {
            len = sizeof(ctx->buffer);
        }

        ctx->data_len = len;
        memcpy(ctx->buffer, data, len);

        if(ctx->xBinarySemaphore != NULL) {
            xSemaphoreGive(ctx->xBinarySemaphore);
        }
    }
}


void * ipc_param_buffer(void) {
    ipc_packet_t *packet = (ipc_packet_t*)ipc_ctx.buffer;
    return packet->param;
}


bool ipc_send(ipc_based_driver_id_t driver_id, uint32_t cmd, void *param, size_t len, ipc_response_t *response) {
    if (!ipc_ctx.init_flag) {
        return false;
    }

    if (driver_id >= IPC_BASED_DRIVER_ID_MAX) {
        log_e("ipc_send: invalid driver_id");
        return false;
    }

    if (len > sizeof(ipc_ctx.buffer) - sizeof(ipc_packet_t)) {
        log_e("ipc_send: len is too large");
        return false;
    }

    ipc_packet_t *packet = (ipc_packet_t*)ipc_ctx.buffer;
    packet->id = driver_id;
    packet->cmd = cmd;

    if (param && len) {
        memcpy(packet->param, param, len);
    } 

    uint32_t id = driver_id | (cmd << 8);
    ipc_dispatcher_rm(id);
    ipc_dispatcher_add(id, ipc_dispatcher_cb, &ipc_ctx);
    
    bool flag = true;
    int ret = ipc_dispatcher_send(ipc_ctx.buffer, sizeof(ipc_packet_t) + len);

    // log_d("rd_idx:%d, wr_idx:%d", *(volatile uint32_t*)(0x4001b408), *(volatile uint32_t*)(0x4001b420));

    if (ret > 0 && ipc_ctx.xBinarySemaphore != NULL) {
        if(xSemaphoreTake(ipc_ctx.xBinarySemaphore, IPC_TIMEOUT_MS) == pdTRUE) {
            log_d("ipc_send: xSemaphoreTake success");
        } else {
            log_e("ipc_send: xSemaphoreTake timeout<%dms>", IPC_TIMEOUT_MS);
            flag = false;
        }
    } else {
        log_e("ipc_send: ipc_dispatcher_send failed");
        flag = false;
    }
    
    ipc_dispatcher_rm(id);

    if(response && flag) {
        response->packet = (ipc_packet_t*)ipc_ctx.buffer;
        response->param_len = len;
    }

    return flag;
}

#endif