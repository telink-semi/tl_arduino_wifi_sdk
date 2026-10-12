/********************************************************************************************************
 * @file    WebServer.h
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
#if PORT_CONFIG_WEBSERVER_SUPPORTED

#include <inttypes.h>
#include <port_interface.h>
#include <WString.h>

typedef enum {
    HTTP_ERR_500_INTERNAL_SERVER_ERROR,
    HTTP_ERR_501_METHOD_NOT_IMPLEMENTED,
    HTTP_ERR_505_VERSION_NOT_SUPPORTED,
    HTTP_ERR_400_BAD_REQUEST,
    HTTP_ERR_401_UNAUTHORIZED,
    HTTP_ERR_403_FORBIDDEN,
    HTTP_ERR_404_NOT_FOUND,
    HTTP_ERR_405_METHOD_NOT_ALLOWED,
    HTTP_ERR_408_REQ_TIMEOUT,
    HTTP_ERR_411_LENGTH_REQUIRED,
    HTTP_ERR_414_URI_TOO_LONG,
    HTTP_ERR_431_REQ_HDR_FIELDS_TOO_LARGE,
}http_error_code_t;

class WebServer {
protected:
  void *http_server_context;
public:
  WebServer();
  ~WebServer();
  bool start(uint16_t port);
  bool get(const char *url, http_server_handler_func handler);
  bool post(const char *url, http_server_handler_func handler);
  bool stop(void);

  static String getRequestHeader(void *ctx, const char *key);
  static String getRequestHeader(void *ctx, String key) {
    return getRequestHeader(ctx, key.c_str());
  }
  static String getRequsetQueryInURL(void *ctx, const char *key);
  static String getRequsetQueryInURL(void *ctx, String key) {
    return getRequsetQueryInURL(ctx, key.c_str());
  }
  static size_t getRequestContentSize(void *ctx);
  static int recv(void *ctx, void *buf, size_t buf_size);
  static String recvAll(void *ctx, uint32_t timeout_ms=3000);

  static bool setResponseHeader(void *ctx, const char *key, const char *value);
  static bool setResponseHeader(void *ctx, String key, String value) {
    return setResponseHeader(ctx, key.c_str(), value.c_str());
  }
  static bool send(void *ctx, void *data, size_t data_size);

  static bool send(void *ctx, const char *msg) {
    return send(ctx, (void*)msg, strlen(msg));
  }

  static bool send(void *ctx, String msg) {
    return send(ctx, msg.c_str());
  }

  static bool sendChunk(void *ctx, void *data, size_t data_size);
  static bool sendError(void *ctx, http_error_code_t error, const char *msg);
  static bool sendError(void *ctx, http_error_code_t error, String msg) {
    return sendError(ctx, error, msg.c_str());
  }

  static bool sendRedirect(void *ctx, const char *path);
  static bool sendRedirect(void *ctx, String path) {
    return sendRedirect(ctx, path.c_str());
  }
};

#endif