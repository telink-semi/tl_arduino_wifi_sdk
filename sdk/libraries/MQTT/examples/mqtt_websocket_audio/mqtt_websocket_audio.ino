#include "WiFiCommon.h"
#include "HardwareAudio.h"
#include "MQTTCommon.h"
#include "RingBuffer.h"
#include "WiFiConfigure.h"

#define SERVER_IP_ADDR    "xxx.xxx.xxx.xxx"
#define SERVER_PORT       4567
#define KEY_CONTRL_PIN    GPIO4
#define FRAME_MS          30
#define FRAME_SIZE        (FRAME_MS * 16 * 4)
#define PCM_DATA_MAX_LEN  FRAME_SIZE
#define TITLE_MAX_LEN     128

uint32_t testTime;
MQTTClient client;
RingBuffer audioCache(PCM_DATA_MAX_LEN);

void mqtt_subscribe_audio_play_callback(uint8_t *payload, size_t len){
  if(len > 0) {
    String lengthStr = (char*)payload;
    long length = lengthStr.toInt();
    log_d("audio start play: %d", length);
    Audio.startPlay(lengthStr.toInt());
  }
}

void mqtt_subscribe_audio_data_callback(uint8_t *payload, size_t len){
  if(len > 0){
    //  log_d("len:%d", len);
    audioCache.push(payload, len);
  }
}

void setup() {
  logSystemInit(115200, LOG_LEVEL_DEBUG);

  WiFiConfigureClass wifiConfigure;
  wifiConfigure.run(KEY_CONTRL_PIN);

  client.setBufferSize(PCM_DATA_MAX_LEN + TITLE_MAX_LEN);
  if(!client.connect(SERVER_IP_ADDR, SERVER_PORT, "/mqtt")) {
    log_e("client connect failed");
    delay(5000);
    System.restart();
  }

  if(!client.subscribe("root/audio/play", mqtt_subscribe_audio_play_callback, ARDUINO_MQTT_QOS2)) {
    log_e("client subscribe failed");
    delay(5000);
    System.restart();
  }

  if(!client.subscribe("root/audio/data", mqtt_subscribe_audio_data_callback, ARDUINO_MQTT_QOS2)) {
    log_e("client subscribe failed");
    delay(5000);
    System.restart();
  }

  if(!Audio.begin(FRAME_MS)) {
    log_e("Audio begin failed");
    delay(5000);
    System.restart();
  }

  testTime = millis();
}

uint8_t buffer[FRAME_SIZE];
void loop() {
  if(millis() - testTime >= 20000){
    testTime = millis();
    log_d("heap size<%08d>, low_level<%08d>", System.heapSize(), System.heapSizeLowLevel());
  }

  while(audioCache.available()) {
    uint32_t rwLen = audioCache.copy(buffer, sizeof(buffer));
    Audio.playSync(buffer, rwLen, 1000);
    audioCache.pop(rwLen);
  }

}

