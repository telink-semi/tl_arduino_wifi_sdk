/********************************************************************************************************
 * @file    HardwareAudio.h
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
#if PORT_CONFIG_AUDIO_SUPPORTED

#include <stdint.h>
#include <stddef.h>
#include "port_interface.h"

#define  AUDIO_MAX_LEN (0xffffffff)
class HardwareAudio {
protected:
    uint32_t sample_number;
    uint32_t sample_rate;
    float volume;
    ring_buffer_handle_t play_buffer;
    ring_buffer_handle_t record_buffer;
    bool recordProcess(uint8_t *data, uint32_t len);
    bool playProcess(uint8_t *data, uint32_t len);
public:
    void *audio_ctx;
    HardwareAudio();
    ~HardwareAudio();
    bool begin(uint32_t sample_number, uint32_t sample_rate, float volume=0);
    bool begin(void);
    bool begin(uint32_t period_ms);
    bool stop(void);
    void flush(void);
    void clear(void);
    bool startRecord(uint32_t len=AUDIO_MAX_LEN);
    bool stopRecord(void);
    bool startPlay(uint32_t len=AUDIO_MAX_LEN);
    bool stopPlay(void);
    bool opened(void);
    uint32_t record(void *data, uint32_t len, uint32_t timeout_ms);
    uint32_t play(void *data, uint32_t len, uint32_t timeout_ms);
    uint32_t playAsync(void *data, uint32_t len);
    uint32_t playSync(void *data, uint32_t len, uint32_t timeout_ms);
    uint32_t readRecordAsync(void *data, uint32_t len);
    uint32_t readRecordSync(void *data, uint32_t len, uint32_t timeout_ms);
    friend bool _audio_task_record_callback(HardwareAudio *audio, uint8_t *data, uint32_t len);
    friend bool _audio_task_play_callback(HardwareAudio *audio, uint8_t *data, uint32_t len);
    bool isPlayBusy(void);
    bool isRecordBusy(void);
    uint32_t getDataSize(uint32_t period_ms);
    uint32_t getFrameSize(void);
    uint32_t recordAvailable(void);
    uint32_t playFreeSize(void);
    bool clearRecord(void);
    static size_t dualTomono(uint8_t *pcm_data, size_t byte_len);
    static size_t monoTodual(uint8_t *pcm_data, size_t byte_len, size_t max_byte_len);
};

extern HardwareAudio Audio;

#endif