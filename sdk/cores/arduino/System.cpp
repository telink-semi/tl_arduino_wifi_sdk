/********************************************************************************************************
 * @file    System.cpp
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
#include "System.h"
#include "port_interface.h"


void SystemClass::restart(void) {
     port_system_restart();
}

void SystemClass::d25Start(void) {
    port_system_d25_init();
}

void SystemClass::d25Reset(void) {
    port_system_d25_reset();
}

void SystemClass::d25LogLevel(uint8_t level) {
    port_system_d25_log_level(level);
}

uint32_t SystemClass::heapSize(void) {
    return port_system_get_free_heap_size();
}

uint32_t SystemClass::heapSizeLowLevel(void) {
    return port_system_get_heap_size_low_level();
}

uint32_t SystemClass::d25StackFreeSizeLowLevel(void) {
    return port_system_d25_get_stack_free_size_low_level();
}

uint32_t SystemClass::random(void) {
    return port_system_random();
}



SystemClass System;