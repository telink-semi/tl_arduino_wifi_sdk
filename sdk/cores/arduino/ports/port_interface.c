/********************************************************************************************************
 * @file    port_interface.c
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
#include "port_interface.h"

#define __weak__ __attribute__((weak))

__weak__ int port_main(void) {
    return 0;
}

/*task related*/
__weak__ bool port_background_init(void) {
    return false;
}

__weak__ background_handle_t port_background_register_handler(background_handler_func handler, void *para, uint32_t intervalMS) {
    (void)handler;
    (void)para;
    (void)intervalMS;
    return 0;
}
__weak__ bool port_background_unregister_handler(background_handle_t handle) {
    (void)handle;
    return false;
}

__weak__ void port_background_handler_loop(void) {

}
__weak__ void port_background_fastcall_loop(void) {

}

//only call in interrupt handler
__weak__ void port_background_fastcall_handle(background_handler_func handler, void *para) {
    (void)handler;
    (void)para;
}
__weak__ void port_background_fastcall_exit(void) {

}
__weak__ task_handle_t port_create_new_task(void (*task_func)(void *), void *para, uint32_t stack_size_bytes) {
    (void)task_func;
    (void)para;
    (void)stack_size_bytes;
    return 0;
}
__weak__ bool port_delete_task(task_handle_t handle) {
    (void)handle;
    return false;
}
__weak__ size_t port_get_task_stack_high_water_mark(task_handle_t handle) {
    (void)handle;
    return 0;
}

__weak__ void* port_create_mutex(void) {
    return NULL;
}
__weak__ void port_lock_mutex(void *mutex) {
    (void)mutex;
}
__weak__ void port_unlock_mutex(void *mutex) {
    (void)mutex;
}
__weak__ void port_destroy_mutex(void *mutex) {
    (void)mutex;
}

/*serial related*/

__weak__ void *port_serial_create_context(void *owner) {
    (void)owner;
    return NULL;
}

__weak__ void port_serial_destroy_context(void *ctx) {
    (void)ctx;
}

__weak__ bool port_serial_set_configuration(void *ctx, port_serial_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_serial_get_configuration(void *ctx, port_serial_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_serial_begin(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_serial_end(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_serial_is_run(void *ctx) {
    (void)ctx;
    return false;
}

__weak__ void port_serial_console_init(uint8_t index, uint32_t baudrate) {
    (void)index;
    (void)baudrate;
}

__weak__ void port_serial_console_flush(void) {
    
}


/*filesystem related*/
__weak__ bool port_fs_init(void) {
    return false;
}
__weak__ bool port_fs_deinit(void) {
    return false;
}

__weak__ fs_file_t port_fs_file_open(const char *path, const char *mode, bool create) {
    (void)path;
    (void)mode;
    (void)create;
    return 0;
}
__weak__ bool port_fs_file_close(fs_file_t file) {
    (void)file;
    return false;
}

__weak__ const char * port_fs_file_name(const char *path) {
    (void)path;
    return NULL;
}
__weak__ const char * port_fs_get_next_file_path(void) {
    return NULL;
}
__weak__ const char * port_fs_get_next_path(bool *is_dir) {
    (void)is_dir;
    return NULL;
}

__weak__ bool port_fs_exists(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_remove(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_rename(const char *old_path, const char *new_path) {
    (void)old_path;
    (void)new_path;
    return false;
}
__weak__ bool port_fs_mkdir(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_rmdir(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_is_dir(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_dir_rewind(void) {
    return false;
}
__weak__ bool port_fs_dir_seek(uint32_t pos) {
    (void)pos;
    return false;
}
__weak__ bool port_fs_dir_open(const char *path) {
    (void)path;
    return false;
}
__weak__ bool port_fs_dir_close(void) {
    return false;
}

__weak__ size_t port_fs_file_read(fs_file_t file, uint8_t *buf, size_t buf_size) {
    (void)file;
    (void)buf;
    (void)buf_size;
    return 0;
}
__weak__ size_t port_fs_file_write(fs_file_t file, uint8_t *data, size_t len) {
    (void)file;
    (void)data;
    (void)len;
    return 0;
}

__weak__ int port_fs_file_available(fs_file_t file) {
    (void)file;
    return 0;
}
__weak__ int port_fs_file_size(fs_file_t file) {
    (void)file;
    return 0;
}
__weak__ int port_fs_file_seek(fs_file_t file, uint32_t pos, uint8_t mode) {
    (void)file;
    (void)pos;
    (void)mode;
    return 0;
}
__weak__ int port_fs_file_flush(fs_file_t file) {
    (void)file;
    return 0;
}
__weak__ int port_fs_file_position(fs_file_t file) {
    (void)file;
    return 0;
}

__weak__ uint32_t port_fs_get_last_write_time(void) {
    return 0;
}

/*audio related*/

__weak__ void *port_audio_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_audio_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_audio_set_configuration(void *ctx, port_audio_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_audio_get_configuration(void *ctx, port_audio_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_audio_begin(void *ctx, uint32_t samples, uint32_t sample_rate, bool input_enable, bool output_enable, float volume) {
    (void)ctx;
    (void)samples;
    (void)sample_rate;
    (void)input_enable;
    (void)output_enable;
    (void)volume;
    return false;
}
__weak__ bool port_audio_stop(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_audio_is_open(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_audio_start_record(void *ctx, uint32_t len) {
    (void)ctx;
    (void)len;
    return false;
}
__weak__ bool port_audio_play_busy(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_audio_record_busy(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_audio_stop_record(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_audio_start_play(void *ctx, uint32_t len) {
    (void)ctx;
    (void)len;
    return false;
}
__weak__ bool port_audio_stop_play(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ uint32_t port_audio_get_buffer_size(uint32_t samples, uint32_t sample_rate) {
    (void)samples;
    (void)sample_rate;
    return 0;
}
__weak__ uint32_t port_audio_get_frame_size(void *ctx) {
    (void)ctx;
    return 0;
}
__weak__ bool port_audio_prepare_vad(uint16_t frame_size) {
    (void)frame_size;
    return false;
}
__weak__ bool port_audio_do_vad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max) {
    (void)data;
    (void)len;
    (void)rms_enable;
    (void)zcr_enable;
    (void)rms_threshold;
    (void)zcr_threshold_min;
    (void)zcr_threshold_max;
    return false;
}

/*network related*/
__weak__ void* port_network_udp_begin(uint8_t type, ip_addr_t *ip, uint16_t port, uint32_t recvbufsize) {
    (void)type;
    (void)ip;
    (void)port;
    (void)recvbufsize;
    return NULL;
}
__weak__ void* port_network_udp_begin_packet(uint8_t type, uint32_t recvbufsize) {
    (void)type;
    (void)recvbufsize;
    return NULL;
}
__weak__ bool port_network_udp_end_packet(void *udp_server, ip_addr_t *ip, uint16_t port, uint8_t *data, uint32_t len) {
    (void)udp_server;
    (void)ip;
    (void)port;
    (void)data;
    (void)len;
    return false;
}
__weak__ bool port_network_udp_add_multicast(void *udp_server, ip_addr_t* multicast_ip) {
    (void)udp_server;
    (void)multicast_ip;
    return false;
}
__weak__ int port_network_udp_parse_packet(void *udp_server, ip_addr_t *ip, uint16_t *port, uint8_t *data, uint32_t len) {
    (void)udp_server;
    (void)ip;
    (void)port;
    (void)data;
    (void)len;
    return 0;
}
__weak__ void port_network_stop(void *server) {
    (void)server;
}
__weak__ const uint8_t * port_network_get_host_by_name(const char *host) {
    (void)host;
    return NULL;
}
__weak__ void port_network_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw, struct netif *netif) {
    (void)ip;
    (void)nm;
    (void)gw;
    (void)netif;
}
__weak__ void port_network_get_ip_addr(ip_addr_t *ip, struct netif *netif) {
    (void)ip;
    (void)netif;
}
__weak__ void port_network_get_dns_server(ip_addr_t *ip, uint8_t index) {
    (void)ip;
    (void)index;
}
__weak__ void port_network_set_dns_server(ip_addr_t *ip, uint8_t index) {
    (void)ip;
    (void)index;
}
__weak__ void port_network_set_host_name(const char *name, struct netif *netif) {
    (void)name;
    (void)netif;
}
__weak__ const char* port_network_get_host_name(struct netif *netif) {
    (void)netif;
    return NULL;
}
__weak__ void port_network_get_gateway(ip_addr_t *gw, struct netif *netif) {
    (void)gw;
    (void)netif;
}
__weak__ void port_network_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN], struct netif *netif) {
    (void)mac;
    (void)netif;
}
__weak__ void port_network_get_subnet_mask(ip_addr_t *mask, struct netif *netif) {
    (void)mask;
    (void)netif;
}
__weak__ void port_network_get_broadcast_addr(ip_addr_t *broadcast, struct netif *netif) {
    (void)broadcast;
    (void)netif;
}
__weak__ void port_network_get_id(ip_addr_t *id, struct netif *netif) {
    (void)id;
    (void)netif;
}
__weak__ uint8_t port_network_get_subnet_cdir(struct netif *netif) {
    (void)netif;
    return 0;
}
__weak__ bool port_network_get_info(void *handle, ip_addr_t *ip, uint16_t *port, bool local) {
    (void)handle;
    (void)ip;
    (void)port;
    (void)local;
    return false;
}

__weak__ void *port_tcp_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_tcp_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_tcp_set_configuration(void *ctx, port_tcp_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_tcp_get_configuration(void *ctx, port_tcp_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_tcp_connect(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ int port_tcp_read(void *ctx, uint8_t *buffer, size_t len) {
    (void)ctx;
    (void)buffer;
    (void)len;
    return 0;
}
__weak__ int port_tcp_write(void *ctx, uint8_t *data, size_t len) {
    (void)ctx;
    (void)data;
    (void)len;
    return 0;
}
__weak__ bool port_tcp_close(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_tcp_is_opened(void *ctx) {
    (void)ctx;
    return false;
}

__weak__ void *port_http_server_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_http_server_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_http_server_set_configuration(void *ctx, port_http_server_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_http_server_get_configuration(void *ctx, port_http_server_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_http_server_start(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_http_server_stop(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_http_server_get(void *ctx, const char *url, http_server_handler_func handler) {
    (void)ctx;
    (void)url;
    (void)handler;
    return false;
}
__weak__ bool port_http_server_post(void *ctx, const char *url, http_server_handler_func handler) {
    (void)ctx;
    (void)url;
    (void)handler;
    return false;
}
//parse request, need to be free
__weak__ char * port_http_server_get_header(void *ctx, const char *key) {
    (void)ctx;
    (void)key;
    return NULL;
}
__weak__ char * port_http_server_get_url_query(void *ctx, const char *key) {
    (void)ctx;
    (void)key;
    return NULL;
}
__weak__ size_t port_http_server_get_content_length(void *ctx) {
    (void)ctx;
    return 0;
}
__weak__ int port_http_server_recv(void *ctx, void *buf, size_t buf_size) {
    (void)ctx;
    (void)buf;
    (void)buf_size;
    return 0;
}
//send resp
__weak__ bool port_http_server_resp_set_header(void *ctx, const char *key, const char *value) {
    (void)ctx;
    (void)key;
    (void)value;
    return false;
}
__weak__ bool port_http_server_resp_send(void *ctx, void *buf, size_t len) {
    (void)ctx;
    (void)buf;
    (void)len;
    return false;
}
__weak__ bool port_http_server_resp_send_chunk(void *ctx, void *buf, size_t len) {
    (void)ctx;
    (void)buf;
    (void)len;
    return false;
}
__weak__ bool port_http_server_resp_send_error(void *ctx, uint32_t error, const char *msg) {
    (void)ctx;
    (void)error;
    (void)msg;
    return false;
}
__weak__ bool port_http_server_redirect(void *ctx, const char *path) {
    (void)ctx;
    (void)path;
    return false;
}

/*http client related*/

__weak__ void *port_http_client_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_http_client_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_http_client_set_configuration(void *ctx, port_http_client_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_http_client_get_configuration(void *ctx, port_http_client_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_http_client_get(void *ctx, const char *url, void (*set_headers)(void*, void *), 
                                   port_http_client_callbacks_t *callbacks) {
    (void)ctx;
    (void)url;
    (void)set_headers;
    (void)callbacks;
    return false;
}
__weak__ bool port_http_client_post(void *ctx, const char *url, void (*set_headers)(void*, void *), uint8_t *post_data, uint32_t post_data_len, 
                                   port_http_client_callbacks_t *callbacks) {
    (void)ctx;
    (void)url;
    (void)set_headers;
    (void)post_data;
    (void)post_data_len;
    (void)callbacks;
    return false;
}
__weak__ void port_http_client_set_header(void *handle, const char *key, const char *value) {
    (void)handle;
    (void)key;
    (void)value;
}


/*wifi related*/
__weak__ bool port_wifi_sta_start(void) {
    return false;
}
__weak__ bool port_wifi_sta_begin(const char *ssid, const char *key, int channel, const uint8_t *bssid) {
    (void)ssid;
    (void)key;
    (void)channel;
    (void)bssid;
    return false;
}
__weak__ bool port_wifi_sta_connect(void) {
    return false;
}
__weak__ bool port_wifi_sta_is_connected(void) {
    return false;
}
__weak__ bool port_wifi_sta_disconnect(void) {
    return false;
}
__weak__ bool port_wifi_sta_end(void) {
    return false;
}
__weak__ bool port_wifi_sta_wait_for_connected(uint32_t timeout_ms) {
    (void)timeout_ms;
    return false;
}
__weak__ bool port_wifi_sta_wait_for_disconnected(uint32_t timeout_ms) {
    (void)timeout_ms;
    return false;
}
__weak__ bool port_wifi_sta_erase_ap(void) {
    return false;
}
__weak__ bool port_wifi_set_sta_channel(uint8_t channel) {
    (void)channel;
    return false;
}
__weak__ bool port_wifi_get_sta_channel(uint8_t *channel) {
    (void)channel;
    return false;
}
__weak__ void port_wifi_sta_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw) {
    (void)ip;
    (void)nm;
    (void)gw;
}
__weak__ int8_t port_wifi_sta_get_rssi(void) {
    return 0;
}
__weak__ wifi_bandwidth_t port_wifi_sta_get_bandwidth(void) {
    return WIFI_BW_HT20;
}
__weak__ bool port_wifi_sta_set_bandwidth(wifi_bandwidth_t bandwidth) {
    (void)bandwidth;
    return false;
}
__weak__ const char *port_wifi_sta_get_ssid(void) {
    return NULL;
}
__weak__ const char *port_wifi_sta_get_psk(void) {
    return NULL;
}
__weak__ const uint8_t *port_wifi_sta_get_bssid(void) {
    return NULL;
}
__weak__ void port_wifi_sta_set_min_auth_mode(wifi_auth_mode_t mode) {
    (void)mode;
}
__weak__ void port_wifi_sta_get_ip_addr(ip_addr_t *ip) {
    (void)ip;
}
__weak__ void port_wifi_sta_get_gateway(ip_addr_t *gw) {
    (void)gw;
}
__weak__ void port_wifi_sta_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN]) {
    (void)mac;
}
__weak__ void port_wifi_sta_get_subnet_mask(ip_addr_t *mask) {
    (void)mask;
}
__weak__ void port_wifi_sta_get_broadcast_addr(ip_addr_t *broadcast) {
    (void)broadcast;
}
__weak__ void port_wifi_sta_get_id(ip_addr_t *id) {
    (void)id;
}
__weak__ uint8_t port_wifi_sta_get_subnet_cdir(void) {
    return 0;
}
__weak__ bool port_wifi_ap_start(const char *ssid, const char *key, int channel, int ssid_hidden, int max_count, wifi_auth_mode_t auth_mode, wifi_cipher_type_t cipher) {
    (void)ssid;
    (void)key;
    (void)channel;
    (void)ssid_hidden;
    (void)max_count;
    (void)auth_mode;
    (void)cipher;
    return false;
}
__weak__ void port_wifi_ap_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw) {
    (void)ip;
    (void)nm;
    (void)gw;
}
__weak__ void port_wifi_ap_config_dhcp(ip_addr_t *start) {
    (void)start;
}
__weak__ void port_wifi_ap_config_dns(ip_addr_t *dns) {
    (void)dns;
}
__weak__ uint8_t* port_wifi_ap_get_mac_addr(void) {
    return NULL;
}
__weak__ char* port_wifi_ap_get_ssid(void) {
    return NULL;
}
__weak__ uint8_t port_wifi_ap_get_station_number(void) {
    return 0;
}
__weak__ bool port_wifi_ap_set_bandwidth(wifi_bandwidth_t bandwidth) {
    (void)bandwidth;
    return false;
}
__weak__ bool port_wifi_ap_set_hostname(const char* name) {
    (void)name;
    return false;
}
__weak__ const char* port_wifi_ap_get_hostname(void) {
    return NULL;
}
__weak__ bool port_wifi_ap_is_start(void) {
    return false;
}
__weak__ bool port_wifi_ap_stop(void) {
    return false;
}
__weak__ bool port_wifi_ap_clear(void) {
    return false;
}
__weak__ void port_wifi_ap_get_ip_addr(ip_addr_t *ip) {
    (void)ip;
}
__weak__ void port_wifi_ap_get_gateway(ip_addr_t *gw) {
    (void)gw;
}
__weak__ void port_wifi_ap_get_subnet_mask(ip_addr_t *mask) {
    (void)mask;
}
__weak__ void port_wifi_ap_get_broadcast_addr(ip_addr_t *broadcast) {
    (void)broadcast;
}
__weak__ void port_wifi_ap_get_id(ip_addr_t *id) {
    (void)id;
}
__weak__ uint8_t port_wifi_ap_get_subnet_cdir(void) {
    return 0;
}

__weak__ void port_wifi_scan_test(void) {

}

__weak__ int16_t port_wifi_scan(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan, uint8_t channel, const char *ssid, const uint8_t *bssid) {
    (void)async;
    (void)show_hidden;
    (void)passive;
    (void)max_ms_per_chan;
    (void)channel;
    (void)ssid;
    (void)bssid;
    return 0;
} 
__weak__ void port_wifi_scan_delete(void) {

}
__weak__ void port_wifi_scan_set_timeout(uint32_t ms) {
    (void)ms;
}
__weak__ void port_wifi_scan_set_active_min_time(uint32_t ms) {
    (void)ms;
}
__weak__ int16_t port_wifi_scan_compelete(void) {
    return 0;
}
__weak__ bool port_wifi_get_network_info_by_index(uint16_t index, char *ssid, uint8_t *encryptionType, int32_t *RSSI, uint8_t *BSSID, int32_t *channel) {
    (void)index;
    (void)ssid;
    (void)encryptionType;
    (void)RSSI;
    (void)BSSID;
    (void)channel;
    return false;
}

/*websocket related*/

__weak__  void *port_websocket_create_context(void *owner) {
    (void)owner;
    return NULL;
}

__weak__ void port_websocket_destroy_context(void *ctx) {
    (void)ctx;
}

__weak__ bool port_websocket_set_configuration(void *ctx, port_websocket_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_websocket_get_configuration(void *ctx, port_websocket_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_websocket_connect(void *ctx) {
    (void)ctx;
    return false;
}

__weak__ int port_websocket_write(void *ctx, uint8_t *data, size_t len) {
    (void)ctx;
    (void)data;
    (void)len;
    return 0;
}

__weak__ int port_websocket_read(void *ctx, uint8_t *buffer, size_t len) {
    (void)ctx;
    (void)buffer;
    (void)len;
    return 0;
}

__weak__ bool port_websocket_close(void *ctx) {
    (void)ctx;
    return false;
}

__weak__ bool port_websocket_is_opened(void *ctx) {
    (void)ctx;
    return false;
}

/*mqtt related*/

__weak__ void *port_mqtt_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_mqtt_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_mqtt_set_configuration(void *ctx, port_mqtt_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_mqtt_get_configuration(void *ctx, port_mqtt_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_mqtt_connect(void *ctx, const char *name, uint16_t fixed_buffer_size, uint16_t publish_max_count, uint16_t subscribe_max_count, uint32_t timeout_ms) {
    (void)ctx;
    (void)name;
    (void)fixed_buffer_size;
    (void)publish_max_count;
    (void)subscribe_max_count;
    (void)timeout_ms;
    return false;
}
__weak__ void port_mqtt_close(void *ctx) {
    (void)ctx;
}
__weak__ bool port_mqtt_publish(void *ctx, const char *topic, uint8_t *payload, size_t payload_len, arduino_mqtt_qos_t qos) {
    (void)ctx;
    (void)topic;
    (void)payload;
    (void)payload_len;
    (void)qos;
    return false;
}
__weak__ bool port_mqtt_subscribe(void *ctx, const char *topic, mqtt_subscribe_callback_func callback, arduino_mqtt_qos_t qos) {
    (void)ctx;
    (void)topic;
    (void)callback;
    (void)qos;
    return false;
}
__weak__ bool port_mqtt_unsubscribe(void *ctx, const char *topic, arduino_mqtt_qos_t qos) {
    (void)ctx;
    (void)topic;
    (void)qos;
    return false;
}

/*tflm related*/
__weak__ bool port_tflm_reset(const char ** catergroy_labels, int category_labels_count, const unsigned char * model_data) {
    (void)catergroy_labels;
    (void)category_labels_count;
    (void)model_data;
    return false;
}
__weak__ bool port_tflm_init_feature(void) {
    return false;
}
__weak__ bool port_tflm_cal_feature(uint8_t * data, uint32_t len) {
    (void)data;
    (void)len;
    return false;
}
__weak__ bool port_tflm_detect(int expected_label_index) {
    (void)expected_label_index;
    return false;
}

/*opus related*/
__weak__ int port_opus_get_version(void) {
    return 0;
}
__weak__ bool port_opus_prepare_encoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame, uint8_t application,
                          uint32_t bit_rate, uint8_t complexity, bool use_vbr) {
    (void)channels;
    (void)sample_rate;
    (void)samples_per_frame;
    (void)application;
    (void)bit_rate;
    (void)complexity;
    (void)use_vbr;
    return false;
}
__weak__ bool port_opus_prepare_decoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame) {
    (void)channels;
    (void)sample_rate;
    (void)samples_per_frame;
    return false;
}
__weak__ bool port_opus_encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    (void)input;
    (void)input_len;
    (void)output;
    (void)max_output_len;
    (void)output_len;
    return false;
}
__weak__ bool port_opus_decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    (void)input;
    (void)input_len;
    (void)output;
    (void)max_output_len;
    (void)output_len;
    return false;
}

/*system related*/
__weak__ void port_system_restart(void) {

}
__weak__ uint32_t port_system_get_free_heap_size(void) {
    return 0;
}
__weak__ uint32_t port_system_get_heap_size_low_level(void) {
    return 0;
}
__weak__ uint32_t port_system_random(void) {
    return 0;
}
__weak__ void port_system_register_handler(void (*handler)(void *), void * para) {
    (void)handler;
    (void)para;
}
__weak__ void port_system_d25_init(void) {

}

__weak__ void port_system_d25_reset(void) {

}
__weak__ uint32_t port_system_d25_get_stack_free_size_low_level(void) {
    return 0;
}
__weak__ void port_system_d25_log_level(uint8_t level) {
    (void)level;
}
__weak__ uint32_t port_millis(void) {
    return 0;
}
__weak__ uint32_t port_micros(void) {
    return 0;
}
__weak__ void port_delay_us(unsigned int us) {
    (void)us;
}
__weak__ void port_delay_ms(unsigned int ms) {
    (void)ms;
}
__weak__ void port_yield(void) {

}
__weak__ void port_exit(int status) {
    (void)status;
}
__weak__ int port_atexit(void (*func)(void)) {
    (void)func;
    return 0;
}
__weak__ void port_interrupts(void) {

}
__weak__ void port_no_interrupts(void) {

}

/*i2c related*/

__weak__ void *port_i2c_create_context(void *owner) {
    (void)owner;
    return NULL;
}
__weak__ void port_i2c_destroy_context(void *ctx) {
    (void)ctx;
}
__weak__ bool port_i2c_set_configuration(void *ctx, port_i2c_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}
__weak__ bool port_i2c_get_configuration(void *ctx, port_i2c_configuration_t *config) {
    (void)ctx;
    (void)config;
    return false;
}

__weak__ bool port_i2c_begin(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_i2c_end(void *ctx) {
    (void)ctx;
    return false;
}
__weak__ bool port_i2c_master_start_tx(void *ctx, uint8_t address, uint8_t *buf, uint32_t len) {
    (void)ctx;
    (void)address;
    (void)buf;
    (void)len;
    return false;
}
__weak__ bool port_i2c_master_start_rx(void *ctx, uint8_t address, uint32_t len) {
    (void)ctx;
    (void)address;
    (void)len;
    return false;
}
__weak__ bool port_i2c_master_start_trx(void *ctx, uint8_t address, uint8_t *tx_buf, uint32_t tx_len, uint32_t rx_len) {
    (void)ctx;
    (void)address;
    (void)tx_buf;
    (void)tx_len;
    (void)rx_len;
    return false;
}
__weak__ bool port_i2c_master_ask(void *ctx, i2c_master_opt_t *opt, uint32_t *len, uint8_t *buf, uint32_t bufLen) {
    (void)ctx;
    (void)opt;
    (void)len;
    (void)buf;
    (void)bufLen;
    return false;
}
__weak__ void port_i2c_master_clear_timeout(void *ctx) {
    (void)ctx;
}
__weak__ bool port_i2c_master_probe(void *ctx, uint8_t address) {
    (void)ctx;
    (void)address;
    return false;
}
__weak__ bool port_i2c_is_run(void *ctx) {
    (void)ctx;
    return false;
}

/*digital related*/
__weak__ void port_pin_mode(uint8_t pin, uint8_t mode) {
    (void)pin;
    (void)mode;
}
__weak__ void port_digital_write(uint8_t pin, uint8_t val) {
    (void)pin;
    (void)val;
}
__weak__ int port_digital_read(uint8_t pin) {
    (void)pin;
    return 0;
}

/*analog related*/
__weak__ void port_analog_reference(uint8_t mode) {
    (void)mode;
}
__weak__ int port_analog_read(uint8_t pin) {
    (void)pin;
    return 0;
}
__weak__ void port_analog_write(uint8_t pin, int val) {
    (void)pin;
    (void)val;
}

/*pulse related*/
__weak__ unsigned long port_pulse_in(uint8_t pin, uint8_t state, unsigned long timeout) {
    (void)pin;
    (void)state;
    (void)timeout;
    return 0;
}

/*tone related*/
__weak__ void port_tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    (void)pin;
    (void)frequency;
    (void)duration;
}
__weak__ void port_no_tone(uint8_t pin) {
    (void)pin;
}

/*base64 related*/
__weak__ int port_base64_encode(const void *data, int length, char *buffer, uint8_t flags) {
    (void)data;
    (void)length;
    (void)buffer;
    (void)flags;
    return 0;
}
