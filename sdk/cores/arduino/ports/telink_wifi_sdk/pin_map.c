/********************************************************************************************************
 * @file    pin_map.c
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
#include "core.h"
#include "pins_arduino.h"
#include "tlsr_timer.h"
#include "tlsr_adc.h"

#define PWM_PIN_MAP_SIZE (sizeof(pwm_pin_map) / sizeof(pwm_pin_map[0]))
static pwm_pin_map_t pwm_pin_map[] = {
  {GPIO2, TLSR_TIMER_IDX_0, TLSR_TIMER_CH_0, PWM_STATE_OFF, 0},
  {GPIO3, TLSR_TIMER_IDX_0, TLSR_TIMER_CH_1, PWM_STATE_OFF, 0},
  {GPIO4, TLSR_TIMER_IDX_0, TLSR_TIMER_CH_2, PWM_STATE_OFF, 0},
  {GPIO5, TLSR_TIMER_IDX_0, TLSR_TIMER_CH_3, PWM_STATE_OFF, 0},
};

#define ADC_PIN_MAP_SIZE (sizeof(adc_pin_map) / sizeof(adc_pin_map[0]))
static adc_pin_map_t adc_pin_map[] = {
  {GPIO0, TLSR_ADC_SINGLE_CH_6},
  {GPIO1, TLSR_ADC_SINGLE_CH_7},
  {GPIO4, TLSR_ADC_SINGLE_CH_4},
  {GPIO7, TLSR_ADC_SINGLE_CH_5}
};

adc_pin_map_t* get_adc_pin_map(uint8_t pin)
{
  for (int i = 0; i < ADC_PIN_MAP_SIZE; i++) {
    if (adc_pin_map[i].pin == pin) {
      return &adc_pin_map[i];
    }
  }
  return NULL;
}

pwm_pin_map_t* get_pwm_pin_map(uint8_t pin)
{
  for (int i = 0; i < PWM_PIN_MAP_SIZE; i++) {
    if (pwm_pin_map[i].pin == pin) {
      return &pwm_pin_map[i];
    }
  }
  return NULL;
}

#endif