/********************************************************************************************************
 * @file    Task.cpp
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
#include "System.h"
#include "port_interface.h"
#include "TaskCommon.h"
#include <stdlib.h>
#include "port_interface.h"

enum {
    TASK_BACKGROUND = 0,
    TASK_NEW = 1,
};

typedef struct {
    size_t type;
    void *handle;
}taskHandleBlock_t, *taskHandleBlock_t_ptr;

taskHandle_t TaskClass::createTask(void (*handler)(void *), void * para, bool newTask, uint32_t stackDepthBytes) {
    taskHandle_t handle = NULL;
    taskHandleBlock_t_ptr block = reinterpret_cast<taskHandleBlock_t_ptr>(malloc(sizeof(taskHandleBlock_t)));
    do {   
        if(!block) {
            break;
        }

        block->type = newTask ? TASK_NEW : TASK_BACKGROUND;
        if(newTask) {
            block->handle = port_create_new_task(handler, para, stackDepthBytes);
        } else {
            block->handle = port_background_register_handler(handler, para, 0);
        }
        if(!block->handle) {
            break;
        }

        handle = reinterpret_cast<taskHandle_t>(block);

    } while(0);

    if (block && !handle) { 
        free(block);
    }

    return handle;
}

bool TaskClass::deleteTask(taskHandle_t handle) {
    bool ret = false;
    do {
        taskHandleBlock_t_ptr block = reinterpret_cast<taskHandleBlock_t_ptr>(handle);
        if(!block) {
            break;
        }

        if(block->type == TASK_BACKGROUND) {
            ret = port_background_unregister_handler(reinterpret_cast<background_handle_t>(block->handle));
        } else if(block->type == TASK_NEW) {
            ret = port_delete_task(reinterpret_cast<task_handle_t>(block->handle));
        } 

        free(block);
    } while(0);

    return ret;
}

mutexHandle_t TaskClass::createMutex(void) {
    return reinterpret_cast<mutexHandle_t>(port_create_mutex());
}

void TaskClass::deleteMutex(mutexHandle_t handle) {
     port_destroy_mutex(reinterpret_cast<void*>(handle));
}

void TaskClass::lockMutex(mutexHandle_t handle) {
     port_lock_mutex(reinterpret_cast<void *>(handle));
}

void TaskClass::unlockMutex(mutexHandle_t handle) {
     port_unlock_mutex(reinterpret_cast<void *>(handle));
}
