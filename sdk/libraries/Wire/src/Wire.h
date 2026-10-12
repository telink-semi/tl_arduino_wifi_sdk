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

#pragma once

#include "port_config.h"
#if PORT_CONFIG_WIRE_SUPPORTED


#include <inttypes.h>
#include <Stream.h>
#include "port_interface.h"

typedef void (*onRequestCallBackFunc)(void);
typedef void (*onReceiveCallBackFunc)(int);

class TwoWire : public Stream
{
  private:
    void *i2c_context;
    ring_buffer_handle_t tx_buffer;
    ring_buffer_handle_t rx_buffer;
    uint8_t txAddress;
    onRequestCallBackFunc user_onRequest;
    onReceiveCallBackFunc user_onReceive;
    size_t slave_receive_process(uint8_t *buf, uint32_t bufLen);
    size_t slave_request_process(uint8_t *buf, uint32_t bufLen);
    bool runFlag(void);
  public:
    TwoWire(uint8_t index=0);
    ~TwoWire();
    void begin();
    void begin(uint8_t);
    void begin(int);
    void end();
    void setClock(uint32_t);
    void setWireTimeout(uint32_t timeout = 25000, bool reset_with_timeout = false);
    bool getWireTimeoutFlag(void);
    void clearWireTimeoutFlag(void);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
    uint8_t requestFrom(int, int);
    uint8_t requestFrom(int, int, int);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
    void onReceive( onReceiveCallBackFunc );
    void onRequest( onRequestCallBackFunc );

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

    friend size_t _i2c_slave_request_proc(TwoWire *tw, uint8_t *buf, uint32_t bufLen);
    friend size_t _i2c_slave_receive_proc(TwoWire *tw, uint8_t *buf, uint32_t bufLen);
};

extern TwoWire Wire;

#endif