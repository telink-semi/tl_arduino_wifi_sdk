/********************************************************************************************************
 * @file    util_data_block.h
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
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *data_block_handle_t;

data_block_handle_t data_block_create(uint8_t *data, size_t len);
int data_block_write(data_block_handle_t *dataBlock, uint8_t *data, size_t len);
int data_block_read(data_block_handle_t *dataBlock, uint8_t *data, size_t len);
bool data_block_destroy(data_block_handle_t dataBlock);
size_t data_block_len(data_block_handle_t dataBlock);
void * data_block_data(data_block_handle_t dataBlock);


#ifdef __cplusplus
}
#endif