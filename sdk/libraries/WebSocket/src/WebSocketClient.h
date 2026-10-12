/********************************************************************************************************
 * @file    WebSocketClient.h
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
#if PORT_CONFIG_WEBSOCKET_SUPPORTED

#include "Client.h"
#include "port_interface.h"

class WebSocketClient : public Client {
protected:
  String headers;
  ring_buffer_handle_t h_rx_buffer;
  ring_buffer_handle_t h_tx_buffer;
  uint32_t read_buffer_size;
  uint32_t write_buffer_size;
  int read_process();
  int write_process();
public:
  void *ws_context;
  WebSocketClient();
  ~WebSocketClient();
  void setBufferSize(uint32_t r_buffer_size=1024, uint32_t w_buffer_size=1024);
  void setConnectionTimeout(uint32_t milliseconds) {
    port_websocket_configuration_t config = {0};
    config.timeout_ms = milliseconds;
    config.timeout_ms_valid = 1;
    port_websocket_set_configuration(ws_context, &config);
  }
  void setReconnectTime(uint32_t seconds) {
    port_websocket_configuration_t config = {0};
    config.recon_time_ms = seconds * 1000;
    config.recon_time_ms_valid = 1;
    port_websocket_set_configuration(ws_context, &config);
  }
  using Client::connect;
  void addHeader(String key, String value);
  int connect(IPAddress ip, uint16_t port);
  int connect(IPAddress ip, uint16_t port, const char *path, int32_t timeout_ms);
  int connect(const char *host, uint16_t port);
  int connect(const char *host, uint16_t port, const char *path);
  size_t write(uint8_t data);
  size_t write(const uint8_t *buf, size_t size);
  size_t write_P(PGM_P buf, size_t size);
  size_t write(Stream &stream);
  void flush(); 
  int available();
  int availableForWrite();
  int read();
  int read(uint8_t *buf, size_t size);
  size_t readBytes(char *buffer, size_t length);
  size_t readBytes(uint8_t *buffer, size_t length) {
    return readBytes((char *)buffer, length);
  }
  int peek();
  void clear();  // clear rx
  void stop();
  uint8_t connected();

  operator bool() {
    return connected();
  }
  bool operator==(const bool value) {
    return bool() == value;
  }
  bool operator!=(const bool value) {
    return bool() != value;
  }
  bool operator==(const WebSocketClient &);
  bool operator!=(const WebSocketClient &rhs) {
    return !this->operator==(rhs);
  };

  using Print::write;
  friend int _websocket_task_read_callback(WebSocketClient *ws);
  friend int _websocket_task_write_callback(WebSocketClient *ws);
};

#endif