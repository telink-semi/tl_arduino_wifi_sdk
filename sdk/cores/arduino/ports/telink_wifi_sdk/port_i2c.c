/********************************************************************************************************
 * @file    port_i2c.c
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
#include "tlsr_i2c.h"

enum {
    I2C_MASTER_STATE_IDLE = 1,
    I2C_MASTER_STATE_TX,
    I2C_MASTER_STATE_RX,
    I2C_MASTER_STATE_TRX,
    I2C_MASTER_STATE_TX_DONE,
    I2C_MASTER_STATE_RX_DONE
};


struct _port_i2c_ctx_t{
    void *owner;
    void *mutex;
    struct tlsr_i2c_event events[I2C_EVENT_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t state:4;
    uint8_t over_run_flag:1;
    uint8_t begin_flag:1;
    uint8_t tx_len;
    uint8_t rx_len;
    uint8_t tx_buf[I2C_TX_BUFFER_SIZE];
    uint8_t rx_buf[I2C_RX_BUFFER_SIZE];
    background_handle_t handle;
    uint32_t ref_time;
    uint32_t timeout;
    uint32_t freq;
    uint8_t reset_with_timeout:1;
    uint8_t timeout_flag:1;
    uint8_t i2c_index:4;
    uint8_t address;
};

typedef struct _port_i2c_ctx_t port_i2c_ctx_t, *port_i2c_ctx_t_ptr;


__attribute__((weak)) size_t i2c_slave_request_proc(void *ctx, uint8_t *buf, uint32_t bufLen) {
    (void)ctx;
    (void)buf;
    (void)bufLen;
    return 0;
}
__attribute__((weak)) size_t i2c_slave_receive_proc(void *ctx, uint8_t *buf, uint32_t bufLen) {
    (void)ctx;
    (void)buf;
    (void)bufLen;
    return 0;
}

void *port_i2c_create_context(void *owner) {
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)malloc(sizeof(port_i2c_ctx_t));
    if(!i2cCtx) return NULL;
    memset(i2cCtx, 0, sizeof(port_i2c_ctx_t));
    i2cCtx->owner = owner;
    i2cCtx->timeout = I2C_DEFAULT_TIMEOUT;
    i2cCtx->freq = I2C_DEFAULT_FREQ;
    i2cCtx->address = I2C_DEFAULT_ADDRESS;
    i2cCtx->reset_with_timeout = 1;
    return i2cCtx;
}

void port_i2c_destroy_context(void *ctx) {
    if(!ctx) return;
    free(ctx);
}

bool port_i2c_set_configuration(void *ctx, port_i2c_configuration_t *config) {
    if(!ctx || !config) return false;
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;

    if(config->owner_valid) {
        i2cCtx->owner = config->owner;
    }

    if(config->timeout_valid) {
        i2cCtx->timeout = config->timeout;
    }

    if(config->freq_valid) {
        i2cCtx->freq = config->freq;
    }

    if(config->reset_with_timeout_valid) {
        i2cCtx->reset_with_timeout = config->reset_with_timeout;
    }

    if(config->i2c_index_valid) {
        i2cCtx->i2c_index = config->i2c_index;
    }

    if(config->address_valid) {
        i2cCtx->address = config->address;
    }

    return true;
}

bool port_i2c_get_configuration(void *ctx, port_i2c_configuration_t *config) {
    if(!ctx || !config) return false;
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;

    config->owner_valid = 1;
    config->timeout_valid = 1;
    config->freq_valid = 1;
    config->reset_with_timeout_valid = 1;
    config->i2c_index_valid = 1;
    config->address_valid = 1;

    config->owner = i2cCtx->owner;
    config->timeout = i2cCtx->timeout;
    config->freq = i2cCtx->freq;
    config->reset_with_timeout = i2cCtx->reset_with_timeout;
    config->i2c_index = i2cCtx->i2c_index;
    config->address = i2cCtx->address;
    
    return true;
}


bool port_i2c_is_run(void *ctx) {
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
    return (i2cCtx != NULL) && (i2cCtx->begin_flag); 
}

static void i2c_fastcall_handler(void *para) {
    // log_d("i2c_handler");
    if(!para) return;
    volatile port_i2c_ctx_t_ptr i2cCtx = (volatile port_i2c_ctx_t_ptr)para;
    lock_mutex(i2cCtx->mutex);
    while(i2cCtx->head != i2cCtx->tail) {
        struct tlsr_i2c_event event = i2cCtx->events[i2cCtx->tail];
        i2cCtx->tail = (i2cCtx->tail + 1) % I2C_EVENT_BUFFER_SIZE;
        switch (event.type) {
        case TLSR_I2C_EVENT_SLAVE_RX_CMPL:
            i2c_slave_receive_proc(para, i2cCtx->rx_buf, event.data.slave_rx_cmpl.len);
            i2cCtx->rx_len = 0;
            i2cCtx->tx_len = i2c_slave_request_proc(para, i2cCtx->tx_buf, I2C_TX_BUFFER_SIZE);
            break;
        default:
            break;
        }
    }
    unlock_mutex(i2cCtx->mutex);
}

static int notify_cb(struct tlsr_i2c_event *event, void *ctx) 
{
    if(!ctx) return 0;
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)(ctx);
    uint8_t next = (i2cCtx->head + 1) % I2C_EVENT_BUFFER_SIZE;
	if(next == i2cCtx->tail) {
		i2cCtx->over_run_flag = 1;
		return 0;
	} 

    switch (event->type)
	{
	case TLSR_I2C_EVENT_SLAVE_RX_REQUEST :
        if(i2cCtx->rx_len == 0) {
		    tlsr_i2c_slave_rx(i2cCtx->i2c_index, i2cCtx->rx_buf, I2C_RX_BUFFER_SIZE);
        } else {
            i2cCtx->over_run_flag = 1;
        }
        return 0;
	case TLSR_I2C_EVENT_SLAVE_TX_REQUEST:
        if(i2cCtx->tx_len != 0) {
		    tlsr_i2c_slave_tx(i2cCtx->i2c_index, i2cCtx->tx_buf, i2cCtx->tx_len);
        } else {
            i2cCtx->over_run_flag = 1;
        }
		return 0;
    case TLSR_I2C_EVENT_SLAVE_RX_CMPL:
    case TLSR_I2C_EVENT_SLAVE_TX_CMPL:
        port_background_fastcall_handle(i2c_fastcall_handler, i2cCtx);
        break;
    case TLSR_I2C_EVENT_MASTER_TRANS_CMPL:
        if(i2cCtx->state == I2C_MASTER_STATE_RX || i2cCtx->state == I2C_MASTER_STATE_TRX) {
            i2cCtx->state = I2C_MASTER_STATE_RX_DONE;
            i2cCtx->rx_len = event->data.master_trans_cmpl.rx_len;
        } else if(i2cCtx->state == I2C_MASTER_STATE_TX) {
            i2cCtx->state = I2C_MASTER_STATE_TX_DONE;
            i2cCtx->tx_len = event->data.master_trans_cmpl.tx_len;
        }
        return 0;
	default:
		break;
	}

    i2cCtx->events[i2cCtx->head] = *event;
    i2cCtx->head = next;

	return 0;
}

static void i2c_handler(void *para) {
    // log_d("i2c_handler");
    if(!para) return;
    volatile port_i2c_ctx_t_ptr i2cCtx = (volatile port_i2c_ctx_t_ptr)para;
    lock_mutex(i2cCtx->mutex);
    if(i2cCtx->state == I2C_MASTER_STATE_RX || i2cCtx->state == I2C_MASTER_STATE_TX) {
        if(millis() - i2cCtx->ref_time >= i2cCtx->timeout) {
            i2cCtx->timeout_flag = 1;
            if(i2cCtx->reset_with_timeout) {
                tlsr_i2c_reset(i2cCtx->i2c_index);
            }
            i2cCtx->state = I2C_MASTER_STATE_IDLE;
        }
    }
    unlock_mutex(i2cCtx->mutex);
}

bool port_i2c_begin(void *ctx) {
    bool ret = false;
    void *mutex = NULL;
    port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
    do {
        if(!ctx) break;
        if(i2cCtx->begin_flag) break;
        mutex = create_mutex();
        if(!mutex) {
            log_e("I2C<%d> create mutex failed", i2cCtx->i2c_index);
            break;
        }
        i2cCtx->mutex = mutex;

        struct tlsr_i2c_cfg i2c_cfg = {
            .role = (i2cCtx->address == 0) ? TLSR_I2C_ROLE_MASTER : TLSR_I2C_ROLE_SLAVE,
            .addr_len = TLSR_I2C_ADDR_LEN_7BIT,
            .bitrate = i2cCtx->freq,
            .slave_addr = i2cCtx->address,
            .pull_up_en = 1,
        };

        if(i2c_cfg.role == TLSR_I2C_ROLE_MASTER) {
            i2cCtx->state = I2C_MASTER_STATE_IDLE;
        }

        if(tlsr_i2c_init(i2cCtx->i2c_index) != WITS_OK) {
            log_e("I2C<%d> i2c_init failed", i2cCtx->i2c_index);
            break;
        }

        if(tlsr_i2c_configure(i2cCtx->i2c_index, &i2c_cfg, notify_cb, ctx) != WITS_OK) {
            log_e("I2C<%d> i2c_configure failed", i2cCtx->i2c_index);
            break;
        }

        i2cCtx->handle = port_background_register_handler(i2c_handler, ctx, 0); 
        if(i2cCtx->handle == NULL){
            log_e("I2C<%d> register i2c_handler failed", i2cCtx->i2c_index);
            break;
        }
        i2cCtx->begin_flag = 1;
        ret = true;
    }while(0);

    if(!ret) {
        if(mutex) {
            destroy_mutex(mutex);
            i2cCtx->mutex = NULL;
        }
    }

    return ret;
}

bool port_i2c_end(void *ctx) {
    bool ret = false;

    do {
        if(!ctx) break;
        port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
        port_background_unregister_handler(i2cCtx->handle);
        i2cCtx->handle = NULL;

        if(i2cCtx->mutex) {
            destroy_mutex(i2cCtx->mutex);
            i2cCtx->mutex = NULL;
        }

        tlsr_i2c_deinit(i2cCtx->i2c_index);
        i2cCtx->begin_flag = 0;
        
        ret = true;
    }while(0);

    return ret;
}

bool port_i2c_master_start_tx(void *ctx, uint8_t address, uint8_t *buf, uint32_t len) {
    bool ret = false;
    do {
        if(!ctx || !buf) break;

        port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
        int result = -1;
        if(len > I2C_TX_BUFFER_SIZE) break;

        memcpy(i2cCtx->tx_buf, buf, len);
        i2cCtx->ref_time = millis();
        result = tlsr_i2c_master_tx_async(i2cCtx->i2c_index, address, i2cCtx->tx_buf, len);
        if(result == WITS_OK) {
            i2cCtx->state = I2C_MASTER_STATE_TX;
        } else {
            i2cCtx->state = I2C_MASTER_STATE_IDLE;
            break;
        }
        
        ret = (result == WITS_OK);
    }while(0);
    return ret;
}

bool port_i2c_master_start_rx(void *ctx, uint8_t address, uint32_t len) {
    bool ret = false;
    do {
        if(!ctx) break;

        port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
        int result = -1;
        if(len > I2C_RX_BUFFER_SIZE) len = I2C_RX_BUFFER_SIZE;
        i2cCtx->ref_time = millis();
        result = tlsr_i2c_master_rx_async(i2cCtx->i2c_index, address, i2cCtx->rx_buf, len);
        if(result == WITS_OK) {
            i2cCtx->state = I2C_MASTER_STATE_RX;
        } else {
            i2cCtx->state = I2C_MASTER_STATE_IDLE;
            break;
        }
        
        ret = (result == WITS_OK);
    }while(0);
    return ret;
}

bool port_i2c_master_start_trx(void *ctx, uint8_t address, uint8_t *tx_buf, uint32_t tx_len, uint32_t rx_len) {
    bool ret = false;
    do {
        if(!ctx || !tx_buf) break;

        volatile port_i2c_ctx_t_ptr i2cCtx = (volatile port_i2c_ctx_t_ptr)ctx;
        int result = -1;
        if(tx_len > I2C_TX_BUFFER_SIZE) tx_len = I2C_TX_BUFFER_SIZE;
        if(rx_len > I2C_RX_BUFFER_SIZE) rx_len = I2C_RX_BUFFER_SIZE;
        i2cCtx->ref_time = millis();
        memcpy(i2cCtx->tx_buf, tx_buf, tx_len);
        result = tlsr_i2c_master_tx_rx_async(i2cCtx->i2c_index, address, i2cCtx->tx_buf, tx_len, i2cCtx->rx_buf, rx_len);
        log_d("_i2c_master_start_trx: result<%d>", result);
        if(result == WITS_OK) {
            i2cCtx->state = I2C_MASTER_STATE_TRX;
        } else {
            i2cCtx->state = I2C_MASTER_STATE_IDLE;
            break;
        }
        
        ret = (result == WITS_OK);
    }while(0);
    return ret;
}

bool port_i2c_master_ask(void *ctx, i2c_master_opt_t *opt, uint32_t *len, uint8_t *buf, uint32_t bufLen) {
    bool ret = false;
    do {
        if(!ctx) break;
        volatile port_i2c_ctx_t_ptr i2cCtx = (volatile port_i2c_ctx_t_ptr)ctx;

        if(i2cCtx->state == I2C_MASTER_STATE_RX_DONE) {
            *opt = I2C_MASTER_RX;
            *len = ((i2cCtx->rx_len > bufLen)? bufLen: i2cCtx->rx_len);
            if(buf) {
                memcpy(buf, i2cCtx->rx_buf, *len);
            } 
        } else if(i2cCtx->state == I2C_MASTER_STATE_TX_DONE) {
            *opt = I2C_MASTER_TX;
            *len = i2cCtx->tx_len;
        } else {
            break;
        }

        i2cCtx->state = I2C_MASTER_STATE_IDLE;
        ret = true;
    } while(0);

    return ret;
}

void port_i2c_master_clear_timeout(void *ctx) {
     if(!ctx) return;
    volatile port_i2c_ctx_t_ptr i2cCtx = (volatile port_i2c_ctx_t_ptr)ctx;
    i2cCtx->timeout_flag = 0;
    if(i2cCtx->state == I2C_MASTER_STATE_RX || i2cCtx->state == I2C_MASTER_STATE_TX) {
        i2cCtx->state = I2C_MASTER_STATE_IDLE;
    }
}

bool port_i2c_master_probe(void *ctx, uint8_t address) {
    bool ret = false;
    do {
        if(!ctx) break;
        port_i2c_ctx_t_ptr i2cCtx = (port_i2c_ctx_t_ptr)ctx;
        uint8_t status = 0;
        if(tlsr_i2c_master_probe(i2cCtx->i2c_index, address, &status) != WITS_OK) break;
        ret = (status == 1);
    }while(0);

    return ret;
}

#endif