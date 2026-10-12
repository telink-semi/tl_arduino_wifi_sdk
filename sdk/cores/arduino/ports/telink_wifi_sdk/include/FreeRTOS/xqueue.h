/*******************************************************************************************************
 *
 * @file    xqueue.h
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
 
/*
 * xqueue.h - FreeRTOS queue extension
 */

#ifndef __XQUEUE_H__
#define __XQUEUE_H__

#include <FreeRTOS/queue.h>

UBaseType_t uxQueueSpacesAvailableFromISR(const QueueHandle_t xQueue);
UBaseType_t uxQueueGetNumberOfTasksWaitingToReceive(const QueueHandle_t xQueue);
UBaseType_t uxQueueGetNumberOfTasksWaitingToReceiveFromISR(const QueueHandle_t xQueue);

BaseType_t queue_insert_set(QueueSetMemberHandle_t xQueueOrSemaphore,
			    QueueSetHandle_t xQueueSet);

BaseType_t queue_remove_set(QueueSetMemberHandle_t xQueueOrSemaphore,
			    QueueSetHandle_t xQueueSet);
#endif /* __XQUEUE_H__ */
