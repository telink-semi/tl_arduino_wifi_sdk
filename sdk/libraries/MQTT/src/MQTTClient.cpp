/********************************************************************************************************
 * @file    MQTTClient.cpp
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
#if PORT_CONFIG_MQTT_SUPPORTED

#include "Arduino.h"
#include "MQTTClient.h"
#include "NetworkCommon.h"
#include "WebSocketCommon.h"

#define MQTT_READ_TIMEOUT_DEFAULT 10

int _mqtt_transport_write_callback(MQTTClient *client, uint8_t *buffer, size_t bytesToSend) {
    if(!client || !buffer || !bytesToSend) return -1;
    return client->write(buffer, bytesToSend);
}

int _mqtt_transport_recv_callback(MQTTClient *client, uint8_t *buffer, size_t bytesToRecv) {
    if(!client || !buffer || !bytesToRecv) return -1;
    return client->read(buffer, bytesToRecv);
}

extern "C" {

int mqtt_transport_write_callback(void *ctx, uint8_t *buffer, size_t bytesToSend) {
    if(!ctx || !buffer || !bytesToSend) return -1;
    port_mqtt_configuration_t config = {0};
    port_mqtt_get_configuration(ctx, &config);
    if(!config.owner) return -1;
    return _mqtt_transport_write_callback((MQTTClient *)config.owner, buffer, bytesToSend);
}

int mqtt_transport_recv_callback(void *ctx, uint8_t *buffer, size_t bytesToRecv) {
    if(!ctx || !buffer || !bytesToRecv) return -1;
    port_mqtt_configuration_t config = {0};
    port_mqtt_get_configuration(ctx, &config);
    if(!config.owner) return -1;
    return _mqtt_transport_recv_callback((MQTTClient *)config.owner, buffer, bytesToRecv);
}

void mqtt_transport_stop_callback(void *ctx) {
    if(!ctx) return;
    port_mqtt_configuration_t config = {0};
    port_mqtt_get_configuration(ctx, &config);
    if(!config.owner) return;
    MQTTClient *pMqttClient = (MQTTClient *)config.owner;
    pMqttClient->stop();
}

}

/**  
* @brief      This function serves to initialize an MQTTClient instance.  
* @param[in]  timeout_ms - The default connection timeout in milliseconds  
* @param[in]  publish_max_count - Maximum number of messages that can be published  
* @param[in]  subscribe_max_count - Maximum number of subscriptions  
* @param[in]  buffer_size - Size of the buffer for network data  
* @param[in]  name - Name of the MQTT client  
* @return     none 
*/
MQTTClient::MQTTClient() : timeout_ms(MQTT_CONNECT_TIMEOUT_DEFAULT), publish_max_count(PUBLISH_MAX_COUNT_DEFAULT), 
                           subscribe_max_count(SUBSCRIBE_MAX_COUNT_DEFAULT), buffer_size(MQTT_FIXED_BUFFER_SIZE_DEFAULT), 
                           transport_handle(NULL), transport_type(MQTT_TRANSPORT_TYPE_NONE), 
                           name(MQTT_NAME_DEFAULT), mqtt_context(NULL) {
    mqtt_context = port_mqtt_create_context(this);
    transport_handle = NULL;
}

MQTTClient::~MQTTClient() {
    stop();
    port_mqtt_destroy_context(mqtt_context);
}

/**  
* @brief      This function serves to write data through the transport layer.  
* @param[in]  buffer - Pointer to the buffer containing the data to be written  
* @param[in]  bytesToSend - Number of bytes to send  
* @return     The number of bytes actually sent, or -1 if an error occurred  
*/
int MQTTClient::write(uint8_t *buffer, size_t bytesToSend) {
    if(!buffer || !bytesToSend || !transport_handle) return -1;
    if(transport_type == MQTT_TRANSPORT_TYPE_TCP) {
        NetworkClient *pNetworkClient = (NetworkClient *)transport_handle;
        if(!pNetworkClient->connected()) {
            return -1;
        }
        return pNetworkClient->write(buffer, bytesToSend);
    } else if(transport_type == MQTT_TRANSPORT_TYPE_WS) {
        WebSocketClient *pWebSocketClient = (WebSocketClient *)transport_handle;
        if(!pWebSocketClient->connected()) {
            return -1;
        }
        return pWebSocketClient->write(buffer, bytesToSend);
    }
    return -1;
}

/**  
* @brief      This function serves to read data through the transport layer.  
* @param[in]  buffer - Pointer to the buffer where the received data will be stored  
* @param[in]  bytesToRecv - Number of bytes to receive  
* @return     The number of bytes actually received, or -1 if an error occurred  
*/
int MQTTClient::read(uint8_t *buffer, size_t bytesToRecv) {
    if(!buffer || !bytesToRecv || !transport_handle) return -1;
    uint32_t startTime = millis();
    if(transport_type == MQTT_TRANSPORT_TYPE_TCP) {
        NetworkClient *pNetworkClient = (NetworkClient *)transport_handle;
        if(!pNetworkClient->connected()) {
            return -1;
        }
        while(pNetworkClient->available() == 0 && millis() - startTime < MQTT_READ_TIMEOUT_DEFAULT) {
            yield();
        }
        return pNetworkClient->read(buffer, bytesToRecv);
    } else if(transport_type == MQTT_TRANSPORT_TYPE_WS) {
        WebSocketClient *pWebSocketClient = (WebSocketClient *)transport_handle;
        if(!pWebSocketClient->connected()) {
            log_e("websocket not connected");
            return -1;
        }
        while(pWebSocketClient->available() == 0 && millis() - startTime < MQTT_READ_TIMEOUT_DEFAULT) {
            yield();
        }
        return pWebSocketClient->read(buffer, bytesToRecv);
    }
    return -1;
}

/**  
* @brief      This function serves to set the connection timeout for MQTT client.  
* @param[in]  timeout_ms - The new timeout value in milliseconds  
* @return     none 
*/
void MQTTClient::setTimeout(uint32_t timeout_ms) {
    this->timeout_ms = timeout_ms;
}

/**  
* @brief      This function serves to set the maximum number of publish messages.  
* @param[in]  publish_max_count - The new maximum count  
* @return     none 
*/
void MQTTClient::setPublishMaxCount(uint16_t publish_max_count){
    this->publish_max_count = publish_max_count;
}

/**  
* @brief      This function serves to set the maximum number of subscriptions.  
* @param[in]  subscribe_max_count - The new maximum count  
* @return     none 
*/
void MQTTClient::setSubscribeMaxCount(uint16_t subscribe_max_count) {
    this->subscribe_max_count = subscribe_max_count;
}

/**  
* @brief      This function serves to set the name of the MQTT client.  
* @param[in]  name - The new name  
* @return     none 
*/
void MQTTClient::setName(const char *name) {
    this->name = name;
}

/**  
* @brief      This function serves to connect to an MQTT server using a network address and port.  
* @param[in]  ip - Network IP address of the server  
* @param[in]  port - Port number of the server  
* @param[in]  path - Path or endpoint for the connection  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(IPAddress ip, uint16_t port, const char *path) {
    return connect(ip, port, path, timeout_ms);
}

/**  
* @brief      This function serves to connect to an MQTT server using a network address and port with a specified timeout.  
* @param[in]  ip - Network IP address of the server  
* @param[in]  port - Port number of the server  
* @param[in]  path - Path or endpoint for the connection  
* @param[in]  timeout_ms - Connection timeout in milliseconds  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(IPAddress ip, uint16_t port, const char *path, uint32_t timeout_ms) {
    if(!port || !path) return false;
    this->timeout_ms = timeout_ms;
    transport_type = MQTT_TRANSPORT_TYPE_WS;
    transport_handle = (void *)new WebSocketClient();
    if(!transport_handle) return false;
    WebSocketClient *pWebSocketClient = (WebSocketClient *)transport_handle;
    pWebSocketClient->setBufferSize(buffer_size, buffer_size);
    if(!pWebSocketClient->connect(ip, port, path, timeout_ms)) {
        log_e("websocket connect failed");
        delete pWebSocketClient;
        transport_handle = NULL;
        return false;
    }
    return port_mqtt_connect(mqtt_context, name.c_str(), buffer_size, publish_max_count, subscribe_max_count, timeout_ms);
}

/**  
* @brief      This function serves to connect to an MQTT server using a hostname and port.  
* @param[in]  host - Hostname of the server  
* @param[in]  port - Port number of the server  
* @param[in]  path - Path or endpoint for the connection  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(const char *host, uint16_t port, const char *path) {
    IPAddress srv(port_network_get_host_by_name(host));
    return connect(srv, port, path);
}

/**  
* @brief      This function serves to connect to an MQTT server using a network address and timeout.  
* @param[in]  ip - Network IP address of the server  
* @param[in]  port - Port number of the server  
* @param[in]  timeout_ms - Connection timeout in milliseconds  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(IPAddress ip, uint16_t port, uint32_t timeout_ms) {
    if(!port) return false;
    this->timeout_ms = timeout_ms;
    transport_type = MQTT_TRANSPORT_TYPE_TCP;
    transport_handle = (void *)new NetworkClient();
    NetworkClient *pNetworkClient = (NetworkClient *)transport_handle;
    pNetworkClient->setBufferSize(buffer_size, buffer_size);
    if(!pNetworkClient->connect(ip, port, timeout_ms)) {
        delete pNetworkClient;
        transport_handle = NULL;
        return false;
    }
    return port_mqtt_connect(mqtt_context, name.c_str(), buffer_size, publish_max_count, subscribe_max_count, timeout_ms);
}

/**  
* @brief      This function serves to connect to an MQTT server using a network address.  
* @param[in]  ip - Network IP address of the server  
* @param[in]  port - Port number of the server  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(IPAddress ip, uint16_t port) {
    return connect(ip, port, timeout_ms);
}

/**  
* @brief      This function serves to connect to an MQTT server using a hostname.  
* @param[in]  host - Hostname of the server  
* @param[in]  port - Port number of the server  
* @return     true if connection successful, false otherwise 
*/
bool MQTTClient::connect(const char *host, uint16_t port) {
    IPAddress srv(port_network_get_host_by_name(host));
    return connect(srv, port);
}

/**  
* @brief      This function serves to check if the client is connected to an MQTT server.  
* @return     true if connected, false otherwise 
*/
bool MQTTClient::connected() {
    return transport_handle != NULL && mqtt_context != NULL;
}

/**  
* @brief      This function serves to stop the connection and clean up resources for the MQTT client.  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::stop() {
    if(!connected()) return false;
    port_mqtt_close(mqtt_context);

    if(transport_type == MQTT_TRANSPORT_TYPE_TCP) {
        NetworkClient *pNetworkClient = (NetworkClient *)transport_handle;
        pNetworkClient->stop();
        delete pNetworkClient;
    } else if(transport_type == MQTT_TRANSPORT_TYPE_WS) {
        WebSocketClient *pWebSocketClient = (WebSocketClient *)transport_handle;
        pWebSocketClient->stop();
        delete pWebSocketClient;
    }
    transport_handle = NULL;
    transport_type = MQTT_TRANSPORT_TYPE_NONE;

    return true;
}

/**  
* @brief      This function serves to publish a message to an MQTT topic.  
* @param[in]  topic - The topic name  
* @param[in]  payload - Pointer to the payload data  
* @param[in]  payload_len - Length of the payload  
* @param[in]  qos - Quality of Service level  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::publish(const char *topic, uint8_t *payload, size_t payload_len, arduino_mqtt_qos_t qos) {
    if(!connected()) return false;
    return port_mqtt_publish(mqtt_context, topic, payload, payload_len, qos);
}

/**  
* @brief      This function serves to publish a message to an MQTT topic with a string payload.  
* @param[in]  topic - The topic name  
* @param[in]  payload - Pointer to the string payload data  
* @param[in]  qos - Quality of Service level  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::publish(const char *topic, const char *payload, arduino_mqtt_qos_t qos) {
    if(!connected()) return false;
    return port_mqtt_publish(mqtt_context, topic, (uint8_t *)payload, strlen(payload), qos);
}

/**  
* @brief      This function serves to publish a message to an MQTT topic with a String payload.  
* @param[in]  topic - The topic name  
* @param[in]  payload - Reference to the String payload data  
* @param[in]  qos - Quality of Service level  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::publish(const char *topic, String payload, arduino_mqtt_qos_t qos) {
    if(!connected()) return false;
    return port_mqtt_publish(mqtt_context, topic, (uint8_t *)payload.c_str(), payload.length(), qos);
}

/**  
* @brief      This function serves to subscribe to an MQTT topic.  
* @param[in]  topic - The topic name  
* @param[in]  callback - Callback function for received messages  
* @param[in]  qos - Quality of Service level  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::subscribe(const char *topic, mqtt_subscribe_callback_func callback, arduino_mqtt_qos_t qos) {
    if(!connected()) return false;
    return port_mqtt_subscribe(mqtt_context, topic, callback, qos);
}

/**  
* @brief      This function serves to unsubscribe from an MQTT topic.  
* @param[in]  topic - The topic name  
* @param[in]  qos - Quality of Service level  
* @return     true if successful, false otherwise 
*/
bool MQTTClient::unsubscribe(const char *topic, arduino_mqtt_qos_t qos) {
    if(!connected()) return false;
    return port_mqtt_unsubscribe(mqtt_context, topic, qos);
}

#endif