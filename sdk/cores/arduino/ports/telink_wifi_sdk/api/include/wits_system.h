/*
 * Copyright 2018-2024 Telink Semiconductor Co., Ltd.
 */
// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * Originated from esp_system.h of ESP8266_RTOS_SDK
 * (https://github.com/espressif/ESP8266_RTOS_SDK)
 * and modified to provide wits System API as being ESP8266 style
 */

#ifndef __WITS_SYSTEM_H__
#define __WITS_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WITS_MAC_WIFI_STA,
	WITS_MAC_WIFI_SOFTAP,
}	wits_mac_type_t;


/**
 * @brief Reset reasons
 */
typedef enum {
	WITS_RST_UNKNOWN = 0,    //!< Reset reason can not be determined
	WITS_RST_POWERON,        //!< Reset due to power-on event_info
	WITS_RST_EXT,            //!< Reset by external pin
	WITS_RST_SW,             //!< Software reset via wits_restart
	WITS_RST_PANIC,          //!< Software reset due to exception/panic
	WITS_RST_INT_WDT,        //!< Reset (software or hardware) due to interrupt watchdog
	WITS_RST_TASK_WDT,       //!< Reset due to task watchdog
	WITS_RST_WDT,            //!< Reset due to other watchdogs
	WITS_RST_DEEPSLEEP,      //!< Reset after exiting deep sleep mode
	WITS_RST_BROWNOUT,       //!< Brownout reset (software or hardware)
	WITS_RST_SDIO,           //!< Reset over SDIO
}	wits_reset_reason_t;

/**
  * @brief  Set base MAC address with the MAC address which is stored in EFUSE or
  *         external storage e.g. flash and EEPROM.
  *
  * Base MAC address is used to generate the MAC addresses used by the networking interfaces.
  * If using base MAC address stored in EFUSE or external storage, call this API to set base MAC
  * address with the MAC address which is stored in EFUSE or external storage before initializing
  * WiFi.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return WITS_OK on success
  */
wits_err_t wits_base_mac_addr_set(uint8_t *mac);

/**
  * @brief  Return base MAC address which is set using wits_base_mac_addr_set.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return WITS_OK on success
  *         WITS_ERR_INVALID_MAC base MAC address has not been set
  */
wits_err_t wits_base_mac_addr_get(uint8_t *mac);

/**
  * @brief  Return base MAC address which is factory-programmed in EFUSE.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return WITS_OK on success
  */
wits_err_t wits_efuse_mac_get_default(uint8_t *mac);

/**
  * @brief  Read base MAC address and set MAC address of the interface.
  *
  * This function first get base MAC address using wits_base_mac_addr_get or reads base MAC address
  * from EFUSE. Then set the MAC address of the interface including wifi station and wifi softap.
  *
  * @param  mac  MAC address of the interface, length: 6 bytes.
  * @param  type  type of MAC address, 0:wifi station, 1:wifi softap.
  *
  * @return WITS_OK on success
  */
wits_err_t wits_read_mac(uint8_t* mac, wits_mac_type_t type);

/**
  * @brief  Derive local MAC address from universal MAC address.
  *
  * This function derives a local MAC address from an universal MAC address.
  * A `definition of local vs universal MAC address can be found on Wikipedia
  * <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local>`.
  * Universal MAC address is generated from base MAC address in EFUSE or other external storage.
  * Local MAC address is derived from the universal MAC address.
  *
  * @param  local_mac  Derived local MAC address, length: 6 bytes.
  * @param  universal_mac  Source universal MAC address, length: 6 bytes.
  *
  * @return WITS_OK on success
  */
wits_err_t wits_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac);

/**
 * @brief CPU frequency values
 */
typedef enum {
    RTC_CPU_FREQ_80M = 1,       //!< 80 MHz
    RTC_CPU_FREQ_160M = 2,      //!< 160 MHz
} rtc_cpu_freq_t;

/**
 * @brief Switch CPU frequency
 *
 * If a PLL-derived frequency is requested (80, 160), this function
 * will enable the PLL. Otherwise, PLL will be disabled.
 * Note: this function is not optimized for switching speed. It may take several
 * hundred microseconds to perform frequency switch.
 *
 * @param cpu_freq  new CPU frequency
 */
void rtc_clk_cpu_freq_set(rtc_cpu_freq_t cpu_freq);

/**
  * @brief  Reset to default settings.
  */
void system_restore(void);

/**
  * @brief  Restart CPU.
  *
  * This function does not return.
  */
void wits_restart(void);

/**
 * @brief  Get reason of last reset
 * @return See description of wits_reset_reason_t for explanation of each value.
 */
wits_reset_reason_t wits_reset_reason(void);

/**
  * @brief  Get the size of available heap.
  *
  * Note that the returned value may be larger than the maximum contiguous block
  * which can be allocated.
  *
  * @return Available heap size, in bytes.
  */
uint32_t wits_get_free_heap_size(void);

/**
  * @brief Get the minimum heap that has ever been available
  *
  * @return Minimum free heap ever available
  */
uint32_t wits_get_minimum_free_heap_size( void );

/**
 * @brief  Get one random 32-bit word from hardware RNG
 *
 * @return Random value between 0 and UINT32_MAX
 */
uint32_t wits_random(void);

/**
 * Get IDF version
 *
 * @return constant string from IDF_VER
 */
const char* wits_get_idf_version(void);

typedef enum {
	FLASH_SIZE_4M_MAP_256_256 = 0,  /**<  Flash size : 4Mbits. Map : 256KBytes + 256KBytes */
	FLASH_SIZE_2M,                  /**<  Flash size : 2Mbits. Map : 256KBytes */
	FLASH_SIZE_8M_MAP_512_512,      /**<  Flash size : 8Mbits. Map : 512KBytes + 512KBytes */
	FLASH_SIZE_16M_MAP_512_512,     /**<  Flash size : 16Mbits. Map : 512KBytes + 512KBytes */
	FLASH_SIZE_32M_MAP_512_512,     /**<  Flash size : 32Mbits. Map : 512KBytes + 512KBytes */
	FLASH_SIZE_16M_MAP_1024_1024,   /**<  Flash size : 16Mbits. Map : 1024KBytes + 1024KBytes */
	FLASH_SIZE_32M_MAP_1024_1024,   /**<  Flash size : 32Mbits. Map : 1024KBytes + 1024KBytes */
	FLASH_SIZE_32M_MAP_2048_2048,   /**<  Flash size : 32Mbits. Map : 2048KBytes + 2048KBytes */
	FLASH_SIZE_64M_MAP_1024_1024,   /**<  Flash size : 64Mbits. Map : 1024KBytes + 1024KBytes */
	FLASH_SIZE_128M_MAP_1024_1024,  /**<  Flash size : 128Mbits. Map : 1024KBytes + 1024KBytes */
	FLASH_SIZE_MAP_MAX
}	flash_size_map;

/**
 * @brief Chip models
 */
typedef enum {
    CHIP_TLSR91XX = 1, //!< TLSR91XX
} wits_chip_model_t;

/**
 * Chip feature flags, used in wits_chip_info_t
 */
#define CHIP_FEATURE_WIFI_BGN       (1 << 0)

/**
 * @brief The structure represents information about the chip
 */
typedef struct {
    wits_chip_model_t model;  //!< chip model, one of wits_chip_model_t
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint8_t cores;           //!< number of CPU cores
    uint8_t revision;        //!< chip revision number
} wits_chip_info_t;

/**
 * @brief Fill an wits_chip_info_t structure with information about the chip
 * @param[out] out_info structure to be filled
 */
void wits_chip_info(wits_chip_info_t* out_info);

/**
  * @brief  Get the current Flash size and Flash map.
  *
  * @param  null
  *
  * @return enum flash_size_map
  */
flash_size_map system_get_flash_size_map(void);

#ifdef __cplusplus
}
#endif

#endif /* __WITS_SYSTEM_H__ */
