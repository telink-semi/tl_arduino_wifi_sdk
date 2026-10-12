/*******************************************************************************************************
 *
 * @file    vqueue.h
 *
 * @brief   This is the source file for TLSR9118
 *
 * @author  
 * @par     Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * @author  
 * @par     Copyright 2023-2025 Senscomm Semiconductor Co., Ltd.
 * 
 * @author  Telink WIFI GROUP
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 
#ifndef __VQUEUE_H__
#define __VQUEUE_H__

#include <stdint.h>
#include <freebsd/errors.h>
#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

int vqueue(int count, int size);
int vqueue_put(int fd, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
int vqueue_get(int fd, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
int vqueue_capacity(int fd);
int vqueue_msg_size(int fd);
int vqueue_count(int fd);
int vqueue_space(int fd);

#ifdef __cplusplus
}
#endif

#endif /* __VQUEUE_H__ */
