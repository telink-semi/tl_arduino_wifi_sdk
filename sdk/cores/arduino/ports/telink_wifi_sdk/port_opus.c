/********************************************************************************************************
 * @file    port_opus.c
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
#if BASED_ON_TELINK_WIFI_SDK
#if PORT_CONFIG_USE_D25

#include "port_interface.h"
#include "core.h"

int port_opus_get_version(void) {
    return opus_get_version();
}
bool port_opus_prepare_encoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame, uint8_t application,
                          uint32_t bit_rate, uint8_t complexity, bool use_vbr) {
    return opus_prepare_encoder(channels, sample_rate, samples_per_frame, application, bit_rate, complexity, use_vbr);
}
bool port_opus_prepare_decoder(uint8_t channels, uint16_t sample_rate, uint16_t samples_per_frame) {
    return opus_prepare_decoder(channels, sample_rate, samples_per_frame);
}
bool port_opus_encode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    return opus_encode(input, input_len, output, max_output_len, output_len);
}
bool port_opus_decode(void *input, size_t input_len, void *output, size_t max_output_len, size_t *output_len) {
    return opus_decode(input, input_len, output, max_output_len, output_len);
}   

#endif
#endif