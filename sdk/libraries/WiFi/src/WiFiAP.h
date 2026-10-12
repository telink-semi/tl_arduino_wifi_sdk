/********************************************************************************************************
 * @file    WiFiAP.h
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

#include "port_interface.h"
#if PORT_CONFIG_WIFI_SUPPORTED

#define WIFI_AP_DEFAULT_AUTH_MODE WIFI_AUTH_WPA2_PSK
#define WIFI_AP_DEFAULT_CIPHER    WIFI_CIPHER_TYPE_CCMP  // Disable by default enabled insecure TKIP and use just CCMP.


class WiFiAPClass {

public:
  bool softAP(
    const char *ssid, const char *passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false,
    wifi_auth_mode_t auth_mode = WIFI_AP_DEFAULT_AUTH_MODE, wifi_cipher_type_t cipher = WIFI_AP_DEFAULT_CIPHER
  );
  bool softAP(
    const String &ssid, const String &passphrase = emptyString, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false,
    wifi_auth_mode_t auth_mode = WIFI_AP_DEFAULT_AUTH_MODE, wifi_cipher_type_t cipher = WIFI_AP_DEFAULT_CIPHER
  ) {
    return softAP(ssid.c_str(), passphrase.c_str(), channel, ssid_hidden, max_connection, ftm_responder, auth_mode, cipher);
  }

  bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dhcp_lease_start = (uint32_t)0, IPAddress dns = (uint32_t)0);
  bool softAPdisconnect(bool wifioff = false);

  bool softAPbandwidth(wifi_bandwidth_t bandwidth);

  uint8_t softAPgetStationNum();
  String softAPSSID(void) const;

  IPAddress softAPIP();
  IPAddress softAPBroadcastIP();
  IPAddress softAPNetworkID();
  IPAddress softAPSubnetMask();
  uint8_t softAPSubnetCIDR();

#if CONFIG_LWIP_IPV6
//   bool softAPenableIPv6(bool enable = true);
//   IPAddress softAPlinkLocalIPv6();
#endif

  const char *softAPgetHostname();
  bool softAPsetHostname(const char *hostname);

  uint8_t *softAPmacAddress(uint8_t *mac);
  String softAPmacAddress(void);

protected:
};

#endif /* PORT_CONFIG_WIFI_SUPPORTED*/
