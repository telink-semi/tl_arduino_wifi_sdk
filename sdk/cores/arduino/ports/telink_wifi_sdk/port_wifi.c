/********************************************************************************************************
 * @file    port_wifi.c
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
#include "sdk_common.h"
#include "wits_wifi.h"
#include "tlsr_wifi.h"
#include "dhcps.h"
#include "lwip/ip_addr.h"
#include "arpa/inet.h"
#include <stdio.h>

#define DHCPS_DEFAULT_IP     "192.168.200.1"
#define SUBNET_MASK_DEFAULT  "255.255.255.0"

typedef enum {
  ARDUINO_WIFI_STATE_STA_STOP,
  ARDUINO_WIFI_STATE_STA_START,
  ARDUINO_WIFI_STATE_CONNECTED,
  ARDUINO_WIFI_STATE_GOT_IP,
  ARDUINO_WIFI_STATE_DISCONNECTED,
}arduino_wifi_sta_state_t;

typedef enum {
  ARDUINO_WIFI_STATE_AP_STOP = 0,
  ARDUINO_WIFI_STATE_AP_START,
}arduino_wifi_ap_state_t;

typedef struct {
  struct netif * netif;
  tlsr_wifi_assoc_request request;
  tlsr_wifi_status status;
  arduino_wifi_sta_state_t state;
  ip_addr_t ip;
  ip_addr_t nm;
  ip_addr_t gw;
  ip_addr_t id;
  ip_addr_t broadcast;
  wifi_bandwidth_t bw;
  uint8_t mac[NETWORK_MAC_ADDR_LEN];
  uint8_t cdir;
  uint8_t scan_channel;
  uint8_t ip_config_flag;
  char ifname[WIFI_IFNAME_MAX_SIZE+1];
}arduino_wifi_sta_context_t;

typedef struct {
  bool connect_flag;
  uint8_t aid;
  uint8_t mac[NETWORK_MAC_ADDR_LEN];
}station_item_t, *station_item_t_ptr;

typedef struct {
  struct netif * netif;
  tlsr_wifi_softap_config config;
  arduino_wifi_ap_state_t state;
  ip_addr_t ip;
  ip_addr_t nm;
  ip_addr_t gw;
  ip_addr_t id;
  ip_addr_t broadcast;
  ip_addr_t dns;
  struct dhcps_config dhcps;
  uint8_t cdir;
  uint8_t max_count;
  uint8_t connection_count;
  wifi_bandwidth_t bw;
  uint8_t ip_config_flag:1;
  uint8_t dhcp_config_flag:1;
  uint8_t dns_config_flag:1;
  uint8_t mac[NETWORK_MAC_ADDR_LEN];
  char ifname[WIFI_IFNAME_MAX_SIZE+1];
  station_item_t_ptr station_list; 
  void *mutex;
}arduino_wifi_ap_context_t;


enum {
  ARDUINO_WIFI_SCAN_IDLE,
  ARDUINO_WIFI_SCAN_BUSY,
  ARDUINO_WIFI_SCAN_DONE,
};

typedef struct {
  background_handle_t bg_handle;
  wifi_scan_config_t config;
  uint32_t ref_time;
  uint32_t timeout_ms;
  uint32_t active_min_time;
  uint16_t ap_num;
  wifi_ap_record_t *ap_list;
  uint8_t state:3;
  uint8_t error_flag:1;
}arduino_wifi_scan_context_t;

#define TLSR_NEED_DHCP_START(event) ((event)->event_info.connected.not_en_dhcp == false)

static const tlsr_wifi_auth_mode g_security_map[] = {
  TLSR_WIFI_SECURITY_OPEN,
  TLSR_WIFI_SECURITY_UNKNOWN,
  TLSR_WIFI_SECURITY_WPAPSK,
  TLSR_WIFI_SECURITY_WPA2PSK,
  TLSR_WIFI_SECURITY_UNKNOWN,
  TLSR_WIFI_SECURITY_UNKNOWN,
  TLSR_WIFI_SECURITY_WPA2PSK256,//To Check
  TLSR_WIFI_SECURITY_UNKNOWN,
  TLSR_WIFI_SECURITY_SAE,//To Check
  TLSR_WIFI_SECURITY_UNKNOWN,
  TLSR_WIFI_SECURITY_UNKNOWN
};
#define AUTH_MAP_LEN (sizeof(g_security_map) / sizeof(g_security_map[0]))


static const tlsr_wifi_pairwise g_pairwise_map[] = {
  TLSR_WIFI_PAIRWITS_NONE,
  TLSR_WIFI_PAIRWITS_MAX,
  TLSR_WIFI_PAIRWITS_MAX,
  TLSR_WIFI_PAIRWITS_TKIP,
  TLSR_WIFI_PAIRWITS_AES,
  TLSR_WIFI_PAIRWITS_MAX,
  TLSR_WIFI_PAIRWITS_MAX,
};
#define PAIRWISE_MAP_LEN (sizeof(g_pairwise_map) / sizeof(g_pairwise_map[0]))


void ip4_modify_octet(ip_addr_t *ip, int octet_index, int delta) {
    if (!ip || octet_index < 0 || octet_index > 3) {
      return;
    }

    uint32_t host = ntohl(ip->addr);
    uint8_t octets[4] = {
      (host >> 24) & 0xFF,
      (host >> 16) & 0xFF,
      (host >>  8) & 0xFF,
      (host >>  0) & 0xFF
    };

    int new_val = (int)octets[octet_index] + delta;
    if (new_val < 0) {
      new_val = 1;
    } else if(new_val > 255) {
      new_val = 255;
    }
    octets[octet_index] = (uint8_t)new_val;

    uint32_t new_host = (octets[0] << 24) |
                        (octets[1] << 16) |
                        (octets[2] <<  8) |
                        (octets[3] <<  0);
    ip->addr = htonl(new_host);
}

bool isIPZero(ip_addr_t *ip) {
  bool ret = true;
  uint8_t *ptr = (uint8_t*)ip;
  for(size_t i = 0; i < sizeof(ip_addr_t); i++) {
    if(*(ptr+i) != 0) {
      ret = false;
      break;
    }
  }
  return ret;
}


static arduino_wifi_sta_context_t wifi_sta_context = {0};
static arduino_wifi_ap_context_t wifi_ap_context = {0};
static arduino_wifi_scan_context_t wifi_scan_context = {0};

static void wifi_set_ip_info(wifi_interface_t interface, ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw) {
  wifi_ip_info_t ip_info = {0};
  memcpy(&ip_info.ip, ip, sizeof(ip_addr_t));
  if(nm) {
    memcpy(&ip_info.nm, nm, sizeof(ip_addr_t));
  } else if(isIPZero(nm)) {
    nm = NULL;
  }

  if(gw) {
    memcpy(&ip_info.gw, gw, sizeof(ip_addr_t));
  } else if(isIPZero(gw)) {
    gw = NULL;
  }

  wits_wifi_set_ip_info(interface, &ip_info, nm, gw);
}

static wits_err_t sta_event_handler(void *ctx, system_event_t * event)
{
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
    wifi_sta_context.netif = tlsr_wifi_get_netif(WIFI_IF_STA);
    if(wifi_sta_context.ip_config_flag) {
      port_network_config_ip(&wifi_sta_context.ip, &wifi_sta_context.nm, &wifi_sta_context.gw, wifi_sta_context.netif);
    }
		wifi_sta_context.state = ARDUINO_WIFI_STATE_STA_START;
		break;
	case SYSTEM_EVENT_STA_STOP:
		wifi_sta_context.state = ARDUINO_WIFI_STATE_STA_STOP;
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
    port_network_get_ip_addr(&wifi_sta_context.ip, wifi_sta_context.netif);
    port_network_get_subnet_mask(&wifi_sta_context.nm, wifi_sta_context.netif);
    port_network_get_gateway(&wifi_sta_context.gw, wifi_sta_context.netif);
    port_network_get_broadcast_addr(&wifi_sta_context.broadcast, wifi_sta_context.netif);
    port_network_get_id(&wifi_sta_context.id, wifi_sta_context.netif);
    port_network_get_mac_addr(wifi_sta_context.mac, wifi_sta_context.netif);
    wifi_sta_context.cdir = port_network_get_subnet_cdir(wifi_sta_context.netif);
    log_i("IP: %s", ipaddr_ntoa(&wifi_sta_context.ip));
    log_i("Subnet Mask: %s", ipaddr_ntoa(&wifi_sta_context.nm));
    log_i("Gateway: %s", ipaddr_ntoa(&wifi_sta_context.gw));
    wifi_sta_context.state = ARDUINO_WIFI_STATE_GOT_IP;
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		if (TLSR_NEED_DHCP_START(event)) {
			netifapi_dhcp_start(tlsr_wifi_get_netif(WITS_IF_WIFI_STA));
			tlsr_wifi_sta_get_connect_info(&wifi_sta_context.status);
			log_i("SSID: %s, BSSID: "MACSTR", Channel:%d, RSSI:%d", wifi_sta_context.status.ssid, MAC2STR(wifi_sta_context.status.bssid), wifi_sta_context.status.channel, tlsr_wifi_sta_get_ap_rssi());
      wifi_sta_context.state = ARDUINO_WIFI_STATE_CONNECTED;
		} else {
      wifi_sta_context.state = ARDUINO_WIFI_STATE_GOT_IP;
    }
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		log_i("WIFI DISCONNECT");
		wifi_sta_context.state = ARDUINO_WIFI_STATE_DISCONNECTED;
		break;
	case SYSTEM_EVENT_SCAN_DONE:
		log_i("WiFi: Scan results available");
		break;
	case SYSTEM_EVENT_TLSR_CHANNEL:
		log_i("WiFi: Tlsr channel send msg");
		tlsr_wifi_event_send(event, sizeof(system_event_t));
		break;
	default:
		break;
	}
	return WITS_OK;
}

bool port_wifi_set_sta_channel(uint8_t channel) {
  if(wifi_sta_context.bw == WIFI_BW_HT20) {
    return tlsr_wifi_set_channel(wifi_sta_context.ifname, channel, TLSR_WIFI_2ND_CH_NONE) == WITS_OK;
  } else if(wifi_sta_context.bw == WIFI_BW_HT40) {
    return tlsr_wifi_set_channel(wifi_sta_context.ifname, channel, TLSR_WIFI_2ND_CH_ABOVE) == WITS_OK;
  } else {
    return false;
  }
}

bool port_wifi_get_sta_channel(uint8_t *channel) {
  tlsr_wifi_2nd_ch_loc second;
  return tlsr_wifi_get_channel(wifi_sta_context.ifname, channel, &second) == WITS_OK;
}


bool port_wifi_sta_start() {

  if(wifi_sta_context.state != ARDUINO_WIFI_STATE_STA_STOP) return true;
  wifi_sta_context.bw = WIFI_BW_HT20; 

  int len = 0;
  bool ret = false;

  do {
    if(tlsr_wifi_register_event_callback(sta_event_handler, NULL) != WITS_OK) break;
    if(tlsr_wifi_sta_start(wifi_sta_context.ifname, &len) != WITS_OK) break;
    wifi_sta_context.ifname[len] = '\0';
    uint32_t start = millis();
    while(true) {
      if(ARDUINO_WIFI_STATE_STA_STOP != wifi_sta_context.state) {
        ret = true;
        break;
      }
      if(millis() - start > 1000) {
        log_e("wifi sta start timeout<1000ms>");
        break;
      }
      delay_ms(10);
    }
  }while(0);
  
  return ret;
}

bool port_wifi_sta_begin(const char *ssid, const char *key, int channel, const uint8_t *bssid) {

  wifi_sta_context.scan_channel = channel;
  if(ssid) {
    size_t ssid_len = strlen(ssid);
    if(ssid_len > sizeof(wifi_sta_context.request.ssid)) ssid_len = sizeof(wifi_sta_context.request.ssid) - 1;
    memcpy(wifi_sta_context.request.ssid, ssid, ssid_len);
    wifi_sta_context.request.ssid[ssid_len] = '\0';
  }

  if(key) {
    size_t key_len = strlen(key);
    if(key_len > sizeof(wifi_sta_context.request.key)) key_len = sizeof(wifi_sta_context.request.key) - 1;
    memcpy(wifi_sta_context.request.key, key, key_len);
    wifi_sta_context.request.key[key_len] = '\0';
  }

  if(bssid) {
	  memcpy(wifi_sta_context.request.bssid, bssid, sizeof(wifi_sta_context.request.bssid));
  }

  wifi_sta_context.request.auth = TLSR_WIFI_SECURITY_WPA2PSK;
  wifi_sta_context.request.pairwise = TLSR_WIFI_PAIRWITS_AES;
  
  return tlsr_wifi_sta_set_config(&wifi_sta_context.request, NULL) == WITS_OK;
}

bool port_wifi_sta_connect(void) {
  memset(&wifi_sta_context.status, 0, sizeof(wifi_sta_context.status));
  return tlsr_wifi_sta_connect() == WITS_OK;
}

bool port_wifi_sta_is_connected(void) {
  return wifi_sta_context.state == ARDUINO_WIFI_STATE_GOT_IP;
}

bool port_wifi_sta_disconnect(void) {
  return tlsr_wifi_sta_disconnect() == WITS_OK;
}

bool port_wifi_sta_end(void) {
  return tlsr_wifi_sta_stop() == WITS_OK;
}

bool port_wifi_sta_wait_for_connected(uint32_t timeout_ms) {
  uint32_t start_ms = millis();
  while(!port_wifi_sta_is_connected()) {
    if(millis() - start_ms >= timeout_ms) break;
     delay_ms(10);
  }

  if(!port_wifi_sta_is_connected()) {
    tlsr_wifi_sta_stop();
    return false;
  }

  return true;
}

bool port_wifi_sta_wait_for_disconnected(uint32_t timeout_ms) {
  uint32_t start_ms = millis();
  while(port_wifi_sta_is_connected()) {
    if(millis() - start_ms >= timeout_ms) break;
     delay_ms(10);
  }

  if(port_wifi_sta_is_connected()) {
    tlsr_wifi_sta_stop();
    return false;
  }

  return true;
}

bool port_wifi_sta_erase_ap(void) {
  memset(&wifi_sta_context.request, 0, sizeof(wifi_sta_context.request));
  return true;
}

int8_t port_wifi_sta_get_rssi(void) {
  return tlsr_wifi_sta_get_ap_rssi();
}

wifi_bandwidth_t port_wifi_sta_get_bandwidth(void) {
  wifi_bandwidth_t ret;
  wits_wifi_get_bandwidth(WIFI_IF_STA, &ret);
  return ret;
}

bool port_wifi_sta_set_bandwidth(wifi_bandwidth_t bandwidth) {
  wifi_sta_context.bw = bandwidth;
  //return wits_wifi_set_bandwidth(WIFI_IF_STA, bandwidth) == WITS_OK;//not support
  return true;
}

const char *port_wifi_sta_get_ssid(void) {
  return wifi_sta_context.status.ssid;
}

const char *port_wifi_sta_get_psk(void) {
  return wifi_sta_context.request.key;
}

const uint8_t *port_wifi_sta_get_bssid(void) {
  return wifi_sta_context.status.bssid;
}

void port_wifi_sta_set_min_auth_mode(wifi_auth_mode_t mode) {
  wifi_sta_context.request.auth = g_security_map[mode % AUTH_MAP_LEN];
}

void port_wifi_sta_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw) {
  memcpy(&wifi_sta_context.ip, ip, sizeof(ip_addr_t));
  memcpy(&wifi_sta_context.nm, nm, sizeof(ip_addr_t));
  memcpy(&wifi_sta_context.gw, gw, sizeof(ip_addr_t));
  wifi_sta_context.ip_config_flag = 1;
}

void port_wifi_sta_get_ip_addr(ip_addr_t *ip) {
  memcpy(ip, &wifi_sta_context.ip, sizeof(ip_addr_t));
}

void port_wifi_sta_get_gateway(ip_addr_t *gw) {
  memcpy(gw, &wifi_sta_context.gw, sizeof(ip_addr_t));
}

void port_wifi_sta_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN]) {
  memcpy(mac, wifi_sta_context.mac, NETWORK_MAC_ADDR_LEN);
}

void port_wifi_sta_get_subnet_mask(ip_addr_t *mask) {
  memcpy(mask, &wifi_sta_context.nm, sizeof(ip_addr_t));
}

void port_wifi_sta_get_broadcast_addr(ip_addr_t *broadcast) {
  memcpy(broadcast, &wifi_sta_context.broadcast, sizeof(ip_addr_t));
}

uint8_t port_wifi_sta_get_subnet_cdir(void) {
  return wifi_sta_context.cdir;
}

void port_wifi_sta_get_id(ip_addr_t *id) {
  memcpy(id, &wifi_sta_context.id, sizeof(ip_addr_t));
}

 bool port_wifi_ap_disconnect_station(uint8_t mac[NETWORK_MAC_ADDR_LEN]) {
  char buffer[32];
  sprintf(buffer, MACSTR, MAC2STR(mac));
  return tlsr_wifi_sap_deauth_sta(buffer, strlen(buffer)) == WITS_OK;
}

 bool port_wifi_ap_add_to_station_list(uint8_t mac[NETWORK_MAC_ADDR_LEN], uint8_t aid) {
  bool ret = false;
  lock_mutex(wifi_ap_context.mutex);
  station_item_t_ptr station;
  for(int i = 0; i < wifi_ap_context.max_count; i++) {
    station = &wifi_ap_context.station_list[i];
    if(!station->connect_flag) {
      station->connect_flag = true;
      station->aid = aid;
      memcpy(station->mac, mac, NETWORK_MAC_ADDR_LEN);
      ret = true;
      break;
    }
  }
  unlock_mutex(wifi_ap_context.mutex);

  return ret;
}

 bool port_wifi_ap_remove_from_station_list(uint8_t mac[NETWORK_MAC_ADDR_LEN], uint8_t aid) {
  bool ret = false;
  lock_mutex(wifi_ap_context.mutex);
  station_item_t_ptr station;
  for(int i = 0; i < wifi_ap_context.max_count; i++) {
    station = &wifi_ap_context.station_list[i];
    if(station->connect_flag) {
      if((station->aid == aid) && (memcmp(station->mac, mac, NETWORK_MAC_ADDR_LEN) == 0)) {
        station->connect_flag = false;
        station->aid = 0;
        memset(station->mac, 0, NETWORK_MAC_ADDR_LEN);
        ret = true;
        break; 
      }
    }
  }
  unlock_mutex(wifi_ap_context.mutex);

  return ret;
}

#if 0
wits_err_t ap_event_handler(void *ctx, system_event_t * event)
{
	switch (event->event_id) {
	case SYSTEM_EVENT_AP_START:
		/* Handle the event when WiFi Access Point (AP) mode starts. */
		{
			log_d("SYSTEM_EVENT_AP_START");

			/* ip addr for dhcps */
			//wifi_ip_info_t ip_info = {0};
			//IP4_ADDR(&ip_info.ip, 192, 168, 200, 1);
			//IP4_ADDR(&ip_info.nm, 255, 255, 255, 0);

			tlsr_wifi_set_ip("wlan1", "192.168.200.1", "255.255.255.0", NULL);
			netifapi_dhcps_start(tlsr_wifi_get_netif(WITS_IF_WIFI_AP));
		}
		break;
	case SYSTEM_EVENT_AP_STOP:
		/* Handle the event when WiFi Access Point (AP) mode stops. */
		log_d("SYSTEM_EVENT_AP_STOP");
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		/* Handle the event when a station connects to the WiFi AP. */
		log_d("SYSTEM_EVENT_AP_STACONNECTED");
		log_d("Connected STA:" MACSTR "", MAC2STR(event->event_info.sta_connected.mac));
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		/* Handle the event when a station disconnects from the WiFi AP. */
		log_d("SYSTEM_EVENT_AP_STADISCONNECTED");
		log_d("Disconnected STA:" MACSTR "", MAC2STR(event->event_info.sta_disconnected.mac));
		break;

	default:
		break;
	}

	return WITS_OK;
}
#else 
static wits_err_t ap_event_handler(void *ctx, system_event_t * event)
{
  
	switch (event->event_id) {
	case SYSTEM_EVENT_AP_START:
    wifi_ap_context.netif = tlsr_wifi_get_netif(WITS_IF_WIFI_AP);
    if(!wifi_ap_context.ip_config_flag) {
      ipaddr_aton(DHCPS_DEFAULT_IP, &wifi_ap_context.ip);
      ipaddr_aton(DHCPS_DEFAULT_IP, &wifi_ap_context.gw);
      ipaddr_aton(SUBNET_MASK_DEFAULT, &wifi_ap_context.nm);
    }

    if(wifi_ap_context.dns_config_flag) {
      netifapi_dhcps_setdns(wifi_ap_context.netif, &wifi_ap_context.dns);
    }

    wifi_set_ip_info(WITS_IF_WIFI_AP, &wifi_ap_context.ip, &wifi_ap_context.nm, &wifi_ap_context.gw);
    if(wifi_ap_context.dhcp_config_flag) {
      netifapi_dhcps_configure(wifi_ap_context.netif, &wifi_ap_context.dhcps);
    }
    
		netifapi_dhcps_start(wifi_ap_context.netif);
  

    port_network_get_ip_addr(&wifi_ap_context.ip, wifi_ap_context.netif);
    port_network_get_subnet_mask(&wifi_ap_context.nm, wifi_ap_context.netif);
    port_network_get_gateway(&wifi_ap_context.gw, wifi_ap_context.netif);
    port_network_get_broadcast_addr(&wifi_ap_context.broadcast, wifi_ap_context.netif);
    port_network_get_id(&wifi_ap_context.id, wifi_ap_context.netif);
    port_network_get_mac_addr(wifi_ap_context.mac, wifi_ap_context.netif);
    wifi_ap_context.cdir = port_network_get_subnet_cdir(wifi_ap_context.netif);

    log_i("IP: %s", ipaddr_ntoa(&wifi_ap_context.ip));
    log_i("Subnet Mask: %s", ipaddr_ntoa(&wifi_ap_context.nm));
    log_i("Gateway: %s", ipaddr_ntoa(&wifi_ap_context.gw));
    log_i("Broadcast: %s", ipaddr_ntoa(&wifi_ap_context.broadcast));
    log_i("ID: %s", ipaddr_ntoa(&wifi_ap_context.id));
    log_i("MAC: " MACSTR, MAC2STR(wifi_ap_context.mac));
    log_i("CDIR: %d", wifi_ap_context.cdir);

    wifi_ap_context.state = ARDUINO_WIFI_STATE_AP_START;
		break;
	case SYSTEM_EVENT_AP_STOP:
    wifi_ap_context.state = ARDUINO_WIFI_STATE_AP_STOP;
		/* Handle the event when WiFi Access Point (AP) mode stops. */;
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		/* Handle the event when a station connects to the WiFi AP. */
    // log_d("sta connected, connection_count:%d, max:%d",  wifi_ap_context.connection_count, wifi_ap_context.max_count);
    if(wifi_ap_context.connection_count >= wifi_ap_context.max_count) {
      port_wifi_ap_disconnect_station(event->event_info.sta_connected.mac);
    } else {
      if(port_wifi_ap_add_to_station_list(event->event_info.sta_connected.mac, event->event_info.sta_connected.aid)) {
        wifi_ap_context.connection_count++;
      } else {
        port_wifi_ap_disconnect_station(event->event_info.sta_connected.mac);
      }
    }
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		/* Handle the event when a station disconnects from the WiFi AP. */
    if(port_wifi_ap_remove_from_station_list(event->event_info.sta_connected.mac, event->event_info.sta_connected.aid)) {
      wifi_ap_context.connection_count--;
    }
		break;
	default:
		break;
	}

	return WITS_OK;
}
#endif
bool port_wifi_ap_start(const char *ssid, const char *key, int channel, int ssid_hidden, int max_count, wifi_auth_mode_t auth_mode, wifi_cipher_type_t cipher) {

  if(wifi_sta_context.state != ARDUINO_WIFI_STATE_STA_STOP) return true;
  wifi_sta_context.bw = WIFI_BW_HT20; 

  int len;
  bool ret = false;

  do {
    if(max_count <= 0) break;
    wifi_ap_context.station_list = (station_item_t_ptr)malloc(max_count * sizeof(station_item_t));
    if(!wifi_ap_context.station_list) break;
    memset(wifi_ap_context.station_list, 0, max_count * sizeof(station_item_t));

    wifi_ap_context.mutex = create_mutex();
    if(!wifi_ap_context.mutex) break;


    len = strlen(ssid);
    if(len > SSID_MAX_LEN) len = SSID_MAX_LEN;
    memcpy(wifi_ap_context.config.ssid, ssid, len);
    wifi_ap_context.config.ssid[len] = '\0';

    len = strlen(key);
    if(len > WPA_MAX_KEY_LEN) len = WPA_MAX_KEY_LEN;
    memcpy(wifi_ap_context.config.key, key, len);
    wifi_ap_context.config.key[len] = '\0';
    // if(len < WPA_MIN_KEY_LEN) {
    //   log_e("Length of WiFi key error, minimum length: %d", WPA_MIN_KEY_LEN);
    //   break;
    // }

    wifi_ap_context.config.channel_num = channel;
    wifi_ap_context.config.ssid_hidden = ssid_hidden;
    wifi_ap_context.config.authmode = g_security_map[auth_mode % AUTH_MAP_LEN];
    wifi_ap_context.config.pairwise = g_pairwise_map[cipher % PAIRWISE_MAP_LEN];
    wifi_ap_context.max_count = max_count;
    wifi_ap_context.connection_count = 0;
    wifi_ap_context.state = ARDUINO_WIFI_STATE_AP_STOP;
    // log_d("max_count:%d", max_count);
    if(tlsr_wifi_sap_set_config(&wifi_ap_context.config) != WITS_OK) break;
    if(tlsr_wifi_register_event_callback(ap_event_handler, NULL) != WITS_OK) break;
    
    if(tlsr_wifi_sap_start(wifi_ap_context.ifname, &len) != WITS_OK) break;
    wifi_ap_context.ifname[len] = '\0';
    uint32_t startTime = millis();
    while(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) {
      if(millis() - startTime > 1000) break;
      yield();
    }
    
    ret = (wifi_ap_context.state != ARDUINO_WIFI_STATE_AP_STOP);
  }while(0);
  
  if(!ret) {
    if(wifi_ap_context.station_list) {
      free(wifi_ap_context.station_list);
      wifi_ap_context.station_list = NULL;
    }

    if(wifi_ap_context.mutex) {
      destroy_mutex(wifi_ap_context.mutex);
      wifi_ap_context.mutex = NULL;
    }
  }

  return ret;
}

bool port_wifi_ap_clear(void) {
  bool ret = true;
  lock_mutex(wifi_ap_context.mutex);
  station_item_t_ptr station;
  for(int i = 0; i < wifi_ap_context.max_count; i++) {
    station = &wifi_ap_context.station_list[i];
    if(station->connect_flag) {
      ret = ret && port_wifi_ap_disconnect_station(station->mac);
    }
  }
  unlock_mutex(wifi_ap_context.mutex);
  return ret;
}

void port_wifi_ap_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw) {
  memcpy(&wifi_ap_context.ip, ip, sizeof(ip_addr_t));
  memcpy(&wifi_ap_context.nm, nm, sizeof(ip_addr_t));
  memcpy(&wifi_ap_context.gw, gw, sizeof(ip_addr_t));
  wifi_ap_context.ip_config_flag = 1;
}

void port_wifi_ap_config_dhcp(ip_addr_t *start) {
  memcpy(&wifi_ap_context.dhcps.start, start, sizeof(ip_addr_t));
  memcpy(&wifi_ap_context.dhcps.end, start, sizeof(ip_addr_t));
  wifi_ap_context.dhcps.lease_to = 600;
  ip4_modify_octet(&wifi_ap_context.dhcps.end, 3, wifi_ap_context.max_count);
  // wifi_ap_context.dhcp_config_flag = 1;
}


void port_wifi_ap_config_dns(ip_addr_t *dns) {
  memcpy(&wifi_ap_context.dns, dns, sizeof(ip_addr_t));
  wifi_ap_context.dns_config_flag = 1;
}

uint8_t* port_wifi_ap_get_mac_addr(void) {
  if(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) return NULL;
  return wifi_ap_context.mac;
}

char* port_wifi_ap_get_ssid(void) {
  if(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) return NULL;
  else return wifi_ap_context.config.ssid;
}

uint8_t port_wifi_ap_get_station_number(void) {
  if(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) return 0;
  else return wifi_ap_context.connection_count;
}

bool port_wifi_ap_stop(void) {
  if(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) return true;
  else {
    if(tlsr_wifi_sap_stop() != WITS_OK) return false;
    uint32_t startTime = millis();
    while(wifi_ap_context.state != ARDUINO_WIFI_STATE_AP_STOP) {
      if(millis() - startTime > 1000) break;
      yield();
    }

    if(wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP) {
      if(wifi_ap_context.station_list) {
        free(wifi_ap_context.station_list);
        wifi_ap_context.station_list = NULL;
      }

      if(wifi_ap_context.mutex) {
        destroy_mutex(wifi_ap_context.mutex);
        wifi_ap_context.mutex = NULL;
      }
    }

    return wifi_ap_context.state == ARDUINO_WIFI_STATE_AP_STOP;
  }
}

bool port_wifi_ap_is_start(void) {
  return wifi_ap_context.state != ARDUINO_WIFI_STATE_AP_STOP;
}

bool port_wifi_ap_set_bandwidth(wifi_bandwidth_t bandwidth) {
  wifi_ap_context.bw = bandwidth;
  return wits_wifi_set_bandwidth(WIFI_IF_AP, bandwidth) == WITS_OK;
}

bool port_wifi_ap_set_hostname(const char* name) {
  if(!port_wifi_ap_is_start()) return false;
  port_network_set_host_name(name, wifi_ap_context.netif);
  return true;
}

const char* port_wifi_ap_get_hostname() {
  if(!port_wifi_ap_is_start()) return NULL;
  return port_network_get_host_name(wifi_ap_context.netif);
}

void port_wifi_ap_get_ip_addr(ip_addr_t *ip) {
  memcpy(ip, &wifi_ap_context.ip, sizeof(ip_addr_t));
}

void port_wifi_ap_get_gateway(ip_addr_t *gw) {
  memcpy(gw, &wifi_ap_context.gw, sizeof(ip_addr_t));
}

void port_wifi_ap_get_subnet_mask(ip_addr_t *mask) {
  memcpy(mask, &wifi_ap_context.nm, sizeof(ip_addr_t));
}

void port_wifi_ap_get_broadcast_addr(ip_addr_t *broadcast) {
  memcpy(broadcast, &wifi_ap_context.broadcast, sizeof(ip_addr_t));
}

uint8_t port_wifi_ap_get_subnet_cdir(void) {
  return wifi_ap_context.cdir;
}

void port_wifi_scan_test(void) {
  #undef max
  #undef min
  tlsr_wifi_sta_start(NULL, NULL);
  wifi_scan_config_t config = {0};
  config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
  config.scan_time.active.max = 1000;

  wits_wifi_scan_start(&config, true, WIFI_IF_STA);
  // uint32_t startTime = millis();
  // while(true) {
  //   if(millis() - startTime > 3000) {
  //     break;
  //   }
  //   yield();
  // }

  wits_wifi_scan_stop();

  uint16_t count = 0;
  wits_wifi_scan_get_ap_num(&count);
  wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(count * sizeof(wifi_ap_record_t));
  
  if(count && ap_list) {
    memset(ap_list, 0, count * sizeof(wifi_ap_record_t));
    wits_wifi_scan_get_ap_records(0, &count, ap_list);
    
    for(uint16_t i = 0; i < count; i++) {
      log_i("<%d> SSID<%s> MAC<" MACSTR ">", i, ap_list[i].ssid, MAC2STR(ap_list[i].bssid));
    }
    free(ap_list);
  }

  tlsr_wifi_sta_stop();
}

static bool get_scan_list(void) {
  wifi_scan_context.ap_num = 0;
  wifi_scan_context.error_flag = 0;
  wits_wifi_scan_get_ap_num(&wifi_scan_context.ap_num);
  if(wifi_scan_context.ap_num == 0) {
    wifi_scan_context.error_flag = 1;
    return false;
  }
  wifi_scan_context.ap_list = (wifi_ap_record_t *)malloc(wifi_scan_context.ap_num * sizeof(wifi_ap_record_t));
  if(!wifi_scan_context.ap_list)  {
    wifi_scan_context.error_flag = 1;
    return false;
  }
  memset(wifi_scan_context.ap_list, 0, wifi_scan_context.ap_num * sizeof(wifi_ap_record_t));
  wits_wifi_scan_get_ap_records(0, &wifi_scan_context.ap_num, wifi_scan_context.ap_list);
  return true;
}

static void scan_handler(void *para) {
  if(!para) return;
  volatile arduino_wifi_scan_context_t *ctx = (volatile arduino_wifi_scan_context_t *)para;

  switch(ctx->state) {
  case ARDUINO_WIFI_SCAN_BUSY:
    if(millis() - ctx->ref_time > ctx->timeout_ms) {
      wits_wifi_scan_stop();
      ctx->state = ARDUINO_WIFI_SCAN_DONE;
    }
    break;
  case ARDUINO_WIFI_SCAN_DONE:
    get_scan_list();
    tlsr_wifi_sta_stop();
    port_background_unregister_handler(ctx->bg_handle);
    ctx->bg_handle = NULL;
    break;
  default:
    break;
  }

}

int16_t port_wifi_scan(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan, uint8_t channel, const char *ssid, const uint8_t *bssid) {

  if(tlsr_wifi_sta_start(NULL, NULL) != WITS_OK) return -1;

  memset(&wifi_scan_context.config, 0, sizeof(wifi_scan_context.config));
  port_wifi_scan_delete();
  wifi_scan_context.config.show_hidden = show_hidden;
  wifi_scan_context.config.scan_type = (passive? WIFI_SCAN_TYPE_PASSIVE: WIFI_SCAN_TYPE_ACTIVE);
  if(max_ms_per_chan > 1500) max_ms_per_chan = 1500;
  if(passive) {
    wifi_scan_context.config.scan_time.passive = max_ms_per_chan;
  } else {
    if(wifi_scan_context.active_min_time == 0) {
      wifi_scan_context.config.scan_time.active.min = max_ms_per_chan;
    } else {
      wifi_scan_context.config.scan_time.active.min = wifi_scan_context.active_min_time;
    }

    if(max_ms_per_chan < wifi_scan_context.active_min_time) {
      max_ms_per_chan = wifi_scan_context.active_min_time;
    }

    wifi_scan_context.config.scan_time.active.max = max_ms_per_chan;
  }

  if(ssid) {
    wifi_scan_context.config.ssid = (uint8_t *)ssid;
    wifi_scan_context.config.spec_ssid = true;
  }

  if(bssid) {
    wifi_scan_context.config.bssid = (uint8_t *)bssid;
  }

  if(wifi_scan_context.timeout_ms == 0) wifi_scan_context.timeout_ms = 3000;
  wifi_scan_context.ref_time = millis();
  if(wits_wifi_scan_start(&wifi_scan_context.config, false, WIFI_IF_STA) != WITS_OK) {
    return -1;
  }

  if(async) {
    wifi_scan_context.bg_handle = port_background_register_handler(scan_handler, &wifi_scan_context, 0);
    if(!wifi_scan_context.bg_handle) return -1;
    return 0;
  }

  while(true) {
    if(millis() - wifi_scan_context.ref_time > wifi_scan_context.timeout_ms) {
      break;
    }
    yield();
  }

  wits_wifi_scan_stop();
  
  int16_t ret;
  if(get_scan_list()) {
    ret = port_wifi_scan_compelete();
  } else {
    ret = -1;
  }

  tlsr_wifi_sta_stop();

  return ret;
}

void port_wifi_scan_delete(void) {
  if(wifi_scan_context.ap_list) {
    free(wifi_scan_context.ap_list);
    wifi_scan_context.ap_list = NULL;
    wifi_scan_context.ap_num = 0;
    wifi_scan_context.error_flag = 0;
  }
}

int16_t port_wifi_scan_compelete(void) {
  if(!wifi_scan_context.ap_list) return -1;
  if(wifi_scan_context.error_flag) return -1;
  return wifi_scan_context.ap_num;
}

void port_wifi_scan_set_timeout(uint32_t ms) {
  wifi_scan_context.timeout_ms = ms;
}

void port_wifi_scan_set_active_min_time(uint32_t ms) {
  wifi_scan_context.active_min_time = ms;
}

bool port_wifi_get_network_info_by_index(uint16_t index, char *ssid, uint8_t *encryptionType, int32_t *RSSI, uint8_t *BSSID, int32_t *channel) {
  if(wifi_scan_context.error_flag || !wifi_scan_context.ap_list || index >= wifi_scan_context.ap_num) return false;
  wifi_ap_record_t *ap = &wifi_scan_context.ap_list[index];
  if(ssid) {
    sprintf(ssid, "%s", ap->ssid);
  }

  if(BSSID) {
    memcpy(BSSID, ap->bssid, sizeof(ap->bssid));
  }

  if(encryptionType) {
    *encryptionType = ap->authmode;
  }

  if(RSSI) {
    *RSSI = ap->rssi;
  }

  if(channel) {
    *channel = ap->primary;
  }
  return true;
}

#endif