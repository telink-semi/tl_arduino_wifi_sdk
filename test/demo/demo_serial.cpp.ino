#include <Arduino.h>
#include <WiFiCommon.h>

uint32_t timeRef = millis();
void setup() {
    log_d("Serial.begin:%d", Serial.begin(115200));
    log_d("Serial1.begin:%d", Serial1.begin(115200));
}

int cnt = 0;
void loop() {

    delay(10);
    Serial1.printf("hello world<%d>\r\n", cnt++);

    if(millis() - timeRef > 1000) {
        timeRef = millis();
        Serial.println("hello world");
        if(Serial1.available()) {
            log_d("Read: %s", Serial1.readString().c_str());
        }
    }
    
}