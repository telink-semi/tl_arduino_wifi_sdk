#include "WiFiCommon.h"
#include "HttpClient.h"
#include "WiFiConfigure.h"
#include "System.h"

#define KEY_CONTRL_PIN          LED_BUILTIN
#define HTTP_SERVER_IP          "xxx.xxx.xxx.xxx"
#define HTTP_SERVER_PORT        "3000"
#define HTTP_GET_TEST_URL       "http://" HTTP_SERVER_IP ":" HTTP_SERVER_PORT "/get"
#define HTTP_POST_TEST_URL       "http://" HTTP_SERVER_IP ":" HTTP_SERVER_PORT "/post"

HttpClient httpClient;
uint32_t timeRef = 0;

void setup() {
    logSystemInit(115200, LOG_LEVEL_INFO);

    WiFiConfigureClass wifiConfigure;
    wifiConfigure.run(KEY_CONTRL_PIN);
    timeRef = millis();

    HttpHeaders headers;
    http_response_t response;
    headers.reserve(10);
    response.reserve(1024);

    // test get
    response.clear();
    httpClient.get(HTTP_GET_TEST_URL, headers, response);
    log_i("headers size = %d", headers.size());
    if(headers.size() > 0) {
        for(size_t i = 0; i < headers.size(); i++) {
            log_i("header[%d] %s: %s", i, headers[i].first, headers[i].second);
        }

        headers.clear();
    }

    log_i("response size: %d", response.size());
    if(response.size() > 0) {
        response.push_back('\0');
        log_i("response: %s", (const char *)response.data());
    }

    // test post
    response.clear();
    httpClient.post(HTTP_POST_TEST_URL, headers, "{\"name\":\"test\"}", response);
    log_i("headers size = %d", headers.size());
    if(headers.size() > 0) {
        for(size_t i = 0; i < headers.size(); i++) {
            log_i("header[%d] %s: %s", i, headers[i].first, headers[i].second);
        }
        headers.clear();
    }
    log_i("response size: %d", response.size());
    if(response.size() > 0) {
        response.push_back('\0');
        log_i("response: %s", (const char *)response.data());
    }
}

void loop() {

    if(millis() - timeRef > 20000) {
        timeRef = millis();
        log_i("hello world, heap = %d, heap low level = %d", System.heapSize(), System.heapSizeLowLevel());
    }
    
}