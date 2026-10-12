/********************************************************************************************************
 * @file    NetworkClient.cpp
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
#if PORT_CONFIG_NETWORK_SUPPORTED

#include "NetworkClient.h"
#include "WiFiCommon.h"
#include "Arduino.h"
#include <errno.h>

#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) ((((__const uint32_t *)(a))[0] == 0) && (((__const uint32_t *)(a))[1] == 0) && (((__const uint32_t *)(a))[2] == htonl(0xffff)))
#endif

#define WIFI_CLIENT_DEF_CONN_TIMEOUT_MS (3000)
#define WIFI_CLIENT_MAX_WRITE_RETRY     (10)
#define WIFI_CLIENT_SELECT_TIMEOUT_US   (1000000)
#define WIFI_CLIENT_FLUSH_BUFFER_SIZE   (1024)
#define WIFI_CLIENT_READ_BUFFER_SIZE   (1024)
#define WIFI_CLIENT_RECON_TIMEOUT_MS    (5000)

#undef connect
#undef write
#undef read


extern "C" {

int tcp_task_read_callback(void *para) {
    if(!para) return -1;
    port_tcp_configuration_t config = {0};
    port_tcp_get_configuration(para, &config);
    if(!config.owner) return -1;
    NetworkClient *pNetworkClient = (NetworkClient *)config.owner;
    return pNetworkClient->read_process();
}

int tcp_task_write_callback(void *para) {
    if(!para) return -1;
    port_tcp_configuration_t config = {0};
    port_tcp_get_configuration(para, &config);
    if(!config.owner) return -1;
    NetworkClient *pNetworkClient = (NetworkClient *)config.owner;
    return pNetworkClient->write_process();
}

void tcp_task_stop_callback(void *para) {
    if(!para) return;
    port_tcp_configuration_t config = {0};
    port_tcp_get_configuration(para, &config);
    if(!config.owner) return;
    NetworkClient *pNetworkClient = (NetworkClient *)config.owner;
    pNetworkClient->stop();
}

}

NetworkClient::NetworkClient() : h_rx_buffer(NULL), h_tx_buffer(NULL),rx_buffer_size(WIFI_CLIENT_READ_BUFFER_SIZE),  tx_buffer_size(WIFI_CLIENT_FLUSH_BUFFER_SIZE), tcp_context(NULL), next(NULL) {
    tcp_context = port_tcp_create_context(this);
    port_tcp_configuration_t config = {0};
    config.recon_time_ms = WIFI_CLIENT_RECON_TIMEOUT_MS;
    config.recon_time_ms_valid = 1;
    config.timeout_ms = WIFI_CLIENT_DEF_CONN_TIMEOUT_MS;
    config.timeout_ms_valid = 1;
    port_tcp_set_configuration(tcp_context, &config);
}

NetworkClient::~NetworkClient() {
    if(connected()) {
      stop();
    }
    port_tcp_destroy_context(tcp_context);
}

void NetworkClient::setBufferSize(uint32_t rx_buffer_size, uint32_t tx_buffer_size) {
  this->rx_buffer_size = rx_buffer_size;
  this->tx_buffer_size = tx_buffer_size;
}

void NetworkClient::stop() {
   port_tcp_close(tcp_context);

  if(h_rx_buffer) {
    ring_buffer_destroy(h_rx_buffer);
    h_rx_buffer = NULL;
  }

  if(h_tx_buffer) {
    ring_buffer_destroy(h_tx_buffer);
    h_tx_buffer = NULL;
  }
}

int NetworkClient::read_process() {
    uint8_t data[512];
    if(h_rx_buffer && connected()) {
      while(true) {
        int len = ring_buffer_get_free_size(h_rx_buffer);
        if(len == 0) break;
        len =  port_tcp_read(tcp_context, data, sizeof(data));
        if(len == 0) {
          break;
        } else if(len > 0) {
          ring_buffer_push(h_rx_buffer, data, len);
        } else if(len < 0) {
          log_e("NetworkClient::read_process: read failed -> disconnect");
          return -1;
        }
      }
    }
    return 0;
}

int NetworkClient::write_process() {
    uint8_t data[512];
    if(h_tx_buffer && connected()) {
        while(true) {
            int len = ring_buffer_available(h_tx_buffer);
            if(len == 0) break;
            len = ring_buffer_copy(h_tx_buffer, data, sizeof(data));
            len =  port_tcp_write(tcp_context, data, len);
            if(len == 0) {
              break;
            } else if(len > 0) {
                ring_buffer_pop(h_tx_buffer, len);
            } else if(len < 0) {
                log_e("NetworkClient::write_process: write failed -> disconnect");
                return -1;
            }   
        }
    }
    return 0;
}

int NetworkClient::connect(IPAddress ip, uint16_t port) {
  return connect(ip, port, _timeout);
}

int NetworkClient::connect(IPAddress ip, uint16_t port, int32_t timeout_ms) {
  if( port_tcp_is_opened(tcp_context)) return 0;
  port_tcp_configuration_t config = {0};

  ip.to_ip_addr_t(&config.ip);
  config.ip_valid = 1;
  config.port = port;
  config.port_valid = 1;
  config.timeout_ms = timeout_ms;
  config.timeout_ms_valid = 1;

  port_tcp_set_configuration(tcp_context, &config);

  int ret = -1;
  ring_buffer_handle_t rx_buffer = NULL;
  ring_buffer_handle_t tx_buffer = NULL;
  do {
    rx_buffer = ring_buffer_create(rx_buffer_size);
    tx_buffer = ring_buffer_create(tx_buffer_size);

    if(!rx_buffer || !tx_buffer) {
      log_e("NetworkClient connect failed: ring_buffer_create failed");
      break;
    }
    h_rx_buffer = rx_buffer;
    h_tx_buffer = tx_buffer;

    if(! port_tcp_connect(tcp_context)) break;

    ret = 1;
  }while(0);

  if(ret < 0) {
    if(rx_buffer) {
      ring_buffer_destroy(rx_buffer);
    }
    if(tx_buffer) {
      ring_buffer_destroy(tx_buffer);
    }
  }

  return ret;
}

int NetworkClient::connect(const char *host, uint16_t port) {
  return connect(host, port, _timeout);
}

int NetworkClient::connect(const char *host, uint16_t port, int32_t timeout_ms) {
  IPAddress srv(port_network_get_host_by_name(host));
  return connect(srv, port, timeout_ms);
}

void NetworkClient::setConnectionTimeout(uint32_t milliseconds) {
  _timeout = milliseconds;
}

size_t NetworkClient::write(uint8_t data) {
  if(tcp_context && h_tx_buffer) {
    if(ring_buffer_get_free_size(h_tx_buffer) == 0) {
      flush();
    }
    return ring_buffer_push(h_tx_buffer, &data, 1);
  }

  return 0;
}

int NetworkClient::read() {
  uint8_t data = 0;
  int res = read(&data, 1);
  if (res < 0) {
    return res;
  }
  if (res == 0) {  //  No data available.
    return -1;
  }
  return data;
}

void NetworkClient::flush() {
  if(!connected() || !h_tx_buffer) {
    return;
  }
  
  while(!ring_buffer_is_empty(h_tx_buffer)) {
    yield();
  }
}

size_t NetworkClient::write(const uint8_t *buf, size_t size) {
  if(connected() && h_tx_buffer && buf && size) {
    if(ring_buffer_get_free_size(h_tx_buffer) < size) {
      flush();
    }
    return ring_buffer_push(h_tx_buffer, (uint8_t *)buf, size);
  }
  return 0;
}

size_t NetworkClient::write_P(PGM_P buf, size_t size) {
  return write(buf, size);
}

size_t NetworkClient::write(Stream &stream) {
  uint8_t buf[WIFI_CLIENT_FLUSH_BUFFER_SIZE];

  size_t toRead = 0, toWrite = 0, written = 0;
  size_t available = stream.available();
  while (available) {
    toRead = (available > WIFI_CLIENT_FLUSH_BUFFER_SIZE) ? WIFI_CLIENT_FLUSH_BUFFER_SIZE : available;
    toWrite = stream.readBytes(buf, toRead);
    written += write(buf, toWrite);
    available = stream.available();
  }

  return written;
}

int NetworkClient::read(uint8_t *buf, size_t size) {
  int res = 0;
  uint16_t data_len = 0;
  if(connected() && h_rx_buffer && buf && size) {
    data_len = ring_buffer_available(h_rx_buffer);
    res = data_len > size ? size : data_len;
    res = ring_buffer_copy(h_rx_buffer, (uint8_t *)buf, res);
    ring_buffer_pop(h_rx_buffer, res);
  }
  
  return res;
}

size_t NetworkClient::readBytes(char *buffer, size_t length) {
  if(!connected() || !h_rx_buffer) {
    return 0;
  }

  size_t left = length, sofar = 0;
  int r = 0;
  uint32_t to = millis() + getTimeout();
  while (left) {
    r = read((uint8_t *)buffer + sofar, left);
    if (r < 0) {
      // Error has occurred
      break;
    }
    if (r > 0) {
      // We got some data
      left -= r;
      sofar += r;
      to = millis() + getTimeout();
    } else {
      // We got no data
      if (millis() >= to) {
        // We have waited for data enough
        log_w("Timeout waiting for data on connection<%p>", tcp_context);
        break;
      }
      // Allow other tasks to run
      yield();
    }
  }
  return sofar;
}

int NetworkClient::peek() {
  if(!connected() || !h_rx_buffer) {
    return -1;
  }

  uint8_t c;
  uint16_t len = ring_buffer_copy(h_rx_buffer, &c, 1);
  if(len == 0) {
    return -1;
  } else {
    return c;
  }
}

int NetworkClient::available() {
  int res = 0;
  if(connected() && h_rx_buffer) {
    res = ring_buffer_available(h_rx_buffer);
  }
  return res;
}

void NetworkClient::clear() {
  if(connected() && h_rx_buffer) {
    ring_buffer_pop(h_rx_buffer, ring_buffer_available(h_rx_buffer));
  }
}

uint8_t NetworkClient::connected() {
  return tcp_context != NULL;
}

IPAddress NetworkClient::remoteIP() const {
  port_tcp_configuration_t config = {0};
  port_tcp_get_configuration(tcp_context, &config);
  return IPAddress(&config.ip);
}

uint16_t NetworkClient::remotePort() const {
  port_tcp_configuration_t config = {0};
  port_tcp_get_configuration(tcp_context, &config);
  return config.port;
}

IPAddress NetworkClient::localIP() const {
#if PORT_CONFIG_WIFI_SUPPORTED
  //Note: for not support getsockname
  return WiFi.localIP();
#else
  return IPAddress(0, 0, 0, 0);
#endif
}

uint16_t NetworkClient::localPort() const {
  //Note: for not support getsockname
  return 0;
}

bool NetworkClient::operator==(const NetworkClient &rhs) {
  return tcp_context == rhs.tcp_context && remotePort() == rhs.remotePort() && remoteIP() == rhs.remoteIP();
}

#endif