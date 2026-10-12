/********************************************************************************************************
 * @file    util_common.h
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
#include "util_queue.h"
#include "util_ring_buffer.h"
#include "util_data_block.h"
#include "util_list.h"
#include "util_log.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void * util_malloc(size_t bytes);
void util_free(void *mem);

void * util_create_mutex();
void util_lock_mutex(void *mutex);
void util_unlock_mutex(void *mutex);
void util_destroy_mutex(void *mutex);

#ifdef __cplusplus
}
#endif