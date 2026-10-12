/********************************************************************************************************
 * @file    WebServer.cpp
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
#include "WebServer.h"
#include "Arduino.h"

/**  
 * @brief      This function serves to initialize the web server.  
 * @param[in]  none  
 * @return     none  
 */
WebServer::WebServer() {
    http_server_context = port_http_server_create_context(this);
    port_http_server_configuration_t config = {0};
    config.port = 80;
    config.port_valid = 1;
    port_http_server_set_configuration(http_server_context, &config);
}

/**  
 * @brief      This function serves to destroy the web server.  
 * @param[in]  none  
 * @return     none  
 */
WebServer::~WebServer() {
    port_http_server_destroy_context(http_server_context);
}

/**  
 * @brief      This function serves to start the web server on a specified port.  
 * @param[in]  port - The port number on which the web server will listen  
 * @return     true if the server starts successfully, false otherwise  
 */
bool WebServer::start(uint16_t port) {
    port_http_server_configuration_t config = {0};
    config.port = port;
    config.port_valid = 1;
    port_http_server_set_configuration(http_server_context, &config);
    return port_http_server_start(http_server_context);
}

/**  
 * @brief      This function serves to register a handler for GET requests on a specified URL.  
 * @param[in]  url - The URL path to handle GET requests  
 * @param[in]  handler - The callback function to handle the request  
 * @return     true if the handler is registered successfully, false otherwise  
 */
bool WebServer::get(const char *url, http_server_handler_func handler) {
    return port_http_server_get(http_server_context, url, handler);
}

/**  
 * @brief      This function serves to register a handler for POST requests on a specified URL.  
 * @param[in]  url - The URL path to handle POST requests  
 * @param[in]  handler - The callback function to handle the request  
 * @return     true if the handler is registered successfully, false otherwise  
 */
bool WebServer::post(const char *url, http_server_handler_func handler) {
    return port_http_server_post(http_server_context, url, handler);
}

/**  
 * @brief      This function serves to stop the web server.  
 * @param[in]  none  
 * @return     true if the server stops successfully, false otherwise  
 */
bool WebServer::stop(void) {
    return port_http_server_stop(http_server_context);
}

/**  
 * @brief      This function serves to get a request header value by key.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  key - The key of the header to retrieve  
 * @return     The value of the header as a String, or an empty string if not found  
 */
String WebServer::getRequestHeader(void *ctx, const char *key) {
    char *value = port_http_server_get_header(ctx, key);
    String ret = value;
    if(value) {
        free(value);
    }

    return ret;
}

/**  
 * @brief      This function serves to get a query parameter value from the URL by key.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  key - The key of the query parameter to retrieve  
 * @return     The value of the query parameter as a String, or an empty string if not found  
 */
String WebServer::getRequsetQueryInURL(void *ctx, const char *key) {
    char *value = port_http_server_get_url_query(ctx, key);
    String ret = value;
    if(value) {
        free(value);
    }

    return ret;
}

/**  
 * @brief      This function serves to get the content length of the request body.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @return     The size of the request body in bytes, or 0 if not available  
 */
size_t WebServer::getRequestContentSize(void *ctx) {
    return port_http_server_get_content_length(ctx);
}

/**  
 * @brief      This function serves to receive data from the client.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  buf - The buffer to store the received data  
 * @param[in]  buf_size - The size of the buffer  
 * @return     The number of bytes received, or -3 if a timeout occurred  
 */
int WebServer::recv(void *ctx, void *buf, size_t buf_size) {
    return port_http_server_recv(ctx, buf, buf_size);
}

/**  
 * @brief      This function serves to receive all data from the client until the request is complete.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  timeout_ms - The maximum time to wait for data in milliseconds  
 * @return     A String containing all received data, or an empty string if an error occurred  
 */
String WebServer::recvAll(void *ctx, uint32_t timeout_ms) {
    size_t size = getRequestContentSize(ctx);
    int rwLen;
    char buf[256];
    uint32_t startTime = millis();
    String ret = "";
    while(size) {
        if((rwLen = recv(ctx, buf, sizeof(buf) - 1)) > 0) {
            buf[rwLen] = '\0';
            ret += buf;
            size -= rwLen;
        } else if(rwLen == -3) {
            yield();
            continue;
        } else {
            break;
        }
        if(millis() - startTime >= timeout_ms) {
            log_e("WebSocket::recvAll timeout<%dms>", timeout_ms);
            break;
        }
    }
    return ret;
}

/**  
 * @brief      This function serves to set a response header for the HTTP response.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  key - The key of the header to set  
 * @param[in]  value - The value of the header to set  
 * @return     true if the header is set successfully, false otherwise  
 */
bool WebServer::setResponseHeader(void *ctx, const char *key, const char *value) {
    return port_http_server_resp_set_header(ctx, key, value);
}

/**  
 * @brief      This function serves to send data as a response.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  data - The data to send  
 * @param[in]  data_size - The size of the data to send  
 * @return     true if the data is sent successfully, false otherwise  
 */
bool WebServer::send(void *ctx, void *data, size_t data_size) {
    return port_http_server_resp_send(ctx, data, data_size);
}

/**  
 * @brief      This function serves to send data in chunks as a response.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  data - The data to send  
 * @param[in]  data_size - The size of the data to send  
 * @return     true if the data is sent successfully, false otherwise  
 */
bool WebServer::sendChunk(void *ctx, void *data, size_t data_size) {
    return port_http_server_resp_send_chunk(ctx, data, data_size);
}

/**  
 * @brief      This function serves to send an error response.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  error - The HTTP error code  
 * @param[in]  msg - A message describing the error  
 * @return     true if the error is sent successfully, false otherwise  
 */
bool WebServer::sendError(void *ctx, http_error_code_t error, const char *msg) {
    return port_http_server_resp_send_error(ctx, error, msg);
}

/**  
 * @brief      This function serves to send a redirect response.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @param[in]  path - The path to redirect to  
 * @return     true if the redirection is sent successfully, false otherwise  
 */
bool WebServer::sendRedirect(void *ctx, const char *path) {
    return port_http_server_redirect(ctx, path);
}