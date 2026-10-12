/********************************************************************************************************
 * @file    WiFiScan.cpp
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
#if PORT_CONFIG_WIFI_SUPPORTED

#include "WiFiCommon.h"
#include "WiFiScan.h"

void WiFiScanClass::setScanTimeout(uint32_t ms) {
  port_wifi_scan_set_timeout(ms);
}

void WiFiScanClass::setScanActiveMinTime(uint32_t ms) {
  port_wifi_scan_set_active_min_time(ms);
}

/**
 * Start scan WiFi networks available
 * @param async         run in async mode
 * @param show_hidden   show hidden networks
 * @return Number of discovered networks
 */
int16_t
  WiFiScanClass::scanNetworks(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan, uint8_t channel, const char *ssid, const uint8_t *bssid) {
  return port_wifi_scan(async, show_hidden, passive, max_ms_per_chan, channel, ssid, bssid);
}

/**
 * called to get the scan state in Async mode
 * @return scan result or status
 *          -1 if scan not fin
 *          -2 if scan not triggered
 */
int16_t WiFiScanClass::scanComplete() {
  return port_wifi_scan_compelete();
}

/**
 * delete last scan result from RAM
 */
void WiFiScanClass::scanDelete() {
  port_wifi_scan_delete();
}

/**
 * loads all infos from a scanned wifi in to the ptr parameters
 * @param networkItem uint8_t
 * @param ssid  const char**
 * @param encryptionType uint8_t *
 * @param RSSI int32_t *
 * @param BSSID uint8_t **
 * @param channel int32_t *
 * @return (true if ok)
 */
bool WiFiScanClass::getNetworkInfo(uint8_t i, String &ssid, uint8_t &encType, int32_t &rssi, uint8_t *&bssid, int32_t &channel) {
  char buffer[33];
  bool ret = port_wifi_get_network_info_by_index(i, buffer, &encType, &rssi, bssid, &channel);
  if(ret) {
    ssid = buffer;
  }

  return ret;
}

/**
 * Return the SSID discovered during the network scan.
 * @param i     specify from which network item want to get the information
 * @return       ssid string of the specified item on the networks scanned list
 */
String WiFiScanClass::SSID(uint8_t i) {
  char buffer[33] = {0};
  port_wifi_get_network_info_by_index(i, buffer, NULL, NULL, NULL, NULL);
  return String(buffer);
}

/**
 * Return the encryption type of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  encryption type (enum wl_enc_type) of the specified item on the networks scanned list
 */
wifi_auth_mode_t WiFiScanClass::encryptionType(uint8_t i) {
  uint8_t auth_mode = WIFI_AUTH_OPEN;
  port_wifi_get_network_info_by_index(i, NULL, &auth_mode, NULL, NULL, NULL);
  return (wifi_auth_mode_t)auth_mode;
}

/**
 * Return the RSSI of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  signed value of RSSI of the specified item on the networks scanned list
 */
int32_t WiFiScanClass::RSSI(uint8_t i) {
  int32_t rssi = 0;
  port_wifi_get_network_info_by_index(i, NULL, NULL, &rssi, NULL, NULL);
  return rssi;
}

/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @param buff optional buffer for the result uint8_t array with length 6
 * @return uint8_t * MAC / BSSID of scanned wifi
 */
uint8_t *WiFiScanClass::BSSID(uint8_t i, uint8_t *buff) {
  bool ret = port_wifi_get_network_info_by_index(i, NULL, NULL, NULL, buff, NULL);
  if(!ret) {
    return NULL;
  } else {
    return buff;
  }
}

/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @return String MAC / BSSID of scanned wifi
 */
String WiFiScanClass::BSSIDstr(uint8_t i) {
  char mac[18] = {0};
  uint8_t buffer[6] = {0};
  uint8_t *bssid = BSSID(i, buffer);
  if(bssid) {
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  }
  return String(mac);
}

int32_t WiFiScanClass::channel(uint8_t i) {
  int32_t channel = 0;
  port_wifi_get_network_info_by_index(i, NULL, NULL, NULL, NULL, &channel);
  return channel;
}

#endif /* PORT_CONFIG_WIFI_SUPPORTED */
