/********************************************************************************************************
 * @file    FSM.h
 *
 * @brief   This is the header file for Arduino SDK
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
#pragma once

#include "port_config.h"
#if PORT_CONFIG_VOICE_WAKEUP_SUPPORTED

#include <stdint.h>
#include <stddef.h>
#include "RingBuffer.h"
#include "TaskCommon.h"

#define SAMPLE_RATE               16000
#define SAMPLES_PER_FRAME         320 //20ms@16kHz
#define MILLIS_PER_FRAME          (1000 * SAMPLES_PER_FRAME / SAMPLE_RATE)
#define SAMPLES_BYTES_PER_FRAME   (SAMPLES_PER_FRAME * 2 * 1) // 2 bytes per channel, 1 channel
#define VOICE_DETECT_BITS         5
#define PCM_CACHE_BYTES           ((VOICE_DETECT_BITS + 3) * SAMPLES_BYTES_PER_FRAME)
#define DETECT_TIME_MS            1000
#define DETECT_TIMEOUT_MS         (DETECT_TIME_MS - VOICE_DETECT_BITS * MILLIS_PER_FRAME)
#define OPUS_CACHE_BYTES          (512) 

#define VAD_RMS_THRESHOLD         80
#define VAD_ZCR_THRESHOLD_MIN     20
#define VAD_ZCR_THRESHOLD_MAX     80

// #define USE_PROMPT_FROM_FILE 

#ifdef USE_PROMPT_FROM_FILE
#include "FS.h"
#endif

typedef enum {
    VOICE_STATE_IDLE = 0,
    VOICE_STATE_DETECT,
    VOICE_STATE_PLAY,
    VOICE_STATE_WAKEUP,
    VOICE_STATE_SPEAK,
    VOICE_STATE_MAX,
} VoiceWakeupState_t;

typedef enum {
    PLAY_OPTION_PROMPT = 0,
    PLAY_OPTION_ANSWER = 1,
} VoiceWakeupPlayOption_t;

struct _VoiceWakeupPlayContext_t {
    VoiceWakeupPlayOption_t playOption;
    uint32_t dataLen;
    uint32_t dataOffset;
#ifdef USE_PROMPT_FROM_FILE
    fs::File file;
#endif
    _VoiceWakeupPlayContext_t(VoiceWakeupPlayOption_t option, uint32_t len, uint32_t offset) : playOption(option), dataLen(len), dataOffset(offset) {}
};

typedef struct _VoiceWakeupPlayContext_t VoiceWakeupPlayContext_t;

typedef enum {
    VOICE_EVENT_0 = 0,
    VOICE_EVENT_1 = 1,
    VOICE_EVENT_MAX,

    VOICE_EVENT_IDLE_VAD_DETECTED = VOICE_EVENT_0, 

    VOICE_EVENT_KWS_DETECTED = VOICE_EVENT_0,
    VOICE_EVENT_KWS_NOT_DETECTED = VOICE_EVENT_1,

    VOICE_EVENT_PLAY_FINISHED = VOICE_EVENT_0,
    VOICE_EVENT_PLAY_INTERRUPTED = VOICE_EVENT_1,
    
    VOICE_EVENT_WAKEUP_VAD_DETECTED = VOICE_EVENT_0,
    VOICE_EVENT_WAKEUP_VAD_TIMEOUT = VOICE_EVENT_1,

    VOICE_EVENT_SPEAK_FINISHED = VOICE_EVENT_0,

    VOICE_EVENT_NONE = VOICE_EVENT_MAX,
} VoiceWakeupEvent_t;


typedef void (*ActionFunc_t)(void*);
typedef void (*OnStartSpeakFunc_t)(void);
typedef void (*OnStopSpeakFunc_t)(void);
typedef void (*OnProcSpeakFunc_t)(uint8_t *data, uint32_t len);

typedef struct {
    VoiceWakeupState_t nextState;
    ActionFunc_t eventAction;
} FsmStateSwitchItem_t;

typedef struct {
    ActionFunc_t stateAction;
    FsmStateSwitchItem_t stateSwitchItems[VOICE_EVENT_MAX];
} FsmStateItem_t;


class VoiceWakeupFSMClass {
protected:
    VoiceWakeupState_t currentState;
    VoiceWakeupEvent_t currentEvent;
    OnStartSpeakFunc_t onStartSpeak;
    OnStopSpeakFunc_t onStopSpeak;
    OnProcSpeakFunc_t onProcSpeak;
    int targetCategoryIndex;
    mutexHandle_t fsmMutex;
    using FsmStateItemArray_t = FsmStateItem_t[VOICE_STATE_MAX];
    static const FsmStateItemArray_t fsmStateItems;
public:
    RingBuffer pcmCache;
    RingBuffer opusCache;
    VoiceWakeupPlayContext_t playContext;
    uint8_t detectFlag;
    uint32_t frameCnt;
    uint32_t timeRef;
    uint8_t pcmBuffer[SAMPLES_PER_FRAME * 2 * 2]; // 2 bytes per channel, 2 channels
public:
    VoiceWakeupState_t getCurrentState(void) { return currentState; }
    VoiceWakeupEvent_t getCurrentEvent(void) { return currentEvent; }
    void setState(VoiceWakeupState_t state) { Task.lockMutex(fsmMutex); currentState = state; Task.unlockMutex(fsmMutex); }
    void setEvent(VoiceWakeupEvent_t event) { Task.lockMutex(fsmMutex); currentEvent = event; Task.unlockMutex(fsmMutex); }
public:
    static void stateIdleAction(void *para);
    static void stateDetectAction(void *para);
    static void statePlayAction(void *para);
    static void stateWakeupAction(void *para);
    static void stateSpeakAction(void *para);
    static void eventIdleVadDetectedAction(void *para);
    static void eventKwsDetectedAction(void *para);
    static void eventKwsNotDetectedAction(void *para);
    static void eventPlayFinishedAction(void *para);
    static void eventPlayInterruptedAction(void *para);
    static void eventWakeupVadDetectedAction(void *para);
    static void eventWakeupVadTimeoutAction(void *para);
    static void eventSpeakFinishedAction(void *para);
public:
    VoiceWakeupFSMClass();
    ~VoiceWakeupFSMClass(){}
    bool init(const char *kCategoryLabels[], size_t categoryLabelsCount, int targetCategoryIndex, const unsigned char *modelData);
    void playInterrupt(void) { if(currentState == VOICE_STATE_PLAY) { setEvent(VOICE_EVENT_PLAY_INTERRUPTED); } }
    void registerOnStartSpeak(OnStartSpeakFunc_t func) { onStartSpeak = func; }
    void registerOnStopSpeak(OnStopSpeakFunc_t func) { onStopSpeak = func; }
    void registerOnProcSpeak(OnProcSpeakFunc_t func) { onProcSpeak = func; }
    void handleLoop(void);
};

#endif