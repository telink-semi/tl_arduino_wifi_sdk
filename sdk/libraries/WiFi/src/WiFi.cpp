/********************************************************************************************************
 * @file    WiFi.cpp
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
#if PORT_CONFIG_WIFI_SUPPORTED 

extern "C" {
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
}

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------- Debug ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

/**
 * Output WiFi settings to an object derived from Print interface (like Serial).
 * @param p Print interface
 */
void WiFiClass::printDiag(Print &p) {
  // const char *modes[] = {"NULL", "STA", "AP", "STA+AP", "NAN"};

  // wifi_mode_t mode = WIFI_MODE_NULL;
  // esp_wifi_get_mode(&mode);

  // uint8_t primaryChan = 0;
  // wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;
  // esp_wifi_get_channel(&primaryChan, &secondChan);

  // p.print("Mode: ");
  // p.println(modes[mode]);

  // p.print("Channel: ");
  // p.println(primaryChan);
  // /*
  //       p.print("AP id: ");
  //       p.println(wifi_station_get_current_ap_id());

  //       p.print("Status: ");
  //       p.println(wifi_station_get_connect_status());
  //   */

  // wifi_config_t conf = {0};
  // esp_wifi_get_config((wifi_interface_t)WIFI_IF_STA, &conf);

  // const char *ssid = reinterpret_cast<const char *>(conf.sta.ssid);
  p.print("SSID (");
  // p.print(strlen(ssid));
  p.print("): ");
  // p.println(ssid);

  // const char *passphrase = reinterpret_cast<const char *>(conf.sta.password);
  p.print("Passphrase (");
  // p.print(strlen(passphrase));
  p.print("): ");
  // p.println(passphrase);

  p.print("BSSID set: ");
  // p.println(conf.sta.bssid_set);
}

void WiFiClass::enableProv(bool status) {
  prov_enable = status;
}

bool WiFiClass::isProvEnabled() {
  return prov_enable;
}

WiFiClass WiFi;

#endif /* PORT_CONFIG_WIFI_SUPPORTED */
