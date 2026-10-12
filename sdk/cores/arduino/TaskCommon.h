/********************************************************************************************************
 * @file    TaskCommon.h
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
#pragma once

#include <inttypes.h>
#include "Arduino.h"

typedef void * taskHandle_t;
typedef void * mutexHandle_t;

class TaskClass {
public:
  TaskClass() {};
  ~TaskClass() {};
  static taskHandle_t createTask(void (*handler)(void *), void * para, bool newTask=false, uint32_t stackDepthBytes=4096);
  static bool deleteTask(taskHandle_t handle);
  static mutexHandle_t createMutex(void);
  static void deleteMutex(mutexHandle_t handle);
  static void lockMutex(mutexHandle_t handle);
  static void unlockMutex(mutexHandle_t handle);
};

extern TaskClass Task;
