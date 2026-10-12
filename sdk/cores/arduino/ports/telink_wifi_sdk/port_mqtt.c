/********************************************************************************************************
 * @file    port_mqtt.c
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

#include "core_mqtt.h"
#include "port_interface.h"
#include "sdk_common.h"

#define MQTT_MAX_CLIENT_CNT 3
#define MQTT_FIXED_BUFFER_SIZE 1024

struct _port_mqtt_ctx_t
{
    void *owner;
    void *handle;
    uint16_t task_hwm;
    uint8_t start_flag:1;
    uint8_t stop_flag:1;
};

typedef struct _port_mqtt_ctx_t port_mqtt_ctx_t, *port_mqtt_ctx_t_ptr;


struct NetworkContext
{
	void *ctx;
};

typedef struct {
	NetworkContext_t *net_context;
	MQTTContext_t *mqtt_context;
} network_mqtt_map_t;

typedef struct {
	network_mqtt_map_t map_list[MQTT_MAX_CLIENT_CNT];
	uint8_t client_cnt;
} network_mqtt_map_list_t;

typedef struct {
	char *topic;
	mqtt_subscribe_callback_func callback;
} mqtt_subscribe_callback_item_t;

typedef struct {
	NetworkContext_t net_context;
	MQTTContext_t mqtt_context;
	TransportInterface_t transport_interface;
	MQTTFixedBuffer_t fixed_buffer;
	MQTTPubAckInfo_t *outgoingPublishes;
	MQTTPubAckInfo_t *incomingPublishes;
	mqtt_subscribe_callback_item_t *subscribe_callback_list;
	uint16_t subscribe_max_count;
	uint16_t publish_max_count;
} mqtt_prv_handle_t;


static network_mqtt_map_list_t network_mqtt_map_list = {0};

__attribute__((weak)) int mqtt_transport_write_callback(void *ctx, uint8_t *buffer, size_t bytesToSend) {
	(void)ctx;
	(void)buffer;
	(void)bytesToSend;
	return 0;
}

__attribute__((weak)) int mqtt_transport_recv_callback(void *ctx, uint8_t *buffer, size_t bytesToRecv) {
	(void)ctx;
	(void)buffer;
	(void)bytesToRecv;
	return 0;
}
__attribute__((weak)) void mqtt_transport_stop_callback(void *ctx) {
	(void)ctx;
}

static NetworkContext_t *get_net_context_by_mqtt_context(MQTTContext_t *mqtt_context)
{
    for(uint8_t i = 0; i < network_mqtt_map_list.client_cnt; i++) {
        if(network_mqtt_map_list.map_list[i].mqtt_context == mqtt_context) {
            return network_mqtt_map_list.map_list[i].net_context;
        }
    }
    return NULL;
}

static bool add_subscribe_callback(mqtt_prv_handle_t *mqtt_handle, const char *topic, mqtt_subscribe_callback_func callback)
{
	uint16_t empty_index = mqtt_handle->subscribe_max_count;
	for(uint16_t i = 0; i < mqtt_handle->subscribe_max_count; i++) {
		if(mqtt_handle->subscribe_callback_list[i].topic && strcmp(mqtt_handle->subscribe_callback_list[i].topic, topic) == 0) {
			return false;
		}

		if(empty_index == mqtt_handle->subscribe_max_count && !mqtt_handle->subscribe_callback_list[i].topic) {
			empty_index = i;
		}
	}

	if(empty_index == mqtt_handle->subscribe_max_count) {
		return false;
	}

    mqtt_handle->subscribe_callback_list[empty_index].topic = (char *)malloc(strlen(topic) + 1);
    strcpy(mqtt_handle->subscribe_callback_list[empty_index].topic, topic);
    mqtt_handle->subscribe_callback_list[empty_index].callback = callback;

    return true;
}

static bool remove_subscribe_callback(mqtt_prv_handle_t *mqtt_handle, const char *topic)
{
	for(uint16_t i = 0; i < mqtt_handle->subscribe_max_count; i++) {
		if(mqtt_handle->subscribe_callback_list[i].topic && strcmp(mqtt_handle->subscribe_callback_list[i].topic, topic) == 0) {
			free((void *)mqtt_handle->subscribe_callback_list[i].topic);
			mqtt_handle->subscribe_callback_list[i].topic = NULL;
			mqtt_handle->subscribe_callback_list[i].callback = NULL;
			return true;
		}
	}
	return false;
}

static bool clear_subscribe_callback_list(mqtt_prv_handle_t *mqtt_handle)
{
    for(uint16_t i = 0; i < mqtt_handle->subscribe_max_count; i++) {
		if(mqtt_handle->subscribe_callback_list[i].topic) {
        	free((void *)mqtt_handle->subscribe_callback_list[i].topic);
			mqtt_handle->subscribe_callback_list[i].topic = NULL;
		}
		mqtt_handle->subscribe_callback_list[i].callback = NULL;
    }

    return true;
}

static bool add_map_to_list(NetworkContext_t *net_context, MQTTContext_t *mqtt_context)
{
    if(network_mqtt_map_list.client_cnt >= MQTT_MAX_CLIENT_CNT) {
        return false;
    }
    network_mqtt_map_list.map_list[network_mqtt_map_list.client_cnt].net_context = net_context;
    network_mqtt_map_list.map_list[network_mqtt_map_list.client_cnt].mqtt_context = mqtt_context;
    network_mqtt_map_list.client_cnt++;
    return true;
}



static int32_t transport_send( NetworkContext_t * pNetworkContext,
                                       const void * pBuffer,
                                       size_t bytesToSend )
{
    int cnt = mqtt_transport_write_callback(pNetworkContext->ctx, (uint8_t *)pBuffer, bytesToSend);
    
    if(cnt < 0) {
        return -1;
    }else{
		// log_array_d("mqtt transport send", (uint8_t *)pBuffer, cnt);
        return cnt;
    }		
}								   

static int32_t transport_recv( NetworkContext_t * pNetworkContext,
                                       void * pBuffer,
                                       size_t bytesToRecv )
{
	int cnt = mqtt_transport_recv_callback(pNetworkContext->ctx, (uint8_t *)pBuffer, bytesToRecv);

	if(cnt > 0) {
		// log_array_d("mqtt transport recv", (uint8_t *)pBuffer, cnt);
	} else {
		if(cnt < 0) {
			log_e("mqtt transport recv failed, cnt:%d", cnt);
			cnt = 0;
		}
	}


	return cnt;
}

static int32_t transport_writev( NetworkContext_t * pNetworkContext,
                                         TransportOutVector_t * pIoVec,
                                         size_t ioVecCount )
{
	int32_t ret = 0;
	// log_d("transport_writev:%d", ioVecCount);
	for(size_t i = 0; i < ioVecCount; i++)
	{
		if(transport_send(pNetworkContext, pIoVec[i].iov_base, pIoVec[i].iov_len) < 0) {
			return -1;
		}
		else{
			ret += pIoVec[i].iov_len;
		}
	}
	return ret;
}

static uint32_t mqtt_get_current_time(void)
{
	return millis();
}


static void mqtt_event_callback( struct MQTTContext * pContext,
                                 struct MQTTPacketInfo * pPacketInfo,
                                 struct MQTTDeserializedInfo * pDeserializedInfo )
{
	// log_d("mqtt_event_callback: <type: %d> <topic: %s>", pPacketInfo->type, pDeserializedInfo->pPublishInfo->pTopicName);
	NetworkContext_t *net_context = get_net_context_by_mqtt_context(pContext);
	if(!net_context) return;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)net_context->ctx;
	if(!mqtt_ctx) return;
	mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
	if(!mqtt_handle) return;
	// handle subscribe callback
	for(uint16_t i = 0; i < mqtt_handle->subscribe_max_count; i++) {
		if(!mqtt_handle->subscribe_callback_list[i].topic) continue;
		if(memcmp(mqtt_handle->subscribe_callback_list[i].topic, pDeserializedInfo->pPublishInfo->pTopicName, strlen(mqtt_handle->subscribe_callback_list[i].topic)) == 0) {
			uint8_t *buffer = (uint8_t*)malloc(pDeserializedInfo->pPublishInfo->payloadLength + 1);
			if(buffer) {
				memcpy(buffer, pDeserializedInfo->pPublishInfo->pPayload, pDeserializedInfo->pPublishInfo->payloadLength);
				buffer[pDeserializedInfo->pPublishInfo->payloadLength] = '\0';
				mqtt_handle->subscribe_callback_list[i].callback(buffer, pDeserializedInfo->pPublishInfo->payloadLength);
				free(buffer);
			}
			break;
		}
	}

}

static bool mqtt_receive_process(MQTTContext_t *pConetext) {
	MQTTStatus_t status;
	status = MQTT_ProcessLoop( pConetext );
	// log_d("mqtt_receive_process: <status:%d>", status);
if(1) {
	if( status != MQTTSuccess && status != MQTTNeedMoreBytes  && status != MQTTBadResponse)
	{
		log_e("MQTT_ProcessLoop failed! <status:%d>", status);
		return false;
	}
}
	return true;
}

static void mqtt_task(void *para) {
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)para;
	do {
		if(!mqtt_ctx->handle) break;
		mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
		if(!mqtt_handle) break;

		mqtt_ctx->stop_flag = 0;
		// TickType_t lastTick = xTaskGetTickCount();
		while(mqtt_ctx->start_flag) {	
			if(!mqtt_receive_process(&mqtt_handle->mqtt_context)) break;
			// vTaskDelayUntil(&lastTick, 2);
			yield();
		}

	}while(0);

	mqtt_ctx->stop_flag = 1;
	if(mqtt_ctx->start_flag) {
		mqtt_transport_stop_callback(mqtt_ctx);
	}

	mqtt_ctx->task_hwm = uxTaskGetStackHighWaterMark(NULL);
	log_i("mqtt_task exit, hwm:%d", mqtt_ctx->task_hwm);
	vTaskDelete(NULL);
}

void *port_mqtt_create_context(void *owner) {
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)malloc(sizeof(port_mqtt_ctx_t));
	if(!mqtt_ctx) return NULL;
	memset(mqtt_ctx, 0, sizeof(port_mqtt_ctx_t));
	mqtt_ctx->owner = owner;
	return mqtt_ctx;
}

void port_mqtt_destroy_context(void *ctx) {
	if(ctx) {
		free(ctx);
	}
}

bool port_mqtt_set_configuration(void *ctx, port_mqtt_configuration_t *config) {
	if(!ctx || !config) return false;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	if(config->owner_valid) {
		mqtt_ctx->owner = config->owner;
	}
	return true;
}

bool port_mqtt_get_configuration(void *ctx, port_mqtt_configuration_t *config) {
	if(!ctx || !config) return false;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	config->owner = mqtt_ctx->owner;
	config->owner_valid = 1;
	return true;
}

bool port_mqtt_connect(void *ctx, const char *name, uint16_t fixed_buffer_size, uint16_t publish_max_count, uint16_t subscribe_max_count, uint32_t timeout_ms) {
	bool ret = false;
	MQTTStatus_t status;
	mqtt_prv_handle_t *mqtt_handle = NULL;

	do {
		if(!ctx) break;
		port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
		mqtt_handle = (mqtt_prv_handle_t *)malloc(sizeof(mqtt_prv_handle_t));
		if(!mqtt_handle) break;
		memset(mqtt_handle, 0, sizeof(mqtt_prv_handle_t));
		mqtt_handle->net_context.ctx = ctx;

		mqtt_handle->transport_interface.send = transport_send;
		mqtt_handle->transport_interface.recv = transport_recv;
		mqtt_handle->transport_interface.writev = transport_writev;
		mqtt_handle->transport_interface.pNetworkContext = &mqtt_handle->net_context;

		log_d("mqtt fixed_buffer_size: %d", fixed_buffer_size);
		mqtt_handle->fixed_buffer.pBuffer = (uint8_t *)calloc(fixed_buffer_size, 1);
		if(!mqtt_handle->fixed_buffer.pBuffer) break;
		mqtt_handle->fixed_buffer.size = fixed_buffer_size;
		mqtt_handle->subscribe_max_count = subscribe_max_count;
		mqtt_handle->publish_max_count = publish_max_count;
		mqtt_handle->outgoingPublishes = (MQTTPubAckInfo_t *)calloc(publish_max_count, sizeof(MQTTPubAckInfo_t));
		if(!mqtt_handle->outgoingPublishes) break;
		mqtt_handle->incomingPublishes = (MQTTPubAckInfo_t *)calloc(subscribe_max_count, sizeof(MQTTPubAckInfo_t));
		if(!mqtt_handle->incomingPublishes) break;
		mqtt_handle->subscribe_callback_list = (mqtt_subscribe_callback_item_t *)calloc(subscribe_max_count, sizeof(mqtt_subscribe_callback_item_t));
		if(!mqtt_handle->subscribe_callback_list) break;
		
		if((status = MQTT_Init(&mqtt_handle->mqtt_context, &mqtt_handle->transport_interface, mqtt_get_current_time, mqtt_event_callback, &mqtt_handle->fixed_buffer)) != MQTTSuccess)
		{
			log_e("MQTT_Init failed! <status:%d>", status);
			break;
		}

		if(!add_map_to_list(&mqtt_handle->net_context, &mqtt_handle->mqtt_context)) {
			log_e("MQTT Client count exceeds maximum limit <max:%d>!", MQTT_MAX_CLIENT_CNT);
			break;
		}

		if((status = MQTT_InitStatefulQoS( &mqtt_handle->mqtt_context, mqtt_handle->outgoingPublishes, publish_max_count, mqtt_handle->incomingPublishes, subscribe_max_count )) != MQTTSuccess)
		{
			log_e("MQTT_InitStatefulQoS failed! <status:%d>", status);
			break;
		}

		MQTTConnectInfo_t connectInfo = {0};
		MQTTPublishInfo_t willInfo = {0}; 
		
		connectInfo.pClientIdentifier = name;
		connectInfo.clientIdentifierLength = strlen(name);
		connectInfo.keepAliveSeconds = 60;
		connectInfo.cleanSession = true;

		willInfo.pTopicName = "device/offline";
		willInfo.topicNameLength = strlen("device/offline");
		willInfo.pPayload = "device_disconnected";
		willInfo.payloadLength = strlen("device_disconnected");
		willInfo.qos = MQTTQoS1;
		willInfo.retain = false;
		bool sessionPresent = false;
		// log_d("start MQTT_Connect, timeout_ms:%d", timeout_ms);
		if((status = MQTT_Connect(&mqtt_handle->mqtt_context, &connectInfo, &willInfo, timeout_ms, &sessionPresent)) != MQTTSuccess)
		{	
			log_e("MQTT_Connect failed! <status:%d>", status);
			break;
		}
		mqtt_ctx->handle = mqtt_handle;

		// log_d("mqtt_connect success, mqtt_ctx:%p, mqtt_handle:%p, mqtt_context:%p", mqtt_ctx, mqtt_handle, &mqtt_handle->mqtt_context);

		mqtt_ctx->start_flag = 1;
		TaskHandle_t mqtt_task_handle = NULL;
		if(xTaskCreate(mqtt_task, "mqtt_task", 512 + 256, mqtt_ctx, APP_TASK_PRIORITY, &mqtt_task_handle) != pdPASS) {
			log_e("mqtt_task create failed!");
			mqtt_ctx->handle = NULL;
			break;
		}
		log_i("mqtt_task create success, handle:%p", mqtt_task_handle);
		ret = true;
	}while(0);

	if(!ret) {
		if(mqtt_handle) {
			if(mqtt_handle->fixed_buffer.pBuffer) {
				free(mqtt_handle->fixed_buffer.pBuffer);
			}
			if(mqtt_handle->outgoingPublishes) {
				free(mqtt_handle->outgoingPublishes);
			}
			if(mqtt_handle->incomingPublishes) {
				free(mqtt_handle->incomingPublishes);
			}
			if(mqtt_handle->subscribe_callback_list) {
				free(mqtt_handle->subscribe_callback_list);
			}
			free(mqtt_handle);
		}
	}

	return ret;	
}

void port_mqtt_close(void *ctx) {
	if(!ctx) return;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	if(!mqtt_ctx->handle) return;
	mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
	if(!mqtt_handle) return;

	mqtt_ctx->start_flag = 0;
	while(!mqtt_ctx->stop_flag) {
		yield();
	}

	MQTT_Disconnect(&mqtt_handle->mqtt_context);
	clear_subscribe_callback_list(mqtt_handle);
	if(mqtt_handle->fixed_buffer.pBuffer) {
		free(mqtt_handle->fixed_buffer.pBuffer);
	}
	if(mqtt_handle->outgoingPublishes) {
		free(mqtt_handle->outgoingPublishes);
	}
	if(mqtt_handle->incomingPublishes) {
		free(mqtt_handle->incomingPublishes);
	}
	free(mqtt_handle);
	mqtt_ctx->handle = NULL;
}

bool port_mqtt_publish(void *ctx, const char *topic, uint8_t *payload, size_t payload_len, arduino_mqtt_qos_t qos) {
	bool ret = false;
	if(!ctx || !topic || !payload || payload_len == 0 || qos > ARDUINO_MQTT_QOS2 || qos < ARDUINO_MQTT_QOS0) return ret;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	if(!mqtt_ctx->handle) return ret;
	mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
	if(!mqtt_handle) return ret;

	MQTTPublishInfo_t publishInfo = {0};
	publishInfo.pTopicName = topic;
	publishInfo.topicNameLength = strlen(topic);
	publishInfo.pPayload = payload;
	publishInfo.payloadLength = payload_len;
	publishInfo.qos = qos;
	MQTTStatus_t status;
	// log_d("mqtt_ctx:%p, mqtt_handle:%p, mqtt_context:%p, topic:%s, qos:%d", mqtt_ctx, mqtt_handle, &mqtt_handle->mqtt_context, topic, qos);
	if((status = MQTT_Publish(&mqtt_handle->mqtt_context, &publishInfo, MQTT_GetPacketId(&mqtt_handle->mqtt_context))) != MQTTSuccess) {
		log_e("MQTT_Publish failed! <status:%d>", status);
	}
	ret = true;
	return ret;
}

bool port_mqtt_subscribe(void *ctx, const char *topic, mqtt_subscribe_callback_func callback, arduino_mqtt_qos_t qos) {
	bool ret = false;
	if(!ctx || !topic || !callback || qos > ARDUINO_MQTT_QOS2 || qos < ARDUINO_MQTT_QOS0) return ret;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	if(!mqtt_ctx->handle) return ret;
	mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
	if(!mqtt_handle) return ret;
	MQTTSubscribeInfo_t subscribeInfo = {0};
	subscribeInfo.pTopicFilter = topic;
	subscribeInfo.topicFilterLength = strlen(topic);
	subscribeInfo.qos = qos;

	MQTTStatus_t status;
	// log_d("topic:%s, qos:%d", topic, qos);
	if((status = MQTT_Subscribe(&mqtt_handle->mqtt_context, &subscribeInfo, 1, MQTT_GetPacketId(&mqtt_handle->mqtt_context))) != MQTTSuccess)
	{
		log_e("MQTT_Subscribe failed! <status=%d>", status);
		return ret;
	}

	// set subscribe callback
	if(!add_subscribe_callback(mqtt_handle, topic, callback)) {
		log_e("add_subscribe_callback failed! <max_count:%d>", mqtt_handle->subscribe_max_count);
		return ret;
	}

	ret = true;
	return ret;
}

bool port_mqtt_unsubscribe(void *ctx, const char *topic, arduino_mqtt_qos_t qos) {
	bool ret = false;
	if(!ctx || !topic || qos > ARDUINO_MQTT_QOS2 || qos < ARDUINO_MQTT_QOS0) return ret;
	port_mqtt_ctx_t_ptr mqtt_ctx = (port_mqtt_ctx_t_ptr)ctx;
	if(!mqtt_ctx->handle) return ret;
	mqtt_prv_handle_t *mqtt_handle = (mqtt_prv_handle_t *)mqtt_ctx->handle;
	if(!mqtt_handle) return ret;
	MQTTSubscribeInfo_t unsubscribeInfo = {0};
	unsubscribeInfo.pTopicFilter = topic;
	unsubscribeInfo.topicFilterLength = strlen(topic);
	unsubscribeInfo.qos = qos;

	MQTTStatus_t status;
	// log_d("topic:%s, qos:%d", topic, qos);
	if((status = MQTT_Unsubscribe(&mqtt_handle->mqtt_context, &unsubscribeInfo, 1, MQTT_GetPacketId(&mqtt_handle->mqtt_context))) != MQTTSuccess)
	{
		log_e("MQTT_Unsubscribe failed! <status=%d>", status);
		return ret;
	}

	// remove subscribe callback
	remove_subscribe_callback(mqtt_handle, topic);

	ret = true;
	return ret;
}

#endif