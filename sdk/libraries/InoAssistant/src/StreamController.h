/********************************************************************************************************
 * @file    StreamController.h
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
#include "RingBuffer.h"

#define TAG_LENGTH            (4)
#define TAG_NUM_DEFAULT       (8)

typedef void (*sc_state_func)(void);
typedef size_t (*sc_proc_func)(void *data, size_t len);

typedef struct {
    char tag[TAG_LENGTH+1];
    sc_state_func onBegin;
    sc_proc_func onRecv;
    sc_state_func onEnd;
} sc_tag_proc_item_t, *sc_tag_proc_item_t_ptr;

typedef struct {
    uint32_t len:16; //length of tag
    uint32_t cur:16; //current tag
    sc_tag_proc_item_t list[]; //tag list
} sc_tag_context_t, *sc_tag_context_t_ptr;

class StreamController {
protected:
    RingBuffer cache;
    sc_tag_context_t_ptr tagCtx;
    void *mutex;
protected:
    uint8_t *checkTag(uint8_t *ptr, size_t len, uint8_t &result, uint16_t &tag_index);
public:
    StreamController(size_t buffer_size=1024, uint16_t tag_num=TAG_NUM_DEFAULT);
    ~StreamController();
    size_t input(void *data, size_t len);
    bool registerProcess(const char *tag, sc_state_func onBegin, sc_proc_func onRecv, sc_state_func onEnd);
    void handleLoop(void);
    size_t freeSize(void);
};