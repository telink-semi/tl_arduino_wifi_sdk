#include "HardwareAudio.h"
#include "test_pcm_data.h"

uint32_t refTime;

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);
  Audio.begin();
  refTime = millis();
}

uint8_t publish_count = 0;
void loop() {
  if(millis() - refTime >= 4000){
    refTime = millis();
    log_i("hello world<%x>", refTime);
    Audio.play((void *)test_pcm_data, sizeof(test_pcm_data), 2000);
  }
}

