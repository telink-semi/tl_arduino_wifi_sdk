/*******************************************************************************************************
 *
 * @file    tlsr_gpio.h
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
 
#ifndef __TLSR_GPIO_H__
#define __TLSR_GPIO_H__

#include <stdint.h>

#include "wits_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * GPIO pin property
 */
enum tlsr_gpio_property {
	TLSR_GPIO_PROP_OUTPUT,
	TLSR_GPIO_PROP_INPUT,
	TLSR_GPIO_PROP_INPUT_PULL_UP,
	TLSR_GPIO_PROP_INPUT_PULL_DOWN,
};

/**
 * TLSR GPIO value
 */
enum tlsr_gpio_value {
	TLSR_GPIO_VALUE_LOW,
	TLSR_GPIO_VALUE_HIGH,
};

/**
 * GPIO interrupt type
 */
enum tlsr_gpio_int_type {
	TLSR_GPIO_INT_RISING_EDGE,
	TLSR_GPIO_INT_FALLING_EDGE,
	TLSR_GPIO_INT_BOTH_EDGE,
};

/**
 * GPIO notification callback
 */
typedef int (*tlsr_gpio_notify)(uint32_t pin, void *ctx);

/**
 * @brief Set GPIO configuration
 *
 * @param[in] pin the pin number
 * @param[in] prop the properties of GPIO
 */
int tlsr_gpio_configure(uint32_t pin, enum tlsr_gpio_property property);

/**
 * @brief Write GPIO output value
 *
 * @param[in] pin the pin number
 * @param[in] value high or low value
 */
int tlsr_gpio_write(uint32_t pin, uint8_t value);

/**
 * @brief Read GPIO input value
 *
 * @param[in] pin the pin number
 * @param[in] value high or low value
 */
int tlsr_gpio_read(uint32_t pin, uint8_t *value);

/**
 * @brief Enable GPIO interrupt
 *
 * @param[in] pin the pin number
 * @param[in] type the interrupt type
 * @param[in] notify notification function
 * @param[in] ctx user context to be used in the callback
 */
int tlsr_gpio_enable_interrupt(uint32_t pin, enum tlsr_gpio_int_type type, tlsr_gpio_notify notify, void *ctx);

/**
 * @brief Disable GPIO interrupt
 *
 * @param[in] pin the pin number
 */
int tlsr_gpio_disable_interrupt(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_GPIO_H__
