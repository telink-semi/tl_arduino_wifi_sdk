/********************************************************************************************************
 * @file    util_queue.c
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

struct utl_queue_member_t
{
    data_block_handle_t dataBlock;
    struct utl_queue_member_t *next;
};

typedef struct utl_queue_member_t utl_queue_member_t, *utl_queue_member_t_ptr;

typedef struct
{
    utl_queue_member_t_ptr header;
    utl_queue_member_t_ptr rear;
    uint32_t inMemCount;
    uint32_t outMemCount;
    uint32_t maxMember;
    void *hMutex;
}queue_prv_handle_block_t, *queue_prv_handle_t;

static void lock(queue_handle_t handle)
{
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    if(!qHandle) return;
    util_lock_mutex(qHandle->hMutex);
}

static void unlock(queue_handle_t handle)
{
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    if(!qHandle) return;
    util_unlock_mutex(qHandle->hMutex);
}

bool queue_push_back_data(queue_handle_t handle, void *data, size_t len)
{
    bool ret = false;
  
    utl_queue_member_t_ptr tempMem = NULL;
    data_block_handle_t dataBlock = NULL;
    lock(handle);
    do
    {
        if(!handle || !data || !len) break;
        queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
        if(!qHandle->rear || !qHandle->header) break;
        if((qHandle->inMemCount + 1) == qHandle->outMemCount) break;
        tempMem = (utl_queue_member_t_ptr)util_malloc(sizeof(utl_queue_member_t));
        if(!tempMem) break;
        tempMem->dataBlock = NULL;
        tempMem->next = NULL;
        dataBlock = data_block_create(data, len);
        if(!dataBlock) break;

        qHandle->rear->next = tempMem;
        qHandle->rear->dataBlock = dataBlock;
        qHandle->rear = tempMem;
        qHandle->inMemCount++;
        ret = true;
    }while(0);
    unlock(handle);

    if(!ret && tempMem)
    {
        if(tempMem->dataBlock)
        {
            data_block_destroy(tempMem->dataBlock);
        }
        util_free(tempMem);
        if(dataBlock) util_free(dataBlock);
    }

    return ret;
}

bool queue_push_back(queue_handle_t handle, data_block_handle_t block)
{
    bool ret = false;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    utl_queue_member_t_ptr tempMem = NULL;
    lock(handle);
    do
    {
        if(!qHandle || !block) break;
        if(!qHandle->rear || !qHandle->header) break;
        if((qHandle->inMemCount + 1) == qHandle->outMemCount) break;
        tempMem = (utl_queue_member_t_ptr)util_malloc(sizeof(utl_queue_member_t));
        if(!tempMem) break;
        tempMem->dataBlock = NULL;
        tempMem->next = NULL;

        qHandle->rear->next = tempMem;
        qHandle->rear->dataBlock = block;
        qHandle->rear = tempMem;
        qHandle->inMemCount++;
        ret = true;
    }while(0);
    unlock(handle);
    if(!ret && tempMem)
    {
        util_free(tempMem);
    }

    return ret;   
}

data_block_handle_t queue_front(queue_handle_t handle)
{
    data_block_handle_t ret = NULL;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        if(!qHandle->rear || !qHandle->header) break;
        if(qHandle->inMemCount == qHandle->outMemCount) break;
        ret = qHandle->header->dataBlock;
    }while(0);
    unlock(handle);
    return ret;
}

bool queue_set_front(queue_handle_t handle, data_block_handle_t block) {
    bool  ret = false;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        if(!qHandle->rear || !qHandle->header) break;
        if(qHandle->inMemCount == qHandle->outMemCount) break;
        qHandle->header->dataBlock = block;
        ret = true;
    }while(0);
    unlock(handle);
    return ret;
}

bool queue_pop(queue_handle_t handle)
{
    bool ret = false;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        if(!qHandle->rear || !qHandle->header) break;
        if(qHandle->inMemCount == qHandle->outMemCount) break;
        utl_queue_member_t_ptr tempMem = qHandle->header;
        if(!tempMem->dataBlock) break;
        qHandle->header = tempMem->next;
        util_free(tempMem);
        qHandle->outMemCount++;
        ret = true;
    }while(0);
    unlock(handle);

    return ret;
}

size_t queue_pop_front_data(queue_handle_t handle, void *data, size_t len)
{
    size_t ret = 0;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle || !data || !len) break;
        if(!qHandle->rear || !qHandle->header) break;
        if(qHandle->inMemCount == qHandle->outMemCount) break;
        utl_queue_member_t_ptr tempMem = qHandle->header;
        if(!tempMem->dataBlock) break;
        
        ret = data_block_read(&tempMem->dataBlock, (uint8_t *)data, len);
        if(data_block_len(tempMem->dataBlock) == 0)
        {
            qHandle->header = tempMem->next;
            data_block_destroy(tempMem->dataBlock);
            util_free(tempMem);
            qHandle->outMemCount++;
        }

    }while(0);
    unlock(handle);
    return ret;
}

data_block_handle_t queue_pop_front(queue_handle_t handle)
{
    data_block_handle_t ret = NULL;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        if(!qHandle->rear || !qHandle->header) break;
        if(qHandle->inMemCount == qHandle->outMemCount) break;
        utl_queue_member_t_ptr tempMem = qHandle->header;
        if(!tempMem->dataBlock) break;
        qHandle->header = tempMem->next;
        ret = tempMem->dataBlock;
        util_free(tempMem);
        qHandle->outMemCount++;
    }while(0);
    unlock(handle);

    return ret;
}

bool queue_is_full(queue_handle_t handle)
{
    bool ret = false;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        if(qHandle->maxMember == 0) 
        {
            ret = false;
        } 
        else 
        {
            ret = (uint32_t)(qHandle->inMemCount - qHandle->outMemCount) >= qHandle->maxMember ;
        }
    }while(0);
    unlock(handle);
    return ret;
}

bool queue_is_empty(queue_handle_t handle)
{
    bool ret = false;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        ret = (qHandle->inMemCount == qHandle->outMemCount);
    }while(0);
    unlock(handle);
    return ret;
}

int16_t queue_length(queue_handle_t handle)
{
    int16_t ret = 0;
    queue_prv_handle_t qHandle = (queue_prv_handle_t)handle;
    lock(handle);
    do
    {
        if(!qHandle) break;
        ret = (qHandle->inMemCount - qHandle->outMemCount);
    }while(0);
    unlock(handle);

    return ret;
} 

queue_handle_t queue_create(uint32_t max_length)
{
    queue_prv_handle_t ret = NULL, handle = NULL;
    utl_queue_member_t_ptr tempMem = NULL;
    do
    {
        handle = (queue_prv_handle_t)util_malloc(sizeof(queue_prv_handle_block_t));
        if(!handle) break;
        tempMem = (utl_queue_member_t_ptr)util_malloc(sizeof(utl_queue_member_t));
        if(!tempMem) break;

        handle->hMutex = util_create_mutex();
        if(!handle->hMutex) break;

        tempMem->next = NULL;
        tempMem->dataBlock = NULL;
        handle->inMemCount = 0;
        handle->outMemCount = 0;
        handle->maxMember = max_length;
        handle->header = tempMem;
        handle->rear = tempMem;

        ret = handle;
    }while(0);

    if(!ret)
    {
        if(handle) util_free(handle);
        if(tempMem) util_free(tempMem);
    }
    return ret;
}

bool queue_destroy(queue_handle_t _handle)
{
    bool ret = false;
    do
    {
        if(!_handle) break;
        queue_prv_handle_t handle = (queue_prv_handle_t)_handle;
        utl_queue_member_t_ptr curMem = handle->header;
        utl_queue_member_t_ptr tempMem = NULL;
        while(curMem)
        {
            tempMem = curMem->next;
            if(curMem->dataBlock)
            {
                data_block_destroy(curMem->dataBlock);
            }
            util_free(curMem);
            curMem = tempMem;
        }

        if(handle->hMutex) util_destroy_mutex(handle->hMutex);
        util_free(handle);
        ret = true;
    }while(0);
    return ret;
}

bool queue_clear(queue_handle_t handle) {
    if(!handle) return false;
    data_block_handle_t data = NULL;
    while(queue_length(handle) > 0) {
        data = queue_pop_front(handle);
        data_block_destroy(data);
    }
    return true;
}