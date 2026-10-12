/********************************************************************************************************
 * @file    port_digital.c
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
#include <inttypes.h>
#include "core.h"
#include "tlsr_gpio.h"
static const uint8_t pinModeMap[] = {
    TLSR_GPIO_PROP_INPUT,
    TLSR_GPIO_PROP_OUTPUT,
    TLSR_GPIO_PROP_INPUT_PULL_UP
};

extern void port_pwm_stop(pwm_pin_map_t* pinMap);

void port_pin_mode(uint8_t pin, uint8_t mode)
{
    tlsr_gpio_configure(pin, pinModeMap[mode % (sizeof(pinModeMap) / sizeof(pinModeMap[0]))]);
}

void port_digital_write(uint8_t pin, uint8_t val)
{
    pwm_pin_map_t* pinMap = get_pwm_pin_map(pin);
    if(pinMap && (pinMap->state == PWM_STATE_ON)) {
        port_pwm_stop(pinMap);
    }

    tlsr_gpio_write(pin, val & 1);
}

int port_digital_read(uint8_t pin)
{
    pwm_pin_map_t* pinMap = get_pwm_pin_map(pin);
    if(pinMap) {
        port_pwm_stop(pinMap);
    }

    uint8_t value;
    tlsr_gpio_read(pin, &value);
    return (int)value;
}

#endif