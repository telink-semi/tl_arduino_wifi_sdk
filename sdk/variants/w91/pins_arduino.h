/********************************************************************************************************
 * @file    pins_arduino.h
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

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

#define GPIO0 (0)
#define GPIO1 (1)
#define GPIO2 (2)
#define GPIO3 (3)
#define GPIO4 (4)
#define GPIO5 (5)
#define GPIO6 (6)
#define GPIO7 (7)

#define GPIO15 (15)
#define GPIO16 (16)
#define GPIO17 (17)
#define GPIO18 (18)
#define GPIO19 (19)
#define GPIO20 (20)
#define GPIO21 (21)
#define GPIO22 (22)
#define GPIO23 (23)
#define GPIO24 (24)


#if defined(BOARD_AIOT_DK2)


static const uint8_t A0 = GPIO0;
static const uint8_t A1 = GPIO1;
static const uint8_t A2 = GPIO4;

static const uint8_t D0 = GPIO0;
static const uint8_t D1 = GPIO1;
static const uint8_t D2 = GPIO2;
static const uint8_t D3 = GPIO3;
static const uint8_t D4 = GPIO4;
static const uint8_t D5 = GPIO5;
static const uint8_t D6 = GPIO23;
static const uint8_t D7 = GPIO24;

#define LED_BUILTIN GPIO15

#else

#define LED_BUILTIN GPIO15

#endif

#ifdef __cplusplus
} // extern "C"
#endif


