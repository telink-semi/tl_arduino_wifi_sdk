/********************************************************************************************************
 * @file    HttpClient.cpp
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
#include "HttpClient.h"
#include "Arduino.h"

struct CallbackContext {
    HttpHeaders* headers;
    http_response_t* response;
};

static void onHeaderSent(void *user_data) {
    CallbackContext *ctx = (CallbackContext *)user_data;
    ctx->headers->clear();
    ctx->response->clear();
}

static void onHeader(const char *key, const char *value, void *user_data) {
    CallbackContext *ctx = (CallbackContext *)user_data;

    ctx->headers->add(key, value);
}

static void onData(uint8_t *data, uint32_t len, void *user_data) {
    CallbackContext *ctx = (CallbackContext *)user_data;
    ctx->response->insert(ctx->response->end(), data, data + len);
}


HttpClient::HttpClient() {
    http_client_context = port_http_client_create_context(this);
}

HttpClient::~HttpClient() {
    port_http_client_destroy_context(http_client_context);
}

void HttpClient::setUserAgent(const char *userAgent) {
    port_http_client_configuration_t config = {0};
    config.user_agent = userAgent;
    config.user_agent_valid = 1;
    port_http_client_set_configuration(http_client_context, &config);
}

void HttpClient::setHttpTimeout(uint32_t timeout) {
    port_http_client_configuration_t config = {0};
    config.timeout_ms = timeout;
    config.timeout_ms_valid = 1;
    port_http_client_set_configuration(http_client_context, &config);
}

bool HttpClient::get(const char *url, HttpHeaders &headers, http_response_t &response) {

    CallbackContext ctx{&headers, &response};

    port_http_client_callbacks_t callbacks = {
        .on_header_sent = onHeaderSent,
        .on_header = onHeader,
        .on_data = onData,
        .user_data = &ctx,
    };

    return port_http_client_get(http_client_context, url, [](void *handle, void *user_data) {
        CallbackContext *ctx = (CallbackContext *)user_data;
        for (size_t i = 0; i < ctx->headers->size(); i++) {
            port_http_client_set_header(handle, ctx->headers->at(i).first, ctx->headers->at(i).second);
        }
    }, &callbacks);
}

bool HttpClient::post(const char *url, HttpHeaders &headers, http_content_t &content, http_response_t &response) {

    CallbackContext ctx{&headers, &response};
    port_http_client_callbacks_t callbacks = {
        .on_header_sent = onHeaderSent,
        .on_header = onHeader,
        .on_data = onData,
        .user_data = &ctx,
    };

    return port_http_client_post(http_client_context, url, [](void *handle, void *user_data) {
        CallbackContext *ctx = (CallbackContext *)user_data;
        for (size_t i = 0; i < ctx->headers->size(); i++) {
            port_http_client_set_header(handle, ctx->headers->at(i).first, ctx->headers->at(i).second);
        }
    }, content.data(), content.size(), &callbacks);
}

bool HttpClient::post(const char *url, HttpHeaders &headers, const char *content, http_response_t &response) {
    http_content_t content_buf;
    content_buf.reserve(strlen(content));
    content_buf.insert(content_buf.end(), content, content + strlen(content));
    return post(url, headers, content_buf, response);
}