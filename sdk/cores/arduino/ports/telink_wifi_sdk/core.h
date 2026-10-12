/********************************************************************************************************
 * @file    core.h
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

#include "ipc.h"
#include "ipc_based_driver.h"

typedef struct{
  uint8_t pin;
  uint8_t channel;
} adc_pin_map_t;

enum pwm_state_t{
  PWM_STATE_OFF = 0,
  PWM_STATE_ON = 1,
};

typedef struct{
  uint8_t pin;
  uint8_t idx; //timer index
  uint8_t channel:7;//timer channel
  uint8_t state:1;
  uint8_t duty;
} pwm_pin_map_t;


extern void flash_hw_lock(void);
extern void flash_hw_unlock(void);
extern uint32_t d25_get_stack_free_size_low_level(void);
extern pwm_pin_map_t* get_pwm_pin_map(uint8_t pin);
extern adc_pin_map_t* get_adc_pin_map(uint8_t pin);
extern void initVariant(void);
extern void reset_d25_Core(void);
extern void create_d25_log_dump_task(void);

#ifdef __cplusplus
} // extern "C"
#endif

