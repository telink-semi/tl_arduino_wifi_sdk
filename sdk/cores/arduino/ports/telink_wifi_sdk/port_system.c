/********************************************************************************************************
 * @file    port_system.c
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
#include "sdk_common.h"
#include "wits_system.h"

void port_system_restart(void) {
    taskENTER_CRITICAL();
    wits_restart();
    taskEXIT_CRITICAL();
}

uint32_t port_system_get_free_heap_size(void) {
    return wits_get_free_heap_size();
}

uint32_t port_system_random(void) {
    return wits_random();
}

uint32_t port_system_get_heap_size_low_level(void) {
    return wits_get_minimum_free_heap_size();
}

void port_system_register_handler(void (*handler)(void *), void * para) {
    port_background_register_handler((background_handler_func)handler, para, 0);
}
#if PORT_CONFIG_USE_D25

static bool d25_init_flag = false;
void port_system_d25_init(void) {
    if(!d25_init_flag) {
        create_d25_log_dump_task();
        reset_d25_Core();
        d25_init_flag = true;
    }
}

void port_system_d25_reset(void) {
    reset_d25_Core();
}

uint32_t port_system_d25_get_stack_free_size_low_level(void) {
    return d25_get_stack_free_size_low_level();
}

void port_system_d25_log_level(uint8_t level) {
    ipc_driver_d25_log_level(level);
}

#endif

#define TIMER_FREQ_MHZ        (1)
#define TIMER_CHANNEL         (TLSR_TIMER_CH_0)
#define TIMER_IDX             (TLSR_TIMER_IDX_0)

#define MACHINE_TIMER_FREQ_HZ (F_CPU)

uint32_t port_micros(void) {
    uint64_t tick = prvReadMtime();
    return tick * 1000000 / MACHINE_TIMER_FREQ_HZ;
}

uint32_t port_millis(void) {
    uint64_t tick = prvReadMtime();
    return tick * 1000 / MACHINE_TIMER_FREQ_HZ;
}

uint32_t port_random(void) {
    uint32_t value = 0;
    tlsr_crypto_trng_read((uint8_t*)&value, sizeof(value));
    return value;
}

void port_interrupts(void) {
    taskENTER_CRITICAL();
}

void port_no_interrupts(void) {
    taskEXIT_CRITICAL();
}

void port_delay_us(unsigned int us) {
    unsigned int start_micros = micros();
    while(micros() - start_micros < us) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        if((us > 1000) && ((micros() - start_micros) <= (us - 1000))) {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

void port_delay_ms(unsigned int ms) {
    unsigned int start_micros = port_micros();
    unsigned int us = ms * 1000;
    vTaskDelay(pdMS_TO_TICKS(ms));
    while(port_micros() - start_micros < us) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
}

void port_yield(void) {
    vTaskDelay(pdMS_TO_TICKS(1));
}

#endif