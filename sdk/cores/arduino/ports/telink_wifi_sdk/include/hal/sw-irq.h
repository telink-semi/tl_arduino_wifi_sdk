/*******************************************************************************************************
 *
 * @file    sw-irq.h
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
 
#ifndef __SW_IRQ_H__
#define __SW_IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int request_sw_irq(int irq,
		       int (*handler)(int, void *),
		       const char *name,
		       unsigned priority,
		       void *priv);

extern void free_sw_irq(int irq, const char *name);
extern int get_sw_irq_stat(char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif
