/********************************************************************************************************
 * @file    util_queue.h
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

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include "util_data_block.h"

typedef void *queue_handle_t;


#ifdef __cplusplus
extern "C"{
#endif

queue_handle_t queue_create(uint32_t max_length);
bool queue_destroy(queue_handle_t handle);
bool queue_push_back_data(queue_handle_t handle, void *data, size_t len);
bool queue_push_back(queue_handle_t handle, data_block_handle_t block);
data_block_handle_t queue_front(queue_handle_t handle);
bool queue_set_front(queue_handle_t handle, data_block_handle_t block);
bool queue_pop(queue_handle_t handle);
size_t queue_pop_front_data(queue_handle_t handle, void *data, size_t len);
data_block_handle_t queue_pop_front(queue_handle_t handle);
bool queue_is_full(queue_handle_t handle);
bool queue_is_empty(queue_handle_t handle);
int16_t queue_length(queue_handle_t handle);
bool queue_clear(queue_handle_t handle);

#ifdef __cplusplus
}
#endif