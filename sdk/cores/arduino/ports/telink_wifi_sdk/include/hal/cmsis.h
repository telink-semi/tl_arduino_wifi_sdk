/*******************************************************************************************************
 *
 * @file    cmsis.h
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
 #ifndef __CMSIS_H__
#define __CMSIS_H__

/*
 *  Only Cortex-M0 and Cortex-M3 are considered.
 */

#ifdef __arm__

#if __ARM_ARCH == 6 && __ARM_ARCH_6M__ == 1 && __ARM_ARCH_ISA_THUMB == 1
#include "cmsis/core_cm0.h"
#elif __ARM_ARCH == 7 && __ARM_ARCH_7M__ == 1 && __ARM_ARCH_ISA_THUMB == 2
#include "cmsis/core_cm3.h"
#elif __ARM_ARCH == 7
#else
#error Unknown Arm processor
#endif

#endif /* __ARM_EABI */

#endif
