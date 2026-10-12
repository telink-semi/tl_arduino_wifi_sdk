/*******************************************************************************************************
 *
 * @file    time.h
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
 #ifndef __WITS_SYS_TIME_H__
#define __WITS_SYS_TIME_H__

#include_next <sys/time.h>

#ifndef __USE_NATIVE_HEADER__

int _os_gettimeofday(struct timeval *tv, struct timezone *tz);

int _gettimeofday(struct timeval *t, struct timezone *tz);
#define gettimeofday 		_gettimeofday

int _settimeofday(struct timeval *t, struct timezone *tz);
#define settimeofday		_settimeofday

#endif /* __USE_NATIVE_HEADER__ */

#endif
