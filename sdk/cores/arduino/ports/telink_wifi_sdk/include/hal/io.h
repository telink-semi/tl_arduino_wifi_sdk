/*******************************************************************************************************
 *
 * @file    io.h
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
 #ifndef __WITS_IO_H__
#define __WITS_IO_H__

#include <hal/types.h>
#include <hal/kernel.h>
#ifndef CONFIG_SANDBOX
#include <asm/io.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define __iomem

#define barrier() __asm__ __volatile__("": : :"memory")

#ifndef readl
#define readl(a) \
	({u32 __v = *((volatile u32 *)(a)); barrier(); __v; })
#endif

#ifndef readw
#define readw(a) \
	({u16 __v = *((volatile u16 *)(a)); barrier(); __v; })
#endif

#ifndef readb
#define readb(a) \
	({u8 __v = *((volatile u8 *)(a)); barrier(); __v; })
#endif

#ifndef writel
#define writel(v, a) \
	({u32 __v = v; barrier(); *((volatile u32 *)(a)) = __v; })
#endif

#ifndef writew
#define writew(v, a) \
	({u16 __v = v; barrier(); *((volatile u16 *)(a)) = __v; })
#endif

#ifndef writeb
#define writeb(v, a) \
	({u8  __v = v; barrier(); *((volatile u8 *)(a)) = __v; })
#endif


void memset_io(void __iomem *to, int c, size_t len);
void memcpy_toio(void __iomem *to, const void *from, size_t len);
void memcpy_fromio(void * to, void __iomem *from, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __WITS_IO_H__ */
