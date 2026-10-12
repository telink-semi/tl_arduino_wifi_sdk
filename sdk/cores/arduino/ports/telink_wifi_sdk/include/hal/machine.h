/*******************************************************************************************************
 *
 * @file    machine.h
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
 #ifndef __MACHINE_H__
#define __MACHINE_H__

#include <hal/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hal_soc {
	const char *name;

	int (*init)(void);
	void (*timer_init)(void);
};

struct hal_board {
	const char *name;
	struct hal_soc *soc;

	int (*init)(void);
	int (*soc_fixup)(struct hal_soc *soc);
};

void soc_preinit(void);
void soc_init(void);
void board_init(void);

#define SOC(_name_) \
	ll_entry_declare(struct hal_soc, _name_, soc)

#define BOARD(_name_) \
	ll_entry_declare(struct hal_board, _name_, board)

#ifdef __cplusplus
}
#endif

#endif
