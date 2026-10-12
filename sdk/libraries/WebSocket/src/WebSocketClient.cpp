/********************************************************************************************************
 * @file    WebSocketClient.cpp
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
#if PORT_CONFIG_WEBSOCKET_SUPPORTED

#include "WebSocketClient.h"
#include "Arduino.h"

#define WEBSOCKET_CLIENT_DEF_CONN_TIMEOUT_MS (5000)
#define WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE   (1024)
#define WEBSOCKET_CLIENT_READ_BUFFER_SIZE   (1024)
#define WEBSOCKET_RECON_TIME_DEFAULT        (5000)

int _websocket_task_read_callback(WebSocketClient *ws) {
  if(!ws) return -1;
  return ws->read_process();
}

int _websocket_task_write_callback(WebSocketClient *ws) {
  if(!ws) return -1;
  return ws->write_process();
}

extern "C" {

int websocket_task_read_callback(void *ctx) {
    if(!ctx) return -1;
    port_websocket_configuration_t config = {0};
    port_websocket_get_configuration(ctx, &config);

    WebSocketClient *pWebsocketClient = (WebSocketClient *)config.owner;
    return _websocket_task_read_callback(pWebsocketClient);
}

int websocket_task_write_callback(void *ctx) {
    if(!ctx) return -1;
    port_websocket_configuration_t config = {0};
    port_websocket_get_configuration(ctx, &config);
    if(!config.owner) return -1;
    WebSocketClient *pWebsocketClient = (WebSocketClient *)config.owner;
    return _websocket_task_write_callback(pWebsocketClient);
}

void websocket_task_stop_callback(void *ctx) {
    if(!ctx) return;
    port_websocket_configuration_t config = {0};
    port_websocket_get_configuration(ctx, &config);
    if(!config.owner) return;
    WebSocketClient *pWebsocketClient = (WebSocketClient *)config.owner;
    pWebsocketClient->stop();
}

}

WebSocketClient::WebSocketClient() : read_buffer_size(WEBSOCKET_CLIENT_READ_BUFFER_SIZE), write_buffer_size(WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE), ws_context(NULL) {
  headers.clear();
  h_rx_buffer = NULL;
  h_tx_buffer = NULL; 
  ws_context = port_websocket_create_context(this);
  port_websocket_configuration_t config = {0};
  config.timeout_ms = WEBSOCKET_CLIENT_DEF_CONN_TIMEOUT_MS;
  config.timeout_ms_valid = 1;
  config.recon_time_ms = WEBSOCKET_RECON_TIME_DEFAULT;
  config.recon_time_ms_valid = 1;
  port_websocket_set_configuration(ws_context, &config);
}

WebSocketClient::~WebSocketClient() {
  stop();
  port_websocket_destroy_context(ws_context);
}



int WebSocketClient::read_process() {
    uint8_t data[512];
    if(h_rx_buffer && connected()) {
      while(true) {
        int len = ring_buffer_get_free_size(h_rx_buffer);
        if(len == 0) break;
        len = port_websocket_read(ws_context, data, sizeof(data));
        if(len == 0) {
          break;
        } else if(len > 0) {
          ring_buffer_push(h_rx_buffer, data, len);
        } else if(len < 0) {
          log_e("WebSocketClient::read_process: read failed -> disconnect");
          return -1;
        }
      }
    }
    return 0;
}

int WebSocketClient::write_process() {
    uint8_t data[512];
    if(h_tx_buffer && connected()) {
        while(true) {
            int len = ring_buffer_available(h_tx_buffer);
            if(len == 0) break;
            len = ring_buffer_copy(h_tx_buffer, data, sizeof(data));
            len = port_websocket_write(ws_context, data, len);
            if(len == 0) {
              break;
            } else if(len > 0) {
                ring_buffer_pop(h_tx_buffer, len);
            } else if(len < 0) {
                log_e("WebSocketClient::write_process: write failed -> disconnect");
                return -1;
            }   
        }
    }
    return 0;
}

void WebSocketClient::setBufferSize(uint32_t r_buffer_size, uint32_t w_buffer_size) {
    read_buffer_size = r_buffer_size;
    write_buffer_size = w_buffer_size;
}

void WebSocketClient::addHeader(String key, String value) {
  headers += key + ": " + value + "\r\n";
}

int WebSocketClient::connect(IPAddress ip, uint16_t port) {

    return connect(ip, port, "/", WEBSOCKET_CLIENT_DEF_CONN_TIMEOUT_MS);
}

int WebSocketClient::connect(IPAddress ip, uint16_t port, const char *path, int32_t timeout_ms) {
    port_websocket_configuration_t config = {0};
    ip.to_ip_addr_t(&config.ip);
    config.ip_valid = 1;

    config.info.path = path;
    if(headers.isEmpty()) {
      config.info.headers = NULL;
    } else {
      config.info.headers = headers.c_str();
    }
    config.info_valid = 1;

    config.port = port;
    config.port_valid = 1;

    config.timeout_ms = timeout_ms;
    config.timeout_ms_valid = 1;

    port_websocket_set_configuration(ws_context, &config);
    
    h_rx_buffer = ring_buffer_create(WEBSOCKET_CLIENT_READ_BUFFER_SIZE);
    h_tx_buffer = ring_buffer_create(WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE);
    if(!h_rx_buffer || !h_tx_buffer) {
        log_e("WebSocketClient::connect: buffer init failed");
        return 0;
    }

    return port_websocket_connect(ws_context);
}

int WebSocketClient::connect(const char *host, uint16_t port) {
    IPAddress srv(port_network_get_host_by_name(host));
    return connect(srv, port);
}
int WebSocketClient::connect(const char *host, uint16_t port, const char *path) {
    IPAddress srv(port_network_get_host_by_name(host));
    return connect(srv, port, path, WEBSOCKET_CLIENT_DEF_CONN_TIMEOUT_MS);
}

size_t WebSocketClient::write(uint8_t data) {
    return write(&data, 1);
}

size_t WebSocketClient::write(const uint8_t *buf, size_t size) {
  if(connected() && h_tx_buffer && buf && size) {
    if(ring_buffer_get_free_size(h_tx_buffer) < size) {
      flush();
    }
    return ring_buffer_push(h_tx_buffer, (uint8_t *)buf, size);
  }
  return 0;
}

size_t WebSocketClient::write_P(PGM_P buf, size_t size) {
    return write((uint8_t *)buf, size);
}

size_t WebSocketClient::write(Stream &stream) {
  uint8_t buf[WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE];
  size_t toRead = 0, toWrite = 0, written = 0;
  size_t available = stream.available();
  while (available) {
    toRead = (available > WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE) ? WEBSOCKET_CLIENT_FLUSH_BUFFER_SIZE : available;
    toWrite = stream.readBytes(buf, toRead);
    written += write(buf, toWrite);
    available = stream.available();
  }

  return written;
}

void WebSocketClient::flush() {
  if(!connected() || !h_tx_buffer) {
    return;
  }
  
  while(!ring_buffer_is_empty(h_tx_buffer)) {
    yield();
  }
}

int WebSocketClient::available() {
  int res = 0;
  if(connected() && h_rx_buffer) {
    res = ring_buffer_available(h_rx_buffer);
  }
  return res;
}

int WebSocketClient::availableForWrite() {
  int res = 0;
  if(connected() && h_tx_buffer) {
    res = ring_buffer_get_free_size(h_tx_buffer);
  }
  
  return res;
}

int WebSocketClient::read() {
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

int WebSocketClient::read(uint8_t *buf, size_t size) {
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

size_t WebSocketClient::readBytes(char *buffer, size_t length) {
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
        log_w("Timeout waiting for data on connection<%p>", ws_context);
        break;
      }
      // Allow other tasks to run
      delay(2);
    }
  }
  return sofar;
}

int WebSocketClient::peek() {
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

void WebSocketClient::clear() {
    if(connected() && h_rx_buffer) {
        ring_buffer_pop(h_rx_buffer, ring_buffer_available(h_rx_buffer));
    }
}
void WebSocketClient::stop() {
    if(connected()) {
        port_websocket_close(ws_context);
    }

    if(h_rx_buffer) {
        ring_buffer_destroy(h_rx_buffer);
        h_rx_buffer = NULL;
    }

    if(h_tx_buffer) {
        ring_buffer_destroy(h_tx_buffer);
        h_tx_buffer = NULL;
    }
}

uint8_t WebSocketClient::connected() {
    return port_websocket_is_opened(ws_context);
}

bool WebSocketClient::operator==(const WebSocketClient & ws) {
    return ws_context == ws.ws_context;
}

#endif
