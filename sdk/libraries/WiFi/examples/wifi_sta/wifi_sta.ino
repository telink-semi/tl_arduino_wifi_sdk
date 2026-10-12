#include "WiFiCommon.h"

static uint32_t refTime = 0;

void setup() {

  WiFi.begin("ssid", "password");
  if(WiFi.waitForConnectResult(5000) != WL_CONNECTED) { 
    log_e("WiFi connect failed");
    while(true) {
      delay(500);
    }
  }

}

void loop() {
  if(millis() - refTime >= 1000){
    refTime = millis();
    log_d("Hello World!");
  }
}

