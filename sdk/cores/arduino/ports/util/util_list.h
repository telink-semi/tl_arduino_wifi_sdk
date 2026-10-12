/********************************************************************************************************
 * @file    util_list.h
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

typedef void *list_handle_t;


#ifdef __cplusplus
extern "C"{
#endif
typedef void (*listFreeFunc)(void *data);
typedef bool (*listCompareFunc)(const void *a, const void *b);

list_handle_t list_create(void);
bool list_destroy(list_handle_t handle, listFreeFunc func);
bool list_push_front(list_handle_t handle, void *data);
bool list_push_back(list_handle_t handle, void *data);
bool list_insert(list_handle_t handle, size_t index, void *data);
bool list_remove(list_handle_t handle, void *data, listCompareFunc compareFunc, listFreeFunc freeFunc);
bool list_remove_at(list_handle_t handle, size_t index, listFreeFunc func);
int list_find(list_handle_t handle, void *data, listCompareFunc func);
void *list_get(list_handle_t handle, size_t index);
size_t list_size(list_handle_t handle);
bool list_clear(list_handle_t handle, listFreeFunc func);
bool list_is_empty_(list_handle_t handle);

#ifdef __cplusplus
}
#endif