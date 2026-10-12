/********************************************************************************************************
 * @file    KWSCommon.h
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

#include <inttypes.h>
#include <stddef.h>
class KWSClass
{
protected:
public:
    KWSClass(){}
    ~KWSClass(){}
    static bool resetModel(const char ** catergroy_labels, int category_labels_count, const unsigned char * model_data);
    static bool extractFeature(uint8_t * data, uint32_t len);
    static bool initFeature(void);
    static bool detect(int expected_label_index);
};

extern KWSClass KWS;

#endif