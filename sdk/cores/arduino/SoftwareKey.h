/********************************************************************************************************
 * @file    SoftwareKey.h
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

#include <inttypes.h>

typedef void (*callbackFunc)(void);

class SoftwareKey {
private:
    uint32_t timeRef;
    callbackFunc onDown;
    callbackFunc onUp;
    uint8_t pin;
    uint8_t value;
public:
    SoftwareKey(uint8_t pin, callbackFunc onDown=0, callbackFunc onUp=0);
    ~SoftwareKey();
    void handleEvent();
    void setDownCallback(callbackFunc onDown);
    void setUpCallback(callbackFunc onUp);
};