/********************************************************************************************************
 * @file    port_fs.c
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
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include "sdk_common.h"
#include "core.h"
#include "spi-flash.h"
#include "cjson/cJSON.h"

#define LFS_BLOCK_SIZE    4096       
#define LFS_BLOCK_COUNT   64       //   1M / 4KB = 256
#define LFS_FLASH_BASE    0x80180000  


// read callback
static int lfs_flash_read(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, void *buffer, lfs_size_t size) {
    uint32_t addr = LFS_FLASH_BASE + block * c->block_size + off;
    // log_i("lfs_flash_read: addr<0x%08x>, size<%d>", addr, size);
    return flash_read(addr, buffer, size) ? LFS_ERR_IO : LFS_ERR_OK;
}

static int lfs_flash_prog(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, const void *buffer, lfs_size_t size) {
    uint32_t addr = LFS_FLASH_BASE + block * c->block_size + off;
    // log_i("lfs_flash_prog: addr<0x%08x>, size<%d>", addr, size);
    return flash_write(addr, (void*)buffer, size) ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_flash_erase(const struct lfs_config *c, lfs_block_t block) {
    uint32_t addr = LFS_FLASH_BASE + block * c->block_size;
    // log_i("lfs_flash_erase: addr<0x%08x>, block_size<%d>, block<%d>", addr, c->block_size, block);
    return (flash_erase(addr, c->block_size, 0) >= 0) ? LFS_ERR_OK : LFS_ERR_IO;
}

static int lfs_flash_sync(const struct lfs_config *c) {
    return LFS_ERR_OK;
}

typedef struct {
    lfs_dir_t dir;        
    char base_path[LFS_NAME_MAX + 10]; 
    bool is_open;          
} lfs_file_iterator_t;

static uint32_t last_write_time = 0;
static lfs_t lfs;
static bool lfs_init_flag = false;
static lfs_file_iterator_t lfs_file_it = {0};
static struct lfs_info lfs_info; 
static char lfs_file_path[LFS_NAME_MAX + 20] = {0};
static struct lfs_config cfg = {
    .context = NULL,

    .read  = lfs_flash_read,
    .prog  = lfs_flash_prog,
    .erase = lfs_flash_erase,
    .sync  = lfs_flash_sync,

    .read_size = 256,        
    .prog_size = 256,      
    .block_size = LFS_BLOCK_SIZE,
    .block_count = LFS_BLOCK_COUNT,
    .block_cycles = 500,    // default 500
    .cache_size = 256,       // >= prog_size
    .lookahead_size = 16,   // 32bit align
};

#define FS_LOCK()         do {flash_hw_lock();} while(0)
#define FS_UNLOCK()       do {flash_hw_unlock();} while(0)


static int lfs_open_next_init(lfs_file_iterator_t *it, const char *dir_path) {
    if (!it || !dir_path) return -LFS_ERR_INVAL;

    it->is_open = false;

    strncpy(it->base_path, dir_path, sizeof(it->base_path) - 1);
    it->base_path[sizeof(it->base_path) - 1] = '\0';

    int err = lfs_dir_open(&lfs, &it->dir, dir_path);
    if (err != LFS_ERR_OK) {
        return err;
    }

    it->is_open = true;
    return LFS_ERR_OK;
}

static const char * lfs_get_next_file_path(lfs_file_iterator_t *it) {
    if (!it || !it->is_open) {
        return NULL;
    }

    struct lfs_info info;
    while (true) {
        memset(&info, 0, sizeof(info));
        int res = lfs_dir_read(&lfs, &it->dir, &info);

        if (res <= 0) {
            return NULL;
        }

        if (strcmp(info.name, ".") == 0 || 
            strcmp(info.name, "..") == 0 ||
            info.type != LFS_TYPE_REG) {
            continue;
        }


        if (strcmp(it->base_path, "/") == 0) {
            snprintf(lfs_file_path, sizeof(lfs_file_path), "/%s", info.name);
        } else {
            snprintf(lfs_file_path, sizeof(lfs_file_path), "%s/%s", it->base_path, info.name);
        }

        return lfs_file_path; 
    }   
}

static const char * lfs_get_next_path(lfs_file_iterator_t *it, bool *is_dir) {
    if (!it || !it->is_open) {
        return NULL;
    }

    struct lfs_info info;
    while (true) {
        memset(&info, 0, sizeof(info));
        int res = lfs_dir_read(&lfs, &it->dir, &info);

        if (res <= 0) {
            return NULL;
        }

        if (strcmp(info.name, ".") == 0 || 
            strcmp(info.name, "..") == 0) {
            continue;
        }

        *is_dir = (info.type == LFS_TYPE_DIR);

        if (strcmp(it->base_path, "/") == 0) {
            snprintf(lfs_file_path, sizeof(lfs_file_path), "/%s", info.name);
        } else {
            snprintf(lfs_file_path, sizeof(lfs_file_path), "%s/%s", it->base_path, info.name);
        }

        return lfs_file_path; 
    }   
}

static void lfs_open_next_close(lfs_file_iterator_t *it) {
    if (it && it->is_open) {
        lfs_dir_close(&lfs, &it->dir);
        it->is_open = false;
    }
}

bool port_fs_init(void) {
    if(lfs_init_flag) return true;

    bool ret = false;
    FS_LOCK();
    do {
        struct spi_flash *flash = spi_flash_find_by_addr(LFS_FLASH_BASE);
        if(!flash) break;
        cfg.prog_size = flash->page_size;
        cfg.read_size = flash->page_size;
        cfg.cache_size = flash->page_size;
        if(lfs_mount(&lfs, &cfg)) {
            log_w("LittleFS mount failed");
            break;
        }
        ret = true;
        lfs_init_flag = ret;
    }while(0);
    FS_UNLOCK();

    return ret;
}

bool port_fs_deinit(void) {
    if(!lfs_init_flag) return true;
    lfs_init_flag = false;
    FS_LOCK();
    lfs_unmount(&lfs);
    FS_UNLOCK();
    return true;
}

fs_file_t port_fs_file_open(const char *path, const char *mode, bool create) {
    if(!lfs_init_flag) return NULL;

    fs_file_t ret = NULL;
    lfs_file_t *file = NULL;
    FS_LOCK();
    do {
        if(!path || !mode) break;
        file = (lfs_file_t *)malloc(sizeof(lfs_file_t));
        if(!file) break;
        int flags = 0;
        if(strcmp(mode, "r") ==0) {
            flags |= LFS_O_RDONLY;
        } else if(strcmp(mode, "w") == 0) {
            flags |= LFS_O_WRONLY;
            if(create) {
                flags |= LFS_O_CREAT;
                flags |= LFS_O_TRUNC;
            }
        } else if(strcmp(mode, "a") == 0) {
            flags |= LFS_O_APPEND;
            if(create) {
                flags |= LFS_O_CREAT;
            }
        }
        if(lfs_file_open(&lfs, file, path, flags)) {
            break;
        }

        ret = file;
    }while(0);
    FS_UNLOCK();
    
    if(!ret) {
        free(file);
        file = NULL;
    }

    return ret;
}

bool port_fs_file_close(fs_file_t file) {
    if(!lfs_init_flag || !file) return false;
    FS_LOCK();
    int ret = lfs_file_close(&lfs, (lfs_file_t *)file);
    FS_UNLOCK();
    if(ret >= 0) {
        free(file);
    }

    return ret >= 0;
}

bool port_fs_exists(const char *path) {
    if(!lfs_init_flag) return false;
    FS_LOCK();
    bool ret = lfs_stat(&lfs, path, &lfs_info) == 0;
    FS_UNLOCK();
    return ret;
}

bool port_fs_remove(const char *path) {
    if(!lfs_init_flag) return false;
    FS_LOCK();
    bool ret = lfs_remove(&lfs, path) == 0;
    FS_UNLOCK();
    return ret;
}

bool port_fs_rename(const char *old_path, const char *new_path) {
    if(!lfs_init_flag) return false;
    FS_LOCK();
    bool ret = lfs_rename(&lfs, old_path, new_path) == 0;
    FS_UNLOCK();
    return ret;
}

bool port_fs_mkdir(const char *path) {
    if(!lfs_init_flag) return false;
    FS_LOCK();
    bool ret = lfs_mkdir(&lfs, path) == 0;
    FS_UNLOCK();
    return ret;
}

bool port_fs_rmdir(const char *path) {
    if(!lfs_init_flag) return false;
    FS_LOCK();
    bool ret = lfs_remove(&lfs, path) == 0;
    FS_UNLOCK();
    return ret;
}

bool port_fs_is_dir(const char *path) {
    if(!lfs_init_flag) return false;
    
    FS_LOCK();
    bool ret = lfs_stat(&lfs, path, &lfs_info) == 0;
    FS_UNLOCK();
    if(ret) {
        ret = lfs_info.type == LFS_TYPE_DIR;
    }

    return ret;
}

bool port_fs_dir_rewind(void) {
    if(!lfs_init_flag || !lfs_file_it.is_open) return false;
    FS_LOCK();
    int ret = lfs_dir_rewind(&lfs, &lfs_file_it.dir);
    FS_UNLOCK();
    return ret == 0;
}

bool port_fs_dir_seek(uint32_t pos) {
    if(!lfs_init_flag || !lfs_file_it.is_open) return -1;
    FS_LOCK();
    int ret = lfs_dir_seek(&lfs, &lfs_file_it.dir, pos);
    FS_UNLOCK();
    return ret == 0;
}

bool port_fs_dir_open(const char *path) {
    if(!lfs_init_flag) return false;
    bool ret = false;
    FS_LOCK();
    
    do {
        if(lfs_file_it.is_open) {
            lfs_open_next_close(&lfs_file_it);
        }
        if(lfs_open_next_init(&lfs_file_it, path) != 0) break;
        ret = true;
    }while(0);
    FS_UNLOCK();

    return ret;
}

bool port_fs_dir_close(void) {
    if(!lfs_init_flag) return false;
    bool ret = false;
    FS_LOCK();
    do {
        if(lfs_file_it.is_open) {
            lfs_open_next_close(&lfs_file_it);
        }
        ret = true;
    }while(0);
    FS_UNLOCK(); 
    
    return ret;
}

const char *port_fs_file_name(const char *path) {
    if(!lfs_init_flag) return NULL;
    FS_LOCK();
    bool ret = lfs_stat(&lfs, path, &lfs_info) == 0;
    FS_UNLOCK();

    if(!ret) return NULL;

    return lfs_info.name;
}

const char *port_fs_get_next_file_path(void) {
    if(!lfs_init_flag || !lfs_file_it.is_open) return NULL;
    FS_LOCK();
    const char *ret = lfs_get_next_file_path(&lfs_file_it);
    FS_UNLOCK();
    return ret;
}

const char *port_fs_get_next_path(bool *is_dir) {
    if(!lfs_init_flag || !lfs_file_it.is_open) return NULL;
    FS_LOCK();
    const char *ret = lfs_get_next_path(&lfs_file_it, is_dir);
    FS_UNLOCK();
    return ret;
}

size_t port_fs_file_read(fs_file_t file, uint8_t *buf, size_t buf_size) {
    if(!lfs_init_flag || !file || !buf) return 0;
    size_t ret = 0;
    FS_LOCK();
    lfs_ssize_t rw_len = lfs_file_read(&lfs, (lfs_file_t *)file, buf, buf_size);
    FS_UNLOCK();
    ret = (rw_len > 0)? rw_len: 0;
    return ret;
}

size_t port_fs_file_write(fs_file_t file, uint8_t *data, size_t len) {
    if(!lfs_init_flag || !file || !data) return 0;
    size_t ret = 0;
    FS_LOCK();
    lfs_ssize_t rw_len = lfs_file_write(&lfs, (lfs_file_t *)file, data, len);
    FS_UNLOCK();
    ret = (rw_len > 0)? rw_len: 0;
    if(ret) {
        last_write_time = millis();
    }
    return ret;
}

uint32_t port_fs_get_last_write_time(void) {
    if(!lfs_init_flag) return 0;
    return last_write_time;
}

int port_fs_file_available(fs_file_t file) {
    if(!lfs_init_flag || !file) return -1;
    FS_LOCK();
    int cur_pos = lfs_file_tell(&lfs, (lfs_file_t *)file);
    int file_size = lfs_file_size(&lfs, (lfs_file_t *)file);
    FS_UNLOCK();
    if(cur_pos < 0 || file_size < 0) return -1;
    return file_size - cur_pos;
}

int port_fs_file_size(fs_file_t file) {
    if(!lfs_init_flag || !file) return -1;
    FS_LOCK();
    int file_size = lfs_file_size(&lfs, (lfs_file_t *)file);
    FS_UNLOCK();
    return file_size;
}

int port_fs_file_seek(fs_file_t file, uint32_t pos, uint8_t mode) {
    if(!lfs_init_flag || !file) return -1;
    FS_LOCK();
    int ret = lfs_file_seek(&lfs, (lfs_file_t *)file, (lfs_off_t)pos, mode);
    FS_UNLOCK();
    return ret;
}

int port_fs_file_flush(fs_file_t file) {
    if(!lfs_init_flag || !file) return -1;
    FS_LOCK();
    int ret = lfs_file_sync(&lfs, (lfs_file_t *)file);
    FS_UNLOCK();
    return ret;
}

int port_fs_file_position(fs_file_t file) {
    if(!lfs_init_flag || !file) return -1;
    FS_LOCK();
    int ret = lfs_file_tell(&lfs, (lfs_file_t *)file);
    FS_UNLOCK();
    return ret;
}

#endif