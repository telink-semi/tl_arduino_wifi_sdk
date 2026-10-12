/*******************************************************************************************************
 *
 * @file    proc.h
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
 
#ifndef __WITS_PROC_H__
#define __WITS_PROC_H__

#include <FreeRTOS/FreeRTOS.h>
#include <FreeRTOS/task.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef TaskHandle_t thread_t;
typedef TaskStatus_t threadinfo_t;

void iterate_task(void (*callback)(threadinfo_t *, uint32_t *, void *), uint32_t *jiffies, void *arg);
thread_t find_task_by_pid(int pid);

static inline thread_t find_task_by_name(const char *name)
{
	return xTaskGetHandle(name);
};

static inline int task_pid(thread_t th)
{
	threadinfo_t tsk;
	vTaskGetTaskInfo(th, &tsk, pdFALSE, eRunning);
	return tsk.xTaskNumber;
}

static inline const char *task_name(thread_t th)
{
	threadinfo_t tsk;
	vTaskGetTaskInfo(th, &tsk, pdFALSE, eRunning);
	return tsk.pcTaskName;
}

int task_is_freezable(thread_t task);
int task_suspend(int pid);
int task_resume(int pid);

#ifdef __cplusplus
}
#endif

#endif /* __WITS_PROC_H__ */
