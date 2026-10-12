/********************************************************************************************************
 * @file    WiFiAP.cpp
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
#include "port_config.h"
#if PORT_CONFIG_WIFI_SUPPORTED

#include "WiFiCommon.h"
#include "WiFiAP.h"
#include <common.h>

/**
 * Set up an access point
 * @param ssid              Pointer to the SSID (max 63 char).
 * @param passphrase        (for WPA2 min 8 char, for open use NULL)
 * @param channel           WiFi channel number, 1 - 13.
 * @param ssid_hidden       Network cloaking (0 = broadcast SSID, 1 = hide SSID)
 * @param max_connection    Max simultaneous connected clients, 1 - 4.
*/
bool WiFiAPClass::softAP(
  const char *ssid, const char *passphrase, int channel, int ssid_hidden, int max_connection, bool ftm_responder, wifi_auth_mode_t auth_mode,
  wifi_cipher_type_t cipher
) {
  (void)ftm_responder;
  return port_wifi_ap_start(ssid, passphrase, channel, ssid_hidden, max_connection, auth_mode, cipher);
}

/**
 * Return the current SSID associated with the network
 * @return SSID
 */
String WiFiAPClass::softAPSSID() const {
  return String(port_wifi_ap_get_ssid());
}

/**
 * Configure access point
 * @param local_ip      access point IP
 * @param gateway       gateway IP
 * @param subnet        subnet mask
 */
bool WiFiAPClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dhcp_lease_start, IPAddress dns) {
  if(port_wifi_ap_is_start()) return false;
  ip_addr_t _ip, _gw, _nm, _start, _dns;
  local_ip.to_ip_addr_t(&_ip);
  gateway.to_ip_addr_t(&_gw);
  subnet.to_ip_addr_t(&_nm);
  
  port_wifi_ap_config_ip(&_ip, &_nm, &_gw);

  if(dhcp_lease_start != INADDR_NONE) {
    dhcp_lease_start.to_ip_addr_t(&_start);
    port_wifi_ap_config_dhcp(&_start);
  } 

  if(dns != INADDR_NONE) {
    dns.to_ip_addr_t(&_dns);
    port_wifi_ap_config_dns(&_dns);
  } 

  return true;
}

/**
 * Disconnect from the network (close AP)
 * @param wifioff disable mode?
 * @return one value of wl_status_t enum
 */
bool WiFiAPClass::softAPdisconnect(bool wifioff) {
  if(!port_wifi_ap_clear()) {
    return false;
  }


  if (wifioff) {
    return port_wifi_ap_stop();
  }
  return true;
}

/**
 * Sets the working bandwidth of the AP mode
 * @param m wifi_bandwidth_t
 */
bool WiFiAPClass::softAPbandwidth(wifi_bandwidth_t bandwidth) {
  return port_wifi_ap_set_bandwidth(bandwidth);
}

/**
 * Get the count of the Station / client that are connected to the softAP interface
 * @return Stations count
 */
uint8_t WiFiAPClass::softAPgetStationNum() {
  return port_wifi_ap_get_station_number();
}

/**
 * Get the softAP interface IP address.
 * @return IPAddress softAP IP
 */
IPAddress WiFiAPClass::softAPIP() {
  ip_addr_t ip;
  port_wifi_ap_get_ip_addr(&ip);
  return IPAddress(&ip);
}

/**
 * Get the softAP broadcast IP address.
 * @return IPAddress softAP broadcastIP
 */
IPAddress WiFiAPClass::softAPBroadcastIP() {
  ip_addr_t broadcast;
  port_wifi_ap_get_broadcast_addr(&broadcast);
  return IPAddress(&broadcast);
}

/**
 * Get the softAP network ID.
 * @return IPAddress softAP networkID
 */
IPAddress WiFiAPClass::softAPNetworkID() {
  ip_addr_t id;
  port_wifi_ap_get_id(&id);
  return IPAddress(&id);
}

/**
 * Get the softAP subnet mask.
 * @return IPAddress subnetMask
 */
IPAddress WiFiAPClass::softAPSubnetMask() {
  ip_addr_t nm;
  port_wifi_ap_get_subnet_mask(&nm);
  return IPAddress(&nm);
}

/**
 * Get the softAP subnet CIDR.
 * @return uint8_t softAP subnetCIDR
 */
uint8_t WiFiAPClass::softAPSubnetCIDR() {
  return port_wifi_ap_get_subnet_cdir();
}

/**
 * Get the softAP interface MAC address.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t*
 */
uint8_t *WiFiAPClass::softAPmacAddress(uint8_t *mac) {
  return port_wifi_ap_get_mac_addr();
}

/**
 * Get the softAP interface MAC address.
 * @return String mac
 */
String WiFiAPClass::softAPmacAddress(void) {
  char buffer[32];
  uint8_t* mac = port_wifi_ap_get_mac_addr();
  sprintf(buffer, MACSTR, MAC2STR(mac));
  return String(buffer);
}

/**
 * Get the softAP interface Host name.
 * @return char array hostname
 */
const char *WiFiAPClass::softAPgetHostname() {
  return port_wifi_ap_get_hostname();
}

/**
 * Set the softAP    interface Host name.
 * @param  hostname  pointer to const string
 * @return true on   success
 */
bool WiFiAPClass::softAPsetHostname(const char *hostname) {
  return port_wifi_ap_set_hostname(hostname);
}

#if CONFIG_LWIP_IPV6
/**
 * Enable IPv6 on the softAP interface.
 * @return true on success
 */
// bool WiFiAPClass::softAPenableIPv6(bool enable) {
//   return AP.enableIPv6(enable);
// }

/**
 * Get the softAP interface IPv6 address.
 * @return IPAddress softAP IPv6
 */

// IPAddress WiFiAPClass::softAPlinkLocalIPv6() {
//   return AP.linkLocalIPv6();
// }
#endif
#endif /* PORT_CONFIG_WIFI_SUPPORTED */
