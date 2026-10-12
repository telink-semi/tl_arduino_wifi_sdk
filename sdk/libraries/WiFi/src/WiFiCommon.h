/********************************************************************************************************
 * @file    WiFiCommon.h
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

#include <stdint.h>

#include "Print.h"
#include "IPAddress.h"

#include "WiFiSTA.h"
#include "WiFiAP.h"

#include "WiFiClient.h"
// #include "WiFiServer.h"
#include "WiFiUdp.h"
#include "WiFiScan.h"
#include "port_interface.h"
class WiFiClass : public WiFiSTAClass, public WiFiAPClass, public WiFiScanClass {
private:
  bool prov_enable;

public:
  WiFiClass() {
    prov_enable = false;
  }
  
  using WiFiSTAClass::BSSID;
  using WiFiSTAClass::BSSIDstr;
  using WiFiSTAClass::RSSI;
  using WiFiSTAClass::SSID;

  using WiFiScanClass::BSSID;
  using WiFiScanClass::BSSIDstr;
  using WiFiScanClass::channel;
  using WiFiScanClass::encryptionType;
  using WiFiScanClass::RSSI;
  using WiFiScanClass::SSID;

public:
  void printDiag(Print &dest);
  friend class NetworkClient;
  // friend class NetworkServer;
  friend class NetworkUDP;
  void enableProv(bool status);
  bool isProvEnabled();
};

extern WiFiClass WiFi;

#endif /* PORT_CONFIG_WIFI_SUPPORTED */
