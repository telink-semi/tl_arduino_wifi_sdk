/********************************************************************************************************
 * @file    sdk_common.h
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

#ifndef __cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <hal/unaligned.h>
#include <hal/kernel.h>
#include <hal/wlan.h>
#include <hal/kmem.h>
#include "kernel.h"
#include "compat_if.h"
#include "if_media.h"
#include "sncmu_d11.h"
#include "fwil_types.h"
#include "fweh.h"
#include "scdc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <net80211/ieee80211_var.h>
#include <wits_err.h>
#include <wits_log.h>
#include <wits_wifi.h>
#include <wits_event_loop.h>
#include <common.h>
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "core_mqtt.h"
#include "tlsr_wifi.h"
#include "tlsr_timer.h"
#include "tlsr_gpio.h"
#include "tlsr_crypto.h"
#include "tlsr_uart.h"
#include "FreeRTOS_tick_config.h"
#include "semphr.h"
#include "pinctrl.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include <hal/spi-flash.h>

#endif /* __cplusplus */


#define millis() port_millis()
#define yield() port_yield()
#define micros() port_micros()
#define delay_ms(ms) port_delay_ms(ms)
#define delay_us(us) port_delay_us(us)
#define lock_mutex(mutex) port_lock_mutex(mutex)
#define unlock_mutex(mutex) port_unlock_mutex(mutex)
#define create_mutex() port_create_mutex()
#define destroy_mutex(mutex) port_destroy_mutex(mutex)