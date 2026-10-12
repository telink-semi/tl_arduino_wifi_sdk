/********************************************************************************************************
 * @file    api.cpp
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
#include "port_interface.h"
#include "Arduino.h"

void pinMode(uint8_t pin, uint8_t mode) {
	port_pin_mode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t val) {
	port_digital_write(pin, val);
}

int digitalRead(uint8_t pin) {
	return port_digital_read(pin);
}

void analogWrite(uint8_t pin, int val) {
	port_analog_write(pin, val);
}

int analogRead(uint8_t pin) {
	return port_analog_read(pin);
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout){
    return port_pulse_in(pin, state, timeout);
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration){
    port_tone(pin, frequency, duration);
}

void noTone(uint8_t pin){
    port_no_tone(pin);
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		digitalWrite(clockPin, HIGH);
		if (bitOrder == LSBFIRST)
			value |= digitalRead(dataPin) << i;
		else
			value |= digitalRead(dataPin) << (7 - i);
		digitalWrite(clockPin, LOW);
	}
	return value;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST) {
			digitalWrite(dataPin, val & 1);
			val >>= 1;
		} else {	
			digitalWrite(dataPin, (val & 128) != 0);
			val <<= 1;
		}
			
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);		
	}
}

unsigned int millis(void) {
	return port_millis();
}
unsigned int micros(void) {
	return port_micros();
}

void interrupts(void) {
	port_interrupts();
}

void noInterrupts(void) {
	port_no_interrupts();
}

void yield(void) {
	port_yield();
}

void exit(int status) {
	port_exit(status);
	while(1);
}

int atexit(void (*func)(void)) {
	return port_atexit(func);
}

