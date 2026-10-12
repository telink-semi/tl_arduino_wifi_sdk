/********************************************************************************************************
 * @file    SoftwareKey.cpp
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
    
#include "SoftwareKey.h"
#include "Arduino.h"

/**
 * @brief      Constructor for SoftwareKey class.
 * @param[in]  pin - The GPIO pin number connected to the key.
 * @param[in]  onDown - Callback function triggered when the key is pressed down.
 * @param[in]  onUp - Callback function triggered when the key is released.
 */
SoftwareKey::SoftwareKey(uint8_t pin, callbackFunc onDown, callbackFunc onUp) : onDown(onDown), onUp(onUp), pin(pin) {
    timeRef = 0;
    value = 0xff;
    pinMode(pin, INPUT_PULLUP); // Initialize pin with internal pull-up resistor
}

SoftwareKey::~SoftwareKey() {

}

/**
 * @brief      Handles key event processing.
 *             Reads the pin state periodically and detects key press/release events.
 */
void SoftwareKey::handleEvent() {
    if((uint32_t)(millis() - timeRef) > 5) {
        value <<= 1;
        value |= digitalRead(pin);

        // Detect key down event (0xf0 = 11110000 in binary)
        if(value == 0xf0) {
            if(onDown) onDown();
        }
        // Detect key up event (0x0f = 00001111 in binary)
        else if(value == 0x0f) {
            if(onUp) onUp();
        }

        timeRef = millis();
    }
}

/**
 * @brief      Sets the callback function for key down event.
 * @param[in]  onDown - The callback function to be called when key is pressed.
 */
void SoftwareKey::setDownCallback(callbackFunc onDown) {
    this->onDown = onDown;
}

/**
 * @brief      Sets the callback function for key up event.
 * @param[in]  onUp - The callback function to be called when the key is released.
 */
void SoftwareKey::setUpCallback(callbackFunc onUp) {
    this->onUp = onUp;
}


