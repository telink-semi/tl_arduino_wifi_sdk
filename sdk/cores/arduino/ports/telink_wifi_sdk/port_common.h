/********************************************************************************************************
 * @file    port_common.h
 *
 * @brief   This is the header file for Arduino SDK
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
#pragma once

#ifdef abs
#define _OLD_ABS abs
#undef abs
#endif

#ifdef min
#define _OLD_MIN min
#undef min
#endif

#ifdef max
#define _OLD_MAX max
#undef max
#endif

#include <lwip/ip_addr.h>
#include <wits_wifi.h>
#include <wits_wifi_types.h>

#ifdef _OLD_ABS
#define abs _OLD_ABS
#undef _OLD_ABS
#endif

#ifdef _OLD_MIN
#define min _OLD_MIN
#undef _OLD_MIN
#endif

#ifdef _OLD_MAX
#define max _OLD_MAX
#undef _OLD_MAX
#endif

#define PORT_CPU_FREQ_HZ (10000000)


enum port_spi_bit_order_t {
    PORT_SPI_BIT_ORDER_MSB = 0,
    PORT_SPI_BIT_ORDER_LSB = 1,
};

enum port_spi_data_mode_t {
    PORT_SPI_DATA_MODE_0 = 0,
    PORT_SPI_DATA_MODE_1 = 1,
    PORT_SPI_DATA_MODE_2 = 2,
    PORT_SPI_DATA_MODE_3 = 3,
};