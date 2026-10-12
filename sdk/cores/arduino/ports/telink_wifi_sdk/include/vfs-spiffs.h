/*******************************************************************************************************
 *
 * @file    vfs-spiffs.h
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
 
#ifndef __VFS_SPIFFS_H__
#define __VFS_SPIFFS_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int vfs_spiffs_mount(struct filesystem *fs, const char *src, const char *dest,
		     unsigned long mountflags, const void *private);
extern int vfs_spiffs_open(struct file **filep, const char *pathname,
			   int flags, mode_t mode);
extern int vfs_spiffs_remove(const char *pathname);
extern int vfs_spiffs_stat(const char *pathname, struct stat *statbuf);
extern int vfs_spiffs_rename(const char *oldpath, const char *newpath);
extern DIR *vfs_spiffs_opendir(const char *name);
extern int vfs_spiffs_closedir(DIR *dirp);
extern struct dirent *vfs_spiffs_readdir(DIR *dirp);
extern void vfs_spiffs_unmount(void);
extern int vfs_spiffs_format(void);

#ifdef __cplusplus
}
#endif

#endif
