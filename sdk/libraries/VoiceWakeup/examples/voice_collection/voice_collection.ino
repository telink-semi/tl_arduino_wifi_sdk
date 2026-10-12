#include "WiFiCommon.h"
#include "WebSocketCommon.h"
#include "StreamController.h"
#include "HardwareAudio.h"
#include "SoftwareKey.h"
#include "WiFiConfigure.h"

#define SERVER_IP_ADDR "xxx.xxx.xxx.xxx"
#define SERVER_PORT    4567
#define KEY_CONTRL_PIN GPIO4

#define SAMPLE_RATE       16000
#define SAMPLES_PER_FRAME 320 //20ms@16kHz

#define TAG_PCM "<PCM"
#define TAG_RST "<RST"
#define TAG_END "<END"

typedef enum {
  RECORD_IDLE,
  RECORD_START,
  RECORD_SND_DATA,
  RECORD_END,
}record_state_t;

typedef struct {
  record_state_t state;
  uint32_t frameCnt;
}record_context_t;

uint32_t testTime;
WebSocketClient client;
StreamController streamCtl(1024);
record_context_t recordContext;

SoftwareKey button(KEY_CONTRL_PIN);
uint8_t pcm_buffer[SAMPLES_PER_FRAME * 2 * 2]; // 2 bytes per channel, 2 channels

void recordLoop() {
  uint32_t recordLen;

  if(recordContext.state == RECORD_SND_DATA || recordContext.state == RECORD_END) {
    while(true) {
      recordLen = sizeof(pcm_buffer);
      if((Audio.recordAvailable() < recordLen) || (client.availableForWrite() < (int)(recordLen >> 1))) break;
      recordLen = Audio.readRecordAsync(pcm_buffer, recordLen);
      recordLen = Audio.dualTomono(pcm_buffer, recordLen);
      recordLen = client.write(pcm_buffer, recordLen);
      recordContext.frameCnt += 1;
    }

    if(recordContext.state == RECORD_END) {
      recordLen = client.availableForWrite();
      if(recordLen >= TAG_LENGTH) {
        recordLen = TAG_LENGTH;
        client.write((uint8_t *)TAG_END, recordLen);
        recordContext.state = RECORD_IDLE;
        log_d("recordHandle -> total: %d", recordContext.frameCnt);
      }
    }

  } else if(recordContext.state == RECORD_START) {
    recordLen = client.availableForWrite();
    if(recordLen >= 2 * TAG_LENGTH) {
      recordLen = TAG_LENGTH;
      client.write((uint8_t *)TAG_RST, recordLen);
      client.write((uint8_t *)TAG_PCM, recordLen);
      recordContext.state = RECORD_SND_DATA;
      recordContext.frameCnt = 0;
    }
  }
}

void keyDownProcess() {
  Audio.startRecord(AUDIO_MAX_LEN);
  Audio.stopPlay();
  recordContext.state = RECORD_START;
  recordContext.frameCnt = 0;
}

void keyUpProcess() {
  Audio.stopRecord();
  recordContext.state = RECORD_END;
}

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);

  WiFiConfigureClass wifiConfigure;
  wifiConfigure.run(KEY_CONTRL_PIN);

  if(!Audio.begin(SAMPLES_PER_FRAME, SAMPLE_RATE)) {
    log_e("Audio begin failed");
    delay(5000);
    System.restart();
  }

  button.setDownCallback(keyDownProcess);
  button.setUpCallback(keyUpProcess);

  client.setBufferSize(1024, 1024);
  client.addHeader("user", "arduino");
  client.addHeader("apiKey", "==telink@voice-collection==");
  if(!client.connect(SERVER_IP_ADDR, SERVER_PORT, "/voice-collection")) {
    log_e("client connect failed");
    delay(5000);
    System.restart();
  }

  testTime = millis();
}

void loop() {
  if(millis() - testTime >= 5000){
    testTime = millis();
    log_d("[heap] freeSize:%d, lowLevel:%d", System.heapSize(), System.heapSizeLowLevel());
  }
  button.handleEvent();
  recordLoop();
}
