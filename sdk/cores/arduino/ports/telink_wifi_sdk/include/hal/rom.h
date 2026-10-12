/*******************************************************************************************************
 *
 * @file    rom.h
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
 
#ifndef __ROM_H__
#define __ROM_H__

#include <u-boot/linker-lists.h>

struct fcopy {
	unsigned long dst;
	unsigned long src;
};

#define _PROVIDE(_func_) \
	ll_entry_declare(struct fcopy, _func_, provide)

#define PROVIDE(fn, d, s)   \
	_PROVIDE(fn) = {	    \
		.dst = (unsigned long)d, 		    \
		.src = (unsigned long)s		    \
	}

#define provide_start() ll_entry_start(struct fcopy, provide)
#define provide_end() ll_entry_end(struct fcopy, provide)

#define _PATCH(_func_) \
	ll_entry_declare(struct fcopy, _func_, patch)

/*
 * please set your function name with prefix patch_xxx
 * ex: patch_original_func_name
 */
#define PATCH(fn, d, s)   \
	_PATCH(fn) = {	    \
		.dst = (unsigned long)d, 		    \
		.src = (unsigned long)s		    \
	}

#define patch_start() ll_entry_start(struct fcopy, patch)
#define patch_end() ll_entry_end(struct fcopy, patch)

void patch(void);
void provide(void);

#endif // __ROM_H__
