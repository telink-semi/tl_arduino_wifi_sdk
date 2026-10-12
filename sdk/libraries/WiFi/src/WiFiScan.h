/********************************************************************************************************
 * @file    WiFiScan.h
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

class WiFiScanClass {

public:
  void setScanTimeout(uint32_t ms);
  void setScanActiveMinTime(uint32_t ms);

  int16_t scanNetworks(
    bool async = false, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300, uint8_t channel = 0, const char *ssid = nullptr,
    const uint8_t *bssid = nullptr
  );

  int16_t scanComplete();
  void scanDelete();

  // scan result
  bool getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t *&BSSID, int32_t &channel);

  String SSID(uint8_t networkItem);
  wifi_auth_mode_t encryptionType(uint8_t networkItem);
  int32_t RSSI(uint8_t networkItem);
  uint8_t *BSSID(uint8_t networkItem, uint8_t *bssid = NULL);
  String BSSIDstr(uint8_t networkItem);
  int32_t channel(uint8_t networkItem);

};

#endif /* PORT_CONFIG_WIFI_SUPPORTED */
