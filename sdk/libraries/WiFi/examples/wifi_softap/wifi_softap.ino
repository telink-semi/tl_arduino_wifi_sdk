#include "WiFiCommon.h"

uint32_t timeRef = millis();
void setup() {
    WiFi.softAPConfig("192.168.3.1", "192.168.3.1", "255.255.255.0");
    WiFi.softAP("Arduino-WiFi", "12345678");
}

void loop() {

    delay(10);

    if(millis() - timeRef > 1000) {
        timeRef = millis();
        log_i("hello world");
    }
    
}