/********************************************************************************************************
 * @file    port_main.c
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
#include "core.h"

#define ATEXIT_MAX_FUNC_CNT 3

typedef void (*atexit_func_t)(void);

static uint8_t exitFuncCnt = 0;
static atexit_func_t functions[ATEXIT_MAX_FUNC_CNT];
static uint8_t exitFlag = 0;
static int exitStatus = 0;

int port_atexit(void (*func)(void)) {
    if (exitFuncCnt >= ATEXIT_MAX_FUNC_CNT) {
        return -1;
    }
    functions[exitFuncCnt++] = func;
    return 0;
}

void port_exit(int status) {
    exitFlag = 1;
    exitStatus = status;
    port_background_fastcall_exit();
    vTaskDelete(NULL);
}

__attribute__((weak)) void setup(void) {

}

__attribute__((weak)) void loop(void) {

}

static void call_atexit_funcs(void) {
    for (int i = exitFuncCnt - 1; i >= 0; i--) {
        functions[i]();
    }
}

static void main_task(void *para) {
    (void)para;
    setup();

    for (;;) {
        loop();    
    }

    vTaskDelete(NULL);
}

static void background_task(void *para) {
    (void)para;
    for (;;) {
        if (exitFlag) break;
        port_background_handler_loop();
    }

    vTaskDelete(NULL);
}

static void initCppClass(void) {
    typedef void (*init_func_t)(void);

    extern void *__init_array_start;
    extern void *__init_array_end;
    for (init_func_t *__init = (init_func_t *)&__init_array_start;
         __init < (init_func_t *)&__init_array_end;
         __init++) {
        (*__init)();
    }
}

static void init(void) {
    log_init();
    if(!port_background_init()) {
        log_e("background init failed");
        while(1) {
            yield();
        }
    }
}


int port_main(void)
{
    log("\033[2JTelink W91 Arduino Application, created @time<%s %s>\n", __DATE__, __TIME__);

    exitFlag = 0;
    init();
    if(xTaskCreate(background_task, "background_task", 1024*1, NULL, APP_TASK_PRIORITY, NULL) != pdPASS) {
        log_e("background_task create failed");
        return -1;
    }
    
    initVariant();
    initCppClass();

    if(xTaskCreate(main_task, "arduino_task", 1024*2, NULL, APP_TASK_PRIORITY, NULL) != pdPASS) {
        log_e("arduino_task create failed");
        return -1;
    }

    while(!exitFlag) {
        port_background_fastcall_loop();
    }

    log_i("exitStatus: %d", exitStatus);
    call_atexit_funcs();     
    return 0;
}

#endif