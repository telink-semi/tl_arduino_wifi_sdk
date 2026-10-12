/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "port_interface.h"

#include "HardwareSerial.h"

uint16_t serial_read_prepare_process(HardwareSerial *serial, uint16_t len) {
  if(!serial) return 0;
  return serial->read_prepare_process(len);
}

uint16_t serial_read_done_process(HardwareSerial *serial, uint8_t *buffer, uint16_t len) {
  if(!serial) return 0;
  return serial->read_done_process(buffer, len);
}

uint16_t serial_write_prepare_process(HardwareSerial *serial, uint8_t *buffer, uint16_t len) {
  if(!serial) return 0;
  return serial->write_prepare_process(buffer, len);
}

uint16_t serial_write_done_process(HardwareSerial *serial, uint16_t len) {
  if(!serial) return 0;
  return serial->write_done_process(len);
}

extern "C" {

uint16_t serial_read_prepare_process(void *ctx, uint16_t len) {
  port_serial_configuration_t config = {0};
  if(!port_serial_get_configuration(ctx, &config)) return 0;
  if(!config.owner) return 0;
  return serial_read_prepare_process((HardwareSerial*)config.owner, len);
}

uint16_t serial_read_done_process(void *ctx, uint8_t *buffer, uint16_t len) {
  port_serial_configuration_t config = {0};
  if(!port_serial_get_configuration(ctx, &config)) return 0;
  if(!config.owner) return 0;
  return serial_read_done_process((HardwareSerial*)config.owner, buffer, len);
}

uint16_t serial_write_prepare_process(void *ctx, uint8_t *buffer, uint16_t len) {
  port_serial_configuration_t config = {0};
  if(!port_serial_get_configuration(ctx, &config)) return 0;
  if(!config.owner) return 0;
  return serial_write_prepare_process((HardwareSerial*)config.owner, buffer, len);
}

uint16_t serial_write_done_process(void *ctx, uint16_t len) {
  port_serial_configuration_t config = {0};
  if(!port_serial_get_configuration(ctx, &config)) return 0;
  if(!config.owner) return 0;
  return serial_write_done_process((HardwareSerial*)config.owner, len);
}

} // extern "C"

// Protected Methods //////////////////////////////////////////////////////////////
uint16_t HardwareSerial::read_prepare_process(uint16_t len) {
  if(len == 0 || !port_serial_is_run(serial_ctx)) return 0;
  return ring_buffer_get_free_size(h_rx_buffer);
}

uint16_t HardwareSerial::read_done_process(uint8_t *buffer, uint16_t len) {
  if(!buffer || len == 0 || !port_serial_is_run(serial_ctx)) return 0;
  return ring_buffer_push(h_rx_buffer, buffer, len);
}

uint16_t HardwareSerial::write_prepare_process(uint8_t *buffer, uint16_t len) {
  if(!buffer || len == 0 || !port_serial_is_run(serial_ctx)) return 0;
  uint32_t tx_len = ring_buffer_available(h_tx_buffer);
  tx_len = (len > tx_len)? tx_len: len;
  return ring_buffer_copy(h_tx_buffer, buffer, tx_len);
}

uint16_t HardwareSerial::write_done_process(uint16_t len) {
  if(len == 0 || !port_serial_is_run(serial_ctx)) return 0;
  return ring_buffer_pop(h_tx_buffer, len);
}

// Public Methods //////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(uint8_t serial_index): tx_buffer_size(SERIAL_TX_BUFFER_SIZE), rx_buffer_size(SERIAL_RX_BUFFER_SIZE), serial_ctx(NULL){
  serial_ctx = port_serial_create_context(this);
  port_serial_configuration_t psConfig = {0};
  psConfig.serial_index = serial_index;
  psConfig.serial_index_valid = 1;
  port_serial_set_configuration(serial_ctx, &psConfig);
  h_tx_buffer = ring_buffer_create(tx_buffer_size);
  h_rx_buffer = ring_buffer_create(rx_buffer_size);
}

HardwareSerial::~HardwareSerial() {
  port_serial_destroy_context(serial_ctx);
  ring_buffer_destroy(h_rx_buffer);
  ring_buffer_destroy(h_tx_buffer);
  h_rx_buffer = NULL;
  h_tx_buffer = NULL;
}

bool HardwareSerial::begin(unsigned long baud, byte config)
{
  port_serial_configuration_t psConfig = {0};
  psConfig.baudrate = baud;
  psConfig.baudrate_valid = 1;
  psConfig.attribute_config = config;
  psConfig.attribute_config_valid = 1;
  if(!port_serial_set_configuration(serial_ctx, &psConfig)) return false;
  return port_serial_begin(serial_ctx);
}

bool HardwareSerial::end()
{
  bool ret = port_serial_end(serial_ctx);
  ring_buffer_pop(h_rx_buffer, ring_buffer_available(h_rx_buffer));
  ring_buffer_pop(h_tx_buffer, ring_buffer_available(h_tx_buffer));
  return ret;
}

int HardwareSerial::available(void)
{
  if(!port_serial_is_run(serial_ctx)) return 0;
  return ring_buffer_available(h_rx_buffer);
}

int HardwareSerial::peek(void)
{
  if(!port_serial_is_run(serial_ctx)) return -1;
  uint8_t c;
  uint16_t len = ring_buffer_copy(h_rx_buffer, &c, 1);
  if(len == 0) {
    return -1;
  } else {
    return c;
  }
}

int HardwareSerial::read(void)
{
  if(!port_serial_is_run(serial_ctx)) return -1;
  uint8_t c;
  uint16_t len = ring_buffer_copy(h_rx_buffer, &c, 1);
  if(len == 0) {
    return -1;
  } else {
    ring_buffer_pop(h_rx_buffer, 1);
    return c;
  }
}


int HardwareSerial::availableForWrite(void)
{
  if(ring_buffer_is_full(h_tx_buffer)) {
    return 0;
  } else {
    return ring_buffer_get_free_size(h_tx_buffer);
  }
}

void HardwareSerial::flush()
{
  if(!port_serial_is_run(serial_ctx)) return;
  while(!ring_buffer_is_empty(h_tx_buffer)) {
    port_yield();
  }
}

size_t HardwareSerial::write(uint8_t c)
{
  if(!port_serial_is_run(serial_ctx)) return 0;
  if(availableForWrite() == 0) {
    flush();
  }
  return ring_buffer_push(h_tx_buffer, &c, 1);
}


HardwareSerial Serial0(0);
HardwareSerial Serial1(1);

#define logSerial Serial0

void logSystemInit(uint32_t baud, uint8_t level) {
  log_level(level);
  logSerial.begin(baud);
  port_serial_configuration_t psConfig = {0};
  port_serial_get_configuration(logSerial.serial_ctx, &psConfig);
  port_serial_console_init(psConfig.serial_index, psConfig.baudrate);
    
  log_set_write([](uint8_t *data, size_t len) {
    int ret = logSerial.write(data, len);
    return ret;
  });
}
