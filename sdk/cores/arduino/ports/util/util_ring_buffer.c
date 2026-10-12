/********************************************************************************************************
 * @file    util_ring_buffer.c
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

/**
 * @brief       This is the definition of the handle for cycle-buffer.
 */
typedef struct
{
    uint32_t *buffer; // pointer to save the address of cache.
    size_t write_idx; // the write-index.
    size_t read_idx; // the read-index.
    size_t buffer_size; // the buffer size of cache.
    size_t data_len; // length of valid-data.
    void *mutex; // mutex to protect the cycle-buffer.
    uint8_t init_flag:1; // init-flag of the handle.
}ring_buffer_prv_handle_t, *ring_buffer_prv_handle_t_ptr;

/**
 * @brief       This function used to get the length of valid-data saved in cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      length of the valid-data.
 */
static size_t ring_buffer_data_length(ring_buffer_handle_t handle)
{
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag) return 0;
    size_t write_idx = ((h_c_buffer->write_idx >= h_c_buffer->read_idx)? h_c_buffer->write_idx: (h_c_buffer->write_idx + h_c_buffer->buffer_size));
    return (write_idx - h_c_buffer->read_idx);
}

/*************************************************************************************
* @Brief Cycle Buffer Related
**************************************************************************************/
#define _ring_buffer_is_full(h_c_buffer) (((h_c_buffer->write_idx + 1) % h_c_buffer->buffer_size) == h_c_buffer->read_idx)
/**
 * @brief       This function used to judge whether the cycle-buffer is full.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      1:Yes, 0:No.
 */
bool ring_buffer_is_full(ring_buffer_handle_t handle)
{
    return ring_buffer_get_free_size(handle) == 0;
}

#define _ring_buffer_is_empty(h_c_buffer) (h_c_buffer->read_idx == h_c_buffer->write_idx)
/**
 * @brief       This function used to judge whether the cycle-buffer is empty.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      1:Yes, 0:No.
 */
bool ring_buffer_is_empty(ring_buffer_handle_t handle)
{   
    return ring_buffer_data_length(handle) == 0;
}

/**
 * @brief       This function used to pop specific length of data saved in cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   len - data length to pop.
 * @return      length of the pop-data.
 */
size_t ring_buffer_pop(ring_buffer_handle_t handle, size_t len)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag || !h_c_buffer->mutex) return ret;
    util_lock_mutex(h_c_buffer->mutex);
    do
    {
        if(_ring_buffer_is_empty(h_c_buffer)) break;
        if(len > h_c_buffer->data_len) {
            len = h_c_buffer->data_len;
        }
        h_c_buffer->read_idx += len;
        h_c_buffer->read_idx %= h_c_buffer->buffer_size;
        h_c_buffer->data_len = ring_buffer_data_length(handle);
        ret = len;
    }while(0);
    util_unlock_mutex(h_c_buffer->mutex);
    return ret;
}

/**
 * @brief       This function used to copy specific length of data saved in cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   buffer - buffer to save the data.
 * @param[in]   len - data length to copy.
 * @return      length of the copy-data.
 */
size_t ring_buffer_copy(ring_buffer_handle_t handle, uint8_t *buffer, size_t len)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag || !h_c_buffer->mutex || !buffer) return ret;

    util_lock_mutex(h_c_buffer->mutex);
    do
    {
        if(_ring_buffer_is_empty(h_c_buffer)) break;
        if(len > h_c_buffer->data_len) len = h_c_buffer->data_len;
        uint8_t *read_ptr = (uint8_t*)h_c_buffer->buffer + h_c_buffer->read_idx;
        uint8_t *end_ptr = (uint8_t*)h_c_buffer->buffer + h_c_buffer->buffer_size;
        // log_d("ring_buffer_copy: read_ptr:%p, len:%d, end_ptr:%p", read_ptr, len, end_ptr);
        for(size_t i = 0; i < len; i++)
        {
            buffer[i] = *read_ptr;
            read_ptr++;
            if(read_ptr >= end_ptr)
            {
                read_ptr -= h_c_buffer->buffer_size;
            }
        }
        ret = len;
    }while(0);
    util_unlock_mutex(h_c_buffer->mutex);

    return ret;
}

/**
 * @brief       This function used to push specific length of data to cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @param[in]   data - data to save.
 * @param[in]   len - data lenth to save.
 * @return      length of the push-data.
 */
size_t ring_buffer_push(ring_buffer_handle_t handle, uint8_t *data, size_t len)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag || !h_c_buffer->mutex || !data || len == 0) return ret;

    util_lock_mutex(h_c_buffer->mutex);
    do
    {
        if(_ring_buffer_is_full(h_c_buffer)) break;
        uint8_t *write_ptr = (uint8_t*)(h_c_buffer->buffer) + h_c_buffer->write_idx;
        uint8_t *end_ptr = (uint8_t*)(h_c_buffer->buffer) + h_c_buffer->buffer_size;
        size_t buffer_remain = h_c_buffer->buffer_size - h_c_buffer->data_len - 1;
        if(len > buffer_remain) len = buffer_remain;

        for(size_t i = 0; i < len; i++)
        {
            *write_ptr = data[i];
            write_ptr++;
            if(write_ptr >= end_ptr)
            {
                write_ptr -= h_c_buffer->buffer_size;
            }
        }

        h_c_buffer->write_idx += len;
        h_c_buffer->write_idx %= h_c_buffer->buffer_size;
        h_c_buffer->data_len = ring_buffer_data_length(handle);
        ret = len;
    }while(0);
    util_unlock_mutex(h_c_buffer->mutex);

    return ret;
}

/**
 * @brief       This function used to create the cycle-buffer.
 * @param[in]   buffer_size - buffer length to save.
 * @return      handle of the cycle-buffer.
 */
ring_buffer_handle_t ring_buffer_create(size_t buffer_size)
{
    ring_buffer_handle_t ret = NULL;
    ring_buffer_prv_handle_t_ptr h_c_buffer = (ring_buffer_prv_handle_t_ptr)util_malloc(sizeof(ring_buffer_prv_handle_t));
    do
    {
        if(!h_c_buffer) break;
        h_c_buffer->buffer = (uint32_t*)util_malloc(ring_buffer_get_u8_size_align4(buffer_size + 1));
        if(!h_c_buffer->buffer) break;
        h_c_buffer->buffer_size = buffer_size + 1;
        h_c_buffer->data_len = 0;
        h_c_buffer->read_idx = 0;
        h_c_buffer->write_idx = 0;
        h_c_buffer->mutex = util_create_mutex();
        if(!h_c_buffer->mutex) break;
        h_c_buffer->init_flag = true;
        ret = (ring_buffer_handle_t)h_c_buffer;
    }while(0);

    if(!ret && h_c_buffer)
    {
        if(h_c_buffer->buffer)
        {
            util_free(h_c_buffer->buffer);
        }

        if(h_c_buffer->mutex)
        {
            util_destroy_mutex(h_c_buffer->mutex);
        }
        
        util_free(h_c_buffer);
    }

    return ret;
}

/**
 * @brief       This function used to destroy the cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      None.
 */
void ring_buffer_destroy(ring_buffer_handle_t handle)
{
    ring_buffer_prv_handle_t_ptr h_c_buffer = (ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag) return;
    h_c_buffer->init_flag = false;
    h_c_buffer->data_len = 0;
    h_c_buffer->read_idx = 0;
    h_c_buffer->write_idx = 0;
    if(h_c_buffer->buffer) util_free(h_c_buffer->buffer);
    if(h_c_buffer->mutex) util_destroy_mutex(h_c_buffer->mutex);
    util_free(h_c_buffer);
}

/**
 * @brief       This function used to get the util_free size of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      util_free size of the cycle-buffer.
 */
size_t ring_buffer_get_free_size(ring_buffer_handle_t handle)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag) return ret;
    util_lock_mutex(h_c_buffer->mutex);
    ret = h_c_buffer->buffer_size - h_c_buffer->data_len - 1;
    util_unlock_mutex(h_c_buffer->mutex);
    return ret;
}

/**
 * @brief       This function used to get the available data length of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      available data length of the cycle-buffer.
 */
size_t ring_buffer_available(ring_buffer_handle_t handle)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    if(!h_c_buffer || !h_c_buffer->init_flag) return ret;
    util_lock_mutex(h_c_buffer->mutex);
    ret = h_c_buffer->data_len;
    util_unlock_mutex(h_c_buffer->mutex);
    return ret;
}

/**
 * @brief       This function used to get the buffer size of cycle-buffer.
 * @param[in]   handle - handle of the cycle-buffer.
 * @return      buffer size of the cycle-buffer.
 */
size_t ring_buffer_get_buffer_size(ring_buffer_handle_t handle)
{
    size_t ret = 0;
    volatile ring_buffer_prv_handle_t_ptr h_c_buffer = (volatile ring_buffer_prv_handle_t_ptr)handle;
    do
    {
        if(!h_c_buffer || !h_c_buffer->init_flag) break;
        ret = h_c_buffer->buffer_size - 1;
    }while(0);

    return ret;
}
