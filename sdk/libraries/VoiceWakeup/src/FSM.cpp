/********************************************************************************************************
 * @file    FSM.cpp
 *
 * @brief   This is the source file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "port_config.h"
#if PORT_CONFIG_VOICE_WAKEUP_SUPPORTED

#include "FSM.h"
#include "Arduino.h"
#include "HardwareAudio.h"
#include "KWSCommon.h"
#include "VoiceWakeup.h"
#include "OpusCommon.h"

#if defined(USE_PROMPT_FROM_FILE)
#define PROMPT_FILE_PATH    "voice/prompt.bin"
#else
#include "test_pcm_data.h"
#endif

const FsmStateItem_t VoiceWakeupFSMClass::fsmStateItems[VOICE_STATE_MAX] = {
    // VOICE_STATE_IDLE
    {
        stateIdleAction, // stateAction
        {
            {VOICE_STATE_DETECT, eventIdleVadDetectedAction}, // Event0
            {VOICE_STATE_IDLE, NULL}, // Event1
        }
    }, 
    // VOICE_STATE_DETECT
    {
        stateDetectAction, // stateAction
        {
            {VOICE_STATE_PLAY, eventKwsDetectedAction}, // Event0
            {VOICE_STATE_IDLE, eventKwsNotDetectedAction}, // Event1
        }
    }, 
    // VOICE_STATE_PLAY
    {
        statePlayAction, // stateAction
        {
            {VOICE_STATE_WAKEUP, eventPlayFinishedAction}, // Event0
            {VOICE_STATE_WAKEUP, eventPlayInterruptedAction}, // Event1
        }
    }, 
    // VOICE_STATE_WAKEUP
    {
        stateWakeupAction, // stateAction
        {
            {VOICE_STATE_SPEAK, eventWakeupVadDetectedAction}, // Event0
            {VOICE_STATE_IDLE, eventWakeupVadTimeoutAction}, // Event1
        }
    }, 
    // VOICE_STATE_SPEAK
    {
        stateSpeakAction, // stateAction
        {
            {VOICE_STATE_PLAY, eventSpeakFinishedAction}, // Event0
            {VOICE_STATE_SPEAK, NULL}, // Event1
        }
    }
};

static int getBit1Number(uint8_t byte) {
  int ret = 0;
  for(int i = 0; i < 8; i++) {
    if(byte & (1 << i)) ret++;
  }
  return ret;
}


static void resetRecord() {
    KWS.initFeature();
    Audio.clearRecord();
}

bool VoiceWakeupFSMClass::init(const char *kCategoryLabels[], size_t categoryLabelsCount, int targetCategoryIndex, const unsigned char *modelData) {
    if(!Audio.begin(SAMPLES_PER_FRAME, SAMPLE_RATE)) {
        return false;
    }

    Audio.startPlay();
    Audio.startRecord();
    log_d("kCategoryLabels:%p, CATEGORY_LABELS_COUNT:%d", kCategoryLabels, categoryLabelsCount);
    VoiceWakeup.prepareVad(SAMPLES_PER_FRAME);
    this->targetCategoryIndex = targetCategoryIndex;

    if(!kCategoryLabels || categoryLabelsCount == 0) {
        return false;
    }

    KWS.resetModel(kCategoryLabels, categoryLabelsCount, modelData);
    resetRecord();

    return true;
}

void VoiceWakeupFSMClass::stateIdleAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    size_t recordLen = sizeof(pcmBuffer);
    if(Audio.recordAvailable() < recordLen) {
        return;
    }
    recordLen = Audio.readRecordAsync(fsm->pcmBuffer, recordLen);
    recordLen = Audio.dualTomono(fsm->pcmBuffer, recordLen);
    //do voice detection
    bool isVoice = VoiceWakeup.doVad(fsm->pcmBuffer, recordLen, true, true, VAD_RMS_THRESHOLD, VAD_ZCR_THRESHOLD_MIN, VAD_ZCR_THRESHOLD_MAX);
    fsm->detectFlag = (fsm->detectFlag << 1) | isVoice;

    if(fsm->pcmCache.freeSize() < recordLen) {
        fsm->pcmCache.pop(recordLen);
    }
    fsm->pcmCache.push(fsm->pcmBuffer, recordLen);

    if(getBit1Number(fsm->detectFlag) >= VOICE_DETECT_BITS) {
        fsm->setEvent(VOICE_EVENT_IDLE_VAD_DETECTED);
        fsm->detectFlag = 0;
    }
}

void VoiceWakeupFSMClass::stateDetectAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
     while(fsm->pcmCache.freeSize() >= SAMPLES_BYTES_PER_FRAME && Audio.recordAvailable() >= sizeof(fsm->pcmBuffer)) {
      size_t recordLen = Audio.readRecordAsync(fsm->pcmBuffer, sizeof(fsm->pcmBuffer));
      recordLen = Audio.dualTomono(fsm->pcmBuffer, recordLen);
      fsm->pcmCache.push(fsm->pcmBuffer, recordLen);   
    }

    if(fsm->pcmCache.available() >= SAMPLES_BYTES_PER_FRAME) {
        fsm->pcmCache.copy(fsm->pcmBuffer, SAMPLES_BYTES_PER_FRAME);
        fsm->pcmCache.pop(SAMPLES_BYTES_PER_FRAME);
        KWS.extractFeature(fsm->pcmBuffer, SAMPLES_BYTES_PER_FRAME);
        fsm->frameCnt++;
    }

    if (fsm->frameCnt >= 50) {
      uint32_t start = millis();
      bool ret = KWS.detect(fsm->targetCategoryIndex);
      log_i("detect cost:%dms, ret:%d", millis() - start, ret);
      if(ret) {
        fsm->setEvent(VOICE_EVENT_KWS_DETECTED);
      } else {
        fsm->setEvent(VOICE_EVENT_KWS_NOT_DETECTED);
      }
    }
}

void VoiceWakeupFSMClass::statePlayAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    if (fsm->playContext.playOption == PLAY_OPTION_PROMPT) {
    #ifdef USE_PROMPT_FROM_FILE
        //read pcm from file
        if (fsm->playContext.dataOffset < fsm->playContext.dataLen && Audio.playFreeSize() >= sizeof(fsm->pcmBuffer)) {
            size_t len = fsm->playContext.file.readBytes((char *)fsm->pcmBuffer, sizeof(pcmBuffer) / 2);
            fsm->playContext.dataOffset += len;
            len = Audio.monoTodual(fsm->pcmBuffer, len, sizeof(pcmBuffer));
            Audio.playAsync(fsm->pcmBuffer, len);
        } else if (fsm->playContext.dataOffset >= fsm->playContext.dataLen) {
            fsm->setEvent(VOICE_EVENT_PLAY_FINISHED);
        }

    #else 
        //play data from test_pcm_data.h
        Audio.play((void *)test_pcm_data, sizeof(test_pcm_data), 2000);
        fsm->setEvent(VOICE_EVENT_PLAY_FINISHED);
    #endif
    } else if (fsm->playContext.playOption == PLAY_OPTION_ANSWER) {
        //play data in opusCache, wait external event VOICE_EVENT_SPEAK_FINISHED
        uint8_t buffer[64];
        if (fsm->opusCache.available() > 0) {
            fsm->opusCache.copy(buffer, 1);
            size_t opus_packet_len = buffer[0];
            if((fsm->opusCache.available() >= (opus_packet_len + 1)) && (Audio.playFreeSize() >= sizeof(fsm->pcmBuffer))) {
                if (opus_packet_len > sizeof(buffer)) {
                    log_e("opus packet len error.");
                    fsm->opusCache.pop(opus_packet_len);
                    return;
                }
                fsm->opusCache.pop(1);
                fsm->opusCache.copy(buffer, opus_packet_len);
                fsm->opusCache.pop(opus_packet_len);
                size_t outLen = 0;
                // log_d("opus_packet_len:%d, %d %d %d", opus_packet_len, buffer[0], buffer[1], buffer[2]);
                if(Opus.decode(buffer, opus_packet_len, fsm->pcmBuffer, sizeof(fsm->pcmBuffer), &outLen)) {
                size_t rwLen = Audio.monoTodual(fsm->pcmBuffer, outLen, sizeof(fsm->pcmBuffer));
                Audio.playAsync(fsm->pcmBuffer, rwLen);
                } else {
                    log_e("Opus.decode failed");
                }
            }
        }
    }
}

void VoiceWakeupFSMClass::stateWakeupAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    (void)fsm;
    size_t recordLen = sizeof(pcmBuffer);
    if(Audio.recordAvailable() < recordLen) {
        return;
    }
    recordLen = Audio.readRecordAsync(fsm->pcmBuffer, recordLen);
    recordLen = Audio.dualTomono(fsm->pcmBuffer, recordLen);
    //do voice detection
    bool isVoice = VoiceWakeup.doVad(fsm->pcmBuffer, recordLen, true, true, VAD_RMS_THRESHOLD, VAD_ZCR_THRESHOLD_MIN, VAD_ZCR_THRESHOLD_MAX);
    fsm->detectFlag = (fsm->detectFlag << 1) | isVoice;

    // log_d("voiceWakeupContext.detectFlag:%02x", voiceWakeupContext.detectFlag);

    if(fsm->pcmCache.freeSize() < recordLen) {
        fsm->pcmCache.pop(recordLen);
    }
    fsm->pcmCache.push(fsm->pcmBuffer, recordLen);

    if(getBit1Number(fsm->detectFlag) >= VOICE_DETECT_BITS) {
        fsm->setEvent(VOICE_EVENT_WAKEUP_VAD_DETECTED);
    } else {
        if(millis() - fsm->timeRef >= DETECT_TIME_MS) {
            log_w("wakeup timeout<%dms>", DETECT_TIME_MS);
            fsm->setEvent(VOICE_EVENT_WAKEUP_VAD_TIMEOUT);
            fsm->detectFlag = 0;
        }
    }
}

void VoiceWakeupFSMClass::stateSpeakAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    (void)fsm;
    size_t recordLen = sizeof(fsm->pcmBuffer);
    if(Audio.recordAvailable() < recordLen) {
        return;
    }
    recordLen = Audio.readRecordAsync(fsm->pcmBuffer, recordLen);
    recordLen = Audio.dualTomono(fsm->pcmBuffer, recordLen);

    if(fsm->onProcSpeak) {
        fsm->onProcSpeak(fsm->pcmBuffer, recordLen);
    }

    //do voice detection
    bool isVoice = VoiceWakeup.doVad(fsm->pcmBuffer, recordLen, true, true, VAD_RMS_THRESHOLD, VAD_ZCR_THRESHOLD_MIN, VAD_ZCR_THRESHOLD_MAX);
    fsm->detectFlag = (fsm->detectFlag << 1) | isVoice;

    do {
        if(fsm->detectFlag != 0x00) {
            fsm->timeRef = millis();
            break;
        }

        if(millis() - fsm->timeRef >= DETECT_TIME_MS) {
            fsm->setEvent(VOICE_EVENT_SPEAK_FINISHED);
        }
    }while (0);

}

void VoiceWakeupFSMClass::eventIdleVadDetectedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    fsm->detectFlag = 0;
    fsm->frameCnt = VOICE_DETECT_BITS;
    fsm->timeRef = millis();
    fsm->pcmCache.pop(fsm->pcmCache.available() - VOICE_DETECT_BITS * SAMPLES_BYTES_PER_FRAME);
    log_d("voice detect, pcmCache:%d, freeSize:%d", fsm->pcmCache.available(), fsm->pcmCache.freeSize());
}

void VoiceWakeupFSMClass::eventKwsDetectedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    fsm->playContext.playOption = PLAY_OPTION_PROMPT;
#ifdef USE_PROMPT_FROM_FILE
    fsm->playContext.file = littleFS.open(PROMPT_FILE_PATH);
    fsm->playContext.dataLen = fsm->playContext.file.size();
#else 
    fsm->playContext.dataLen = 0;
#endif
    fsm->playContext.dataOffset = 0;
    log_d("kws detected, play prompt");
}

void VoiceWakeupFSMClass::eventKwsNotDetectedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    (void)fsm;
    resetRecord();
    log_d("kws not detected");
}

void VoiceWakeupFSMClass::eventPlayFinishedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    fsm->detectFlag = 0;
    fsm->frameCnt = 0;
    fsm->timeRef = millis();
    fsm->pcmCache.pop(fsm->pcmCache.available());
    log_d("play finished");
}

void VoiceWakeupFSMClass::eventPlayInterruptedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    (void)fsm;
    log_d("play interrupted");
}

void VoiceWakeupFSMClass::eventWakeupVadDetectedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    if(fsm->onStartSpeak) {
        fsm->onStartSpeak();
    }
    fsm->detectFlag = 0xff;
    fsm->timeRef = millis();
    log_d("wakeup vad detected");
}

void VoiceWakeupFSMClass::eventWakeupVadTimeoutAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    (void)fsm;
    fsm->detectFlag = 0;
    resetRecord();
    log_d("wakeup vad timeout");
}

void VoiceWakeupFSMClass::eventSpeakFinishedAction(void *para) {
    if(!para) return;
    VoiceWakeupFSMClass *fsm = (VoiceWakeupFSMClass *)para;
    if(fsm->onStopSpeak) {
        fsm->onStopSpeak();
    }
    fsm->playContext.playOption = PLAY_OPTION_ANSWER;
    log_d("speak finished");
}

void VoiceWakeupFSMClass::handleLoop(void) {
    if(currentState >= VOICE_STATE_MAX) {
        currentState = VOICE_STATE_IDLE;
        return;
    }
    const FsmStateItem_t *stateItem = &fsmStateItems[currentState];
    if(stateItem->stateAction) stateItem->stateAction(this);
    if(currentEvent >= VOICE_EVENT_MAX) {
        currentEvent = VOICE_EVENT_NONE;
        return;
    }
    const FsmStateSwitchItem_t *item = &stateItem->stateSwitchItems[currentEvent];
    if(item->eventAction) {
        item->eventAction(this);
    }
    currentState = ((item->nextState < VOICE_STATE_MAX)? item->nextState: VOICE_STATE_IDLE);
    currentEvent = VOICE_EVENT_NONE;
}

VoiceWakeupFSMClass::VoiceWakeupFSMClass() : 
    currentState(VOICE_STATE_IDLE), 
    currentEvent(VOICE_EVENT_NONE), 
    onStartSpeak(NULL),
    onStopSpeak(NULL),
    onProcSpeak(NULL),
    targetCategoryIndex(0),
    fsmMutex(Task.createMutex()),
    pcmCache(PCM_CACHE_BYTES),
    opusCache(OPUS_CACHE_BYTES),
    playContext(PLAY_OPTION_PROMPT, 0, 0),
    detectFlag(0),
    frameCnt(0),
    timeRef(0) {
}

#endif