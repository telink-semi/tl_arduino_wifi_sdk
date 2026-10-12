/*******************************************************************************************************
 *
 * @file    tlsr_efuse.h
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
 
#ifndef __TLSR_EFUSE_H__
#define __TLSR_EFUSE_H__

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TLSR_EFUSE_ADDR_ROOT_KEY         0
#define TLSR_EFUSE_SIZE_ROOT_KEY         128

#define TLSR_EFUSE_ADDR_PARITY           128
#define TLSR_EFUSE_SIZE_PARITY           1

#define TLSR_EFUSE_ADDR_HARD_KEY         129
#define TLSR_EFUSE_SIZE_HARD_KEY         1

#define TLSR_EFUSE_ADDR_FLASH_PROT       130
#define TLSR_EFUSE_SIZE_FLASh_PROT       1

#define TLSR_EFUSE_ADDR_SECURE_BOOT      131
#define TLSR_EFUSE_SIZE_SECURE_BOOT      1

#define TLSR_EFUSE_ADDR_AR_BL_EN         132
#define TLSR_EFUSE_SIZE_AR_BL_EN         1

#define TLSR_EFUSE_ADDR_SDIO_OCR_EN      132
#define TLSR_EFUSE_SIZE_SDIO_OCR_EN      1

#define TLSR_EFUSE_ADDR_AR_FW_EN         133
#define TLSR_EFUSE_SIZE_AR_FW_EN         1

#define TLSR_EFUSE_ADDR_CUST_ID          160
#define TLSR_EFUSE_SIZE_CUST_ID          8

#define TLSR_EFUSE_ADDR_CHIP_ID          192
#define TLSR_EFUSE_SIZE_CHIP_ID          32

#define TLSR_EFUSE_ADDR_PK_HASH          224
#define TLSR_EFUSE_SIZE_PK_HASH          256

#define TLSR_EFUSE_ADDR_SDIO_OCR         544
#define TLSR_EFUSE_SIZE_SDIO_OCR         20

#define TLSR_EFUSE_ADDR_WLAN_MAC_ADDR    576
#define TLSR_EFUSE_SIZE_WLAN_MAC_ADDR    48

#define TLSR_EFUSE_ADDR_BLE_MAC_ADDR     624
#define TLSR_EFUSE_SIZE_BLE_MAC_ADDR     48

#define TLSR_EFUSE_ADDR_RF_CAL           672
#define TLSR_EFUSE_SIZE_RF_CAL           64

#define TLSR_EFUSE_ADDR_AL_BL_VER        736
#define TLSR_EFUSE_SIZE_AL_BL_VER        64


#define TLSR_EFUSE_ADDR_AUXADC_LINEAR    800
#define TLSR_EFUSE_SIZE_AUXADC_LINEAR    32

#define TLSR_EFUSE_ADDR_RESERVED         832
#define TLSR_EFUSE_SIZE_RESERVED         192

enum tlsr_efuse_mode {
	TLSR_EFUSE_MODE_RAW			= 0,
	TLSR_EFUSE_MODE_RAM_BUFFER	= 1,
	TLSR_EFUSE_MODE_FLASH_BUFFER = 2,
};

/**
 * @brief
 *
 */
int tlsr_efuse_read(uint16_t bit_offset, uint16_t bit_count, uint8_t *val);

/**
 * @brief
 *
 */
int tlsr_efuse_write(uint16_t bit_offset, uint16_t bit_count, const uint8_t *val);

/**
 * @brief
 *
 */
int tlsr_efuse_clr_buffer(uint16_t bit_offset, uint16_t bit_count);

/**
 * @brief
 *
 */
int tlsr_efuse_set_mode(enum tlsr_efuse_mode mode);

/**
 * @brief
 *
 */
int tlsr_efuse_get_mode(enum tlsr_efuse_mode *mode);

/**
 * @brief
 *
 */
int tlsr_efuse_sync(void);

/**
 * @brief
 *
 */
int tlsr_efuse_load(void);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_EFUSE_H__
