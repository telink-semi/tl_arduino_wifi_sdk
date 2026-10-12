/********************************************************************************************************
 * @file    util_ring_buffer.h
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

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

typedef void *ring_buffer_handle_t;


#define ring_buffer_get_u32_size(buffer_size) ((buffer_size + sizeof(uint32_t) - 1) / sizeof(uint32_t))
#define ring_buffer_get_u8_size_align4(buffer_size) ((buffer_size + sizeof(uint32_t) - 1) / sizeof(uint32_t) * sizeof(uint32_t))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       This function used to create the cycle-buffer.
 * @param[in]   buffer_size - buffer length to save.
 * @return      handle of the cycle-buffer.
 */
ring_buffer_handle_t ring_buffer_create(size_t buffer_size);

/**
 * @brief       This function used to destroy the cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      None.
 */
void ring_buffer_destroy(ring_buffer_handle_t handle);

/**
 * @brief       This function used to judge whether the cycle-buffer is full.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      1:Yes, 0:No.
 */
bool ring_buffer_is_full(ring_buffer_handle_t handle);

/**
 * @brief       This function used to judge whether the cycle-buffer is empty.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      1:Yes, 0:No.
 */
bool ring_buffer_is_empty(ring_buffer_handle_t handle);

/**
 * @brief       This function used to pop specific length of data saved in cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   len - data length to pop.
 * @return      length of the pop-data.
 */
size_t ring_buffer_pop(ring_buffer_handle_t handle, size_t len);

/**
 * @brief       This function used to push specific length of data to cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   data - data to save.
 * @param[in]   len - data lenth to save.
 * @return      length of the push-data.
 */
size_t ring_buffer_push(ring_buffer_handle_t handle, uint8_t *data, size_t len);

/**
 * @brief       This function used to copy specific length of data saved in cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   buffer - buffer to save the data.
 * @param[in]   len - data length to copy.
 * @return      length of the copy-data.
 */
size_t ring_buffer_copy(ring_buffer_handle_t handle, uint8_t *buffer, size_t len);

/**
 * @brief       This function used to get the free size of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      free size of the cycle-buffer.
 */
size_t ring_buffer_get_free_size(ring_buffer_handle_t handle);

/**
 * @brief       This function used to get the available data length of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      available data length of the cycle-buffer.
 */
size_t ring_buffer_available(ring_buffer_handle_t handle);

/**
 * @brief       This function used to get the buffer size of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      buffer size of the cycle-buffer.
 */
size_t ring_buffer_get_buffer_size(ring_buffer_handle_t handle);

#ifdef __cplusplus
}
#endif //__cplusplus
