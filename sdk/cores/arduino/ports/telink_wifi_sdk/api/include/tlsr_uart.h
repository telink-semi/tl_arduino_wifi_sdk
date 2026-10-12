/*******************************************************************************************************
 *
 * @file    tlsr_uart.h
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
 
#ifndef __TLSR_UART_H__
#define __TLSR_UART_H__

#include <stdint.h>

#include "hal/uart.h"
#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TLSR_UART_TRANSFER_MAX_LEN	CONFIG_CLI_UARTBUF_MAXLEN

/**
 * UART device index
 */
enum tlsr_uart_idx {
	TLSR_UART_IDX_0,
	TLSR_UART_IDX_1,
	TLSR_UART_IDX_2,
	TLSR_UART_IDX_MAX,
};

enum tlsr_uart_baudrate {
	TLSR_UART_BDR_50		= 50,
	TLSR_UART_BDR_75		= 75,
	TLSR_UART_BDR_110	= 110,
	TLSR_UART_BDR_134	= 134,
	TLSR_UART_BDR_150	= 150,
	TLSR_UART_BDR_200	= 200,
	TLSR_UART_BDR_300	= 300,
	TLSR_UART_BDR_600	= 600,
	TLSR_UART_BDR_1200	= 1200,
	TLSR_UART_BDR_1800	= 1800,
	TLSR_UART_BDR_2400	= 2400,
	TLSR_UART_BDR_4800	= 4800,
	TLSR_UART_BDR_9600	= 9600,
	TLSR_UART_BDR_19200	= 19200,
	TLSR_UART_BDR_38400	= 38400,
	TLSR_UART_BDR_57600	= 57600,
	TLSR_UART_BDR_115200	= 115200,
};

enum tlsr_uart_data_bits {
	TLSR_UART_DATA_BITS_5,
	TLSR_UART_DATA_BITS_6,
	TLSR_UART_DATA_BITS_7,
	TLSR_UART_DATA_BITS_8,
};

enum tlsr_uart_parity {
	TLSR_UART_NO_PARITY,
	TLSR_UART_ODD_PARITY,
	TLSR_UART_EVENT_PARITY,
};

enum tlsr_uart_stop_bits {
	TLSR_UART_STOP_BIT_1,
	TLSR_UART_STOP_BIT_2,
};

/**
 * UART device properties
 */
struct tlsr_uart_cfg {
	enum tlsr_uart_baudrate baudrate;
	enum tlsr_uart_data_bits data_bits;
	enum tlsr_uart_parity parity;
	enum tlsr_uart_stop_bits stop_bits;
	uint8_t dma_en;
};

/**
 * UART event types
 */
enum tlsr_uart_event_type {
	TLSR_UART_EVENT_TX_CMPL,
	TLSR_UART_EVENT_RX_CMPL,
};

enum tlsr_uart_err {
	TLSR_UART_ERR_NO			= 0,
	TLSR_UART_ERR_OVERRUN	= (1 << 0),
	TLSR_UART_ERR_PARITY		= (1 << 1),
	TLSR_UART_ERR_FRAMING	= (1 << 2),
	TLSR_UART_ERR_LINE_BREAK = (1 << 3),
};


/**
 * UART event
 */
struct tlsr_uart_event {
	enum tlsr_uart_event_type type;
	uint8_t err;
};

/**
 * UART notification callback
 */
typedef int (*tlsr_uart_notify)(enum tlsr_uart_idx idx, struct tlsr_uart_event *event,
        void *ctx);


/**
 * @brief Initialize UART
 *
 * @param[in] idx index of the UART
 * @param[in] cfg the config parameter
 */
int tlsr_uart_init(enum tlsr_uart_idx idx, struct tlsr_uart_cfg *cfg);

/**
 * @brief Deinitialize UART
 *
 * @param[in] idx index of the UART
 */
int tlsr_uart_deinit(enum tlsr_uart_idx idx);

/**
 * @brief UART transmit data
 *
 * @param[in] idx index of the UART
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] timeout wait ms time for uart transmit complete, or -1(indefinitely)
 */
int tlsr_uart_tx(enum tlsr_uart_idx idx, uint8_t *tx_buf, uint32_t tx_len, int timeout);

/**
 * @brief UART transmit data
 *
 * @param[in] idx index of the UART
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] notify event notification function
 * @param[in] ctx user context associated with the notification event
 */
int tlsr_uart_tx_async(enum tlsr_uart_idx idx, uint8_t *tx_buf, uint32_t tx_len,
        tlsr_uart_notify notify, void *ctx);

/**
 * @brief UART receive data
 *
 * @param[in] idx index of the UART
 * @param[in] rx_buf buffer to receive
 * @param[in/out] (in) rx_len length of the buffer, (out) actual received length
 * @param[in] timeout wait ms time for uart receive complete, or -1(indefinitely)
 */
int tlsr_uart_rx(enum tlsr_uart_idx idx, uint8_t *rx_buf, uint32_t *rx_len, int timeout);

/**
 * @brief UART receive data
 *
 * @param[in] idx index of the UART
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] notify event notification function
 * @param[in] ctx user context associated with the notification event
 */
int tlsr_uart_rx_async(enum tlsr_uart_idx idx, uint8_t *tx_buf, uint32_t tx_len,
        tlsr_uart_notify notify, void *ctx);

/**
 * @brief UART reset
 *
 * @param[in] idx index of the UART
 */

int tlsr_uart_reset(enum tlsr_uart_idx idx);

/**
 * @brief UART get receive length
 *
 * @param[in] idx index of the UART
 * @param[out] rx_len receive length
 */
int tlsr_uart_get_rx_len(enum tlsr_uart_idx idx, uint32_t *rx_len);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_UART_H__
