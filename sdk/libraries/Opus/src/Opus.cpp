/********************************************************************************************************
 * @file    Opus.cpp
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
#if PORT_CONFIG_OPUS_SUPPORTED

#include "OpusCommon.h"
#include "Arduino.h"

int OpusClass::getVersion(void) {
    return port_opus_get_version();
}

bool OpusClass::encodeInit(uint8_t channels, uint16_t sampleRate, uint32_t samplesPerFrame, opus_application_t application, uint32_t bitRate, uint8_t complexity, bool useVbr) {
    return port_opus_prepare_encoder(channels, sampleRate, samplesPerFrame, application, bitRate, complexity, useVbr);
}

bool OpusClass::decodeInit(uint8_t channels, uint16_t sampleRate, uint32_t samplesPerFrame) {
    return port_opus_prepare_decoder(channels, sampleRate, samplesPerFrame);
}

bool OpusClass::encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    return port_opus_encode(input, input_len, output, max_output_len, output_len);
}

bool OpusClass::decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    return port_opus_decode(input, input_len, output, max_output_len, output_len);
}

OpusClass Opus;

#endif