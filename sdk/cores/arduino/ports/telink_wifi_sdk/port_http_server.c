/********************************************************************************************************
 * @file    port_http_server.c
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
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include "tlsr_http_server.h"

struct _port_http_server_ctx_t{
    void *owner;
    httpd_handle_t server;
    httpd_config_t config;
    list_handle_t list;
    uint16_t port;
    bool startFlag;
};
typedef struct _port_http_server_ctx_t http_server_ctx_t, *http_server_ctx_t_ptr;


static httpd_config_t default_config = HTTPD_DEFAULT_CONFIG();


void *port_http_server_create_context(void *owner) {
    http_server_ctx_t_ptr httpCtx = malloc(sizeof(http_server_ctx_t));
    if(!httpCtx) return NULL;
    memset(httpCtx, 0, sizeof(http_server_ctx_t));
    httpCtx->owner = owner;
    httpCtx->list = list_create();
    httpCtx->config = default_config;
    httpCtx->server = NULL;
    return httpCtx;
}

void port_http_server_destroy_context(void *ctx) {
    if(!ctx) return;
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(httpCtx->list) {
        list_destroy(httpCtx->list, free);
        httpCtx->list = NULL;
    }
    free(ctx);
}

bool port_http_server_set_configuration(void *ctx, port_http_server_configuration_t *config) {
    if(!ctx || !config) return false;
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(config->owner_valid) {
        httpCtx->owner = config->owner;
    }
    if(config->port_valid) {
        httpCtx->port = config->port;
    }
    return true;
}

bool port_http_server_get_configuration(void *ctx, port_http_server_configuration_t *config) {
    if(!ctx || !config) return false;
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    config->owner_valid = 1;
    config->port_valid = 1;
    config->owner = httpCtx->owner;
    config->port = httpCtx->port;
    return true;
}

//start/stop
bool port_http_server_start(void *ctx) {
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(!httpCtx) return false;
    httpCtx->config.server_port = httpCtx->port;
    log_d("Starting server on port: '%d'", httpCtx->config.server_port);
    httpCtx->startFlag = (httpd_start(&httpCtx->server, &httpCtx->config) == WITS_OK);
    if(httpCtx->startFlag) {
        size_t listSize = list_size(httpCtx->list);
        for(size_t i = 0; i < listSize; i++) {
            httpd_uri_t * uri = (httpd_uri_t *)list_get(httpCtx->list, i);
            if(uri) {
                httpd_register_uri_handler(httpCtx->server, uri);
                log_d("<http server> register handler<url:%s, method:%d, handler:%p> done", uri->uri, uri->method, uri->handler);
            }
        }  
    } else {
        if(httpCtx->server) {
            httpd_stop(httpCtx->server);
            httpCtx->startFlag = false;
            httpCtx->server = NULL;
        }
    }

    return httpCtx->startFlag;
}

bool port_http_server_stop(void *ctx) {
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(!httpCtx) return false;
    if(httpCtx->startFlag) {
        httpd_stop(httpCtx->server);
        httpCtx->startFlag = false;
        httpCtx->server = NULL;
    }

    return true;
}

typedef esp_err_t (*httpd_handler_func)(httpd_req_t *r);
bool port_http_server_get(void *ctx, const char *url, http_server_handler_func handler) {
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(!httpCtx) return false;
    if(!httpCtx->list) return false;

    httpd_uri_t *uri = (httpd_uri_t *)malloc(sizeof(httpd_uri_t));
    if(!uri) return false;
    memset(uri, 0, sizeof(httpd_uri_t));
    uri->uri = url;
    uri->handler = (httpd_handler_func)handler;
    uri->method = HTTP_GET;
    uri->user_ctx = NULL;

    if(list_push_back(httpCtx->list, uri)) {
        return true;
    } else {
        free(uri);
        return false;
    }
}

bool port_http_server_post(void *ctx, const char *url, http_server_handler_func handler) {
    http_server_ctx_t_ptr httpCtx = (http_server_ctx_t_ptr)ctx;
    if(!httpCtx) return false;
    if(!httpCtx->list) return false;

    httpd_uri_t *uri = (httpd_uri_t *)malloc(sizeof(httpd_uri_t));
    if(!uri) return false;
    uri->uri = url;
    uri->handler = (httpd_handler_func)handler;
    uri->method = HTTP_POST;
    uri->user_ctx = NULL;

    if(list_push_back(httpCtx->list, uri)) {
        return true;
    } else {
        free(uri);
        return false;
    }
}

//parse request, need to be free
char * port_http_server_get_header(void *ctx, const char *key) {
    if(!ctx || !key) return NULL;
    httpd_req_t *req = (httpd_req_t *)ctx;
    size_t len = httpd_req_get_hdr_value_len(req, key) + 1;
    if(len <= 1) return NULL;
    char * buff = (char *)malloc(len);
    if(!buff) return NULL;
    if(httpd_req_get_hdr_value_str(req, key, buff, len) != WITS_OK) {
        free(buff);
        buff = NULL;
    }

    return buff;
}

char * port_http_server_get_url_query(void *ctx, const char *key) {
    if(!ctx || !key) return NULL;
    httpd_req_t *req = (httpd_req_t *)ctx;
    size_t len = httpd_req_get_url_query_len(req) + 1;
    if(len <= 1) return NULL;
    char * buff = (char *)malloc(len);
    if(!buff) return NULL;
    if(httpd_req_get_url_query_str(req, buff, len) != WITS_OK) {
        free(buff);
        return NULL;
    }

    char *param = (char *)malloc(32);
    if(param) {
        if(httpd_query_key_value(buff, key, param, 32) != WITS_OK) {
            free(param);
            param = NULL;
        } 
    }

    return param;
}

size_t port_http_server_get_content_length(void *ctx) {
    if(!ctx) return 0;
    httpd_req_t *req = (httpd_req_t *)ctx;
    return req->content_len;
}

int port_http_server_recv(void *ctx, void *buf, size_t buf_size) {
    if(!ctx || !buf || !buf_size) return 0;
    httpd_req_t *req = (httpd_req_t *)ctx;
    return httpd_req_recv(req, buf, buf_size);
}

//send resp
bool port_http_server_resp_set_header(void *ctx, const char *key, const char *value) {
    if(!ctx || !key || !value) return false;
    httpd_req_t *req = (httpd_req_t *)ctx;
    return httpd_resp_set_hdr(req, key, value);
}

bool port_http_server_resp_send(void *ctx, void *buf, size_t len) {
    if(!ctx || !buf || !len) return false;
    return (httpd_resp_send((httpd_req_t *)ctx, (char *)buf, len) == WITS_OK);
}

bool port_http_server_resp_send_chunk(void *ctx, void *buf, size_t len) {
    if(!ctx) return false;
    return (httpd_resp_send_chunk((httpd_req_t *)ctx, (char *)buf, len) == WITS_OK);
}

static const httpd_err_code_t error_map[] = {
    HTTPD_500_INTERNAL_SERVER_ERROR,
    HTTPD_501_METHOD_NOT_IMPLEMENTED,
    HTTPD_505_VERSION_NOT_SUPPORTED,
    HTTPD_400_BAD_REQUEST,
    HTTPD_401_UNAUTHORIZED,
    HTTPD_403_FORBIDDEN,
    HTTPD_404_NOT_FOUND,
    HTTPD_405_METHOD_NOT_ALLOWED,
    HTTPD_408_REQ_TIMEOUT,
    HTTPD_411_LENGTH_REQUIRED,
    HTTPD_414_URI_TOO_LONG,
    HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE,
};

bool port_http_server_resp_send_error(void *ctx, uint32_t error, const char *msg) {
    if(!ctx || error > sizeof(error_map)) return false;
    return (httpd_resp_send_err((httpd_req_t *)ctx, error_map[error], msg) == WITS_OK);
}

bool port_http_server_redirect(void *ctx, const char *path) {
    if(!ctx) return false;
    httpd_req_t *req = (httpd_req_t *)ctx;
    if(httpd_resp_set_status(req, "302") != WITS_OK) return false;
    if(httpd_resp_set_type(req, "Found") != WITS_OK) return false;
    if(httpd_resp_set_hdr(req, "Location", path) != WITS_OK) return false;
    if(httpd_resp_sendstr(req, "302 Found") != WITS_OK) return false;
    return true;
}

#endif