/********************************************************************************************************
 * @file    port_config.h
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

#define BASED_ON_TELINK_WIFI_SDK                 1


#if BASED_ON_TELINK_WIFI_SDK
#define PORT_CONFIG_WIFI_SUPPORTED               1
#define PORT_CONFIG_AUDIO_SUPPORTED              1
#define PORT_CONFIG_OPUS_SUPPORTED               1
#define PORT_CONFIG_TFLM_SUPPORTED               1
#define PORT_CONFIG_FS_SUPPORTED                 1
#define PORT_CONFIG_WIRE_SUPPORTED               1
#define PORT_CONFIG_SPI_SUPPORTED                1

#define PORT_CONFIG_USE_D25                      1

#else
#define PORT_CONFIG_WIFI_SUPPORTED               0
#define PORT_CONFIG_AUDIO_SUPPORTED              0
#define PORT_CONFIG_OPUS_SUPPORTED               0
#define PORT_CONFIG_TFLM_SUPPORTED               0
#define PORT_CONFIG_FS_SUPPORTED                 0
#define PORT_CONFIG_WIRE_SUPPORTED               0
#define PORT_CONFIG_SPI_SUPPORTED                0
#endif


#if PORT_CONFIG_WIFI_SUPPORTED
#define PORT_CONFIG_NETWORK_SUPPORTED            1
#define PORT_CONFIG_WEBSOCKET_SUPPORTED          1
#define PORT_CONFIG_WEBSERVER_SUPPORTED          1
#define PORT_CONFIG_MQTT_SUPPORTED               1
#else
#define PORT_CONFIG_NETWORK_SUPPORTED            0
#define PORT_CONFIG_WEBSOCKET_SUPPORTED          0
#define PORT_CONFIG_WEBSERVER_SUPPORTED          0
#define PORT_CONFIG_MQTT_SUPPORTED               0
#endif


#if PORT_CONFIG_AUDIO_SUPPORTED && PORT_CONFIG_OPUS_SUPPORTED && PORT_CONFIG_TFLM_SUPPORTED
#define PORT_CONFIG_VOICE_WAKEUP_SUPPORTED       1
#else
#define PORT_CONFIG_VOICE_WAKEUP_SUPPORTED       0
#endif

