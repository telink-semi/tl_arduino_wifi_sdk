/*******************************************************************************************************
 *
 * @file    tlsr_i2s.h
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
 
#ifndef __TLSR_I2S_H__
#define __TLSR_I2S_H__

#include <stdint.h>

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * I2S data format
 */
enum tlsr_i2s_fmt {
	TLSR_I2S_FMT_I2S,
	TLSR_I2S_FMT_LJ,
	TLSR_I2S_FMT_RJ,
};

/**
 * I2S word length
 */
enum tlsr_i2s_wl {
	TLSR_I2S_WL_16,
	TLSR_I2S_WL_20,
	TLSR_I2S_WL_24,
};

/**
 * I2S role
 */
enum tlsr_i2s_role {
	TLSR_I2S_ROLE_MASTER,
	TLSR_I2S_ROLE_SLAVE,
};

/**
 * I2S data direction
 */
enum tlsr_i2s_direction {
	TLSR_I2S_RX 		= 0x1,
	TLSR_I2S_TX 		= 0x2,
	TLSR_I2S_BOTH 	= 0x3,
};

/**
 * I2S device configuration
 */
struct tlsr_i2s_cfg {
	enum tlsr_i2s_wl word_length;	/* word length in bits */
	enum tlsr_i2s_fmt format;		/* I2S data format */
	enum tlsr_i2s_role role;			/* Role on I2S bus */
	enum tlsr_i2s_direction dir;		/* I2S direction */
	uint32_t fs;					/* Sampling frequency, i.e., LRCLK frequency */
	int duration_per_block;			/* Duration in ms per an audio sample block */
	int number_of_blocks;			/* Number of audio sample blocks */
	int timeout;					/* Read, write timeout */
};

/**
 * @brief Initialize I2S
 */
int tlsr_i2s_init(void);

/**
 * @brief Deinitialize I2S
 */
int tlsr_i2s_deinit(void);

/**
 * @brief Configure I2S
 *
 * @param[in] cfg the cfg parameter
 */
int tlsr_i2s_configure(struct tlsr_i2s_cfg *cfg);

/**
 * @brief Read a block from an I2S stream
 */
int tlsr_i2s_read_block(uint8_t *buf, size_t *size);

/**
 * @brief Write a block into an I2S stream
 */
int tlsr_i2s_write_block(uint8_t *buf, size_t size);

/**
 * @brief Start an I2S stream
 */
int tlsr_i2s_start(enum tlsr_i2s_direction dir);

/**
 * @brief Stop an I2S stream
 */
int tlsr_i2s_stop(enum tlsr_i2s_direction dir);

/**
 * @brief Get a configured block buffer size
 */
int tlsr_i2s_get_block_buffer_size(struct tlsr_i2s_cfg *cfg);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_I2S_H__
