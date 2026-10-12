/********************************************************************************************************
 * @file    util_data_block.c
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
#include <string.h>

typedef struct
{
    size_t len;
    uint8_t data[0];
}data_block_t, *data_block_prv_handle_t;

data_block_handle_t data_block_create(uint8_t *data, size_t len)
{
    data_block_prv_handle_t ret = NULL;
    do
    {
        size_t blockByteLen = len + sizeof(data_block_t);
		ret = (data_block_prv_handle_t)(util_malloc(blockByteLen));
        if(!ret) break;
		
		memset(ret, 0, blockByteLen);
        ret->len = len;
        
        if(data && len)
        {
            memcpy(ret->data, data, len);
        }
    }while(0);

    return ret;
}

int data_block_write(data_block_handle_t *handle, uint8_t *data, size_t len)
{
    if(!handle || !data || !len) return -1;

    int ret = 0;
    data_block_prv_handle_t tempDataBlock = NULL;
    data_block_prv_handle_t dataBlock = (data_block_prv_handle_t)(*handle);
    do
    {
        if(!dataBlock) break;
        size_t dataLen = dataBlock->len + len;
        tempDataBlock = (data_block_prv_handle_t)data_block_create(NULL, dataLen);
        if(!tempDataBlock) break;
        uint8_t *curPos = tempDataBlock->data;
        memcpy(curPos, dataBlock->data, dataBlock->len);
        curPos += dataBlock->len;
        memcpy(curPos, data, len);
        data_block_destroy(*handle);
        *handle = tempDataBlock;
        ret = tempDataBlock->len;
    }while(0);

    return ret;
}

int data_block_read(data_block_handle_t *handle, uint8_t *data, size_t len)
{
     if(!handle || !data || !len) return -1;

    int ret = 0;
    data_block_prv_handle_t tempDataBlock = NULL;
    data_block_prv_handle_t dataBlock = (data_block_prv_handle_t)(*handle);
    do
    {
        if(!dataBlock) break;
        size_t dataLen = dataBlock->len > len ? (dataBlock->len - len) : 0;
        tempDataBlock = (data_block_prv_handle_t)data_block_create(NULL, dataLen);
        if(!tempDataBlock) break;
        uint8_t *curPos = dataBlock->data;
        if(len > dataBlock->len) len = dataBlock->len;
        memcpy(data, curPos, len);
        curPos += len;
        if(dataLen)
        {
            memcpy(tempDataBlock->data, curPos, dataLen);
        }
        data_block_destroy(*handle);
        *handle = tempDataBlock;
        ret = len;

    }while(0);

    return ret;
}

size_t data_block_len(data_block_handle_t dataBlock)
{
    if(!dataBlock) return 0;
    return ((data_block_prv_handle_t)dataBlock)->len;
}

void* data_block_data(data_block_handle_t dataBlock)
{
    if(!dataBlock) return NULL;
    return ((data_block_prv_handle_t)dataBlock)->data;
}

bool data_block_destroy(data_block_handle_t dataBlock)
{
    if(!dataBlock) return false;
	util_free(dataBlock);
    return true;
}