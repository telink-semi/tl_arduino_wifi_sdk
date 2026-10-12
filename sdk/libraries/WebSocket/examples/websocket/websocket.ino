#include "WebSocketCommon.h"
#include "WiFiCommon.h"

#define SERVER_IP_ADDR "xxx.xxx.xxx.xxx"
#define SERVER_PORT 4567
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

static uint32_t refTime = 0;

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);

  refTime = millis();

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if(WiFi.waitForConnectResult(5000) != WL_CONNECTED) { 
    log_e("WiFi connect failed");
    while(true) {
      delay(500);
    }
  }

  WebSocketClient client;
  if(!client.connect(SERVER_IP_ADDR, SERVER_PORT)) {
    log_e("client connect failed");
    while(true) {
      delay(500);
    }
  }

  for(int i = 0; i < 100; i++){
    client.println(String("hello world") + i);
  }
  client.flush();

  Serial.println("client data:");
  Serial.println(client.readString());

  Serial.flush();
  client.stop();

}

void loop() {
  if(millis() - refTime >= 1000){
    refTime = millis();
    log_d("Hello World!");
  }

}

