/********************************************************************************************************
 * @file    port_tone.c
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
#include "core.h"

extern void port_pwm_stop(pwm_pin_map_t* pinMap);
static void pwm_run_tone_duty(pwm_pin_map_t* pinMap, uint32_t freq, uint32_t duration_us)
{
  if(!pinMap) {
    return;
  }

  port_pwm_stop(pinMap);
  pinMap->duty = freq;
  
  uint32_t period_us = 1000000 / freq;
  uint32_t high = period_us >> 1;
  uint32_t low = high;
  
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

  unsigned int start_micros = micros();
  while(micros() - start_micros < duration_us) {
    if(duration_us > 1000 && (micros() - start_micros) <= (duration_us - 1000)) {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }

  port_pwm_stop(pinMap);
}

static void sw_pwm_run_tone_duty(uint8_t pin, uint32_t freq, uint32_t duration_us)
{
  uint32_t period_us = 1000000 / freq;
  period_us >>= 1;

  uint32_t start_micros = micros();
  uint32_t ref_micros = start_micros;
  uint8_t state = HIGH;

  port_digital_write(pin, state);
  
  while(micros() - start_micros < duration_us) {
    if(micros() - ref_micros >= period_us) {
      port_digital_write(pin, (state = !state));
      ref_micros = micros();
    }
  }
}

void port_tone(uint8_t pin, unsigned int frequency, unsigned long duration)
{
  uint32_t duration_us = duration * 1000;
  pwm_pin_map_t* pinMap = get_pwm_pin_map(pin);
  if(pinMap) {
    pwm_run_tone_duty(pinMap, frequency, duration_us);
  } else {
    sw_pwm_run_tone_duty(pin, frequency, duration_us);
  }
}


void port_no_tone(uint8_t pin)
{
  pwm_pin_map_t* pinMap = get_pwm_pin_map(pin);
  if(pinMap) {
    port_pwm_stop(pinMap);
  }
  port_digital_write(pin, LOW);
}

#endif