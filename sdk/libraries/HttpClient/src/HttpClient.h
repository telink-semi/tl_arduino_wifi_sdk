/********************************************************************************************************
 * @file    HttpClient.h
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
#include <vector>

typedef std::vector<uint8_t> http_response_t;
typedef std::vector<uint8_t> http_content_t;

class HttpHeaders {
private:
  std::vector<std::pair<char*, char*>> headers;
  HttpHeaders(const HttpHeaders&) = delete;
  HttpHeaders& operator=(const HttpHeaders&) = delete;
public:
  HttpHeaders() {
    headers.reserve(5);
  }
  ~HttpHeaders() {
    clear();
  }
  void add(const char *key, const char *value) {
    char *_key = new char[strlen(key) + 1];
    strcpy(_key, key);
    char *_value = new char[strlen(value) + 1];
    strcpy(_value, value);
    headers.push_back(std::make_pair(_key, _value));
  }

  void clear() {
    for (size_t i = 0; i < headers.size(); i++) {
      if (headers[i].first) delete[] headers[i].first;
      if (headers[i].second) delete[] headers[i].second;
    }
    headers.clear();
  }
  void reserve(size_t size) {
    headers.reserve(size);
  }

  size_t size() const {
    return headers.size();
  }

  std::pair<char*, char*>& operator[](size_t index) {
    return headers[index];
  }

  std::pair<char*, char*>& at(size_t index) {
    return headers[index];
  }
};

class HttpClient {
protected:
  void *http_client_context;
public:
  HttpClient();
  ~HttpClient();
  void setUserAgent(const char *userAgent);
  void setHttpTimeout(uint32_t timeout);
  bool get(const char *url, HttpHeaders &headers, http_response_t &response);
  bool post(const char *url, HttpHeaders &headers, http_content_t &content, http_response_t &response);
  bool post(const char *url, HttpHeaders &headers, const char *content, http_response_t &response);
};

#endif