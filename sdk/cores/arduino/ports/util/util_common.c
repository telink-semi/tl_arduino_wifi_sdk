/********************************************************************************************************
 * @file    util_common.c
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
#include "util_common.h"
#include "port_interface.h"

void * util_malloc(size_t size) {
    return malloc(size);
}

void util_free(void *mem) {
    free(mem);
}

void * util_create_mutex() {
    return port_create_mutex();
}

void util_lock_mutex(void *mutex) {
    port_lock_mutex(mutex);
}

void util_unlock_mutex(void *mutex) {
    port_unlock_mutex(mutex);
}

void util_destroy_mutex(void *mutex) {
    port_destroy_mutex(mutex);
}