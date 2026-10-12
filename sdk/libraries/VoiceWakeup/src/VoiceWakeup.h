/********************************************************************************************************
 * @file    VoiceWakeup.h
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
#pragma once

#include "port_config.h"
#if PORT_CONFIG_VOICE_WAKEUP_SUPPORTED

#include <inttypes.h>
#include <stddef.h>
#include "TaskCommon.h"
#include "FSM.h"

class VoiceWakeupClass : public VoiceWakeupFSMClass
{
protected:
    taskHandle_t taskHandle;
public:
    VoiceWakeupClass();
    ~VoiceWakeupClass(){}
    bool begin(const char *kCategoryLabels[], size_t categoryLabelsCount, int targetCategoryIndex, const unsigned char *modelData);
    bool end(void);
    static bool prepareVad(uint16_t frame_size);
    static bool doVad(uint8_t *data, uint32_t len, bool rms_enable=true, bool zcr_enable=true, uint16_t rms_threshold=500, uint16_t zcr_threshold_min=50, uint16_t zcr_threshold_max=200);
};

extern VoiceWakeupClass VoiceWakeup;

#endif