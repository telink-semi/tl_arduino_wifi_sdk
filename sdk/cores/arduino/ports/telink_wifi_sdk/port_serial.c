/********************************************************************************************************
 * @file    port_serial.c
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
#include "core.h"

#define SERIAL_RX_BUFFER_SIZE          (128)
#define SERIAL_TX_BUFFER_SIZE          (128)
#define SERIAL_RX_TIMEOUT_MS_CONST     (100) //rx window
#define SERIAL_RX_TIMEOUT_MS_INTERVAL  (5) //rx timeout between bytes
#define SERIAL_TX_TIMEOUT_MS_CONST     (100)
#define SERIAL_TX_TIMEOUT_MS_PER_BYTE  (1)
#define SERIAL_CALC_TIMEOUT(len)       (SERIAL_TX_TIMEOUT_MS_CONST + (len) * SERIAL_TX_TIMEOUT_MS_PER_BYTE)

enum {
  SERIAL_STATE_IDLE = 0,
  SERIAL_STATE_TX_BUSY,
  SERIAL_STATE_RX_BUSY,
};

enum {
  SERIAL_STATUS_NONE,
  SERIAL_STATUS_TX_DONE,
  SERIAL_STATUS_RX_DONE,
  SERIAL_STATUS_TX_TIMEOUT,
  SERIAL_STATUS_RX_TIMEOUT,
  SERIAL_STATUS_ERROR,
};

typedef struct {
  uint32_t tx_len:13;
  uint32_t rx_len:13;
  uint32_t state:3;
  uint32_t status:3;
  uint32_t ref_time;
  uint32_t tx_timeout;
  uint16_t last_rx_len;
  uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
  uint8_t tx_buffer[SERIAL_TX_BUFFER_SIZE];
}serial_context_t, *serial_context_t_ptr;

struct _port_serial_ctx_t{
  void *owner;
  void *context;
  void *mutex;
  background_handle_t handle;
  unsigned long baud;
  uint8_t serial_index; 
  uint8_t config;
};
typedef struct _port_serial_ctx_t port_serial_ctx_t, *port_serial_ctx_t_ptr;


#ifndef byte
#define byte uint8_t
#endif

typedef union {
  byte config;
  struct{
    byte data_bits: 4;
    byte parity: 2;
    byte stop_bits: 2;
  }config_bits;
}serial_config_t;

static void serial_handler(void *ctx);

volatile int notify_cnt = 0;
volatile uint8_t last_type = 0;


void *port_serial_create_context(void *owner) {
  port_serial_ctx_t_ptr ret = (port_serial_ctx_t_ptr)malloc(sizeof(port_serial_ctx_t));
  if(ret) {
    memset(ret, 0, sizeof(port_serial_ctx_t));
    ret->owner = owner;
  }
  return ret;
}

void port_serial_destroy_context(void *ctx) {
  if(ctx) {
    free(ctx);
  }
}


bool port_serial_set_configuration(void *ctx, port_serial_configuration_t *config) {
  if(!ctx || !config) return false;
  port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
  if(config->owner_valid) {
    pSerialCtx->owner = config->owner;
  }

  if(config->baudrate_valid) {
    pSerialCtx->baud = config->baudrate;
  }

  if(config->attribute_config_valid) {
    pSerialCtx->config = config->attribute_config;
  }

  if(config->serial_index_valid) {
    pSerialCtx->serial_index = config->serial_index;
  }

  return true;
}

bool port_serial_get_configuration(void *ctx, port_serial_configuration_t *config) {
  if(!ctx || !config) return false;
  port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
  config->owner = pSerialCtx->owner;
  config->owner_valid = 1;
  config->baudrate = pSerialCtx->baud;
  config->baudrate_valid = 1;
  config->attribute_config = pSerialCtx->config;
  config->attribute_config_valid = 1;
  config->serial_index = pSerialCtx->serial_index;
  config->serial_index_valid = 1;
  return true;
}


__attribute__((weak)) uint16_t serial_read_prepare_process(void *ctx, uint16_t len) {
  (void)ctx; (void)len;
  return 0;
}

__attribute__((weak)) uint16_t serial_read_done_process(void *ctx, uint8_t *buffer, uint16_t len) {
  (void)ctx; (void)buffer; (void)len;
  return 0;
}
__attribute__((weak)) uint16_t serial_write_prepare_process(void *ctx, uint8_t *buffer, uint16_t len) {
  (void)ctx; (void)buffer; (void)len;
  return 0;
}
__attribute__((weak)) uint16_t serial_write_done_process(void *ctx, uint16_t len) {
  (void)ctx; (void)len;
  return 0;
}


static int serial_notify(enum tlsr_uart_idx idx, struct tlsr_uart_event *event, void *ctx) {
  
  port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
  if(!pSerialCtx || !pSerialCtx->context) return 0;
  volatile serial_context_t_ptr serialCtx = (volatile serial_context_t_ptr)pSerialCtx->context;
  notify_cnt++;
  last_type = event->type;
  switch(event->type) {
  case TLSR_UART_EVENT_TX_CMPL:
    if(serialCtx->state == SERIAL_STATE_TX_BUSY) {
      serialCtx->state = SERIAL_STATE_IDLE;
      serialCtx->status = (event->err == TLSR_UART_ERR_NO)? SERIAL_STATUS_TX_DONE: SERIAL_STATUS_ERROR;
      port_background_fastcall_handle(serial_handler, pSerialCtx);
    }
    break;
  case TLSR_UART_EVENT_RX_CMPL:
    if(serialCtx->state == SERIAL_STATE_RX_BUSY) {
      serialCtx->state = SERIAL_STATE_IDLE;
      serialCtx->status = (event->err == TLSR_UART_ERR_NO)? SERIAL_STATUS_RX_DONE: SERIAL_STATUS_ERROR;
      port_background_fastcall_handle(serial_handler, pSerialCtx);
    }
    break;
  default:
    break;
  }

  return 0;
}

static void serial_idle_process(port_serial_ctx_t_ptr pSerialCtx, serial_context_t_ptr serialCtx) {
  uint32_t rx_len = 0;
  switch(serialCtx->status) {
  case SERIAL_STATUS_TX_DONE:
    if(serial_write_done_process(pSerialCtx, serialCtx->tx_len) != serialCtx->tx_len) {
      log_e("serial<%d> write done process error", pSerialCtx->serial_index);
    }
    serialCtx->status = SERIAL_STATUS_NONE;
    break;
  case SERIAL_STATUS_RX_DONE:
    if(tlsr_uart_get_rx_len(pSerialCtx->serial_index, &rx_len) != WITS_OK) {
      log_e("serial<%d> get length or rx error", pSerialCtx->serial_index);
      serialCtx->status = SERIAL_STATUS_NONE;
      break;
    }
    serialCtx->last_rx_len = rx_len; 
  case SERIAL_STATUS_RX_TIMEOUT:
    
    if(serial_read_done_process(pSerialCtx, serialCtx->rx_buffer, serialCtx->last_rx_len) != serialCtx->last_rx_len) {
      log_e("serial<%d> read done process error", pSerialCtx->serial_index);
    }
    serialCtx->status = SERIAL_STATUS_NONE;
    break;
  case SERIAL_STATUS_ERROR:
  case SERIAL_STATUS_TX_TIMEOUT:
    tlsr_uart_reset(pSerialCtx->serial_index);
    serialCtx->status = SERIAL_STATUS_NONE;
    break;
  default:
    break;
  }


  serialCtx->tx_len = serial_write_prepare_process(pSerialCtx, serialCtx->tx_buffer, SERIAL_TX_BUFFER_SIZE);
#ifdef CONFIG_SERIAL_CONSOLE
  if(pSerialCtx->serial_index == 0 && serialCtx->tx_len > 0) {
    uint8_t data = 0;
    if(serialCtx->tx_len < SERIAL_TX_BUFFER_SIZE) {
      serialCtx->tx_buffer[serialCtx->tx_len] = 0;
    } else {
      data = serialCtx->tx_buffer[serialCtx->tx_len - 1];
      serialCtx->tx_buffer[serialCtx->tx_len - 1] = 0;
    }
    log_lock();
    printf("%s", serialCtx->tx_buffer);
    if(serialCtx->tx_len == SERIAL_TX_BUFFER_SIZE) { 
      printf("%c", data);
    }
    log_unlock();
    serialCtx->status = SERIAL_STATUS_TX_DONE;
    return;
  }
#endif
  if(serialCtx->tx_len > 0) {
    serialCtx->state = SERIAL_STATE_TX_BUSY;
    serialCtx->status = SERIAL_STATUS_NONE;
    serialCtx->ref_time = millis();
    serialCtx->tx_timeout = SERIAL_CALC_TIMEOUT(serialCtx->tx_len);

    if(tlsr_uart_tx_async(pSerialCtx->serial_index, serialCtx->tx_buffer, serialCtx->tx_len, serial_notify, pSerialCtx) != WITS_OK) {
      log_e("serial<%d> start tx error", pSerialCtx->serial_index);
      tlsr_uart_reset(pSerialCtx->serial_index);
      serialCtx->state = SERIAL_STATE_IDLE;
    }
  } else {
    serialCtx->rx_len = serial_read_prepare_process(pSerialCtx, SERIAL_RX_BUFFER_SIZE);
    if(serialCtx->rx_len > 0) {
      serialCtx->state = SERIAL_STATE_RX_BUSY;
      serialCtx->status = SERIAL_STATUS_NONE;
      serialCtx->ref_time = millis();
      serialCtx->last_rx_len = 0;
      tlsr_uart_reset(pSerialCtx->serial_index);
      if(tlsr_uart_rx_async(pSerialCtx->serial_index, serialCtx->rx_buffer, serialCtx->rx_len, serial_notify, pSerialCtx) != WITS_OK) {
        log_e("serial<%d> start rx error", pSerialCtx->serial_index);
        tlsr_uart_reset(pSerialCtx->serial_index);
        serialCtx->state = SERIAL_STATE_IDLE;
      }
    }
  }

}

static void serial_rx_busy_process(port_serial_ctx_t_ptr pSerialCtx, serial_context_t_ptr serialCtx) {
  uint32_t rx_len = 0;
  if(tlsr_uart_get_rx_len(pSerialCtx->serial_index, &rx_len) != WITS_OK) return;

  if(rx_len == 0 && serialCtx->last_rx_len == 0) {
    if(millis() - serialCtx->ref_time >= SERIAL_RX_TIMEOUT_MS_CONST) {
      serialCtx->last_rx_len = rx_len;
      tlsr_uart_reset(pSerialCtx->serial_index);
      serialCtx->status = SERIAL_STATUS_RX_TIMEOUT;
      serialCtx->state = SERIAL_STATE_IDLE;
    }
  } else if(rx_len > 0) {
    if(rx_len <= serialCtx->last_rx_len) {
      if(millis() - serialCtx->ref_time >= SERIAL_RX_TIMEOUT_MS_INTERVAL) {
        serialCtx->last_rx_len = rx_len;
        tlsr_uart_reset(pSerialCtx->serial_index);
        serialCtx->status = SERIAL_STATUS_RX_TIMEOUT;
        serialCtx->state = SERIAL_STATE_IDLE;
      }
    } else {
      serialCtx->ref_time = millis();
      serialCtx->last_rx_len = rx_len;
    }
  }

}

static void serial_tx_busy_process(port_serial_ctx_t_ptr pSerialCtx, serial_context_t_ptr serialCtx) {
  if(millis() - serialCtx->ref_time >= serialCtx->tx_timeout) {
    printf("serial<%d> tx timeout:%d\n", pSerialCtx->serial_index, serialCtx->tx_timeout);
    serialCtx->status = SERIAL_STATUS_TX_TIMEOUT;
    serialCtx->state = SERIAL_STATE_IDLE; 
  }
}

static bool console_serial_flag = false;
static void serial_handler(void *ctx) {
  if(console_serial_flag) return;
  port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
  if(!pSerialCtx || !pSerialCtx->context) return;
  lock_mutex(pSerialCtx->mutex);
  volatile serial_context_t_ptr serialCtx = (volatile serial_context_t_ptr)pSerialCtx->context;
  switch(serialCtx->state) {
  case SERIAL_STATE_IDLE:
    serial_idle_process(pSerialCtx, serialCtx);
    break;
  case SERIAL_STATE_RX_BUSY:
    serial_rx_busy_process(pSerialCtx, serialCtx);
    break;
  case SERIAL_STATE_TX_BUSY:
    serial_tx_busy_process(pSerialCtx, serialCtx);
    break;
  default:
    break;
  }
  unlock_mutex(pSerialCtx->mutex);
}


const char * serial_parity_str[] = {"None", "Odd", "Even"};
const char * serial_stop_bits_str[] = {"1", "2"};
const char * serial_data_bits_str[] = {"5", "6", "7", "8"};

bool port_serial_begin(void *ctx) {
  bool ret = false;
  serial_context_t_ptr context = NULL;
  void *mutex = NULL;
  do {
    if(!ctx) break;
    if(port_serial_is_run(ctx)) break;
    port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
    context = (serial_context_t_ptr)malloc(sizeof(serial_context_t));
    if(!context) break;

    memset(context, 0, sizeof(serial_context_t));
    pSerialCtx->context = context;
    context->state = SERIAL_STATE_IDLE;
    context->status = SERIAL_STATUS_NONE;

    struct tlsr_uart_cfg uart_cfg = {0};
    if(pSerialCtx->baud > 115200) {
      log_w("serial %d baudrate %d is too high, set to 115200", pSerialCtx->serial_index, pSerialCtx->baud);
      pSerialCtx->baud  = 115200;
    }

    uart_cfg.baudrate = pSerialCtx->baud;

    serial_config_t sConfig;
    sConfig.config = pSerialCtx->config;
    switch(sConfig.config_bits.data_bits){
      case 0:
        uart_cfg.data_bits = TLSR_UART_DATA_BITS_5;
        break;
      case 1:
        uart_cfg.data_bits = TLSR_UART_DATA_BITS_6;
        break;
      case 2:
        uart_cfg.data_bits = TLSR_UART_DATA_BITS_7;
        break;
      default:
        uart_cfg.data_bits = TLSR_UART_DATA_BITS_8;
        break;
    }

    switch(sConfig.config_bits.parity){
      case 1:
        uart_cfg.parity = TLSR_UART_ODD_PARITY;
        break;
      case 2:
        uart_cfg.parity = TLSR_UART_EVENT_PARITY;
        break;
      default:
        uart_cfg.parity = TLSR_UART_NO_PARITY;
        break;
    }

    switch(sConfig.config_bits.stop_bits){
      case 1:
        uart_cfg.stop_bits = TLSR_UART_STOP_BIT_2;
        break;
      default:
        uart_cfg.stop_bits = TLSR_UART_STOP_BIT_1;
        break;
    }
  #ifdef CONFIG_SERIAL_CONSOLE
    if(pSerialCtx->serial_index != 0) {
      (void)uart_cfg;
      if(tlsr_uart_init(pSerialCtx->serial_index, &uart_cfg) != WITS_OK) {
        log_e("serial<%d> init failed", pSerialCtx->serial_index);
        break;
      }


      tlsr_uart_reset(pSerialCtx->serial_index);
    }
  #else 
    if(tlsr_uart_init(pSerialCtx->serial_index, &uart_cfg) != WITS_OK) {
        log_e("serial<%d> init failed", pSerialCtx->serial_index);
        break;
    }

    tlsr_uart_reset(pSerialCtx->serial_index);
  #endif

    mutex = create_mutex();
    if(!mutex) {
      log_e("Serial<%d> create mutex failed", pSerialCtx->serial_index);
      break;
    }

    pSerialCtx->mutex = mutex;

    pSerialCtx->handle = port_background_register_handler(serial_handler, ctx, 0);
    if(!pSerialCtx->handle) {

      log_e("serial<%d> register handler failed");
      break;
    }
    log_d("serial<%d> baudrate: %d, data_bits: %s, parity: %s, stop_bits: %s", pSerialCtx->serial_index, pSerialCtx->baud, serial_data_bits_str[uart_cfg.data_bits], serial_parity_str[uart_cfg.parity], serial_stop_bits_str[uart_cfg.stop_bits]);
    ret = true;
  } while(0);

  if(!ret) {
    if(context) {
      free(context);
    }

    if(mutex) {
      destroy_mutex(mutex);
    }
  }

  return ret;
}

bool port_serial_end(void *ctx) {
  bool ret = false;
  do {
    if(!ctx) break;
    port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
    port_background_unregister_handler(pSerialCtx->handle);
    pSerialCtx->handle = NULL;

    if(pSerialCtx->context) {
      free(pSerialCtx->context);
      pSerialCtx->context = NULL;
    }
  #ifdef CONFIG_SERIAL_CONSOLE
    if(pSerialCtx->serial_index != 0) {
      tlsr_uart_deinit(pSerialCtx->serial_index);
    }
  #else
      tlsr_uart_deinit(pSerialCtx->serial_index);
  #endif

    if(pSerialCtx->mutex) {
      destroy_mutex(pSerialCtx->mutex);
      pSerialCtx->mutex = NULL;
    }

    ret = true;
  } while(0);

  return ret;
}

bool port_serial_is_run(void *ctx) {
  port_serial_ctx_t_ptr pSerialCtx = (port_serial_ctx_t_ptr)ctx;
  return (pSerialCtx != NULL) && (pSerialCtx->context != NULL) && (pSerialCtx->handle != NULL);
}


#include "console.h"

static uint8_t console_serial_index = 0;
static uint32_t byte_us = 10;
static int serial_console_write(struct console *con, char *s, int len){
  uint32_t start = port_micros();
  tlsr_uart_tx(console_serial_index, (uint8_t *)s, len, 0); // when system dump, tx timeout is invalid, set to 0
  while(port_micros() - start < byte_us * len + 10); // wait for tx complete
  return len;
}

static struct console serial_console = {
  .name = "arduino_serial",
	.write = serial_console_write,
};

void port_serial_console_init(uint8_t index, uint32_t baudrate) {
if(1) {
	console_serial_index = index;
	byte_us = (1000000 / baudrate + 1) * 10;
	register_console(&serial_console);
}
}

void port_serial_console_flush(void) {
  console_serial_flag = true;
	console_flush();
  console_serial_flag = false;
}


#endif