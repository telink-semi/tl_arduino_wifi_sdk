/********************************************************************************************************
 * @file    util_log.h
 *
 * @brief   This is the header file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef UTIL_LOG_H__
#define UTIL_LOG_H__
#include <inttypes.h>

enum UtilDebugLevel {
  UTIL_DEBUG_LEVEL_NONE = 0,
  UTIL_DEBUG_LEVEL_ERROR = 1,
  UTIL_DEBUG_LEVEL_WARNING = 2,
  UTIL_DEBUG_LEVEL_INFO = 3,
  UTIL_DEBUG_LEVEL_DEBUG = 4,
};

#define CONFIG_DEBUG_LEVEL UTIL_DEBUG_LEVEL_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

extern int utilDebugPrint(uint8_t level, const char *fmt, ...);
extern int utilPrint(const char *fmt, ...);
extern void utilLogInit(void);
extern void utilSetLogLevel(uint8_t level);
extern void utilLogLock(void);
extern void utilLogUnlock(void);
extern void utilDebugPrintArray(uint8_t level, const char *desc, void *array, uint32_t size);
extern void utilSetLogWrite(int (*write)(uint8_t *data, size_t len));

#define log_init() utilLogInit()
#define log_set_write(write) utilSetLogWrite(write)
#define log_level(level) utilSetLogLevel(level)
#define log(fmt, ...) utilPrint(fmt, ##__VA_ARGS__)

#define log_e(fmt, ...) utilDebugPrint(UTIL_DEBUG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define log_w(fmt, ...) utilDebugPrint(UTIL_DEBUG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define log_i(fmt, ...) utilDebugPrint(UTIL_DEBUG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define log_d(fmt, ...) utilDebugPrint(UTIL_DEBUG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define log_array_e(desc, array, size) utilDebugPrintArray(UTIL_DEBUG_LEVEL_ERROR, desc, array, size)
#define log_array_w(desc, array, size) utilDebugPrintArray(UTIL_DEBUG_LEVEL_WARNING, desc, array, size)
#define log_array_i(desc, array, size) utilDebugPrintArray(UTIL_DEBUG_LEVEL_INFO, desc, array, size)
#define log_array_d(desc, array, size) utilDebugPrintArray(UTIL_DEBUG_LEVEL_DEBUG, desc, array, size)

#define log_lock() utilLogLock()
#define log_unlock() utilLogUnlock()

#define LOG_LEVEL_NONE      UTIL_DEBUG_LEVEL_NONE
#define LOG_LEVEL_ERROR     UTIL_DEBUG_LEVEL_ERROR
#define LOG_LEVEL_WARNING   UTIL_DEBUG_LEVEL_WARNING
#define LOG_LEVEL_INFO      UTIL_DEBUG_LEVEL_INFO
#define LOG_LEVEL_DEBUG     UTIL_DEBUG_LEVEL_DEBUG

#ifdef __cplusplus
}
#endif
#endif /* UTIL_LOG_H__ */
