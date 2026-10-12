#include "WiFiConfigure.h"

#define KEY_CONTRL_PIN GPIO4
#define WIFI_SSID      "Arduino-WiFi"
#define WIFI_PASSWD    "12345678"

uint32_t timeRef;
void setup() {
    logSystemInit(115200, LOG_LEVEL_INFO);
    WiFiConfigureClass wifiCfg(WIFI_SSID, WIFI_PASSWD);
    wifiCfg.run(KEY_CONTRL_PIN);
    timeRef = millis();
}

void loop() {

    delay(10);

    if(millis() - timeRef > 1000) {
        timeRef = millis();
        log_i("hello world");
    }
    
}