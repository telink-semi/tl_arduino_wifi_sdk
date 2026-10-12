/*******************************************************************************************************
 *
 * @file    tlsr_channel.h
 *
 * @brief   This is the source file for TLSR9118
 *
 * @author  
 * @par     Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * @author  
 * @par     Copyright 2023-2025 Senscomm Semiconductor Co., Ltd.
 * 
 * @author  Telink WIFI GROUP
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 
#ifndef __WIFI_CHANNEL_API_H__
#define __WIFI_CHANNEL_API_H__

#include "wits_channel.h"
#include "wits_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int tlsr_channel_register_rx_cb(wits_channel_cb_fn fn);
int tlsr_channel_send_to_host(char *buf, int len);
int tlsr_channel_construct_ip_msg(char *net_if, char *msg, int *msg_len);
int tlsr_channel_add_filter(char *filter, wifi_filter_type type);
int tlsr_channel_del_filter(char *filter, wifi_filter_type type);
int tlsr_channel_query_filter(char **filter, int *num, wifi_filter_type type);
int tlsr_channel_set_default_filter(wifi_packet_filter direction);
int tlsr_channel_reset_filter(wifi_filter_type type);
uint32_t tlsr_channel_host_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_CHANNEL_API_H__ */
