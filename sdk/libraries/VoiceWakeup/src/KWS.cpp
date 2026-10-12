/********************************************************************************************************
 * @file    KWS.cpp
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

#include "KWSCommon.h"
#include "Arduino.h"

/**
 * @brief      Resets the KWS model with the provided category labels and model data.
 * @param[in]  catergroy_labels - Array of category labels.
 * @param[in]  category_labels_count - Number of category labels.
 * @param[in]  model_data - Pointer to the model data buffer.
 * @return     True if the model is successfully reset, false otherwise.
 */
bool KWSClass::resetModel(const char ** catergroy_labels, int category_labels_count, const unsigned char * model_data) {
    return port_tflm_reset(catergroy_labels, category_labels_count, model_data);
}

/**
 * @brief      Initializes the feature extraction model.
 * @return     True if the model is successfully initialized, false otherwise.
 */
bool KWSClass::initFeature(void) {
    return port_tflm_init_feature();
}

/**
 * @brief      Extracts features from the audio data.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @return     True if features are successfully extracted, false otherwise.
 */
bool KWSClass::extractFeature(uint8_t * data, uint32_t len) {
    return port_tflm_cal_feature(data, len);
}

/**
 * @brief      Detects the expected keyword in the audio data.
 * @param[in]  expected_label_index - The index of the expected keyword label.
 * @return     True if the expected keyword is detected, false otherwise.
 */
bool KWSClass::detect(int expected_label_index) {
    return port_tflm_detect(expected_label_index);
}

KWSClass KWS;

#endif