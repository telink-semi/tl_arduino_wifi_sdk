/********************************************************************************************************
 * @file    port_interface.h
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


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "port_common.h"
#include "util/util_common.h"
#include "util/stdlib_noniso.h"
#include "third_party/littlefs/lfs.h"


#define APP_TASK_PRIORITY       3
#define NETWORK_MAC_ADDR_LEN    6

#define F_CPU                   PORT_CPU_FREQ_HZ

#define VTPTR32(object) (*((uint32_t *)(&(object))))
#define COPY_VTPTR(dst, src) (VTPTR32(dst) = VTPTR32(src))

#define BASE64_ENCODE_SIZE(__size) (((((__size) - 1) / 3) * 4) + 4)

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

typedef void * task_handle_t;
typedef void *background_handle_t;
typedef void (*background_handler_func)(void *para);
typedef void *fs_file_t;
typedef void *serial_task_handle_t; 

#ifndef byte
#define byte uint8_t
#endif


enum IPType {
  IPv4,
  IPv6
};

enum {
    HTTP_SERVER_HANDLER_OK = 0,
    HTTP_SERVER_HANDLER_FAILED = -1,
};

typedef int (*http_server_handler_func)(void *ctx);

typedef struct {
    const char *path;
    const char *headers;
}websocket_info_t, *websocket_info_t_ptr;

typedef enum
{
    MQTT_TRANSPORT_TYPE_NONE = 0,
    MQTT_TRANSPORT_TYPE_TCP = 1,
    MQTT_TRANSPORT_TYPE_WS = 2,
}mqtt_transport_type_t;

typedef void (*mqtt_subscribe_callback_func)(uint8_t *payload, size_t payload_len);

typedef enum {
    ARDUINO_MQTT_QOS0 = 0,
    ARDUINO_MQTT_QOS1 = 1,
    ARDUINO_MQTT_QOS2 = 2,
}arduino_mqtt_qos_t;

typedef enum {
    ARDUINO_AUDIO_WL_16 = 0,
    ARDUINO_AUDIO_WL_20,
    ARDUINO_AUDIO_WL_24,
} arduino_audio_word_length_t;


#define I2C_RX_BUFFER_SIZE          (32)
#define I2C_TX_BUFFER_SIZE          (32)
#define I2C_DEFAULT_ADDRESS         (0x5A)
#define I2C_EVENT_BUFFER_SIZE       (4)
#define I2C_DEFAULT_FREQ            (40000)
#define I2C_DEFAULT_TIMEOUT         (1000)

typedef enum {
  I2C_MASTER_NONE,
  I2C_MASTER_TX,
  I2C_MASTER_RX,
}i2c_master_opt_t;

#ifdef __cplusplus
extern "C" {
#endif

int port_main(void);

/*task related*/
bool port_background_init(void);
background_handle_t port_background_register_handler(background_handler_func handler, void *para, uint32_t intervalMS);
bool port_background_unregister_handler(background_handle_t handle);
void port_background_handler_loop(void);
void port_background_fastcall_loop(void);

//only call in interrupt handler
void port_background_fastcall_handle(background_handler_func handler, void *para);
void port_background_fastcall_exit(void);
task_handle_t port_create_new_task(void (*task_func)(void *), void *para, uint32_t stack_size_bytes);
bool port_delete_task(task_handle_t handle);
size_t port_get_task_stack_high_water_mark(task_handle_t handle);

void* port_create_mutex(void);
void port_lock_mutex(void *mutex);
void port_unlock_mutex(void *mutex);
void port_destroy_mutex(void *mutex);

/*serial related*/

typedef struct {
    //configuration
    void *owner;
    uint32_t baudrate;
    uint8_t attribute_config;
    uint8_t serial_index;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t baudrate_valid:1;
    uint8_t attribute_config_valid:1; 
    uint8_t serial_index_valid:1;
} port_serial_configuration_t;

void *port_serial_create_context(void *owner);
void port_serial_destroy_context(void *ctx);
bool port_serial_set_configuration(void *ctx, port_serial_configuration_t *config);
bool port_serial_get_configuration(void *ctx, port_serial_configuration_t *config);
bool port_serial_begin(void *ctx);
bool port_serial_end(void *ctx);
bool port_serial_is_run(void *ctx);
void port_serial_console_init(uint8_t index, uint32_t baudrate);
void port_serial_console_flush(void);


/*filesystem related*/
bool port_fs_init(void);
bool port_fs_deinit(void);

fs_file_t port_fs_file_open(const char *path, const char *mode, bool create);
bool port_fs_file_close(fs_file_t file);

const char *port_fs_file_name(const char *path);
const char *port_fs_get_next_file_path(void);
const char *port_fs_get_next_path(bool *is_dir);

bool port_fs_exists(const char *path);
bool port_fs_remove(const char *path);
bool port_fs_rename(const char *old_path, const char *new_path);
bool port_fs_mkdir(const char *path);
bool port_fs_rmdir(const char *path);
bool port_fs_is_dir(const char *path);
bool port_fs_dir_rewind(void);
bool port_fs_dir_seek(uint32_t pos);
bool port_fs_dir_open(const char *path);
bool port_fs_dir_close(void);

size_t port_fs_file_read(fs_file_t file, uint8_t *buf, size_t buf_size);
size_t port_fs_file_write(fs_file_t file, uint8_t *data, size_t len);

int port_fs_file_available(fs_file_t file);
int port_fs_file_size(fs_file_t file);
int port_fs_file_seek(fs_file_t file, uint32_t pos, uint8_t mode);
int port_fs_file_flush(fs_file_t file);
int port_fs_file_position(fs_file_t file);

uint32_t port_fs_get_last_write_time(void);

/*audio related*/

typedef struct {
    //configuration
    void *owner;
    //valid flag
    uint8_t owner_valid:1;
} port_audio_configuration_t;

void *port_audio_create_context(void *owner);
void port_audio_destroy_context(void *ctx);
bool port_audio_set_configuration(void *ctx, port_audio_configuration_t *config);
bool port_audio_get_configuration(void *ctx, port_audio_configuration_t *config);
bool port_audio_begin(void *ctx, uint32_t samples, uint32_t sample_rate, bool input_enable, bool output_enable, float volume);
bool port_audio_stop(void *ctx);
bool port_audio_is_open(void *ctx);
bool port_audio_start_record(void *ctx, uint32_t len);
bool port_audio_play_busy(void *ctx);
bool port_audio_record_busy(void *ctx);
bool port_audio_stop_record(void *ctx);
bool port_audio_start_play(void *ctx, uint32_t len);
bool port_audio_stop_play(void *ctx);
uint32_t port_audio_get_buffer_size(uint32_t samples, uint32_t sample_rate);
uint32_t port_audio_get_frame_size(void *ctx);
bool port_audio_prepare_vad(uint16_t frame_size);
bool port_audio_do_vad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max);

/*network related*/
void* port_network_udp_begin(uint8_t type, ip_addr_t *ip, uint16_t port, uint32_t recvbufsize);
void* port_network_udp_begin_packet(uint8_t type, uint32_t recvbufsize);
bool port_network_udp_end_packet(void *udp_server, ip_addr_t *ip, uint16_t port, uint8_t *data, uint32_t len);
bool port_network_udp_add_multicast(void *udp_server, ip_addr_t* multicast_ip);
int port_network_udp_parse_packet(void *udp_server, ip_addr_t *ip, uint16_t *port, uint8_t *data, uint32_t len);
void port_network_stop(void *server);
const uint8_t * port_network_get_host_by_name(const char *host);
void port_network_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw, struct netif *netif);
void port_network_get_ip_addr(ip_addr_t *ip, struct netif *netif);
void port_network_get_dns_server(ip_addr_t *ip, uint8_t index);
void port_network_set_dns_server(ip_addr_t *ip, uint8_t index);
void port_network_set_host_name(const char *name, struct netif *netif);
const char* port_network_get_host_name(struct netif *netif);
void port_network_get_gateway(ip_addr_t *gw, struct netif *netif);
void port_network_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN], struct netif *netif);
void port_network_get_subnet_mask(ip_addr_t *mask, struct netif *netif);
void port_network_get_broadcast_addr(ip_addr_t *broadcast, struct netif *netif);
void port_network_get_id(ip_addr_t *id, struct netif *netif);
uint8_t port_network_get_subnet_cdir(struct netif *netif);
bool port_network_get_info(void *handle, ip_addr_t *ip, uint16_t *port, bool local);

typedef struct {
    //configuration
    void *owner;
    ip_addr_t ip;
    uint16_t port;
    uint32_t recon_time_ms;
    uint32_t timeout_ms;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t ip_valid:1;
    uint8_t port_valid:1;
    uint8_t recon_time_ms_valid:1;
    uint8_t timeout_ms_valid:1;
} port_tcp_configuration_t;

void *port_tcp_create_context(void *owner);
void port_tcp_destroy_context(void *ctx);
bool port_tcp_set_configuration(void *ctx, port_tcp_configuration_t *config);
bool port_tcp_get_configuration(void *ctx, port_tcp_configuration_t *config);
bool port_tcp_connect(void *ctx);
int port_tcp_read(void *ctx, uint8_t *buffer, size_t len);
int port_tcp_write(void *ctx, uint8_t *data, size_t len);
bool port_tcp_close(void *ctx);
bool port_tcp_is_opened(void *ctx);

/*http related*/

typedef struct {
    //configuration
    void *owner;
    uint16_t port;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t port_valid:1;
} port_http_server_configuration_t;

void *port_http_server_create_context(void *owner);
void port_http_server_destroy_context(void *ctx);
bool port_http_server_set_configuration(void *ctx, port_http_server_configuration_t *config);
bool port_http_server_get_configuration(void *ctx, port_http_server_configuration_t *config);

//start/stop
bool port_http_server_start(void *ctx);
bool port_http_server_stop(void *ctx);
bool port_http_server_get(void *ctx, const char *url, http_server_handler_func handler);
bool port_http_server_post(void *ctx, const char *url, http_server_handler_func handler);
//parse request, need to be free
char * port_http_server_get_header(void *ctx, const char *key);
char * port_http_server_get_url_query(void *ctx, const char *key);
size_t port_http_server_get_content_length(void *ctx);
int port_http_server_recv(void *ctx, void *buf, size_t buf_size);
//send resp
bool port_http_server_resp_set_header(void *ctx, const char *key, const char *value);
bool port_http_server_resp_send(void *ctx, void *buf, size_t len);
bool port_http_server_resp_send_chunk(void *ctx, void *buf, size_t len);
bool port_http_server_resp_send_error(void *ctx, uint32_t error, const char *msg);
bool port_http_server_redirect(void *ctx, const char *path); 

/*http client related*/
typedef struct {
    //configuration
    void *owner;
    const char *user_agent;
    uint32_t timeout_ms;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t user_agent_valid:1;
    uint8_t timeout_ms_valid:1;
} port_http_client_configuration_t;

typedef struct {
    void (*on_header_sent)(void *);
    void (*on_header)(const char*, const char*, void*);
    void (*on_data)(uint8_t*, uint32_t, void *);
    void *user_data;
} port_http_client_callbacks_t;

void *port_http_client_create_context(void *owner);
void port_http_client_destroy_context(void *ctx);
bool port_http_client_set_configuration(void *ctx, port_http_client_configuration_t *config);
bool port_http_client_get_configuration(void *ctx, port_http_client_configuration_t *config);
bool port_http_client_get(void *ctx, const char *url, void (*set_headers)(void*, void *), 
                          port_http_client_callbacks_t *callbacks);
bool port_http_client_post(void *ctx, const char *url, void (*set_headers)(void*, void *), uint8_t *post_data, uint32_t post_data_len, 
                           port_http_client_callbacks_t *callbacks);
void port_http_client_set_header(void *handle, const char *key, const char *value);

/*wifi related*/
bool port_wifi_sta_start(); 
bool port_wifi_sta_begin(const char *ssid, const char *key, int channel, const uint8_t *bssid);
bool port_wifi_sta_connect(void); 
bool port_wifi_sta_is_connected(void); 
bool port_wifi_sta_disconnect(void);
bool port_wifi_sta_end(void);
bool port_wifi_sta_wait_for_connected(uint32_t timeout_ms);
bool port_wifi_sta_wait_for_disconnected(uint32_t timeout_ms);
bool port_wifi_sta_erase_ap(void);
bool port_wifi_set_sta_channel(uint8_t channel);
bool port_wifi_get_sta_channel(uint8_t *channel);
void port_wifi_sta_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw);

int8_t port_wifi_sta_get_rssi(void);
wifi_bandwidth_t port_wifi_sta_get_bandwidth(void);
bool port_wifi_sta_set_bandwidth(wifi_bandwidth_t bandwidth);
const char *port_wifi_sta_get_ssid(void);
const char *port_wifi_sta_get_psk(void);
const uint8_t *port_wifi_sta_get_bssid(void);
void port_wifi_sta_set_min_auth_mode(wifi_auth_mode_t mode);
void port_wifi_sta_get_ip_addr(ip_addr_t *ip);
void port_wifi_sta_get_gateway(ip_addr_t *gw);
void port_wifi_sta_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN]);
void port_wifi_sta_get_subnet_mask(ip_addr_t *mask);
void port_wifi_sta_get_broadcast_addr(ip_addr_t *broadcast);
void port_wifi_sta_get_id(ip_addr_t *id);
uint8_t port_wifi_sta_get_subnet_cdir(void);

bool port_wifi_ap_start(const char *ssid, const char *key, int channel, int ssid_hidden, int max_count, wifi_auth_mode_t auth_mode, wifi_cipher_type_t cipher);
void port_wifi_ap_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw);
void port_wifi_ap_config_dhcp(ip_addr_t *start);
void port_wifi_ap_config_dns(ip_addr_t *dns);
uint8_t* port_wifi_ap_get_mac_addr(void);
char* port_wifi_ap_get_ssid(void);
uint8_t port_wifi_ap_get_station_number(void);
bool port_wifi_ap_set_bandwidth(wifi_bandwidth_t bandwidth);
bool port_wifi_ap_set_hostname(const char* name);
const char* port_wifi_ap_get_hostname();
bool port_wifi_ap_is_start(void);
bool port_wifi_ap_stop(void);
bool port_wifi_ap_clear(void);
void port_wifi_ap_get_ip_addr(ip_addr_t *ip);
void port_wifi_ap_get_gateway(ip_addr_t *gw);
void port_wifi_ap_get_subnet_mask(ip_addr_t *mask);
void port_wifi_ap_get_broadcast_addr(ip_addr_t *broadcast);
void port_wifi_ap_get_id(ip_addr_t *id);
uint8_t port_wifi_ap_get_subnet_cdir(void);

void port_wifi_scan_test(void);

int16_t port_wifi_scan(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan, uint8_t channel, const char *ssid, const uint8_t *bssid); 
void port_wifi_scan_delete(void);
void port_wifi_scan_set_timeout(uint32_t ms);
void port_wifi_scan_set_active_min_time(uint32_t ms);
int16_t port_wifi_scan_compelete(void);
bool port_wifi_get_network_info_by_index(uint16_t index, char *ssid, uint8_t *encryptionType, int32_t *RSSI, uint8_t *BSSID, int32_t *channel);

/*websocket related*/
typedef struct {
    //configuration
    void *owner;
    ip_addr_t ip;
    uint16_t port;
    uint32_t recon_time_ms;
    uint32_t timeout_ms;
    websocket_info_t info;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t ip_valid:1;
    uint8_t port_valid:1;
    uint8_t recon_time_ms_valid:1;
    uint8_t timeout_ms_valid:1;
    uint8_t info_valid:1;
} port_websocket_configuration_t;

void *port_websocket_create_context(void *owner);
void port_websocket_destroy_context(void *ctx);
bool port_websocket_set_configuration(void *ctx, port_websocket_configuration_t *config);
bool port_websocket_get_configuration(void *ctx, port_websocket_configuration_t *config);
bool port_websocket_connect(void *ctx);
int port_websocket_write(void *ctx, uint8_t *data, size_t len);
int port_websocket_read(void *ctx, uint8_t *buffer, size_t len);
bool port_websocket_close(void *ctx);
bool port_websocket_is_opened(void *ctx);

/*mqtt related*/
typedef struct {
    //configuration
    void *owner;
    //valid flag
    uint8_t owner_valid:1;
} port_mqtt_configuration_t;

void *port_mqtt_create_context(void *owner);
void port_mqtt_destroy_context(void *ctx);
bool port_mqtt_set_configuration(void *ctx, port_mqtt_configuration_t *config);
bool port_mqtt_get_configuration(void *ctx, port_mqtt_configuration_t *config);
bool port_mqtt_connect(void *ctx, const char *name, uint16_t fixed_buffer_size, uint16_t publish_max_count, uint16_t subscribe_max_count, uint32_t timeout_ms); 
void port_mqtt_close(void *ctx);
bool port_mqtt_publish(void *ctx, const char *topic, uint8_t *payload, size_t payload_len, arduino_mqtt_qos_t qos);
bool port_mqtt_subscribe(void *ctx, const char *topic, mqtt_subscribe_callback_func callback, arduino_mqtt_qos_t qos);
bool port_mqtt_unsubscribe(void *ctx, const char *topic, arduino_mqtt_qos_t qos);

/*tflm related*/
bool port_tflm_reset(const char ** catergroy_labels, int category_labels_count, const unsigned char * model_data);
bool port_tflm_init_feature(void);
bool port_tflm_cal_feature(uint8_t * data, uint32_t len);
bool port_tflm_detect(int expected_label_index);

/*opus related*/
int port_opus_get_version(void);
bool port_opus_prepare_encoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame, uint8_t application,
                          uint32_t bit_rate, uint8_t complexity, bool use_vbr);
bool port_opus_prepare_decoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame);
bool port_opus_encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len);
bool port_opus_decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len);

/*system related*/
void port_system_restart(void);
uint32_t port_system_get_free_heap_size(void);
uint32_t port_system_get_heap_size_low_level(void);
uint32_t port_system_random(void);
void port_system_register_handler(void (*handler)(void *), void * para);
uint32_t port_system_d25_get_stack_free_size_low_level(void);
void port_system_d25_log_level(uint8_t level);
void port_system_d25_init(void);
void port_system_d25_reset(void);
uint32_t port_millis(void);
uint32_t port_micros(void);
void port_delay_us(unsigned int us);
void port_delay_ms(unsigned int ms);
void port_yield(void);
void port_exit(int status);
int port_atexit(void (*func)(void));
void port_interrupts(void);
void port_no_interrupts(void);


/*i2c related*/

typedef struct {
    //configuration
    void *owner;
    uint32_t timeout;
    uint32_t freq;
    uint8_t reset_with_timeout:1;
    uint8_t timeout_flag:1;
    uint8_t i2c_index:4;
    uint8_t address;
    //valid flag
    uint8_t owner_valid:1;
    uint8_t timeout_valid:1;
    uint8_t freq_valid:1;
    uint8_t reset_with_timeout_valid:1;
    uint8_t i2c_index_valid:1;
    uint8_t address_valid:1;
} port_i2c_configuration_t;

void *port_i2c_create_context(void *owner);
void port_i2c_destroy_context(void *ctx);
bool port_i2c_set_configuration(void *ctx, port_i2c_configuration_t *config);
bool port_i2c_get_configuration(void *ctx, port_i2c_configuration_t *config);
bool port_i2c_begin(void *ctx);
bool port_i2c_end(void *ctx);
bool port_i2c_master_start_tx(void *ctx, uint8_t address, uint8_t *buf, uint32_t len);
bool port_i2c_master_start_rx(void *ctx, uint8_t address, uint32_t len);
bool port_i2c_master_start_trx(void *ctx, uint8_t address, uint8_t *tx_buf, uint32_t tx_len, uint32_t rx_len);
bool port_i2c_master_ask(void *ctx, i2c_master_opt_t *opt, uint32_t *len, uint8_t *buf, uint32_t bufLen);
void port_i2c_master_clear_timeout(void *ctx);
bool port_i2c_master_probe(void *ctx, uint8_t address);
bool port_i2c_is_run(void *ctx);


/*digital related*/
void port_pin_mode(uint8_t pin, uint8_t mode);
void port_digital_write(uint8_t pin, uint8_t val);
int port_digital_read(uint8_t pin);

/*analog related*/
void port_analog_reference(uint8_t mode);
int port_analog_read(uint8_t pin);
void port_analog_write(uint8_t pin, int val);

/*pulse related*/
unsigned long port_pulse_in(uint8_t pin, uint8_t state, unsigned long timeout);

/*tone related*/
void port_tone(uint8_t pin, unsigned int frequency, unsigned long duration);
void port_no_tone(uint8_t pin);

/*base64 related*/
int port_base64_encode(const void *data, int length, char *buffer, uint8_t flags);

#ifdef __cplusplus
} // extern "C"
#endif
