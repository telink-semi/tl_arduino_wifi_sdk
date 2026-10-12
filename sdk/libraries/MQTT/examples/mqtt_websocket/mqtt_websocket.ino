#include "WiFiCommon.h"
#include "MQTTCommon.h"
#include "SoftwareKey.h"
#include "WiFiConfigure.h"

#define SERVER_IP_ADDR    "xxx.xxx.xxx.xxx"
#define SERVER_PORT       4567
#define LED_TEST          LED_BUILTIN
#define KEY_CONTRL_PIN    GPIO4
#define HTTP_URL          "/mqtt"

static uint8_t ledState = LOW;
static uint32_t publicRefTime = 0;
static SoftwareKey button(KEY_CONTRL_PIN);
static MQTTClient client;

void ledInit(void){
  pinMode(LED_TEST, OUTPUT);
  digitalWrite(LED_TEST, ledState);
}

void ledBlink(void){
  ledState = HIGH - ledState;
  digitalWrite(LED_TEST, ledState);
}

void onKeyUp(void) {
  ledBlink();
  client.publish("root/light", (ledState == HIGH) ? "on" : "off", ARDUINO_MQTT_QOS1);
  publicRefTime = millis();
}


void mqtt_subscribe_led_callback(uint8_t *payload, size_t len){
  // log_d("mqtt_subscribe_led_callback: %s", payload);
  if(len > 0){
    if(strcmp((const char *)payload, "on") == 0){
      ledState = HIGH;
    } else if(strcmp((const char *)payload, "off") == 0){
      ledState = LOW;
    }
    digitalWrite(LED_TEST, ledState);
  }
}

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);
  ledInit();


  WiFiConfigureClass wifiConfigure;
  wifiConfigure.run(KEY_CONTRL_PIN);

  if(!client.connect(SERVER_IP_ADDR, SERVER_PORT, HTTP_URL)) {
    log_e("client connect failed");
    delay(5000);
    System.restart();
  }

  if(!client.subscribe("root/light", mqtt_subscribe_led_callback, ARDUINO_MQTT_QOS2)) {
    log_e("client subscribe failed");
    delay(5000);
    System.restart();
  }

  button.setUpCallback(onKeyUp);
  publicRefTime = millis();
}


void loop() {
  button.handleEvent();
  if(millis() - publicRefTime >= 3000){
    publicRefTime = millis();
    log_i("public led state<%d>", ledState);
    client.publish("root/light", (ledState == HIGH) ? "on" : "off", ARDUINO_MQTT_QOS1);
  }
}