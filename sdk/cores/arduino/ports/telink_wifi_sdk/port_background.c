/********************************************************************************************************
 * @file    port_background.c
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

extern unsigned int millis(void);

// For interrupt handler
typedef struct {
    background_handler_func handler;  // Pointer to the background handler function
    void *para;                      // Parameters to pass to the handler function
} irq_prv_handle_t;

static QueueHandle_t irqQueue = NULL;  // Interrupt queue handle

// For background handler
typedef struct {
    background_handler_func handler;  // Pointer to the background handler function
    void *para;                      // Parameters to pass to the handler function
    uint32_t refTime;                  // Last execution timestamp
    uint32_t intervalMS;               // Execution interval (in milliseconds)
} background_handle_block_t, *background_prv_handle_t;

static list_handle_t listHandle = NULL;  // Background handler list handle
static size_t bgHandlerIndex = 0;        // Current background handler index

/**
 * @brief Initializes the background handler system.
 * @return True if initialization is successful, false otherwise.
 */
bool port_background_init(void) {
    listHandle = list_create();
    irqQueue = xQueueCreate(10, sizeof(irq_prv_handle_t));
    return listHandle != NULL && irqQueue != NULL;
}

/**
 * @brief Compares two background handler blocks for equality.
 * @param handle1 - Pointer to the first background handler block
 * @param handle2 - Pointer to the second background handler block
 * @return True if the handles are equal, false otherwise
 */
static bool background_compare(const void *handle1, const void *handle2) {
    if(!handle1 || !handle2) return false;
    background_prv_handle_t bgHandle1 = (background_prv_handle_t)handle1;
    background_prv_handle_t bgHandle2 = (background_prv_handle_t)handle2;
    return (bgHandle1->handler == bgHandle2->handler) && 
           (bgHandle1->para == bgHandle2->para);
}

/**
 * @brief Frees the memory allocated for a background handler block.
 * @param data - Pointer to the data to be freed
 */
static void background_free(void *data) {
    util_free(data);
}

/**
 * @brief Registers a new background handler function with specified parameters.
 * @param handler - The background handler function to register
 * @param para - Parameters to pass to the handler function
 * @param intervalMS - Interval at which the handler should be executed (in milliseconds)
 * @return A handle to the registered background handler, or NULL if registration fails
 */
background_handle_t port_background_register_handler(background_handler_func handler, void *para, uint32_t intervalMS) {
    background_handle_t handle = NULL;
    background_prv_handle_t bgHandle = NULL;
    do {
        if(!handler) break;
        bgHandle = (background_prv_handle_t)malloc(sizeof(background_handle_block_t));
        if(!bgHandle) break;
        bgHandle->handler = handler;
        bgHandle->para = para;
        bgHandle->refTime = millis();
        bgHandle->intervalMS = intervalMS;
        if(!list_push_back(listHandle, bgHandle)) break;
        handle = bgHandle;
    } while(0);

    if(handle == NULL) {
        if(bgHandle) {
            free(bgHandle);
        }
    }
    return handle;
}

/**
 * @brief Unregisters an existing background handler function.
 * @param handle - The handle of the background handler to unregister
 * @return True if the unregistration is successful, false otherwise
 */
bool port_background_unregister_handler(background_handle_t handle) {
    return list_remove(listHandle, handle, background_compare, background_free);
}

/**
 * @brief Main loop for executing registered background handlers.
 * This function iterates through the list of background handlers and executes them based on their interval.
 */
void port_background_handler_loop(void) {
    if(bgHandlerIndex >= list_size(listHandle)) bgHandlerIndex = 0;
    background_prv_handle_t bgHandle = (background_prv_handle_t)list_get(listHandle, bgHandlerIndex++);
    if(bgHandle) {
        if(!bgHandle->intervalMS || (millis() - bgHandle->refTime >= bgHandle->intervalMS)) {
            bgHandle->handler(bgHandle->para);
            bgHandle->refTime = millis();
        }
    }
}

/**
 * @brief Main loop for handling interrupts and executing registered handlers.
 */
void port_background_fastcall_loop(void) {
    if(!irqQueue) return;
    irq_prv_handle_t irqHandle;
    if(xQueueReceive(irqQueue, &irqHandle, portMAX_DELAY)) {
        if(irqHandle.handler) {
            irqHandle.handler(irqHandle.para);
        } else {
            if(!irqHandle.para) {
                vQueueDelete(irqQueue);
                irqQueue = NULL;
            }
        }
    }
}

/**
 * @brief Handles an interrupt by adding it to the interrupt queue.
 * This function should be called from an interrupt handler.
 * @param handler - The interrupt handler function
 * @param para - Parameters to pass to the handler function
 */
void port_background_fastcall_handle(background_handler_func handler, void *para) {
    if(!handler) return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    irq_prv_handle_t irqHandle;
    irqHandle.handler = handler;
    irqHandle.para = para;
    xQueueSendFromISR(irqQueue, &irqHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Exits the background fastcall loop.
 * This function is used to signal that the interrupt handling is complete.
 */
void port_background_fastcall_exit(void) {
    irq_prv_handle_t irqHandle = {NULL, NULL};
    xQueueSend(irqQueue, &irqHandle, portMAX_DELAY);
}

#endif