/********************************************************************************************************
 * @file    MQTTClient.h
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

#include "port_config.h"
#if PORT_CONFIG_MQTT_SUPPORTED

#include <inttypes.h>
#include <IPAddress.h>
#include "port_interface.h"

#define PUBLISH_MAX_COUNT_DEFAULT 30
#define SUBSCRIBE_MAX_COUNT_DEFAULT 30
#define MQTT_CONNECT_TIMEOUT_DEFAULT 10000
#define MQTT_NAME_DEFAULT "Telink Arduino"
#define MQTT_FIXED_BUFFER_SIZE_DEFAULT 1024

class MQTTClient
{
protected:
    uint32_t timeout_ms;
    uint16_t publish_max_count;
    uint16_t subscribe_max_count;
    uint16_t buffer_size;
    void *transport_handle;
    uint8_t transport_type;
    String name;
    int write(uint8_t *buffer, size_t bytesToSend);
    int read(uint8_t *buffer, size_t bytesToRecv);
public:
    void *mqtt_context;
    MQTTClient();
    ~MQTTClient();
    void setTimeout(uint32_t timeout_ms);
    void setBufferSize(uint16_t size) {this->buffer_size = size;}
    void setPublishMaxCount(uint16_t publish_max_count);
    void setSubscribeMaxCount(uint16_t subscribe_max_count);
    void setName(const char *name);
    bool connect(IPAddress ip, uint16_t port, const char *path);
    bool connect(IPAddress ip, uint16_t port, const char *path, uint32_t timeout_ms);
    bool connect(const char *host, uint16_t port, const char *path);
    bool connect(IPAddress ip, uint16_t port);
    bool connect(IPAddress ip, uint16_t port, uint32_t timeout_ms);
    bool connect(const char *host, uint16_t port);
    bool connected();
    bool stop();
    bool publish(const char *topic, uint8_t *payload, size_t payload_len, arduino_mqtt_qos_t qos=ARDUINO_MQTT_QOS0);
    bool publish(const char *topic, const char *payload, arduino_mqtt_qos_t qos=ARDUINO_MQTT_QOS0);
    bool publish(const char *topic, String payload, arduino_mqtt_qos_t qos=ARDUINO_MQTT_QOS0);
    bool subscribe(const char *topic, mqtt_subscribe_callback_func callback, arduino_mqtt_qos_t qos=ARDUINO_MQTT_QOS0);
    bool unsubscribe(const char *topic, arduino_mqtt_qos_t qos=ARDUINO_MQTT_QOS0);
    friend int _mqtt_transport_write_callback(MQTTClient *client, uint8_t *buffer, size_t bytesToSend);
    friend int _mqtt_transport_recv_callback(MQTTClient *client, uint8_t *buffer, size_t bytesToRecv);
};

#endif