/*******************************************************************************************************
 *
 * @file    string.h
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
 
#ifndef __WITS_STRING_H__
#define __WITS_STRING_H__

#include <stddef.h>
#include <stdarg.h>

#include_next <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __USE_NATIVE_HEADER__

char *os_strndup2(const char *s, size_t n);
char *os_strcasestr2(const char *str, const char *substr);

#undef strndup
#define strndup os_strndup2

#undef strcasestr
#define strcasestr os_strcasestr2

#endif /* __USE_NATIVE_HEADER__ */

#ifdef __cplusplus
}
#endif

#endif
