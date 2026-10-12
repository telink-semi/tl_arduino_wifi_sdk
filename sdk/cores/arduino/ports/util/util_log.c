/********************************************************************************************************
 * @file    util_log.c
 *
 * @brief   This is the source file for Arduino SDK
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

#include "port_interface.h"
#include "util_common.h"
#include <stdarg.h>

typedef int (*write_t)(uint8_t *data, size_t len);


#ifdef CONFIG_DEBUG_LEVEL
static uint8_t debug_level = CONFIG_DEBUG_LEVEL;
#else
static uint8_t debug_level = UTIL_DEBUG_LEVEL_DEBUG;
#endif

static void *log_mutex = NULL;
static write_t log_write = NULL;


static int __log_write(uint8_t *data, size_t len) {
  (void)data;
  (void)len;
  for(int i=0; i<len; i++) {
    putchar(data[i]);
  }
  return len;
}

static char loc_buf[64];
static int __vprintf(const char *fmt, va_list args) {
  char *temp = loc_buf;
  va_list copy;
  va_copy(copy, args);
  int len = vsnprintf(temp, sizeof(loc_buf), fmt, copy);
  va_end(copy);
  if (len < 0) {
    va_end(args);
    return 0;
  }
  if (len >= (int)sizeof(loc_buf)) {  // comparison of same sign type for the compiler
    temp = (char *)util_malloc(len + 1);
    if (temp == NULL) {
      va_end(args);
      return 0;
    }
    len = vsnprintf(temp, len + 1, fmt, args);
  }
  va_end(args);
  len = log_write((uint8_t *)temp, len);
  if (temp != loc_buf) {
    util_free(temp);
  }
  return len;
}




static int __printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = __vprintf(fmt, args);
  va_end(args);
  return ret;
}

// #define __printf printf
// #define __vprintf vprintf


void utilLogInit(void) {
  log_mutex = util_create_mutex();
  log_write = __log_write;
}

void utilSetLogLevel(uint8_t level) {
  debug_level = level;
}

void utilLogLock(void) {
  util_lock_mutex(log_mutex);
}

void utilLogUnlock(void) {
  util_unlock_mutex(log_mutex);
}

int utilPrint(const char *fmt, ...) {
  utilLogLock();
  va_list args;
  va_start(args, fmt);
  int ret = __vprintf(fmt, args);
  va_end(args);
  utilLogUnlock();
  return ret;
}

void utilDebugPrintArray(uint8_t level, const char *desc, void *array, uint32_t size) {
  if(level > debug_level) return;
  utilLogLock();
  __printf("[N22][%08.3f]", port_millis()/1000.0);
  switch(level) {
    case UTIL_DEBUG_LEVEL_ERROR:
      __printf("[ERROR] ");
      break;
    case UTIL_DEBUG_LEVEL_WARNING:
      __printf("[WARN ] ");
      break;
    case UTIL_DEBUG_LEVEL_DEBUG:
      __printf("[DEBUG] ");
      break;
    default:
      __printf("[INFO ] ");
      break;
  }
  __printf("%s<%d>:", desc, size);
  for(int i = 0; i < size; i++) {
    __printf("%02x ", ((uint8_t *)array)[i]);
  }
  __printf("\n");
  utilLogUnlock();
}


void utilSetLogWrite(int (*write)(uint8_t *data, size_t len)) {
  log_write = write;
}

int utilDebugPrint(uint8_t level, const char *fmt, ...)
{
  if(level > debug_level) return 0;
  utilLogLock();
  __printf("[N22][%08.3f]", port_millis()/1000.0);
  switch(level) {
    case UTIL_DEBUG_LEVEL_ERROR:
      __printf("[ERROR] ");
      break;
    case UTIL_DEBUG_LEVEL_WARNING:
      __printf("[WARN ] ");
      break;
    case UTIL_DEBUG_LEVEL_DEBUG:
      __printf("[DEBUG] ");
      break;
    default:
      __printf("[INFO ] ");
      break;
  }
  va_list args;
  va_start(args, fmt);
  int ret = __vprintf(fmt, args);
  va_end(args);
  __printf("\n");
  utilLogUnlock();
  return ret;
}