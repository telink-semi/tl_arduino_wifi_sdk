/********************************************************************************************************
 * @file    StreamController.cpp
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
#include "port_interface.h"
#include "StreamController.h" 
#include <stdio.h>


#define TAG_HEADER_ASCII               ((uint8_t)('<'))
#define TAG_INDEX_NONE                  0xffff

enum {
  CHECK_NONE_TAG,
  CHECK_TAG_HEADER,
  CHECK_TAG,
};

/**
 * @brief      Check and identify tags from the data stream.
 * @param[in]  ptr - Pointer to the data buffer.
 * @param[in]  len - Length of the data buffer.
 * @param[out] result - The check result (CHECK_NONE_TAG, CHECK_TAG_HEADER, or CHECK_TAG).
 * @param[out] tag_index - The index of the matched tag in the tag list.
 * @return     Pointer to the position of the matched tag header in the buffer, or the start of the buffer if no tag is found.
 */
uint8_t *StreamController::checkTag(uint8_t *ptr, size_t len, uint8_t &result, uint16_t &tag_index) {
    if(!ptr) return NULL;
    uint8_t *cur = ptr;
    uint8_t *ret = ptr;
    uint8_t tagLen = TAG_LENGTH;
    result = CHECK_NONE_TAG;
    while(cur < (ptr + len)) {
        if(*cur == TAG_HEADER_ASCII) {
            result = CHECK_TAG_HEADER;
            ret = cur;
            break;
        }
        cur++;
    }

    if((cur + tagLen) > (ptr + len)) return ret;

    sc_tag_proc_item_t_ptr item;
    port_lock_mutex(mutex);
    for(uint32_t i = 0; i < tagCtx->len; i++) {
        item = &tagCtx->list[i];
        if(memcmp(cur, item->tag, tagLen) == 0) {
            result = CHECK_TAG;
            tag_index = i;
            break;
        } 
    }
    port_unlock_mutex(mutex);

    if(result == CHECK_TAG) {
        ret = cur;
    }else {
        result = CHECK_NONE_TAG;
        ret = ptr;
    } 

    return ret;
}

/**
 * @brief      Constructor for StreamController.
 * @param[in]  buffer_size - The size of the internal ring buffer in bytes.
 * @param[in]  tag_num - The maximum number of tags that can be registered.
 */
StreamController::StreamController(size_t buffer_size, uint16_t tag_num) : cache(buffer_size), tagCtx(NULL), mutex(NULL) {
    size_t len = sizeof(sc_tag_context_t) + tag_num * sizeof(sc_tag_proc_item_t);
    tagCtx = (sc_tag_context_t_ptr)malloc(len);
    if(tagCtx) {
        memset(tagCtx, 0, len);
        tagCtx->len = tag_num;
        tagCtx->cur = TAG_INDEX_NONE;
    }
    mutex = port_create_mutex();
}

/**
 * @brief      Destructor for StreamController, releases allocated resources.
 */
StreamController::~StreamController() {
    if(tagCtx) {
        free(tagCtx);
    }

    if(mutex) {
        port_destroy_mutex(mutex);
    }
}

/**
 * @brief      Register a processing callback for a specific tag.
 * @param[in]  tag - The tag string to match (length must equal TAG_LENGTH).
 * @param[in]  onBegin - Callback function invoked when this tag becomes active.
 * @param[in]  onRecv - Callback function invoked to process received data.
 * @param[in]  onEnd - Callback function invoked when this tag is replaced by another.
 * @return     True if registration is successful, false otherwise.
 */
bool StreamController::registerProcess(const char *tag, sc_state_func onBegin, sc_proc_func onRecv, sc_state_func onEnd) {
    if(!tagCtx || !mutex || (strlen(tag) != TAG_LENGTH)) return false;

    bool ret = false;
    port_lock_mutex(mutex);
    do {
        sc_tag_proc_item_t_ptr item = NULL;
        for(uint16_t i = 0; i < tagCtx->len; i++) {
            if(strlen(tagCtx->list[i].tag) != TAG_LENGTH || memcmp(tagCtx->list[i].tag, tag, TAG_LENGTH) == 0) {
                item = &tagCtx->list[i];
                break;
            }
        }

        if(!item) break;

        memcpy(item->tag, tag, TAG_LENGTH);
        item->tag[TAG_LENGTH] = '\0';

        item->onBegin = onBegin;
        item->onRecv = onRecv;
        item->onEnd = onEnd;

        ret = true;
    } while(0);

    port_unlock_mutex(mutex);
    
    return ret;
}

/**
 * @brief      Feed data into the stream controller for processing.
 * @param[in]  data - Pointer to the data to be input.
 * @param[in]  len - Length of the data in bytes.
 * @return     The number of bytes actually written to the internal buffer.
 */
size_t StreamController::input(void *data, size_t len) {
    if(!tagCtx || !mutex) return 0;
    return cache.push(data, len);
}

/**
 * @brief      Get the available free space in the internal buffer.
 * @return     The number of bytes that can still be written to the buffer.
 */
size_t StreamController::freeSize(void) {
    if(!tagCtx || !mutex) return 0;
    return cache.freeSize();
}

/**
 * @brief      Main processing handler. This should be called periodically to process
 *             the data in the buffer, detect tags, and invoke registered callbacks.
 */
void StreamController::handleLoop(void) {
    uint8_t buffer[512];
    uint8_t result = CHECK_NONE_TAG;
    uint8_t *ptr = NULL;
    uint16_t tag_index = TAG_INDEX_NONE;
    sc_tag_proc_item_t_ptr item = NULL;
    size_t len = cache.copy(buffer, sizeof(buffer));
    size_t rw_len;
    if(len) {
        ptr = checkTag(buffer, len, result, tag_index);
        switch(result) {
        case CHECK_NONE_TAG: 
            rw_len = len;
            if(tagCtx->cur < tagCtx->len) {
                item = &tagCtx->list[tagCtx->cur];
                if(item->onRecv && rw_len) {
                    rw_len = item->onRecv(buffer, rw_len);
                }
            }
            cache.pop(rw_len);
            break;
        case CHECK_TAG_HEADER:
            rw_len = ptr - buffer;
            if(tagCtx->cur < tagCtx->len) {
                item = &tagCtx->list[tagCtx->cur];
                if(item->onRecv && rw_len) {
                    rw_len = item->onRecv(buffer, rw_len);
                }
            }
            cache.pop(rw_len);
            break;
        case CHECK_TAG:
            rw_len = ptr - buffer;
            len = rw_len;
            if(tagCtx->cur < tagCtx->len) {
                item = &tagCtx->list[tagCtx->cur];
            } else {
                item = NULL;
            }

            if(rw_len && item && item->onRecv) {
                rw_len = item->onRecv(buffer, rw_len);
            }

            if(len == rw_len) {
                rw_len += TAG_LENGTH;
                if(tagCtx->cur != tag_index) {
                    if(item && item->onEnd) {
                        item->onEnd();
                    }
                        
                    tagCtx->cur = tag_index;
                    if(tagCtx->cur < tagCtx->len) {
                        item = &tagCtx->list[tagCtx->cur];
                    } else {
                        item = NULL;
                    }

                    if(item && item->onBegin) {
                        item->onBegin();
                    }
                }
            }

            cache.pop(rw_len);
            break;
        default:
            break;
        }
    }    
}