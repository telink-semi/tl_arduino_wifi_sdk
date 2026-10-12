/*
  Copyright (c) 2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdint.h>
namespace std {
  [[gnu::weak, noreturn]] void terminate() {
    abort();
  }
}


extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));


void __cxa_pure_virtual(void) {
  std::terminate();
}

void __cxa_deleted_virtual(void) {
  std::terminate();
}


extern "C" {
/*system calls*/
#include <sys/stat.h>
#include <errno.h>
__attribute__((used)) void _exit(int status) { while(1) {} }
__attribute__((used)) void *_sbrk(int incr) { errno = ENOMEM; return (void *)-1; }
__attribute__((used)) int _close(int file) { errno = ENOSYS; return -1; }
__attribute__((used)) int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
__attribute__((used)) int _isatty(int file) { return 1; }
__attribute__((used)) int _lseek(int file, int ptr, int dir) { errno = ENOSYS; return -1; }
__attribute__((weak)) int _read(int file, char *ptr, int len) { errno = ENOSYS; return -1; }
__attribute__((weak)) int _write(int file, char *ptr, int len) { errno = ENOSYS; return -1; }

void* __dso_handle = 0;
}
