#include "HardwareAudio.h"
#include "SoftwareKey.h"

#define KEY_CONTRL_PIN     D0

uint32_t debugTime;
uint32_t recordSize = 0;
uint32_t playSize = 0;
uint8_t buffer[40960*2] = {0};
uint8_t recordBusyFlag = 0;

void keyDownProcess() {
  Audio.startRecord(AUDIO_MAX_LEN);
  recordSize = 0;
  recordBusyFlag = 1;
}

void keyUpProcess() {
  Audio.stopRecord();
  recordBusyFlag = 0;
}

SoftwareKey recordKey(KEY_CONTRL_PIN, keyDownProcess, keyUpProcess);

void recordLoop() {
  if(recordBusyFlag == 1) {
    recordSize += Audio.readRecordAsync(buffer + recordSize, sizeof(buffer) - recordSize);
  }
}

void playLoop() {
  if(millis() - debugTime >= 4000){

    debugTime = millis();
    log_i("hello world<%x>", debugTime);

    if(recordSize > 0 && recordBusyFlag == 0) {
      Audio.startPlay(recordSize);
      playSize = recordSize;
    }
  }

  if(playSize > 0) {
    playSize -= Audio.playAsync(buffer + recordSize - playSize, playSize);
  }
}

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);

  debugTime = millis();
  Audio.begin(20);
}

uint8_t publish_count = 0;
void loop() {
  playLoop();
  recordLoop();
  recordKey.handleEvent();
}
