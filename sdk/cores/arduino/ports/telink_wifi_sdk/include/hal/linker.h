/*******************************************************************************************************
 *
 * @file    linker.h
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
 #ifndef __WITS_LINKER_H__
#define __WITS_LINKER_H__

#define DECLARE_SECTION_INFO(s)				   \
extern char __##s##_start[], __##s##_end[], __##s##_lma[]; \
extern unsigned __##s##_size

#define LMA(s) __##s##_lma
#define VMA(s) __##s##_start
#define SSIZE(s) (__##s##_end - __##s##_start)

#ifndef CONFIG_WATCHER
DECLARE_SECTION_INFO(vector);
DECLARE_SECTION_INFO(init);
DECLARE_SECTION_INFO(rram);
DECLARE_SECTION_INFO(text);
DECLARE_SECTION_INFO(rodata);
DECLARE_SECTION_INFO(data);
DECLARE_SECTION_INFO(bss);
#ifdef CONFIG_SUPPORT_NODE_POOL
DECLARE_SECTION_INFO(nodebss);
#endif
#ifdef CONFIG_PM_MULTI_CORE
DECLARE_SECTION_INFO(pmsigram);
#endif
DECLARE_SECTION_INFO(pmrram);
DECLARE_SECTION_INFO(watcher);
DECLARE_SECTION_INFO(waon);
DECLARE_SECTION_INFO(shm1);
DECLARE_SECTION_INFO(shm2);
#ifdef CONFIG_LINK_TO_ROM
DECLARE_SECTION_INFO(romd);
DECLARE_SECTION_INFO(romb);
#endif

#ifdef CONFIG_SUPPORT_GCOV
DECLARE_SECTION_INFO(init_array);
DECLARE_SECTION_INFO(gcovdata);
DECLARE_SECTION_INFO(gcovbss);
#endif
#else
DECLARE_SECTION_INFO(pmrram);
#endif
#endif /* __WITS_LINKER_H__ */
