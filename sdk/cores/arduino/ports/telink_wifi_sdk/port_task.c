/********************************************************************************************************
 * @file    port_task.c
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
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include "sdk_common.h"

typedef struct {
    void (*handler)(void *);
    void *param;
    TaskHandle_t handle;
    StaticSemaphore_t exit_sem_buffer;
    SemaphoreHandle_t exit_sem_handle;
    uint16_t run_flag:1;
    uint16_t task_id:15;
    char name[16];
} task_handle_block_t, *task_handle_block_t_ptr;


static uint16_t task_id = 0;
static void new_task(void *param) {
    task_handle_block_t_ptr task = (task_handle_block_t_ptr)param;
    if(task && task->handler && task->exit_sem_handle) {
        while(task->run_flag) {
            task->handler(task->param);
            yield();
        }

        xSemaphoreGive(task->exit_sem_handle);
    }
    vTaskDelete(NULL);
}

task_handle_t port_create_new_task(void (*handler)(void *), void *param, uint32_t stack_depth_bytes) {
    task_handle_block_t_ptr task = (task_handle_block_t_ptr)malloc(sizeof(task_handle_block_t));
    task_handle_t handle = NULL;
    do {
        if(!task) {
            log_e("create_new_task malloc failed!");
            break;
        }
        task->handler = handler;
        task->param = param;
        task->run_flag = 1;
        task->task_id = task_id++;
        snprintf(task->name, sizeof(task->name), "new_task_%d", task->task_id);
        task->exit_sem_handle = xSemaphoreCreateBinaryStatic(&task->exit_sem_buffer);
        if(!task->exit_sem_handle) {
            log_e("create %s exit_sem failed!", task->name);
            break;
        }
        if(xTaskCreate(new_task, task->name, (stack_depth_bytes >> 2), task, APP_TASK_PRIORITY, &task->handle) != pdPASS) {
            log_e("create %s failed!", task->name);
            break;
        }
        log_d("create %s success, handle:%p", task->name, task->handle);
        handle = task;
    } while(0);

    if (!handle && task) {
        free(task);
    }

    return handle;
}

bool port_delete_task(task_handle_t handle) {
    bool ret = false;
    do {
        if (!handle) {
            log_e("port_delete_task handle is NULL!");
            break;
        }

        task_handle_block_t_ptr task = (task_handle_block_t_ptr)handle;
        task->run_flag = 0;
        if(task->exit_sem_handle) {
            xSemaphoreTake(task->exit_sem_handle, portMAX_DELAY);
            log_d("delete_task %s exit_sem give success!", task->name);
        }

        free(task);
        ret = true;

    } while(0);

    return ret;
}


size_t port_get_task_stack_high_water_mark(task_handle_t handle) {
    size_t hwm = 0;
    do {
        if (!handle) {
            log_e("port_get_task_hwm handle is NULL!");
            break;
        }

        task_handle_block_t_ptr task = (task_handle_block_t_ptr)handle;
        hwm = uxTaskGetStackHighWaterMark(task->handle);

    } while(0);

    return hwm;
}


void* port_create_mutex(void) {
    return (void*)xSemaphoreCreateMutex();
}

void port_lock_mutex(void *mutex) {
    (void)mutex;
    if(!mutex) return;
    xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY);
    // log_d("lock_mutex:%p", mutex);
}

void port_unlock_mutex(void *mutex) {
    (void)mutex;
    if(!mutex) return;
    xSemaphoreGive((SemaphoreHandle_t)mutex);
    // log_d("unlock_mutex:%p", mutex);
}

void port_destroy_mutex(void *mutex) {
    (void)mutex;
    if(!mutex) return;
    vSemaphoreDelete((SemaphoreHandle_t)mutex);
}

#endif