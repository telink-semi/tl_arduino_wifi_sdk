/*******************************************************************************************************
 *
 * @file    tlsr_i2c.h
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
 
#ifndef __TLSR_I2C_H__
#define __TLSR_I2C_H__

#include <stdint.h>

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * I2C device index
 */
enum tlsr_i2c_idx {
	TLSR_I2C_IDX_0,
	TLSR_I2C_IDX_1,
	TLSR_I2C_IDX_GPIO,
	TLSR_I2C_IDX_MAX,
};

/**
 * I2C device role
 */
enum tlsr_i2c_role {
	TLSR_I2C_ROLE_MASTER,
	TLSR_I2C_ROLE_SLAVE,
};

/**
 * I2C device addressing
 */
enum tlsr_i2c_addr_len {
	TLSR_I2C_ADDR_LEN_7BIT,
	TLSR_I2C_ADDR_LEN_10BIT,
};

/**
 * I2C device properties
 */
struct tlsr_i2c_cfg {
	enum tlsr_i2c_role role;         /* master or slave */
	enum tlsr_i2c_addr_len addr_len; /* addressing length */
	uint32_t bitrate;               /* target bitrate in bps */
	uint16_t slave_addr;            /* used for slave mode only */
	uint8_t dma_en;                 /* use of the DMA */
	uint8_t pull_up_en;             /* use of the internal pull up */
};

/**
 * I2C event types
 */
enum tlsr_i2c_event_type {
	TLSR_I2C_EVENT_MASTER_TRANS_CMPL,
	TLSR_I2C_EVENT_SLAVE_RX_REQUEST,
	TLSR_I2C_EVENT_SLAVE_TX_REQUEST,
	TLSR_I2C_EVENT_SLAVE_RX_CMPL,
	TLSR_I2C_EVENT_SLAVE_TX_CMPL,
};

/**
 * I2C event data when event type is TLSR_I2C_EVENT_MASTER_TRANS_CMPL
 */
struct tlsr_i2c_event_master_trans_cmpl {
	int tx_len;  /* number of transferred bytes */
	int rx_len;  /* number of transferred bytes */
};

/**
 * I2C event data when event type is TLSR_I2C_EVENT_SLAVE_TX_CMPL
 */
struct tlsr_i2c_event_slave_tx_cmpl {
	int len;  		   /* number of transferred bytes */
	uint8_t truncated; /* underrun, application supplied not enough tx buffer */
};

/**
 * I2C event data when event type is TLSR_I2C_EVENT_SLAVE_RX_CMPL
 */
struct tlsr_i2c_event_slave_rx_cmpl {
	int len;  		   /* number of transferred bytes */
	uint8_t truncated; /* overrun, application supplied not enough rx buffer */
};

/*
 * I2C event
 */
struct tlsr_i2c_event {
	enum tlsr_i2c_event_type type;
	union {
		struct tlsr_i2c_event_master_trans_cmpl master_trans_cmpl;
		struct tlsr_i2c_event_slave_tx_cmpl slave_tx_cmpl;
		struct tlsr_i2c_event_slave_rx_cmpl slave_rx_cmpl;
	} data;
};

/**
 * I2C notification callback
 */
typedef int (*tlsr_i2c_notify)(struct tlsr_i2c_event *event, void *ctx);

/**
 * @brief Initialize I2C
 *
 * @param[in] idx index of the I2C
 */
int tlsr_i2c_init(enum tlsr_i2c_idx idx);

/**
 * @brief Deinitialize I2C
 *
 * @param[in] idx index of the I2C
 */
int tlsr_i2c_deinit(enum tlsr_i2c_idx idx);

/**
 * @brief Configure I2C
 *
 * @param[in] idx index of the I2C
 * @param[in] cfg the cfg parameter
 * @param[in] notify event notification function
 * @param[in] ctx user context associated with the notification event
 */
int tlsr_i2c_configure(enum tlsr_i2c_idx idx, struct tlsr_i2c_cfg *cfg, tlsr_i2c_notify notify, void *ctx);

/**
 * @brief Reset I2C
 *
 * @param[in] idx index of the I2C
 */
int tlsr_i2c_reset(enum tlsr_i2c_idx idx);

/**
 * @brief Probe the I2C bus to see if there is a device with this address is present
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address
 * @param[in] status probed status
 */
int tlsr_i2c_master_probe(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *status);

/**
 * @brief I2C master to transmit data
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address (If idx is TLSR_I2C_IDX_GPIO, write bit must be included in address.)
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] timeout wait ms time for i2c complete
 */
int tlsr_i2c_master_tx(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len, uint32_t timeout);

/**
 * @brief I2C master to transmit data
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 */
int tlsr_i2c_master_tx_async(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len);

/**
 * @brief I2C master to receive data
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address (If idx is TLSR_I2C_IDX_GPIO, read bit must be included in address.)
 * @param[in] rx_buf buffer to receive
 * @param[in] rx_len length of the buffer
 * @param[in] timeout wait ms time for i2c complete
 */
int tlsr_i2c_master_rx(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout);

/**
 * @brief I2C master to receive data
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address
 * @param[in] rx_buf buffer to receive
 * @param[in] rx_len length of the buffer
 */
int tlsr_i2c_master_rx_async(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *rx_buf, uint32_t rx_len);

/**
 * @brief I2C master to transmit data and receive data without the stop condition
 *
 * @param[in] idx index of the I2C
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] rx_buf buffer to receive
 * @param[in] rx_len length of the buffer
 * @param[in] timeout wait ms time for i2c complete
 */
int tlsr_i2c_master_tx_rx(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout);

/**
 * @brief I2C master to transmit data and receive data without the stop condition
 *
 * @param[in] idx index of the I2C
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 * @param[in] rx_buf buffer to receive
 * @param[in] rx_len length of the buffer
 */
int tlsr_i2c_master_tx_rx_async(enum tlsr_i2c_idx idx, uint16_t addr, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

/**
 * @brief I2C slave to transmit data. This function must be called upon TLSR_I2C_EVENT_SLAVE_TX_REQUEST event.
 *
 * @param[in] idx index of the I2C
 * @param[in] addr slave address
 * @param[in] tx_buf buffer to transmit
 * @param[in] tx_len length of the buffer
 */
int tlsr_i2c_slave_tx(enum tlsr_i2c_idx idx, uint8_t *tx_buf, uint32_t tx_len);

/**
 * @brief I2C slave to receive data. This function must be called upon TLSR_I2C_EVENT_SLAVE_RX_REQUEST event.
 *
 * @param[in] idx index of the I2C
 * @param[in] rx_buf buffer to receive
 * @param[in] rx_len length of the buffer
 */
int tlsr_i2c_slave_rx(enum tlsr_i2c_idx idx, uint8_t *rx_buf, uint32_t rx_len);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_I2C_H__
