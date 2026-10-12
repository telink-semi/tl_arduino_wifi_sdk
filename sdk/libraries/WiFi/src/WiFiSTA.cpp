/********************************************************************************************************
 * @file    WiFiSTA.cpp
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
#include "WiFiCommon.h"
#include "WiFiSTA.h"

#if PORT_CONFIG_WIFI_SUPPORTED 

/**
 * Return Connection status.
 * @return one of the value defined in wl_status_t
 *
 */
wl_status_t WiFiSTAClass::status() {
  return port_wifi_sta_is_connected() ? WL_CONNECTED : WL_DISCONNECTED;
}


wl_status_t WiFiSTAClass::begin(const char *ssid, const char *passphrase, int32_t channel, const uint8_t *bssid, bool connect) {

  if(!port_wifi_sta_start()) {
    log_e("wifi sta start failed");
    return WL_CONNECT_FAILED;
  }


  if(!port_wifi_sta_begin(ssid, passphrase, channel, bssid)) {
    log_e("wifi sta begin failed");
    return WL_CONNECT_FAILED;
  }


  if(connect) {
    if(!port_wifi_sta_connect()) {
      log_e("wifi sta start connect failed");
      return WL_CONNECT_FAILED;
    }
  }

  return status();
}

/**
 * Use to connect to SDK config.
 * @return wl_status_t
 */
wl_status_t WiFiSTAClass::begin() {
  if(!port_wifi_sta_start()) {
    return WL_CONNECT_FAILED;
  }

  if(!port_wifi_sta_begin(NULL, NULL, 0, NULL)) {
    return WL_CONNECT_FAILED;
  }

  if(!port_wifi_sta_connect()) {
    return WL_CONNECT_FAILED;
  }

  return status();
}

/**
 * will force a disconnect and then start reconnecting to AP
 * @return true when successful
 */
bool WiFiSTAClass::reconnect() {
  if(!port_wifi_sta_disconnect()) {
    return false;
  }

  if(!port_wifi_sta_wait_for_disconnected(1000)) {
      return false;
  }

   if(!port_wifi_sta_connect()) {
    return false;
  }

  return port_wifi_sta_wait_for_connected(20000);
}

/**
 * Disconnect from the network.
 * @param wifioff `true` to turn the Wi-Fi radio off.
 * @param eraseap `true` to erase the AP configuration from the NVS memory.
 * @return `true` when successful.
 */
bool WiFiSTAClass::disconnectAsync(bool wifioff, bool eraseap) {
  return disconnect(wifioff, eraseap, 0);
}

/**
 * Disconnect from the network.
 * @param wifioff `true` to turn the Wi-Fi radio off.
 * @param eraseap `true` to erase the AP configuration from the NVS memory.
 * @param timeoutLength timeout to wait for status change
 * @return `true` when successful.
 */
bool WiFiSTAClass::disconnect(bool wifioff, bool eraseap, unsigned long timeoutLength) {
  if(!port_wifi_sta_disconnect()) {
    return false;
  }

  if(wifioff) {
    if(!port_wifi_sta_end()) {
      return false;
    }
  }

  if(eraseap) {
    if(!port_wifi_sta_erase_ap()) {
      return false;
    }
  }

  if(timeoutLength > 0) {
    if(!port_wifi_sta_wait_for_disconnected(timeoutLength)) {
      return false;
    }
  }

  return true;
}

/**
 * @brief  Reset WiFi settings in NVS to default values.
 *
 * This function will reset settings made using the following APIs:
 * - esp_wifi_set_bandwidth,
 * - esp_wifi_set_protocol,
 * - esp_wifi_set_config related
 * - esp_wifi_set_mode
 *
 * @return true if erase succeeded
 * @note: Resets SSID, password, protocol, mode, etc.
 * These settings are maintained by WiFi driver in IDF.
 * WiFi driver must be initialized.
 */
bool WiFiSTAClass::eraseAP(void) {
  return port_wifi_sta_erase_ap();
}

/**
 * Change IP configuration settings disabling the dhcp client
 * @param local_ip   Static ip configuration
 * @param gateway    Static gateway configuration
 * @param subnet     Static Subnet mask
 * @param dns1       Static DNS server 1
 * @param dns2       Static DNS server 2
 */
bool WiFiSTAClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {

  if(!port_wifi_sta_start()) {
    return false;
  }
  // handle Arduino ordering of parameters: ip, dns, gw, subnet
  if (local_ip.type() == IPv4 && local_ip != INADDR_NONE && subnet[0] != 255) {
    IPAddress tmp = dns1;
    dns1 = gateway;
    gateway = subnet;
    subnet = (tmp != INADDR_NONE) ? tmp : IPAddress(255, 255, 255, 0);
  }

  ip_addr_t ip, gw, nm, dns1_ip, dns2_ip;
  local_ip.to_ip_addr_t(&ip);
  gateway.to_ip_addr_t(&gw);
  subnet.to_ip_addr_t(&nm);
  dns1.to_ip_addr_t(&dns1_ip);
  dns2.to_ip_addr_t(&dns2_ip);

  port_network_set_dns_server(&dns1_ip, 0);
  port_network_set_dns_server(&dns2_ip, 1);  
  port_wifi_sta_config_ip(&ip, &nm, &gw);
  return true;
}

bool WiFiSTAClass::config(IPAddress local_ip, IPAddress dns) {

  if (local_ip == INADDR_NONE) {
    return config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  }

  if (local_ip.type() != IPv4) {
    return false;
  }

  IPAddress gw(local_ip);
  gw[3] = 1;
  if (dns == INADDR_NONE) {
    dns = gw;
  }
  return config(local_ip, gw, IPAddress(255, 255, 255, 0), dns);
}

/**
 * Change DNS server for static IP configuration
 * @param dns1       Static DNS server 1
 * @param dns2       Static DNS server 2 (optional)
 */
bool WiFiSTAClass::setDNS(IPAddress dns1, IPAddress dns2) {
  ip_addr_t ip1, ip2;
  dns1.to_ip_addr_t(&ip1);
  dns2.to_ip_addr_t(&ip2);
  port_network_set_dns_server(&ip1, 0);
  port_network_set_dns_server(&ip2, 1);
  return true;
}

/**
 * Sets the working bandwidth of the STA mode
 * @param m wifi_bandwidth_t
 */
bool WiFiSTAClass::bandwidth(wifi_bandwidth_t bandwidth) {
  if(!port_wifi_sta_start()) {
    return false;
  }
  return port_wifi_sta_set_bandwidth(bandwidth);
}

/**
 * is STA interface connected?
 * @return true if STA is connected to an AP
 */
bool WiFiSTAClass::isConnected() {
  return port_wifi_sta_is_connected();
}

/**
 * Set the minimum security for AP to be considered connectable.
 * Must be called before WiFi.begin().
 * @param minSecurity wifi_auth_mode_t
 */
void WiFiSTAClass::setMinSecurity(wifi_auth_mode_t minSecurity) {
  port_wifi_sta_set_min_auth_mode(minSecurity);
}

/**
 * Set the way that AP is chosen.
 * First SSID match[WIFI_FAST_SCAN] or Sorted[WIFI_ALL_CHANNEL_SCAN] (RSSI or Security)
 * Must be called before WiFi.begin()
 * @param scanMethod wifi_scan_method_t
 */
// void WiFiSTAClass::setScanMethod(wifi_scan_method_t scanMethod) {
//   // return STA.setScanMethod(scanMethod);
// }

/**
 * Set the way that AP is sorted. (requires scanMethod WIFI_ALL_CHANNEL_SCAN)
 * By SSID[WIFI_CONNECT_AP_BY_SIGNAL] or Security[WIFI_CONNECT_AP_BY_SECURITY]
 * Must be called before WiFi.begin()
 * @param sortMethod wifi_sort_method_t
 */
// void WiFiSTAClass::setSortMethod(wifi_sort_method_t sortMethod) {
//   // return STA.setSortMethod(sortMethod);
// }

/**
 * Function used to set the automatic reconnection if the connection is lost.
 * @param autoReconnect `true` to enable this option.
 * @return true
 */
bool WiFiSTAClass::setAutoReconnect(bool autoReconnect) {
  // return STA.setAutoReconnect(autoReconnect);
  return 0;
}
/**
 * Function used to get the automatic reconnection if the connection is lost.
 * @return The function will return `true` if this setting is enabled.
 */
bool WiFiSTAClass::getAutoReconnect() {
  // return STA.getAutoReconnect();
  return 0;
}

/**
 * Wait for WiFi connection to reach a result
 * returns the status reached or disconnect if STA is off
 * @return wl_status_t
 */
uint8_t WiFiSTAClass::waitForConnectResult(unsigned long timeoutLength) {
  if(!port_wifi_sta_wait_for_connected(timeoutLength)) {
    return WL_DISCONNECTED;
  }
  return WL_CONNECTED;
}

/**
 * Get the station interface IP address.
 * @return IPAddress station IP
 */
IPAddress WiFiSTAClass::localIP() {
  // return STA.localIP();
  ip_addr_t ip;
  port_wifi_sta_get_ip_addr(&ip);
  return IPAddress(&ip);
}

/**
 * Get the station interface MAC address.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t *
 */
uint8_t *WiFiSTAClass::macAddress(uint8_t *mac) {
  // return STA.macAddress(mac);
  port_wifi_sta_get_mac_addr(mac);
  return mac;
}

/**
 * Get the station interface MAC address.
 * @return String mac
 */
String WiFiSTAClass::macAddress(void) {
  // return STA.macAddress();
  uint8_t mac[NETWORK_MAC_ADDR_LEN];
  port_wifi_sta_get_mac_addr(mac);
  char buffer[18];
  snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buffer);
}

/**
 * Get the interface subnet mask address.
 * @return IPAddress subnetMask
 */
IPAddress WiFiSTAClass::subnetMask() {
  ip_addr_t mask;
  port_wifi_sta_get_subnet_mask(&mask);
  return IPAddress(&mask);
}

/**
 * Get the gateway ip address.
 * @return IPAddress gatewayIP
 */
IPAddress WiFiSTAClass::gatewayIP() {
  ip_addr_t gw;
  port_wifi_sta_get_gateway(&gw);
  return IPAddress(&gw);
}

/**
 * Get the DNS ip address.
 * @param dns_no
 * @return IPAddress DNS Server IP
 */
IPAddress WiFiSTAClass::dnsIP(uint8_t dns_no) {
  ip_addr_t dns;
  port_network_get_dns_server(&dns, dns_no);
  return IPAddress(&dns);
}

/**
 * Get the broadcast ip address.
 * @return IPAddress broadcastIP
 */
IPAddress WiFiSTAClass::broadcastIP() {
  ip_addr_t broadcast;
  port_wifi_sta_get_broadcast_addr(&broadcast);
  return IPAddress(&broadcast);
}

/**
 * Get the network id.
 * @return IPAddress networkID
 */
IPAddress WiFiSTAClass::networkID() {
  ip_addr_t id;
  port_wifi_sta_get_id(&id);
  return IPAddress(&id);
}

/**
 * Get the subnet CIDR.
 * @return uint8_t subnetCIDR
 */
uint8_t WiFiSTAClass::subnetCIDR() {
  return port_wifi_sta_get_subnet_cdir();
}

/**
 * Return the current SSID associated with the network
 * @return SSID
 */
String WiFiSTAClass::SSID() const {
  return String(port_wifi_sta_get_ssid());
}

/**
 * Return the current pre shared key associated with the network
 * @return  psk string
 */
String WiFiSTAClass::psk() const {
  return String(port_wifi_sta_get_psk());
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return bssid uint8_t *
 */
uint8_t *WiFiSTAClass::BSSID(uint8_t *buff) {
  const uint8_t *bssid = port_wifi_sta_get_bssid();
  if(bssid) {
    memcpy(buff, bssid, NETWORK_MAC_ADDR_LEN);
    return buff;
  }
  return NULL;
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return String bssid mac
 */
String WiFiSTAClass::BSSIDstr(void) {
  const uint8_t *bssid = port_wifi_sta_get_bssid();
  if(bssid) {
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return String(buffer);
  }
  return String("00:00:00:00:00:00");
}

/**
 * Return the current network RSSI.
 * @return  RSSI value
 */
int8_t WiFiSTAClass::RSSI(void) {
  return port_wifi_sta_get_rssi();
}




#endif /* PORT_CONFIG_WIFI_SUPPORTED */
