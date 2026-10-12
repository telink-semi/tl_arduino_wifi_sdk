/********************************************************************************************************
 * @file    port_pulse.c
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

typedef struct {
	uint32_t micros;
	uint8_t state;
}pulse_item_t;
#define PULSE_MAX_ITEMS (2)

static volatile pulse_item_t pulse_items[PULSE_MAX_ITEMS] = {{0, 0}};
static volatile uint8_t pulse_idx = 0;
static volatile uint8_t pulse_state = 0;

static int notify_cb(uint32_t pin, void *ctx)
{
	if(pulse_idx >= PULSE_MAX_ITEMS) {
		return 0;
	}

	uint8_t value;
	pulse_items[pulse_idx].micros = micros();
	tlsr_gpio_read(pin, &value);
	pulse_items[pulse_idx].state = value;

	if((pulse_idx == 0) && (value != pulse_state)) {
		return 0;
	}
	pulse_idx++;

	return 0;
}

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse.
 *
 */
unsigned long port_pulse_in(uint8_t pin, uint8_t state, unsigned long timeout)
{
	pulse_idx = 0;
	pulse_state = state;
		
	if(tlsr_gpio_configure(pin, TLSR_GPIO_PROP_INPUT_PULL_UP)) {
		return 0;
	}

	if(tlsr_gpio_enable_interrupt(pin, TLSR_GPIO_INT_BOTH_EDGE, notify_cb, NULL)) {
		return 0;
	}

	unsigned int start_micros = micros();
	while(pulse_idx < PULSE_MAX_ITEMS) {
		if(micros() - start_micros >= timeout) {
			break;
		}

		if(timeout > 1000 && (micros() - start_micros) <= (timeout - 1000)) {
			vTaskDelay(pdMS_TO_TICKS(1));
		}
	}
	tlsr_gpio_disable_interrupt(pin);

	if(pulse_idx < PULSE_MAX_ITEMS) {
		return 0;
	}else {
		return pulse_items[1].micros - pulse_items[0].micros;
	}
}

#endif