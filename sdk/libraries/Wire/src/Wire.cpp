/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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
 
  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2017 by Chuck Todd (ctodd@cableone.net) to correct Unconfigured Slave Mode reboot
  Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
  Modified 2026 by Robin to support w91
*/

#include "port_config.h"
#if PORT_CONFIG_WIRE_SUPPORTED

#include "Wire.h"
#include "Arduino.h"


size_t _i2c_slave_request_proc(TwoWire *tw, uint8_t *buf, uint32_t bufLen) {
  if(!tw || !buf || (bufLen == 0)) return 0;
  return tw->slave_request_process(buf, bufLen);
}


size_t _i2c_slave_receive_proc(TwoWire *tw, uint8_t *buf, uint32_t bufLen) {
  if(!tw || !buf || (bufLen == 0) ) return 0;
  return tw->slave_receive_process(buf, bufLen);
}

extern "C" {

size_t i2c_slave_request_proc(void *ctx, uint8_t *buf, uint32_t bufLen) {
  if(!ctx) return 0;
  port_i2c_configuration_t config = {0};
  port_i2c_get_configuration(ctx, &config);
  return _i2c_slave_request_proc((TwoWire *)config.owner, buf, bufLen);
}

size_t i2c_slave_receive_proc(void *ctx, uint8_t *buf, uint32_t bufLen) {
  if(!ctx) return 0;
  port_i2c_configuration_t config = {0};
  port_i2c_get_configuration(ctx, &config);
  return _i2c_slave_receive_proc((TwoWire *)config.owner, buf, bufLen);
}

}

// Constructors ////////////////////////////////////////////////////////////////
TwoWire::TwoWire(uint8_t index) : i2c_context(NULL), user_onRequest(NULL), user_onReceive(NULL){
  i2c_context = port_i2c_create_context(this);

  port_i2c_configuration_t config = {0};
  config.i2c_index_valid = 1;
  config.i2c_index = index;
  port_i2c_set_configuration(i2c_context, &config);

  tx_buffer = ring_buffer_create(I2C_TX_BUFFER_SIZE);
  rx_buffer = ring_buffer_create(I2C_RX_BUFFER_SIZE);
}

TwoWire::~TwoWire() {
  if(runFlag()) {
    end();
  }
  if(tx_buffer) ring_buffer_destroy(tx_buffer);
  if(rx_buffer) ring_buffer_destroy(rx_buffer);
  port_i2c_destroy_context(i2c_context);
}

// private Methods //////////////////////////////////////////////////////////////
bool TwoWire::runFlag(void) {
  return (rx_buffer != NULL) && (tx_buffer != NULL) && port_i2c_is_run(i2c_context);
}

size_t TwoWire::slave_receive_process(uint8_t *buf, uint32_t bufLen) {
  if(!runFlag()) return 0;
  ring_buffer_pop(rx_buffer, ring_buffer_available(rx_buffer));
  size_t rxLen = ring_buffer_push(rx_buffer, buf, bufLen);
  if(user_onReceive) {
    user_onReceive(rxLen);
  }

  return rxLen;
}

size_t TwoWire::slave_request_process(uint8_t *buf, uint32_t bufLen) {
  if(!runFlag()) return 0;
  if(user_onRequest) {
    user_onRequest();
  }

  size_t copyLen = ring_buffer_copy(tx_buffer, buf, bufLen);
  ring_buffer_pop(tx_buffer, copyLen);

  return copyLen;
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void)
{
  begin(0);
}

void TwoWire::begin(uint8_t address)
{
  port_i2c_configuration_t config = {0};
  config.address_valid = 1;
  config.address = address;
  port_i2c_set_configuration(i2c_context, &config);
  port_i2c_begin(i2c_context);
}

void TwoWire::begin(int address)
{
  begin((uint8_t)address);
}

void TwoWire::end(void)
{
  if(!runFlag()) return;
  port_i2c_end(i2c_context);
}

void TwoWire::setClock(uint32_t clock)
{
  port_i2c_configuration_t config = {0};
  config.freq_valid = 1;
  config.freq = clock;
  port_i2c_set_configuration(i2c_context, &config);
}

void TwoWire::setWireTimeout(uint32_t timeout, bool reset_with_timeout){
  port_i2c_configuration_t config = {0};
  config.timeout_valid = 1;
  config.timeout = timeout;
  config.reset_with_timeout = reset_with_timeout;
  port_i2c_set_configuration(i2c_context, &config);
}

/***
 * Returns the TWI timeout flag.
 *
 * @return true if timeout has occurred since the flag was last cleared.
 */
bool TwoWire::getWireTimeoutFlag(void){
  port_i2c_configuration_t config = {0};
  port_i2c_get_configuration(i2c_context, &config);
  return config.timeout_flag;
}

/***
 * Clears the TWI timeout flag.
 */
void TwoWire::clearWireTimeoutFlag(void){
  port_i2c_master_clear_timeout(i2c_context);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
  uint8_t buffer[I2C_RX_BUFFER_SIZE + I2C_TX_BUFFER_SIZE];
  if (isize > 0) {
    // send internal address; this mode allows sending a repeated start to access
    // some devices' internal registers. This function is executed by the hardware
    // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)
    beginTransmission(address);
    // the maximum size of internal address is 3 bytes
    if (isize > 3){
      isize = 3;
    }
    while(isize--)
      write((uint8_t)(iaddress >> (isize*8)));
  }

  // clamp to buffer length
  if(quantity > I2C_RX_BUFFER_SIZE){
    quantity = I2C_RX_BUFFER_SIZE;
  }

  uint32_t tx_len = ring_buffer_available(tx_buffer);
  if(tx_len) {
    tx_len = ring_buffer_copy(tx_buffer, buffer, tx_len);
    if(!port_i2c_master_start_trx(i2c_context, address, buffer, tx_len, quantity)) {
      log_e("port_i2c_master_start_trx error");
      return 0;
    }
  } else {
    if(!port_i2c_master_start_rx(i2c_context, address, quantity)) {
      log_e("port_i2c_master_start_rx error");
      return 0;
    }
  }
  
  i2c_master_opt_t opt = I2C_MASTER_NONE;
  uint32_t rxLen = 0;
  uint32_t startTime = millis();

  port_i2c_configuration_t config = {0};
  port_i2c_get_configuration(i2c_context, &config);
  
  while(!port_i2c_master_ask(i2c_context, &opt, &rxLen, buffer, sizeof(buffer))) {
    yield();
    if(millis() - startTime > config.timeout) {
      clearWireTimeoutFlag();
      break;
    }
  }

  if(opt != I2C_MASTER_RX) return 0;

  ring_buffer_pop(rx_buffer, ring_buffer_available(rx_buffer));
  rxLen = ring_buffer_push(rx_buffer, buffer, rxLen);

  return rxLen & 0xff;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
	return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
  txAddress = address;
  ring_buffer_pop(tx_buffer, ring_buffer_available(tx_buffer));
}

void TwoWire::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
  (void)sendStop;
  uint8_t buffer[I2C_TX_BUFFER_SIZE];
  uint32_t txLen = ring_buffer_copy(tx_buffer, buffer, sizeof(buffer));

  if(txLen == 0) {
    if(port_i2c_master_probe(i2c_context, txAddress)) {
      return 0;
    } else {
      return 2;
    }
  } else {

    if(port_i2c_master_start_tx(i2c_context, txAddress, buffer, txLen)) {
      ring_buffer_pop(tx_buffer, txLen);
      i2c_master_opt_t opt = I2C_MASTER_NONE;
      uint32_t len = 0;
      uint32_t startTime = millis();
      port_i2c_configuration_t config = {0};
      port_i2c_get_configuration(i2c_context, &config);
      while(!port_i2c_master_ask(i2c_context, &opt, &len, NULL, 0)) {
        yield();
        if(millis() - startTime > config.timeout) {
          clearWireTimeoutFlag();
        }
      }

      if(opt == I2C_MASTER_TX) {
        if(len == txLen) {
          return 0;
        } else {
          return 3;
        }
      } 
    } else {
      return 1;
    }
  }

  return 4;
}

uint8_t TwoWire::endTransmission(void)
{
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
  return write(&data, 1);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  if(!runFlag()) return 0;
  if(quantity > I2C_TX_BUFFER_SIZE) {
    quantity = I2C_TX_BUFFER_SIZE;
  }
  
  port_i2c_configuration_t config = {0};
  port_i2c_get_configuration(i2c_context, &config);

  if(config.address == 0){
    if(ring_buffer_get_free_size(tx_buffer) < quantity) {
      endTransmission();
    }
  }

  return ring_buffer_push(tx_buffer, (uint8_t *)data, quantity);
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void)
{
  if(!runFlag()) return 0;
  return ring_buffer_available(rx_buffer);
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
  int value = -1;
  
  // get each successive byte on each call
  if(ring_buffer_available(rx_buffer)) {
    uint8_t val;
    if(ring_buffer_copy(rx_buffer, &val, 1) == 1) {
      value = val;
      ring_buffer_pop(rx_buffer, 1);
    }
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
  int value = -1;
  
  if(ring_buffer_available(rx_buffer)) {
    uint8_t val;
    if(ring_buffer_copy(rx_buffer, &val, 1) == 1) {
      value = val;
    }
  }

  return value;
}

void TwoWire::flush(void)
{
  // XXX: to be implemented.
  if(!runFlag()) return;
  while(!ring_buffer_is_empty(tx_buffer)) {
    yield();
  }
}

// sets function called on slave write
void TwoWire::onReceive( onReceiveCallBackFunc function)
{
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( onRequestCallBackFunc function )
{
  user_onRequest = function;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();

#endif
