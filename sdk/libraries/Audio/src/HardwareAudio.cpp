/********************************************************************************************************
 * @file    HardwareAudio.cpp
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
#if PORT_CONFIG_AUDIO_SUPPORTED

#include "HardwareAudio.h"
#include "port_interface.h"
#include "Arduino.h"

#define SAMPLE_NUMBER_DEFAULT (800) // 16000Hz: 50ms
#define SAMPLE_RATE_DEFUALT   16000
#define VOLUE_DEFAULT         32.0
#define QUEUE_LENGTH_MAX      0 //not limit

/**
* @brief      This function serves to receive audio data for recording.
* @param[in]  audio - Pointer to the HardwareAudio instance.
* @param[in]  data - Buffer containing the audio data received.
* @param[in]  len - Length of the data buffer.
* @return     True if successful, false otherwise.
*/
bool _audio_task_record_callback(HardwareAudio *audio, uint8_t *data, uint32_t len) {
    if(!audio) return false;
    return audio->recordProcess(data, len);
}

/**
* @brief      This function serves to receive audio data for playback.
* @param[in]  audio - Pointer to the HardwareAudio instance.
* @param[in]  data - Buffer containing the audio data received.
* @param[in]  len - Length of the data buffer.
* @return     True if successful, NULL otherwise.
*/
bool _audio_task_play_callback(HardwareAudio *audio, uint8_t *data, uint32_t len) {
    
    if(!audio) return NULL;
    return audio->playProcess(data, len);
}

extern "C" {

/**
* @brief      This function serves to receive audio data for playback from a callback.
* @param[in]  ctx - Pointer to the context containing parent HardwareAudio instance.
* @param[in]  data - Buffer containing the audio data received.
* @param[in]  len - Length of the data buffer.
* @return     True if successful, false otherwise.
*/
bool audio_play_task_callback(void *ctx, uint8_t *data, uint32_t len) {
    if(!ctx) return false;
    port_audio_configuration_t audio_config = {0};
    port_audio_get_configuration(ctx, &audio_config);
    HardwareAudio *audio = (HardwareAudio *)audio_config.owner;
    return _audio_task_play_callback(audio, data, len);
}

/**
* @brief      This function serves to receive audio data for recording from a callback.
* @param[in]  ctx - Pointer to the context containing parent HardwareAudio instance.
* @param[in]  data - Buffer containing the audio data received.
* @param[in]  len - Length of the data buffer.
* @return     True if successful, false otherwise.
*/
bool audio_record_task_callback(void *ctx, uint8_t *data, uint32_t len) {
    if(!ctx) return false;
    port_audio_configuration_t audio_config = {0};
    port_audio_get_configuration(ctx, &audio_config);
    HardwareAudio *audio = (HardwareAudio *)audio_config.owner;
    return _audio_task_record_callback(audio, data, len);
}

}

/**
* @brief      This function serves to stop the audio processing.
* @param[in]  ctx - Pointer to the context containing parent HardwareAudio instance.
* @return     None
*/
void audio_stop_callback(void *ctx) {
    if(!ctx) return;
    port_audio_configuration_t audio_config = {0};
    port_audio_get_configuration(ctx, &audio_config);
    HardwareAudio *audio = (HardwareAudio *)audio_config.owner;
    if(audio) {
        audio->stop();
    }
}

/**
 * @brief      Constructor for the HardwareAudio class. Initializes default values and sets up audio buffers.
 */
HardwareAudio::HardwareAudio() : sample_number(SAMPLE_NUMBER_DEFAULT), sample_rate(SAMPLE_RATE_DEFUALT), volume(VOLUE_DEFAULT),
                                 play_buffer(NULL), record_buffer(NULL), audio_ctx(NULL){
    audio_ctx = port_audio_create_context(this);

}

/**
 * @brief      Destructor for the HardwareAudio class. Ensures that the hardware is stopped before destruction.
 */
HardwareAudio::~HardwareAudio() {
    if(opened()) {
        stop();
    }
    port_audio_destroy_context(audio_ctx);
}

/**
 * @brief      Initializes the hardware audio with specific parameters.
 * @param[in]  sample_number - The number of samples per buffer.
 * @param[in]  sample_rate - The sampling rate in Hz.
 * @param[in]  volume - The volume level (not used currently).
 * @return     True if initialization is successful, false otherwise.
 */
bool HardwareAudio::begin(uint32_t sample_number, uint32_t sample_rate, float volume) {
    this->sample_number = sample_number;
    this->sample_rate = sample_rate;
    this->volume = volume;
    return begin();
}

/**
 * @brief      Initializes the hardware audio with a specified period for each block.
 * @param[in]  period_ms - The duration of one block in milliseconds.
 * @return     True if initialization is successful, false otherwise.
 */
bool HardwareAudio::begin(uint32_t period_ms) {
    this->sample_number = period_ms * sample_rate / 1000;
    return begin();
}

/**
 * @brief      Initializes the hardware audio with default settings.
 * @return     True if initialization is successful, false otherwise.
 */
bool HardwareAudio::begin(void) {
    bool ret = false;
    uint32_t buffer_size = port_audio_get_buffer_size(sample_number, sample_rate);

    do {
        if(opened()) break;
        play_buffer = ring_buffer_create(buffer_size);
        if(!play_buffer) break;
        record_buffer = ring_buffer_create(buffer_size);
        if(!record_buffer) break;

        ret = port_audio_begin(audio_ctx, sample_number, sample_rate, true, true, volume);
    } while(0);

    if(!ret) {
        if(play_buffer) {
            ring_buffer_destroy(play_buffer);
            play_buffer = NULL;
        }
        if(record_buffer) {
            ring_buffer_destroy(record_buffer);
            record_buffer = NULL;
        }
    }

    return ret;
}

/**
 * @brief      Checks if the audio hardware is currently open.
 * @return     True if the audio hardware is open, false otherwise.
 */
bool HardwareAudio::opened(void) {
    return port_audio_is_open(audio_ctx);
}

/**
 * @brief      Stops the audio playback and recording.
 * @details    This function will stop the audio playback and recording if they are currently active. It also cleans up any allocated buffers.
 * @return     True if the stop operation was successful, false otherwise.
 */
bool HardwareAudio::stop(void) {
    bool ret = false;
    do {
        if(!opened()) break; // Check if the audio hardware is open
        if(!port_audio_stop(audio_ctx)) break; // Stop the audio playback and recording
        if(play_buffer) {
            ring_buffer_destroy(play_buffer); // Destroy the play buffer if it exists
            play_buffer = NULL;
        }
        if(record_buffer) {
            ring_buffer_destroy(record_buffer); // Destroy the record buffer if it exists
            record_buffer = NULL;
        }
        ret = true; // Set return value to true indicating success
    } while(0);
   return ret; 
}

/**
 * @brief      Asynchronously plays audio data.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @return     Number of bytes actually played.
 */
uint32_t HardwareAudio::playAsync(void *data, uint32_t len) {
    if(!opened()) return 0;
    if(ring_buffer_get_free_size(play_buffer) == 0) {
        return 0;
    }
    uint16_t ret = ring_buffer_push(play_buffer, (uint8_t *)data, len);
    return ret;
} 

/**
 * @brief      Synchronously plays audio data with a timeout.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @param[in]  timeout_ms - Maximum time to wait for playback completion in milliseconds.
 * @return     Total number of bytes actually played.
 */
uint32_t HardwareAudio::playSync(void *data, uint32_t len, uint32_t timeout_ms) {
    if(!opened()) return 0;
    uint8_t *cur = (uint8_t*)data;
    uint32_t rwLen, totalLen = 0;
    uint32_t startTime = millis();
    while(len) {
        rwLen = playAsync(cur, len);
        if(rwLen == 0) {
            if(timeout_ms == 0 || (uint32_t)(millis() - startTime) > timeout_ms) {
                break;
            }
            yield();
            continue;
        }
        len -= rwLen;
        cur += rwLen;
        totalLen += rwLen;
    }

    return totalLen;
}

/**
 * @brief      Flushes the audio data in the play buffer.
 * @return     None.
 */
void HardwareAudio::flush(void) {
    if(!opened()) return;
    while(port_audio_play_busy(audio_ctx)) {
        yield();
    }
    ring_buffer_pop(play_buffer, ring_buffer_available(play_buffer));
}

/**
 * @brief      Starts recording audio with a specified length.
 * @param[in]  len - The length of the audio to record in samples.
 * @return     True if recording starts successfully, false otherwise.
 */
bool HardwareAudio::startRecord(uint32_t len) {
    if(!opened()) return false;
    ring_buffer_pop(record_buffer, ring_buffer_available(record_buffer));
    return port_audio_start_record(audio_ctx, len);
}

/**
 * @brief      Calculates the data size based on a period of time.
 * @param[in]  period_ms - The duration in milliseconds for which to calculate the data size.
 * @return     The calculated data size in samples.
 */
uint32_t HardwareAudio::getDataSize(uint32_t period_ms) {
    uint32_t samples = period_ms * sample_rate / 1000;
    return port_audio_get_buffer_size(samples, sample_rate);
}

/**
 * @brief      Gets the frame size for audio data.
 * @return     The frame size in bytes.
 */
uint32_t HardwareAudio::getFrameSize(void) {
    return port_audio_get_frame_size(audio_ctx);
}

/**
 * @brief      Checks how many audio samples are available for recording.
 * @return     Number of audio samples available in the record buffer.
 */
uint32_t HardwareAudio::recordAvailable(void) {
    if(!opened()) return 0;
    return ring_buffer_available(record_buffer);
}

/**
 * @brief      Clears the audio data in the record buffer.
 * @return     True if the record buffer is successfully cleared, false otherwise.
 */
bool HardwareAudio::clearRecord(void) {
    if(!opened()) return false;
    ring_buffer_pop(record_buffer, ring_buffer_available(record_buffer));
    return true;
}

/**
* @brief      Checks how many audio samples are available for playback.
* @return     Number of audio samples available in the play buffer.
*/
uint32_t HardwareAudio::playFreeSize(void) {
    if(!opened()) return 0;
    return ring_buffer_get_free_size(play_buffer);
}

/**
* @brief      Stops the current recording session.
* @return     True if the recording is successfully stopped, false otherwise.
*/
bool HardwareAudio::stopRecord(void) {
    ring_buffer_pop(record_buffer, ring_buffer_available(record_buffer));
    return port_audio_stop_record(audio_ctx);
}

/**
 * @brief      Starts playback audio with a specified length.
 * @param[in]  len - The length of the audio to play in samples.
 * @return     True if playback starts successfully, false otherwise.
 */
bool HardwareAudio::startPlay(uint32_t len) {
    if(!opened()) return false;
    ring_buffer_pop(play_buffer, ring_buffer_available(play_buffer));
    return port_audio_start_play(audio_ctx, len);
}

/**
 * @brief      Stops the current playback session.
 * @return     True if the playback is successfully stopped, false otherwise.
 */
bool HardwareAudio::stopPlay(void) {
    ring_buffer_pop(play_buffer, ring_buffer_available(play_buffer));
    return port_audio_stop_play(audio_ctx);
}

/**
 * @brief      Plays audio data with a timeout.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @param[in]  timeout_ms - Maximum time to wait for playback completion in milliseconds.
 * @return     Total number of bytes actually played.
 */
uint32_t HardwareAudio::play(void *data, uint32_t len, uint32_t timeout_ms) {
    startPlay(len);
    uint32_t ret = playSync(data, len, timeout_ms);
    stopPlay();
    return ret; 
}

/**
 * @brief      Records audio data with a timeout.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @param[in]  timeout_ms - Maximum time to wait for recording completion in milliseconds.
 * @return     Total number of bytes actually recorded.
 */
uint32_t HardwareAudio::record(void *data, uint32_t len, uint32_t timeout_ms) {
    startRecord(len);
    uint32_t ret = readRecordSync(data, len, timeout_ms);
    stopRecord();
    return ret; 
}

/**
 * @brief      Processes recorded audio data.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @return     True if the data was processed successfully, false otherwise.
 */
bool HardwareAudio::recordProcess(uint8_t *data, uint32_t len) {
    if(!opened()) return false;
    if(ring_buffer_get_free_size(record_buffer) < len) return false;
    ring_buffer_push(record_buffer, data, len);
    return true;
}

/**
 * @brief      Processes audio data for playback.
 * @param[in]  data - Pointer to the audio data buffer.
 * @param[in]  len - Length of the audio data in bytes.
 * @return     True if the data was processed successfully, false otherwise.
 */
bool HardwareAudio::playProcess(uint8_t *data, uint32_t len) {
    if(!opened()) return false;
    if(ring_buffer_available(play_buffer) < len) return false;
    ring_buffer_copy(play_buffer, data, len);
    ring_buffer_pop(play_buffer, len);
    return true;
}

/**
 * @brief      Asynchronously reads recorded audio data.
 * @param[in]  data - Pointer to the buffer where the audio data will be stored.
 * @param[in]  len - Length of the buffer in bytes.
 * @return     Number of bytes actually read.
 */
uint32_t HardwareAudio::readRecordAsync(void *data, uint32_t len) {
    if(!opened()) return 0;
    if(ring_buffer_available(record_buffer) == 0) return 0;
    uint16_t ret = ring_buffer_copy(record_buffer, (uint8_t*)data, len);
    if(ret != 0) {
        ring_buffer_pop(record_buffer, ret);
    }
    return ret;
}

/**
 * @brief      Synchronously reads recorded audio data with a timeout.
 * @param[in]  data - Pointer to the buffer where the audio data will be stored.
 * @param[in]  len - Length of the buffer in bytes.
 * @param[in]  timeout_ms - Maximum time to wait for data availability in milliseconds.
 * @return     Total number of bytes actually read.
 */
uint32_t HardwareAudio::readRecordSync(void *data, uint32_t len, uint32_t timeout_ms) {
    if(!opened()) return 0;
    uint8_t *cur = (uint8_t *)data;
    uint32_t readLen = 0;
    uint32_t ret = 0;
    uint32_t startTime = millis();

    while(readLen < len) {
        ret = readRecordAsync(cur, len - readLen);
        if(ret != 0) {
            readLen += ret;
            cur += ret;
        }

        if(timeout_ms == 0 || (uint32_t)(millis() - startTime) >= timeout_ms) {
            break;
        } else {
            yield();
        }
    }

    return readLen;
}

/**
 * @brief      Clears the audio data in the record buffer.
 * @return     None.
 */
void HardwareAudio::clear() {
    if(!opened()) return;
    ring_buffer_pop(record_buffer, ring_buffer_available(record_buffer));
}

/**
 * @brief      Checks if playback is currently busy.
 * @return     True if playback is busy, false otherwise.
 */
bool HardwareAudio::isPlayBusy(void) {
    return port_audio_play_busy(audio_ctx);
}

/**
 * @brief      Checks if recording is currently busy.
 * @return     True if recording is busy, false otherwise.
 */
bool HardwareAudio::isRecordBusy(void) {
    return port_audio_record_busy(audio_ctx);
}

/**
 * @brief      Converts dual-channel PCM data to mono-channel PCM data.
 * @param[in]  pcm_data - Pointer to the dual-channel PCM data buffer.
 * @param[in]  byte_len - Length of the PCM data in bytes.
 * @return     Total number of bytes of mono-channel PCM data.
 */
size_t HardwareAudio::dualTomono(uint8_t *pcm_data, size_t byte_len) {
    if(byte_len & 0x3) return 0;
    size_t mono_len = byte_len >> 2;
    for (size_t i = 0; i < mono_len; i++) {
    #if 0
        uint16_t mono = (pcm_data[i * 4 + 1] << 8) | pcm_data[i * 4];
    #else
        uint32_t left = (pcm_data[i * 4 + 1] << 8) | pcm_data[i * 4];
        uint32_t right = (pcm_data[i * 4 + 3] << 8) | pcm_data[i * 4 + 2];
        uint16_t mono = (left + right) >> 1;
    #endif
        pcm_data[i * 2] = mono & 0xFF;
        pcm_data[i * 2 + 1] = (mono >> 8) & 0xFF;
    }
    return mono_len << 1;
}

/**
 * @brief      Converts mono-channel PCM data to dual-channel PCM data.
 * @param[in]  pcm_data - Pointer to the mono-channel PCM data buffer.
 * @param[in]  byte_len - Length of the PCM data in bytes.
 * @param[in]  max_byte_len - Maximum length of the output buffer in bytes.
 * @return     Total number of bytes of dual-channel PCM data.
 */
size_t HardwareAudio::monoTodual(uint8_t *pcm_data, size_t byte_len, size_t max_byte_len) {
    if((byte_len & 0x1) || (byte_len > max_byte_len)) return 0;
    int dual_len = byte_len >> 1;
    for (int i = dual_len-1; i >= 0; i--) {
        int16_t mono = (pcm_data[i * 2 + 1] << 8) | pcm_data[i * 2];
        pcm_data[i * 4] = mono & 0xFF;
        pcm_data[i * 4 + 1] = (mono >> 8) & 0xFF;
        pcm_data[i * 4 + 2] = mono & 0xFF;
        pcm_data[i * 4 + 3] = (mono >> 8) & 0xFF;
    }
    return dual_len << 2;
}

HardwareAudio Audio;

#endif