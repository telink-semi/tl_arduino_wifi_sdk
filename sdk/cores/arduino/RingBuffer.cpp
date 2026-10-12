/********************************************************************************************************
 * @file    RingBuffer.cpp
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
#include "RingBuffer.h"

#define RINGBUFFER_SIZE_DEFAULT 1024

/**
 * @brief      Constructs a RingBuffer with the default size.
 */
RingBuffer::RingBuffer() {
    handle = ring_buffer_create(RINGBUFFER_SIZE_DEFAULT);
    if(!handle) {
        log_e("RingBuffer: create failed");
    }
}

/**
 * @brief      Constructs a RingBuffer with a specified size.
 * @param[in]  size - The desired size of the ring buffer.
 */
RingBuffer::RingBuffer(size_t size) {
    handle = ring_buffer_create(size);
    if(!handle) {
        log_e("RingBuffer: create failed");
    }
}

/**
 * @brief      Destructor to destroy the RingBuffer and free resources.
 */
RingBuffer::~RingBuffer() {
    ring_buffer_destroy(handle);
}

/**
 * @brief      Pushes data into the ring buffer.
 * @param[in]  data - Pointer to the data to be pushed.
 * @param[in]  nBytes - The number of bytes to push.
 * @return     The number of bytes actually pushed into the buffer.
 */
size_t RingBuffer::push(void *data, size_t nBytes) {
    return ring_buffer_push(handle, (uint8_t *)data, nBytes);
}

/**
 * @brief      Copies data from the ring buffer to a provided buffer.
 * @param[out] buffer - A pointer to the buffer where the data will be copied.
 * @param[in]  bufferSize - The size of the provided buffer.
 * @return     The number of bytes actually copied.
 */
size_t RingBuffer::copy(void *buffer, size_t bufferSize) {
    return ring_buffer_copy(handle, (uint8_t *)buffer, bufferSize);
}

/**
 * @brief      Removes a specified number of bytes from the ring buffer.
 * @param[in]  nBytes - The number of bytes to remove.
 * @return     The number of bytes actually removed.
 */
size_t RingBuffer::pop(size_t nBytes) {
    return ring_buffer_pop(handle, nBytes);
}

/**
 * @brief      Removes data from the ring buffer and copies it to a provided buffer.
 * @param[out] buffer - A pointer to the buffer where the data will be copied.
 * @param[in]  bufferSize - The size of the provided buffer.
 * @return     The number of bytes actually removed and copied.
 */
size_t RingBuffer::pop(void *buffer, size_t bufferSize) {
    return pop(copy(buffer, bufferSize));
}

/**
 * @brief      Returns the number of bytes currently available to be read from the ring buffer.
 * @return     The number of bytes available.
 */
size_t RingBuffer::available() {
    return ring_buffer_available(handle);
}

/**
 * @brief      Returns the total size of the ring buffer.
 * @return     The total size of the buffer.
 */
size_t RingBuffer::size() {
    return ring_buffer_get_buffer_size(handle);
}

/**
 * @brief      Returns the number of free bytes in the ring buffer.
 * @return     The number of free bytes.
 */
size_t RingBuffer::freeSize() {
    return ring_buffer_get_free_size(handle);
}

/**
 * @brief      Clears all data from the ring buffer.
 * @return     True if the clear operation is successful, false otherwise.
 */
bool RingBuffer::clear() {
    if(!handle) return false;
    pop(available());
    return true;
}

/**
 * @brief      Resizes the ring buffer to a new size.
 * @param[in]  size - The new size of the ring buffer.
 * @return     True if the resize operation is successful, false otherwise.
 */
bool RingBuffer::reSize(size_t size) {
    if(!handle) return false;
    if(size == this->size()) return true;
    ring_buffer_handle_t temp = ring_buffer_create(size);
    if(!temp) return false;
    uint8_t buffer[64];
    size_t rwLen;
    while(available()) {
        rwLen = pop(buffer, sizeof(buffer));
        if(rwLen) {
            ring_buffer_push(temp, buffer, rwLen);
            if(ring_buffer_is_full(temp)) {
                log_w("RingBuffer::reSize -> data lost");
                break;
            }
        }
    }
    ring_buffer_destroy(handle);
    handle = temp;
    return true;
}

/**
 * @brief Checks if the ring buffer is full.
 * @return True if the ring buffer is full, false otherwise.
 */
bool RingBuffer::isFull() {
    return ring_buffer_is_full(handle);
}

/**
 * @brief Checks if the ring buffer is empty.
 * @return True if the ring buffer is empty, false otherwise.
 */
bool RingBuffer::isEmpty() {
    return ring_buffer_is_empty(handle);
}
