/********************************************************************************************************
 * @file    WiFiSTA.h
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
#if PORT_CONFIG_WIFI_SUPPORTED

#include "port_interface.h"
#include "wits_wifi_types.h"
typedef enum {
  WL_NO_SHIELD = 255,  // for compatibility with WiFi Shield library
  WL_STOPPED = 254,
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL = 1,
  WL_SCAN_COMPLETED = 2,
  WL_CONNECTED = 3,
  WL_CONNECT_FAILED = 4,
  WL_CONNECTION_LOST = 5,
  WL_DISCONNECTED = 6
} wl_status_t;

class WiFiSTAClass {
public:

  wl_status_t begin(const char *ssid, const char *passphrase = NULL, int32_t channel = 0, const uint8_t *bssid = NULL, bool connect = true);
  wl_status_t begin(const String &ssid, const String &passphrase = (const char *)NULL, int32_t channel = 0, const uint8_t *bssid = NULL, bool connect = true) {
    return begin(ssid.c_str(), passphrase.c_str(), channel, bssid, connect);
  }
  wl_status_t begin();

  // also accepts Arduino ordering of parameters: ip, dns, gw, mask
  bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);

  // two and one parameter version. 2nd parameter is DNS like in Arduino
  bool config(IPAddress local_ip, IPAddress dns = (uint32_t)0x00000000);

  bool setDNS(IPAddress dns1, IPAddress dns2 = (uint32_t)0x00000000);  // sets DNS IP for all network interfaces

  bool bandwidth(wifi_bandwidth_t bandwidth);

  bool reconnect();
  bool disconnect(bool wifioff = false, bool eraseap = false, unsigned long timeoutLength = 100);
  bool disconnectAsync(bool wifioff, bool eraseap);
  bool eraseAP(void);

  bool isConnected();

  bool setAutoReconnect(bool autoReconnect);
  bool getAutoReconnect();

  uint8_t waitForConnectResult(unsigned long timeoutLength = 60000);

  // Next group functions must be called before WiFi.begin()
  void setMinSecurity(wifi_auth_mode_t minSecurity);  // Default is WIFI_AUTH_WPA2_PSK
  // void setScanMethod(wifi_scan_method_t scanMethod);  // Default is WIFI_FAST_SCAN
  // void setSortMethod(wifi_sort_method_t sortMethod);  // Default is WIFI_CONNECT_AP_BY_SIGNAL

  // STA WiFi info
  wl_status_t status();
  String SSID() const;
  String psk() const;

  uint8_t *BSSID(uint8_t *bssid = NULL);
  String BSSIDstr();

  int8_t RSSI();

  IPAddress localIP();

  uint8_t *macAddress(uint8_t *mac);
  String macAddress();

  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsIP(uint8_t dns_no = 0);

  IPAddress broadcastIP();
  IPAddress networkID();
  uint8_t subnetCIDR();

};

#endif /* PORT_CONFIG_WIFI_SUPPORTED */
