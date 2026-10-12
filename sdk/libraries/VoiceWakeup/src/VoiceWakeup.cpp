/********************************************************************************************************
 * @file    VoiceWakeup.cpp
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

#include "VoiceWakeup.h"
#include "Arduino.h"
#include "FSM.h"

static void voiceWakeupHandler(void *para) {
    if(!para) return;
    VoiceWakeupClass *voiceWakeup = (VoiceWakeupClass *)para;
    voiceWakeup->handleLoop();
    yield();
}


/**
 * @brief      VoiceWakeupClass constructor.
 * @details    Initializes the VoiceWakeupClass object with default values.
 */
VoiceWakeupClass::VoiceWakeupClass() : taskHandle(NULL) {

}


/**
 * @brief      Prepares Voice Activity Detection (VAD) with the specified frame size.
 * @param[in]  frame_size - Size of each audio frame in bytes.
 * @return     True if VAD is successfully prepared, false otherwise.
 */
bool VoiceWakeupClass::prepareVad(uint16_t frame_size) {
    return port_audio_prepare_vad(frame_size);
}

/**
 * @brief      Performs Voice Activity Detection (VAD) on the audio data.
 * @param[in]  rms_enable - Enable RMS thresholding.
 * @param[in]  zcr_enable - Enable Zero Crossing Rate thresholding.
 * @param[in]  rms_threshold - RMS threshold value.
 * @param[in]  zcr_threshold_min - Minimum Zero Crossing Rate threshold value.
 * @param[in]  zcr_threshold_max - Maximum Zero Crossing Rate threshold value.
 * @param[in]  len - Length of the audio data in bytes.
 * @param[in]  data - Pointer to the audio data buffer.
 * @return     True if voice activity is detected, false otherwise.
 */
bool VoiceWakeupClass::doVad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max) {
    return port_audio_do_vad(data, len, rms_enable, zcr_enable, rms_threshold, zcr_threshold_min, zcr_threshold_max);
}

/**
 * @brief      Initializes the VoiceWakeupClass object.
 * @param[in]  kCategoryLabels - Array of category labels for wakeup keywords.
 * @param[in]  categoryLabelsCount - Number of category labels.
 * @param[in]  modelData - Pointer to the pre-trained model data.
 * @return     True if initialization is successful, false otherwise.
 */
bool VoiceWakeupClass::begin(const char *kCategoryLabels[], size_t categoryLabelsCount, int targetCategoryIndex, const unsigned char *modelData) {
    if(!init(kCategoryLabels, categoryLabelsCount, targetCategoryIndex, modelData)) {
        return false;
    }
    taskHandle = Task.createTask(voiceWakeupHandler, this, true);
    return taskHandle != NULL;
}

/**
 * @brief      Releases resources used by the VoiceWakeupClass object.
 * @details    Deletes the task created for voice wakeup events.
 * @return     True if release is successful, false otherwise.
 */
bool VoiceWakeupClass::end(void) {
    if(taskHandle) {
        if(Task.deleteTask(taskHandle)) {
            taskHandle = NULL;
        }
    }
    return taskHandle == NULL;
}


VoiceWakeupClass VoiceWakeup;

#endif