/*******************************************************************************************************
 *
 * @file    console.h
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
 
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <u-boot/list.h>
#include <hal/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * struct console: a minimal console structure
 */
struct console {
	const char *name;
	void *private;
	struct list_head list;

	int (*write)(struct console *, char *, int);
};

int _register_console(struct console *);
extern int (*register_console)(struct console *con);

void console_flush(void);
void klogd(void *ptr);

int snprintk(char *str, size_t size, const char *format, ...);
int db_printf(const char *fmt, ...);

extern int (*printk)(const char *const fmt, ...);

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

int hex_dump_to_buffer(const void *buf, size_t len, int rowsize, int groupsize,
		       char *linebuf, size_t linebuflen, bool ascii);

void print_hex_dump(const char *prefix_str, int prefix_type,
		    int rowsize, int groupsize,
		    const void *buf, size_t len, bool ascii);

#define print_hex_dump_bytes(prefix_str, prefix_type, buf, len) \
	print_hex_dump(prefix_str, prefix_type, 16, 1, buf, len, false)

#ifdef __cplusplus
}
#endif

#endif
