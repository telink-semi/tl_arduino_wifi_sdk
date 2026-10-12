/*******************************************************************************************************
 *
 * @file    serial.h
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
 
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <hal/kernel.h>
#include <hal/device.h>

#include <sys/cdefs.h>
#include <termios.h>

#include <FreeRTOS/FreeRTOS.h>
#include <FreeRTOS/xqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

struct serial_port;

struct serial_port_ops {
	int (*init)(struct serial_port *);
	void (*start_tx)(struct serial_port *);
	int (*ioctl)(struct serial_port *, unsigned, void *);
	int (*putc)(struct serial_port *, char);
	int (*con_write)(struct serial_port *, char *, int);
};

typedef QueueHandle_t queue_t;
typedef enum { TXD = 0, RXD = 1, CTS = 2, RTS = 3 } uart_pin_t;
enum { TX = 0, RX = 1 };

struct serial_port {
	int id;
	struct device *device;
	struct serial_port_ops *ops;
	struct list_head list;
	struct termios oldtermios;
	queue_t queue[2];
	int initialized;
};

extern int register_serial_port(struct serial_port *);
int uart_get_baudrate(struct termios *);

#ifdef __cplusplus
}
#endif

#endif
