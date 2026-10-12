/********************************************************************************************************
 * @file    OpusCommon.h
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
#if PORT_CONFIG_OPUS_SUPPORTED

#include <inttypes.h>
#include <stddef.h>

typedef enum {
    OPUS_APPLICATION_AUDIO,
    OPUS_APPLICATION_VOIP,
    OPUS_APPLICATION_RESTRICTED_LOWDELAY,
} opus_application_t;

class OpusClass
{
protected:
public:
    OpusClass(){}
    ~OpusClass(){}
    int getVersion(void);
    bool encodeInit(uint8_t channels, uint16_t sampleRate, uint32_t samplesPerFrame, opus_application_t application, uint32_t bitRate=8000, uint8_t complexity=4, bool useVbr=true);
    bool decodeInit(uint8_t channels, uint16_t sampleRate, uint32_t samplesPerFrame);
    bool encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len);
    bool decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len);
};

extern OpusClass Opus;

#endif