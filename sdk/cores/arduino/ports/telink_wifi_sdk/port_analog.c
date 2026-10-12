/********************************************************************************************************
 * @file    port_analog.c
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
#include <inttypes.h>
#include "tlsr_adc.h"
#include "tlsr_timer.h"
#include "core.h"

#define PWM_FREQ 1000
#define PWM_PERIOD_MICROS (1000000 / PWM_FREQ)
#define PWM_TIMER_IDX TLSR_TIMER_IDX_0

static adc_pin_map_t* pinMap = NULL;
void port_analog_reference(uint8_t mode)
{
  (void)mode;
	tlsr_adc_reset();
}

int port_analog_read(uint8_t pin)
{
  if(!pinMap){
    pinMap = get_adc_pin_map(pin);
    if(!pinMap) return 0;
  }

  if(pinMap->pin != pin){
    pinMap = get_adc_pin_map(pin);
    if(!pinMap) return 0;
  }

  uint16_t adcVal = 0;
  tlsr_adc_read(pinMap->channel, (uint16_t*)&adcVal, sizeof(adcVal) / sizeof(uint16_t));
  return adcVal;
}

void port_pwm_stop(pwm_pin_map_t* pinMap)
{
  if(!pinMap) return;

  tlsr_timer_stop(pinMap->idx, pinMap->channel);
  pinMap->state = PWM_STATE_OFF;
}

static void pwm_start_duty(pwm_pin_map_t* pinMap, uint8_t duty)
{
  if(!pinMap) {
    return;
  }
  else if(pinMap->state == PWM_STATE_ON && pinMap->duty == duty) {
    return;
  }else if(pinMap->state == PWM_STATE_ON) {
    port_pwm_stop(pinMap);
  }

  pinMap->duty = duty;
  
  uint16_t high = (PWM_PERIOD_MICROS * (duty + 1)) >> 8;
  uint16_t low = PWM_PERIOD_MICROS - high;

  struct tlsr_timer_cfg cfg = {
      .mode = TLSR_TIMER_MODE_PWM,
      .intr_en = 1,
      .data = {
          .pwm = {
              .high = high,
              .low = low,
          }
      }
  };

  if(tlsr_timer_configure(pinMap->idx, pinMap->channel, &cfg, NULL, NULL)){
    log_e("pwm_start_duty: tlsr_timer_configure failed");
    return;
  }
  
  tlsr_timer_start(pinMap->idx, pinMap->channel);
  pinMap->state = PWM_STATE_ON;
}

static void sw_pwm_start_duty(uint8_t pin, uint8_t duty)
{
  unsigned char flag = 0;
  unsigned int high_micros = (PWM_PERIOD_MICROS * (duty + 1)) >> 8;
  unsigned int start_micros = micros();

  port_digital_write(pin, HIGH);
  while(micros() - start_micros < PWM_PERIOD_MICROS) {
    if(micros() - start_micros >= high_micros && !flag) {
      port_digital_write(pin, LOW);
      flag = 1;
    }
  }
}

void port_analog_write(uint8_t pin, int val)
{
  pwm_pin_map_t* pinMap = get_pwm_pin_map(pin);
  if(pinMap) {
    pwm_start_duty(pinMap, val & 0xff);
  } else {
    sw_pwm_start_duty(pin, val & 0xff);
  }
}

#endif