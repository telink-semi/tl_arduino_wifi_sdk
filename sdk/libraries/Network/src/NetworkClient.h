/********************************************************************************************************
 * @file    NetworkClient.h
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
#if PORT_CONFIG_NETWORK_SUPPORTED

#include "Client.h"
#include "port_interface.h"

class NetworkClient : public Client {
protected:
  ring_buffer_handle_t h_rx_buffer;
  ring_buffer_handle_t h_tx_buffer;
  uint32_t rx_buffer_size;
  uint32_t tx_buffer_size;
public:
  void *tcp_context;
  NetworkClient *next;
  NetworkClient();
  ~NetworkClient();

  int read_process();
  int write_process();
  void setBufferSize(uint32_t rx_buffer_size=1024, uint32_t tx_buffer_size=1024);
  void setConnectionTimeout(uint32_t milliseconds);
  using Client::connect;
  int connect(IPAddress ip, uint16_t port);
  int connect(IPAddress ip, uint16_t port, int32_t timeout_ms);
  int connect(const char *host, uint16_t port);
  int connect(const char *host, uint16_t port, int32_t timeout_ms);
  size_t write(uint8_t data);
  size_t write(const uint8_t *buf, size_t size);
  size_t write_P(PGM_P buf, size_t size);
  size_t write(Stream &stream);
  void flush(); 
  int available();
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
  bool operator==(const NetworkClient &);
  bool operator!=(const NetworkClient &rhs) {
    return !this->operator==(rhs);
  };

  IPAddress remoteIP() const;
  uint16_t remotePort() const;
  IPAddress localIP() const;
  uint16_t localPort() const;

  //friend class NetworkServer;
  using Print::write;
};

#endif