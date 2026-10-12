/********************************************************************************************************
 * @file    RingBuffer.h
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

#include "Arduino.h"
#include <inttypes.h>
class RingBuffer {
protected:
    ring_buffer_handle_t handle;
public:
    RingBuffer();
    RingBuffer(size_t size);
    ~RingBuffer();
    size_t push(void *data, size_t nBytes);
    size_t copy(void *buffer, size_t bufferSize);
    size_t pop(size_t nBytes);
    size_t pop(void *buffer, size_t bufferSize);
    size_t available();
    size_t size();
    size_t freeSize();
    bool reSize(size_t size);
    bool isFull();
    bool isEmpty();
    bool clear();
};