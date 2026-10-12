/********************************************************************************************************
 * @file    port_http_client.c
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

#include "sdk_common.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "port_interface.h"

#define MAX_HTTP_OUTPUT_BUFFER 1024
#define HTTP_USER_AGENT "ArduinoHttpClient/1.0"
#define HTTP_TIMEOUT_MS 5000

typedef struct {
    void *owner;
	port_http_client_callbacks_t callbacks;
	uint8_t *buffer;
	uint32_t buffer_len;
	esp_http_client_config_t config;
} port_http_client_context_t, *port_http_client_context_t_ptr;

esp_err_t http_client_event_handler(esp_http_client_event_t *evt)
{
	int mbedtls_err = 0;
	esp_err_t err;
	port_http_client_context_t_ptr httpCtx = (port_http_client_context_t_ptr)evt->user_data;

	switch (evt->event_id) {
	case HTTP_EVENT_ERROR:
		log_d("HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		log_d("HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		log_d("HTTP_EVENT_HEADER_SENT");
		if (httpCtx && httpCtx->callbacks.on_header_sent) {
			httpCtx->callbacks.on_header_sent(httpCtx->callbacks.user_data);
		}
		break;
	case HTTP_EVENT_ON_HEADER:
		log_d("HTTP_EVENT_ON_HEADER, key=%s, value=%s",evt->header_key, evt->header_value);
		if (httpCtx && httpCtx->callbacks.on_header) {
			httpCtx->callbacks.on_header(evt->header_key, evt->header_value, httpCtx->callbacks.user_data);
		}
		break;
	case HTTP_EVENT_ON_DATA:
		log_d("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
		if (httpCtx && httpCtx->callbacks.on_data) {
			httpCtx->callbacks.on_data((uint8_t *)evt->data, evt->data_len, httpCtx->callbacks.user_data);
		}
		break;
	case HTTP_EVENT_ON_FINISH:
		log_d("HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		log_d("HTTP_EVENT_DISCONNECTED");

		err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data,
				&mbedtls_err, NULL);
		if (err != 0) {
			log_e("Last esp error code : 0x%x", err);
			log_e("Last mbedtls failure: 0x%x", mbedtls_err);
		}


		break;
	case HTTP_EVENT_REDIRECT:
		log_d("HTTP_EVENT_REDIRECT");
		esp_http_client_set_redirection(evt->client);
		break;
	default:
		break;
	}

    return ESP_OK;
}

void *port_http_client_create_context(void *owner) {
	port_http_client_context_t_ptr ctx = (port_http_client_context_t_ptr)malloc(sizeof(port_http_client_context_t));
	if (ctx == NULL) {
		return NULL;
	}
	memset(ctx, 0, sizeof(port_http_client_context_t));
	ctx->owner = owner;
	ctx->config.user_data = ctx;
	ctx->config.event_handler = http_client_event_handler;
	ctx->config.timeout_ms = HTTP_TIMEOUT_MS;
	ctx->config.user_agent = HTTP_USER_AGENT;
	return ctx;
}

void port_http_client_destroy_context(void *ctx) {
	if (ctx == NULL) {
		return;
	}
	free(ctx);
}

bool port_http_client_set_configuration(void *ctx, port_http_client_configuration_t *config) {
	if (ctx == NULL || config == NULL) {
		return false;
	}
	
	port_http_client_context_t_ptr httpCtx = (port_http_client_context_t_ptr)ctx;
	if (config->timeout_ms_valid) {
		httpCtx->config.timeout_ms = config->timeout_ms;
	}

	if (config->owner_valid) {
		httpCtx->owner = config->owner;
	}

	if (config->user_agent_valid) {
		httpCtx->config.user_agent = config->user_agent;
	}

	return true;
}

bool port_http_client_get_configuration(void *ctx, port_http_client_configuration_t *config) {
	if (ctx == NULL || config == NULL) {
		return false;
	}
	
	port_http_client_context_t_ptr httpCtx = (port_http_client_context_t_ptr)ctx;
	config->owner = httpCtx->owner;
	config->owner_valid = 1;
	config->user_agent = httpCtx->config.user_agent;
	config->user_agent_valid = 1;
	config->timeout_ms = httpCtx->config.timeout_ms;
	config->timeout_ms_valid = 1;

	return true;
}

bool port_http_client_get(void *ctx, const char *url, void (*set_headers)(void *, void *), 
                          port_http_client_callbacks_t *callbacks)
{
	if (ctx == NULL || url == NULL || callbacks == NULL) {
		return false;
	}

	uint8_t local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
	port_http_client_context_t_ptr httpCtx = (port_http_client_context_t_ptr)ctx;
    memcpy(&httpCtx->callbacks, callbacks, sizeof(port_http_client_callbacks_t));
	httpCtx->buffer = local_response_buffer;
	httpCtx->buffer_len = MAX_HTTP_OUTPUT_BUFFER;
	httpCtx->config.url = url;

    esp_http_client_handle_t client = esp_http_client_init(&httpCtx->config);
    if (client == NULL) {
        log_e("HTTP client initialize failure");
        return false;
    }

    esp_http_client_set_method(client, HTTP_METHOD_GET);
	if (set_headers) {
		set_headers(client, httpCtx->callbacks.user_data);
	}
    
    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

	return err == ESP_OK;
}

bool port_http_client_post(void *ctx, const char *url, void (*set_headers)(void *, void *), uint8_t *post_data, uint32_t post_data_len, 
                           port_http_client_callbacks_t *callbacks)
{
	if (ctx == NULL || url == NULL || callbacks == NULL) {
		return false;
	}

	uint8_t local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
	port_http_client_context_t_ptr httpCtx = (port_http_client_context_t_ptr)ctx;
    memcpy(&httpCtx->callbacks, callbacks, sizeof(port_http_client_callbacks_t));
	httpCtx->buffer = local_response_buffer;
	httpCtx->buffer_len = MAX_HTTP_OUTPUT_BUFFER;
	httpCtx->config.url = url;

    esp_http_client_handle_t client = esp_http_client_init(&httpCtx->config);
    if (client == NULL) {
        log_e("HTTP client initialize failure");
        return false;
    }
    
    esp_http_client_set_method(client, HTTP_METHOD_POST);
	if (set_headers) {
		set_headers(client, httpCtx->callbacks.user_data);
	}

	if (post_data && post_data_len) {
		esp_http_client_set_post_field(client, (char *)post_data, post_data_len);
	}
    
    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

	if (err != ESP_OK) {
		log_e("HTTP client perform failure, err=%d", err);
	}

	return err == ESP_OK;
}

void port_http_client_set_header(void *handle, const char *key, const char *value) {
	esp_http_client_set_header(handle, key, value);
}

#endif