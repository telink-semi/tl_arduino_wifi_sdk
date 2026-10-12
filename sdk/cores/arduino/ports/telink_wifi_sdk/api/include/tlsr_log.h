/*******************************************************************************************************
 *
 * @file    tlsr_log.h
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
 

#ifndef __TLSR_LOG_H__
#define __TLSR_LOG_H__

#include <stdint.h>
#include <stdarg.h>
#include "wits_log.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define TLSR_DBG_EN

#ifdef TLSR_DBG_EN
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL WITS_LOG_DEBUG

#define TLSR_DBG_LOG(tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_DEBUG,    tag, format, ##__VA_ARGS__)
#else
#define TLSR_DBG_LOG(...)
#endif
#define TLSR_ERR_LOG(tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_ERROR,    tag, format, ##__VA_ARGS__)
#define TLSR_INFO_LOG(tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_INFO,    tag, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __TLSR_LOG_H__ */
