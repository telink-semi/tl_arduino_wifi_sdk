/********************************************************************************************************
 * @file    WiFiConfigure.h
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
#if PORT_CONFIG_WIFI_SUPPORTED && PORT_CONFIG_WEBSERVER_SUPPORTED

#define RETRY_COUNT 10


#include <port_interface.h>
#include <inttypes.h>
#include <WString.h>
#include <WebServer.h>
#include <IPAddress.h>
class WiFiConfigureClass {
private:
    static bool scanDone;
    static bool restartFlag;
    IPAddress ip;
    uint16_t port;
    const char *ssid;
    const char *passwd;
    WebServer ws;

private:
    bool configure_handler(void);
    void error_handler(void);
    void manualConfigure(uint8_t gpio);
public:
    static int root_handler(void *ctx); // root handler
    static int index_handler(void *ctx);  // index
    static int scan_handler(void *ctx);  // scan
    static int save_handler(void *ctx);   // save
    //ssid and passwd for AP
    WiFiConfigureClass(const char *ssid = "Arduino-WiFi", const char *passwd="12345678", IPAddress ip = "192.168.200.1", uint16_t port = 80);
    ~WiFiConfigureClass();
    void run(uint8_t manualKey=0xff);
    
};

#endif