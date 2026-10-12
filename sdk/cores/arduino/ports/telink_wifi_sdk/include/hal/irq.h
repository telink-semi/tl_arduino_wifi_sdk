/*******************************************************************************************************
 *
 * @file    irq.h
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
 
#ifndef __IRQ_H__
#define __IRQ_H__

#include <FreeRTOS/FreeRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int request_irq(int irq,
		       int (*handler)(int, void *),
		       const char *name,
		       unsigned priority,
		       void *priv);

extern void free_irq(int irq, const char *name);
extern int get_irq_stat(char *buf, size_t size);
extern void enable_irq(int irq);
extern void disable_irq(int irq);
#ifdef CONFIG_ARM_CORTEX_A7
extern unsigned int disable_cpu_interface();
extern void enable_cpu_interface(unsigned int old);
#endif

#if defined(INC_FREERTOS_H)

#ifdef CONFIG_ARM_CORTEX_A7

#define IRQMASK_REG_NAME_R "cpsr"
#define IRQMASK_REG_NAME_W "cpsr_c"
static inline unsigned long arc_local_irq_save(void)
{
	unsigned long flags;

	asm volatile(
		"	mrs	%0, " IRQMASK_REG_NAME_R "	@ arch_local_irq_save\n"
		"	cpsid	i"
		: "=r" (flags) : : "memory", "cc");
	return flags;
}

static inline void arc_local_irq_restore(unsigned long flags)
{
	asm volatile(
		"	msr	" IRQMASK_REG_NAME_W ", %0	@ local_irq_restore"
		:
		: "r" (flags)
		: "memory", "cc");
}
#define local_irq_save(flags) \
	do { flags = arc_local_irq_save(); } while (0)

#define local_irq_restore(flags) \
	do { arc_local_irq_restore(flags); } while (0)

#else

#define local_irq_save(flags) \
	do { flags = portSET_INTERRUPT_MASK_FROM_ISR(); } while (0)

#define local_irq_restore(flags) \
	do { portCLEAR_INTERRUPT_MASK_FROM_ISR(flags); } while (0)

#endif

#else
/* Currently only support ARM Cortex-M0 and Cortex-M3 */
#define local_irq_save(flags) \
{ \
	asm volatile( \
	    "	mrs	%0, primask	@ local_irq_save\n" \
	    "	cpsid	i" \
	    : "=r"(flags) : : "memory", "cc"); \
}

#define local_irq_restore(flags) \
{ \
	asm volatile( \
	    "	msr	primask, %0	@ local_irq_restore" \
	    : \
	    : "r"(flags) \
	    : "memory", "cc"); \
}

#endif

#ifdef __cplusplus
}
#endif

#endif
