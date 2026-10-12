/********************************************************************************************************
 * @file    WiFiConfigure.cpp
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
#if PORT_CONFIG_WIFI_SUPPORTED && PORT_CONFIG_WEBSERVER_SUPPORTED

#include "WiFiConfigure.h"
#include <WiFiCommon.h>
#include <WebServer.h>
#include <FS.h>
#include <cjson/cJSON.h>

/**  
 * @brief      Flag indicating whether the scan is complete.  
 */
bool WiFiConfigureClass::scanDone = false;

/**  
 * @brief      Flag indicating whether a restart is required.  
 */
bool WiFiConfigureClass::restartFlag = false;

/**  
 * @brief      Handler for the root URL. Redirects to "/index.html".  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @return     0 on success, non-zero on failure  
 */
int WiFiConfigureClass::root_handler(void *ctx) {
    WebServer::sendRedirect(ctx, "/index.html");
    return 0;
}

/**  
 * @brief      Handler for the "/index.html" URL. Sends the contents of "index.html".  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @return     0 on success, non-zero on failure  
 */
int WiFiConfigureClass::index_handler(void *ctx) {
    log_d("root handler");
    fs::File file = littleFS.open("/index.html");
    char buf[256];
    while(true) {
        size_t len = 0;
        len = file.readBytes(buf, sizeof(buf) - 1);
        if(len == 0) {
            WebServer::sendChunk(ctx, NULL, 0);
            break;
        }
        buf[len] = '\0';
        WebServer::sendChunk(ctx, buf, len);
    }
    file.close();
    return 0;
}

/**  
 * @brief      Handler for the "/save" URL. Saves received data to "config.json".  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @return     0 on success, non-zero on failure  
 */
int WiFiConfigureClass::save_handler(void *ctx) {
    log_d("save_handler");
    size_t contentSize = WebServer::getRequestContentSize(ctx);
    String recvData = WebServer::recvAll(ctx);
    if(contentSize > 0) {
        log_d("recv:%s", recvData.c_str());
    }
    
    cJSON *root = cJSON_CreateObject();
    if(root == NULL) {
        return -1;
    }

    bool flag = false;
    if(recvData.length() > 0) { 
        fs::File file = littleFS.open("/config.json", FILE_WRITE, true);
        if(file.isOpen()) {
            cJSON_AddBoolToObject(root, "success", true);
            file.printf("%s", recvData.c_str());
            file.close();
            flag = true;
        } else {
            cJSON_AddBoolToObject(root, "success", false);
        }
    } else {
        cJSON_AddBoolToObject(root, "success", false);
    }

    char *json = cJSON_Print(root);
    log_d("json:%s", json);
    WebServer::send(ctx, json);
    free(json);
    cJSON_Delete(root);

    if(flag) {
        delay(100);
        restartFlag = true;
    }

    return 0;
}

/**  
 * @brief      Handler for the "/scan" URL. Returns a list of available WiFi networks.  
 * @param[in]  ctx - The context pointer passed to the handler  
 * @return     0 on success, non-zero on failure  
 */
int WiFiConfigureClass::scan_handler(void *ctx) {
    log_d("scan_handler");
    cJSON *root = cJSON_CreateObject();
    if(root == NULL) {
        return -1;
    }
    
    /*
        {
        "success": true,
        "networks": [
            {
            "ssid": "WiFi Name",
            "rssi": -60
            },
            // More networks...
        ]
        }
    */
    if(scanDone) {
        cJSON_AddBoolToObject(root, "success", true);
        int networkCount = WiFi.scanNetworks();
        if(networkCount > 0) {
            cJSON *networks = cJSON_CreateArray();
            for(int i = 0; i < networkCount; i++) {
                String ssid = WiFi.SSID(i);
                int rssi = WiFi.RSSI(i);
                cJSON *network = cJSON_CreateObject();
                cJSON_AddStringToObject(network, "ssid", ssid.c_str());
                cJSON_AddNumberToObject(network, "rssi", rssi);
                cJSON_AddItemToArray(networks, network);
            }
            cJSON_AddItemToObject(root, "networks", networks);
        }
        
        scanDone = false;
    } else {
        cJSON_AddBoolToObject(root, "success", false);
    }

    char *json = cJSON_Print(root);
    WebServer::send(ctx, json);
    cJSON_Delete(root);
    free(json);

    return 0;
}

/**  
 * @brief      Constructor for WiFiConfigureClass.  
 * @param[in]  ssid - The SSID of the AP to configure  
 * @param[in]  passwd - The password of the AP to configure  
 * @param[in]  apIp - The IP address of the AP  
 * @param[in]  port - The port number for the WebServer  
 */
WiFiConfigureClass::WiFiConfigureClass(const char *ssid, const char *passwd, IPAddress apIp, uint16_t port) : 
                    ip(apIp), port(port), ssid(ssid), passwd(passwd) {
    scanDone = false;
    restartFlag = false;  
}

/**  
 * @brief      Destructor for WiFiConfigureClass.  
 */
WiFiConfigureClass::~WiFiConfigureClass() {

}

/**  
 * @brief      Configures and starts the WebServer to handle WiFi settings.  
 * @return     true on success, false on failure  
 */
bool WiFiConfigureClass::configure_handler(void) {
    if(!WiFi.softAPConfig(ip, ip, IPAddress("255.255.255.0"))) {
        log_e("WiFi: AP configure failed...");
        return false;
    }

    if(!WiFi.softAP(ssid, passwd)) {
        log_e("WiFi: AP setup failed...");
        return false;
    }

    ws.get("/", root_handler);
    ws.get("/index.html", index_handler);
    ws.get("/scan", scan_handler);
    ws.post("/save", save_handler);
    if(!ws.start(port)) {
        log_e("WebServer: start error");
        return false;
    }
    log_i("WebServer start ok");

    while(true) {
        yield();
        if(!scanDone) {
            WiFi.scanNetworks();
            scanDone = true;
        }

        if(restartFlag) {
            System.restart();
            restartFlag = false;
        }
    }
}

/**  
 * @brief      Error handler that runs indefinitely.  
 */
void WiFiConfigureClass::error_handler(void) {
    while(true) {
        yield();
    }
}

/**  
 * @brief      Manually configures the WiFi settings based on a button press.  
 * @param[in]  gpio - The GPIO pin connected to the manual configuration button  
 */
void WiFiConfigureClass::manualConfigure(uint8_t gpio) {
    uint32_t startTime = millis();
    uint32_t refTime = startTime;
    pinMode(gpio, INPUT_PULLUP);

    uint8_t val = 0xff;
    while(millis() - startTime < 100) {
        if(millis() - refTime > 5) {
            val <<= 1;
            val |= digitalRead(gpio);
            refTime = millis();
        } 

        yield();
    }

    if(val == 0) {
        log_i("WiFi Manual Configure");
        if(!configure_handler()) {
            error_handler();
        }
    }
}

/**  
 * @brief      Runs the WiFi configuration process.  
 * @param[in]  manualKey - The GPIO pin connected to the manual configuration button (or 0xff for automatic configuration)  
 */
void WiFiConfigureClass::run(uint8_t manualKey) {

    if(manualKey != 0xff) {
        manualConfigure(manualKey);
    }

    cJSON *obj = NULL, *ssid = NULL, *passwd = NULL, *errcnt = NULL;
    int errcnts = 0;
    fs::File file = littleFS.open("/config.json", FILE_READ);

    if(file.isOpen()) {
        String cfg = file.readString();
        file.close();
        // log_d("config.json:%s", cfg.c_str());

        obj = cJSON_Parse(cfg.c_str());
        ssid = cJSON_GetObjectItem(obj, "ssid");
        passwd = cJSON_GetObjectItem(obj, "password");
        errcnt = cJSON_GetObjectItem(obj, "errorcnt");

        if(!ssid || !passwd) {
            log_e("configure file error");
            if(!configure_handler()) {
                error_handler();
            }
        }   

        errcnts = (errcnt)? errcnt->valueint: 0;

        if(String(ssid->valuestring).isEmpty() || String(passwd->valuestring).isEmpty() || errcnts >= RETRY_COUNT) {
            log_e("no(invalid) ssid and password");
            if(!configure_handler()) {
                error_handler();
            }
        }
        // log_d("ssid:%s, psswd:%s, errcnt:%d", ssid->valuestring, passwd->valuestring, errcnts);

    } else {
        log_e("open configure file error");
        if(!configure_handler()) {
            error_handler();
        }
    }

    bool err_flag = false;
    WiFi.begin(ssid->valuestring, passwd->valuestring);
    if(WiFi.waitForConnectResult(5000) != WL_CONNECTED) {
        log_e("WiFi: connect to '%s' failed", ssid->valuestring);
        errcnts++;
        err_flag = true;
    } else {
        errcnts = 0;
        log_i("WiFi: connect to '%s' ok", ssid->valuestring);
    }

    if(errcnt) {
        cJSON_SetNumberValue(errcnt, errcnts);
    } else {
        cJSON_AddNumberToObject(obj, "errorcnt", errcnts);
    }

    file = littleFS.open("/config.json", FILE_WRITE, true);
    if(file.isOpen()) {
        char *jstr = cJSON_Print(obj);
        if(jstr) {
            file.write((uint8_t *)jstr, strlen(jstr));
            file.close();
            free(jstr);
            // log_i("write to cofigure file success");
        }
    }
    cJSON_Delete(obj);

    if(err_flag) {    
        delay(5000);
        System.restart();
    }

}

#endif