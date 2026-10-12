/********************************************************************************************************
 * @file    port_websocket.c
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
#include "sdk_common.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ws.h"

enum {
    WEBSOCKET_STATE_IDLE,
    WEBSOCKET_STATE_BUSY,
    WEBSOCKET_STATE_RECONNECT,
};


struct _port_websocket_ctx_t{
    void *handle;
    void *transport_handle;
    void *owner;
    void *context;
    background_handle_t bgHandle;
    uint32_t ref_time;
    uint8_t state;
    // configuration
    ip_addr_t ip;
    websocket_info_t info;
    uint16_t port;
    uint32_t recon_time_ms;
    uint32_t timeout_ms;
};
typedef struct _port_websocket_ctx_t port_websocket_ctx_t, *port_websocket_ctx_t_ptr;

__attribute__((weak)) int websocket_task_read_callback(void *para) {
    (void)para;
    return 0;
}

__attribute__((weak)) int websocket_task_write_callback(void *para) {
    (void)para;
    return 0;
}

__attribute__((weak)) void websocket_task_stop_callback(void *para) {
    (void)para;
}

static bool websocket_check_connected(port_websocket_ctx_t_ptr wsCtx) {
    if(esp_transport_poll_read(wsCtx->handle, 0) < 0) {
        return false;
    }
    int error = esp_transport_get_errno(wsCtx->handle);
    if(error != 0) log_e("websocket_check_connected last error: %d", error);
    return error == 0;
}

static bool websocket_connect(port_websocket_ctx_t_ptr wsCtx) {
    if(wsCtx->info.path) {
        esp_transport_ws_set_path(wsCtx->handle, wsCtx->info.path);
    }

    if(wsCtx->info.headers) {
        esp_transport_ws_set_headers(wsCtx->handle, wsCtx->info.headers);
    }
    
    if(esp_transport_connect(wsCtx->handle, ipaddr_ntoa(&wsCtx->ip), wsCtx->port, wsCtx->timeout_ms) >= 0) {
        return true;
    }

    return false;
}

static void websocket_state_busy_process(port_websocket_ctx_t_ptr wsCtx) {
    if(!websocket_check_connected(wsCtx)) {
        log_e("websocket check connected failed");  
        wsCtx->state = WEBSOCKET_STATE_IDLE;  
        return;
    }

    if(websocket_task_write_callback(wsCtx) < 0) {
        log_e("websocket task write callback error");
        wsCtx->state = WEBSOCKET_STATE_IDLE;  
        return;
    }

    if(websocket_task_read_callback(wsCtx) < 0) {
        log_e("websocket task read callback error");
        wsCtx->state = WEBSOCKET_STATE_IDLE;  
        return;
    }   
}

static void websocket_state_reconnect_process(port_websocket_ctx_t_ptr wsCtx) {
    if(millis() - wsCtx->ref_time > wsCtx->recon_time_ms) {
        esp_transport_close(wsCtx->transport_handle);
        esp_transport_close(wsCtx->handle);
        if(websocket_connect(wsCtx)) {
            wsCtx->state = WEBSOCKET_STATE_BUSY;
            return;
        }

        wsCtx->ref_time = millis();
    }
}

static void websocket_state_idle_process(port_websocket_ctx_t_ptr wsCtx) {
    if(wsCtx->recon_time_ms > 0) {
        wsCtx->state = WEBSOCKET_STATE_RECONNECT;
        wsCtx->ref_time = millis();
    } else {
        websocket_task_stop_callback(wsCtx);
    }
}

//NOTE: if rx_buffer is full, the ping packet can not be responded.
static void websocket_handler(void *ctx) {
    if(!port_websocket_is_opened(ctx)) return;
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;

    switch(wsCtx->state) {
    case WEBSOCKET_STATE_BUSY:
        websocket_state_busy_process(wsCtx);
        break;
    case WEBSOCKET_STATE_RECONNECT:
        websocket_state_reconnect_process(wsCtx);
        break;
    case WEBSOCKET_STATE_IDLE:
        websocket_state_idle_process(wsCtx);
        break;
    default:
        break;
    }
}

void *port_websocket_create_context(void *owner) {
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )malloc(sizeof(port_websocket_ctx_t));
    if(!wsCtx) return NULL;
    memset(wsCtx, 0, sizeof(port_websocket_ctx_t));
    wsCtx->owner = owner;
    return wsCtx;
}

void port_websocket_destroy_context(void *ctx) {
    if(ctx) {
        free(ctx);
    }
} 

bool port_websocket_set_configuration(void *ctx, port_websocket_configuration_t *config) {
    if(!ctx || !config) return false;
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;
    if(config->owner_valid) {
        wsCtx->owner = config->owner;
    }
    if(config->ip_valid) {
        wsCtx->ip = config->ip;
    }
    if(config->port_valid) {
        wsCtx->port = config->port;
    }
    if(config->recon_time_ms_valid) {
        wsCtx->recon_time_ms = config->recon_time_ms;
    }
    if(config->timeout_ms_valid) {
        wsCtx->timeout_ms = config->timeout_ms;
    }
    if(config->info_valid) {
        wsCtx->info.path = config->info.path;
        wsCtx->info.headers = config->info.headers;
    }
    return true;
}

bool port_websocket_get_configuration(void *ctx, port_websocket_configuration_t *config) {
    if(!ctx || !config) return false;
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;
    config->owner = wsCtx->owner;
    config->owner_valid = 1;
    config->ip = wsCtx->ip;
    config->ip_valid = 1;
    config->port = wsCtx->port;
    config->port_valid = 1;
    config->recon_time_ms = wsCtx->recon_time_ms;
    config->recon_time_ms_valid = 1;
    config->timeout_ms = wsCtx->timeout_ms;
    config->timeout_ms_valid = 1;
    config->info.path = wsCtx->info.path;
    config->info.headers = wsCtx->info.headers;
    config->info_valid = 1;
    return true;
}

bool port_websocket_connect(void *ctx)
{
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;
    bool ret = false;
    do {
        if(!wsCtx) break;
        if(port_websocket_is_opened(ctx)) break;
        wsCtx->transport_handle = esp_transport_tcp_init();
        if(!wsCtx->transport_handle) break;
        wsCtx->handle = esp_transport_ws_init(wsCtx->transport_handle);
        if(!wsCtx->handle) break;

        if(!websocket_connect(wsCtx)) break;

        wsCtx->state = WEBSOCKET_STATE_BUSY;
        wsCtx->bgHandle = port_background_register_handler(websocket_handler, ctx, 0);
        if(!wsCtx->bgHandle) break;

        ret = true;
    } while(0);

    if(!ret) {
        if(wsCtx) {

            if(wsCtx->transport_handle) {
                esp_transport_close(wsCtx->transport_handle);
                esp_transport_destroy(wsCtx->transport_handle);
                wsCtx->transport_handle = NULL;
            }

            if(wsCtx->handle) {
                esp_transport_close(wsCtx->handle);
                esp_transport_destroy(wsCtx->handle);
                wsCtx->handle = NULL;
            }
        }
    }

    return ret;
}

int port_websocket_read(void *ctx, uint8_t *buffer, size_t len)
{
    if(!port_websocket_is_opened(ctx) || !buffer) return - 1;
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;
    
    int ret = esp_transport_poll_read(wsCtx->handle, 0);
    if( ret > 0) {
        ret = esp_transport_read(wsCtx->handle, (char *)buffer, len, 0);
    } else {
        if(ret < 0) {
            int error = esp_transport_get_errno(wsCtx->handle);
            if(error != 0) {
                log_e("_websocket_read, last error: %d", error);
            }else {
                ret = 0;
            }      
        }
    }

    return ret;
}

int port_websocket_write(void *ctx, uint8_t *data, size_t len)
{
    if(!port_websocket_is_opened(ctx) || !data) return - 1;
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;

    int ret = esp_transport_poll_write(wsCtx->handle, 0);

    if(ret > 0) {
        ret = esp_transport_write(wsCtx->handle, (char *)data, len, -1);
    } else {
        int error = esp_transport_get_errno(wsCtx->handle);
        if(error != 0) {
            log_e("_websocket_write, last error: %d", error);
        }else {
            ret = 0;
        }      
    }
    
    return ret;
}

bool port_websocket_close(void *ctx)
{
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr )ctx;
     if(!wsCtx) return false;
    port_background_unregister_handler(wsCtx->bgHandle);
    wsCtx->bgHandle = NULL;

    if(wsCtx->handle) {
        esp_transport_close(wsCtx->handle);
        esp_transport_destroy(wsCtx->handle);
        wsCtx->handle = NULL;
    }

    if(wsCtx->transport_handle) {
        esp_transport_close(wsCtx->transport_handle);
        esp_transport_destroy(wsCtx->transport_handle);
        wsCtx->transport_handle = NULL;
    }
   
    return true;
}

bool port_websocket_is_opened(void *ctx) {
    port_websocket_ctx_t_ptr wsCtx = (port_websocket_ctx_t_ptr)ctx;
    return ((wsCtx != NULL) && 
            (wsCtx->handle != NULL) &&
            (wsCtx->transport_handle != NULL));
}

#endif