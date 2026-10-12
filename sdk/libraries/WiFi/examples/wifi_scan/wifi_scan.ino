#include "WiFiCommon.h"

uint32_t timeRef = millis();

void setup() {

}

int cnt = 0;
void loop() {

    delay(10);

    if(millis() - timeRef > 10000) {
        timeRef = millis();
        int16_t count = WiFi.scanNetworks();
        if(count > 0) {
            log_i("========================");
            for(uint8_t i = 0; i < count; i++) {
                log_i("<%d> SSID:%s, BSSID:%s, AuthMode:%d, RSSI:%d, Channel:%d", i, WiFi.SSID(i).c_str(), WiFi.BSSIDstr(i).c_str(), WiFi.encryptionType(i), WiFi.RSSI(i), WiFi.channel(i));
            }
        }

    }
    
}