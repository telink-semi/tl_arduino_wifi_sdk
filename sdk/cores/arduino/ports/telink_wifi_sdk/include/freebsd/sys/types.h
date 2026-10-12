/*******************************************************************************************************
 *
 * @file    types.h
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
 #ifndef __WITS_SYS_TYPES_H__
#define __WITS_SYS_TYPES_H__

#include_next <sys/types.h>

#ifndef __USE_NATIVE_H__

#ifndef __BIT_TYPES_DEFINED__
/* NDS32 does not defines the following types */
typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;

#endif /* __BIT_TYPES_DEFINED__ */

#include <freebsd/byteorder.h>

#ifndef BYTE_ORDER
#define BYTE_ORDER 	__BYTE_ORDER__
#define LITTLE_ENDIAN 	__ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN	__ORDER_BIG_ENDIAN__
#endif

#endif /* __USE_NATIVE_H__ */

#endif /* __WITS_SYS_TYPES_H__ */
