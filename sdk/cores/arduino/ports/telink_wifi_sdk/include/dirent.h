/*******************************************************************************************************
 *
 * @file    dirent.h
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
 
#ifndef __WITS_DIRENT_H__
#define __WITS_DIRENT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __dirstream {
	void *dir;
} DIR;

struct dirent {
	unsigned d_ino;
	unsigned d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[128];
};

extern DIR *(*os_opendir) (const char *name);
extern int (*os_closedir) (DIR * dirp);
extern struct dirent *(*os_readdir) (DIR * dirp);
#ifdef CONFIG_LINK_TO_ROM
extern void (os_unmount) (const char *pathname);
extern int (os_format) (const char *pathname);
#else
#if 0 /* Will be enabled at the next Romization */
extern void (*os_unmount) (const char *pathname);
extern int (*os_format) (const char *pathname);
#endif
#endif

#define opendir os_opendir
#define closedir os_closedir
#define readdir os_readdir
#define unmount os_unmount
#define fformat os_format

#ifdef __cplusplus
}
#endif

#endif /* __WITS_DIRENT_H__ */
