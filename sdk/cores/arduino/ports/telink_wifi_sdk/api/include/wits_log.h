/*
 * Copyright 2018-2024 Telink Semiconductor Co., Ltd.
 */
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * Originated from esp_log.h of ESP8266_RTOS_SDK
 * (https://github.com/espressif/ESP8266_RTOS_SDK)
 * and modified to provide wits Wi-Fi API as being ESP8266 style
 */

/*
 * This is simplified version of esp_log.
 */

#ifndef __WITS_LOG_H__
#define __WITS_LOG_H__

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log level
 *
 */
typedef enum {
    WITS_LOG_NONE = 0,   /*!< No log output */
    WITS_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    WITS_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    WITS_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    WITS_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    WITS_LOG_VERBOSE,    /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */

    WITS_LOG_MAX
} wits_log_level_t;

/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * WITS_LOGE, WITS_LOGW, WITS_LOGI, WITS_LOGD, WITS_LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 */
void wits_log_write(wits_log_level_t level, const char* tag, const char* format, ...) __attribute__ ((format (printf, 3, 4)));

/** @cond */

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL  CONFIG_LOG_DEFAULT_LEVEL
#endif

/** @endcond */

#if LOG_LOCAL_LEVEL > 0
#define WITS_LOGE( tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#else
#define WITS_LOGE( tag, format, ... )
#endif
#if LOG_LOCAL_LEVEL > 1
#define WITS_LOGW( tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_WARN,    tag, format, ##__VA_ARGS__)
#else
#define WITS_LOGW( tag, format, ... )
#endif
#if LOG_LOCAL_LEVEL > 2
#define WITS_LOGI( tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_INFO,    tag, format, ##__VA_ARGS__)
#else
#define WITS_LOGI( tag, format, ... )
#endif
#if LOG_LOCAL_LEVEL > 3
#define WITS_LOGD( tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#else
#define WITS_LOGD( tag, format, ... )
#endif
#if LOG_LOCAL_LEVEL > 4
#define WITS_LOGV( tag, format, ... ) WITS_LOG_LEVEL_LOCAL(WITS_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#else
#define WITS_LOGV( tag, format, ... )
#endif

/** runtime macro to output logs at a specified level.
#define WITS_LOGV( tag, format, ... ) *
 * @param tag tag of the log
 * @param level level of the output log.
 * @param format format of the output log. see ``printf``
 * @param ... variables to be replaced into the log. see ``printf``
 *
 * @see ``printf``
 */
#define WITS_LOG_LEVEL(level, tag, format, ...) do {                     \
        if (level==WITS_LOG_ERROR )          { wits_log_write(WITS_LOG_ERROR,      tag, format, ##__VA_ARGS__); } \
        else if (level==WITS_LOG_WARN )      { wits_log_write(WITS_LOG_WARN,       tag, format, ##__VA_ARGS__); } \
        else if (level==WITS_LOG_DEBUG )     { wits_log_write(WITS_LOG_DEBUG,      tag, format, ##__VA_ARGS__); } \
        else if (level==WITS_LOG_VERBOSE )   { wits_log_write(WITS_LOG_VERBOSE,    tag, format, ##__VA_ARGS__); } \
        else                                 { wits_log_write(WITS_LOG_INFO,       tag, format, ##__VA_ARGS__); } \
    } while(0)

/** runtime macro to output logs at a specified level. Also check the level with ``LOG_LOCAL_LEVEL``.
 *
 * @see ``printf``, ``WITS_LOG_LEVEL``
 */
#define WITS_LOG_LEVEL_LOCAL(level, tag, format, ...) do {               \
        if ( LOG_LOCAL_LEVEL >= level ) WITS_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __WITS_LOG_H__ */
