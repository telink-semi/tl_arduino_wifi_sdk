/*******************************************************************************************************
 *
 * @file    tlsr_audio.h
 *
 * @brief   This is the source file for TLSR9118
 *
 * @author  
 * @par     Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * @author  
 * @par     Copyright 2023-2025 Senscomm Semiconductor Co., Ltd.
 * 
 * @author  Telink WIFI GROUP
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 
#ifndef __TLSR_AUDIO_H__
#define __TLSR_AUDIO_H__

#include <stdint.h>

#include "wits_err.h"
#include "tlsr_i2s.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Audio interface type
 */
enum tlsr_audio_if {
    TLSR_AUDIO_INPUT, /* MIC, ADC */
    TLSR_AUDIO_OUTPUT /* HP(SPK), DAC */
};

/**
 * Audio volume
 */
struct tlsr_audio_volume {
    float min;
    float cur;
    float max;
};

/**
 * Audio CODEC device configuration
 */
struct tlsr_audio_cfg {
    uint32_t mclk_freq;             /* MCLK frequency */
	enum tlsr_i2s_wl word_length;	/* word length in bits */
	enum tlsr_i2s_fmt format;		/* I2S data format */
	enum tlsr_i2s_role role;			/* Role on I2S bus */
	uint32_t fs;					/* Sampling frequency, i.e., LRCLK frequency */
};

/**
 * @brief Initialize Audio CODEC
 */
int tlsr_audio_init(void);

/**
 * @brief Deinitialize Audio CODEC
 */
int tlsr_audio_deinit(void);

/**
 * @brief Configure Audio CODEC
 *
 * @param[in] cfg the cfg parameter
 */
int tlsr_audio_configure(struct tlsr_audio_cfg *cfg);

/**
 * @brief Start Audio CODEC interface
 */
int tlsr_audio_start(enum tlsr_audio_if intf);

/**
 * @brief Stop Audio CODEC interface
 */
int tlsr_audio_stop(enum tlsr_audio_if intf);

/**
 * @brief Get Audio (min/cur/max) volume
 */
int tlsr_audio_get_volume(struct tlsr_audio_volume *vol);

/**
 * @brief Set Audio current volume
 */
int tlsr_audio_set_volume(float vol);

/**
 * @brief Mute Audio
 */
int tlsr_audio_mute(enum tlsr_audio_if intf);

/**
 * @brief Unmute Audio
 */
int tlsr_audio_unmute(enum tlsr_audio_if intf);

#ifdef __cplusplus
}
#endif

#endif //__TLSR_AUDIO_H__
