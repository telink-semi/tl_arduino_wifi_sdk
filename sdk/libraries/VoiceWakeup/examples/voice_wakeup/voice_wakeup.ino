
#include "HardwareAudio.h"
#include "WiFiConfigure.h"
#include "RingBuffer.h"
#include "KWSCommon.h"
#include "VoiceWakeup.h"
#include "test_pcm_data.h"

#define CATEGORY_LABELS_COUNT  (sizeof(kCategoryLabels) / sizeof(kCategoryLabels[0]))
#define KEY_CONTRL_PIN GPIO4

SoftwareKey button(KEY_CONTRL_PIN);
const char* kCategoryLabels[] = {
    "silence",
    "unknown",
    "yes",
    "no",
};

void setup(void) {
    logSystemInit(115200, LOG_LEVEL_INFO);

    System.d25Start();
    System.d25LogLevel(LOG_LEVEL_INFO);

    button.setDownCallback([](){
        VoiceWakeup.playInterrupt();
        log_i("playInterrupt");
    });

    Task.createTask([](void *para){
        button.handleEvent();
    }, NULL);

    VoiceWakeup.registerOnStartSpeak([](){
        log_i("onStartSpeak");
    });

    VoiceWakeup.registerOnProcSpeak([](uint8_t *data, uint32_t len){
        (void)data;
        log_i("onProcSpeak, len:%d", len);
    });

    VoiceWakeup.registerOnStopSpeak([](){
        log_i("wait for remote interruption or press key to start the next conversation");
    });

    if(VoiceWakeup.begin(kCategoryLabels, CATEGORY_LABELS_COUNT, 2, NULL)) {
        log_i("VoiceWakeup begin success");
    } else {
        log_e("VoiceWakeup begin failed");
        exit(-1);
    }
}


void loop(void) {
    delay(20000);
    log_i("heap:%d", System.heapSize());
}