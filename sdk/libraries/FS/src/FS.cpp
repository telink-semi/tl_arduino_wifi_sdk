/********************************************************************************************************
 * @file    FS.cpp
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
#if PORT_CONFIG_FS_SUPPORTED

#include "FS.h"
#include "port_interface.h"

using namespace fs;

File::File(fs_file_t _file, const char *_path) : file(_file), file_path(_path) {

}

size_t File::write(uint8_t c) {
  if (!*this) {
    return 0;
  }

  return write(&c, 1);
}

time_t File::getLastWrite() {
  if (!*this) {
    return 0;
  }

  return port_fs_get_last_write_time();
}

size_t File::write(const uint8_t *buf, size_t size) {
  if (!*this) {
    return 0;
  }

  return port_fs_file_write(file, (uint8_t *)buf, size);
}

int File::available() {
  if (!*this) {
    return false;
  }

  return port_fs_file_available(file);
}

int File::read() {
  if (!*this) {
    return -1;
  }

  uint8_t result;
  if (port_fs_file_read(file, &result, 1) != 1) {
    return -1;
  }

  return result;
}

size_t File::read(uint8_t *buf, size_t size) {
  if (!*this) {
    return -1;
  }

  return port_fs_file_read(file, buf, size);
}

int File::peek() {
  if (!*this) {
    return -1;
  }

  size_t curPos = position();
  int result = read();
  seek(curPos, SeekSet);
  return result;
}

void File::flush() {
  if (!*this) {
    return;
  }

  port_fs_file_flush(file);
}

bool File::seek(uint32_t pos, SeekMode mode) {
  if (!*this) {
    return false;
  }

  return port_fs_file_seek(file, pos, mode) == (int)pos;
}

size_t File::position() const {
  if (!*this) {
    return (size_t)-1;
  }

  return port_fs_file_position(file);
}

size_t File::size() const {
  if (!*this) {
    return 0;
  }

  return port_fs_file_size(file);
}

bool File::setBufferSize(size_t size) {
  (void)size;
  return false;
}

void File::close() {
  port_fs_file_close(file);
  file = NULL;
}

File::operator bool() const {
  return file != NULL;
}

const char *File::path() const {
  if (!*this) {
    return NULL;
  }

  return file_path.c_str();
}

const char *File::name() const {
  if (!*this) {
    return NULL;
  }

  return port_fs_file_name(path());
}

//to implement
boolean File::isDirectory(void) {
  if (!*this) {
    return false;
  }
  return port_fs_is_dir(path());
}

File File::openNextFile(const char *mode) {
  if (!*this) {
    return File();
  }
  const char *file_next_path = port_fs_get_next_file_path();
  if(!file_next_path) {
    return File();
  }

  fs_file_t file_next = port_fs_file_open(file_next_path, mode, false);
  if(!file_next) {
    return File();
  }

  return File(file_next, file_next_path);
}

boolean File::seekDir(long position) {
  return port_fs_dir_seek(position);
}

String File::getNextFileName(void) {
  return port_fs_file_name(port_fs_get_next_file_path());
}

String File::getNextFileName(bool *isDir) {
  return port_fs_get_next_path(isDir);
}

void File::rewindDirectory(void) {
  if (!*this) {
    return;
  }
  port_fs_dir_rewind();
}

FS::FS() {
  port_fs_init();
  port_fs_dir_open(mountpoint());
}

FS::~FS() {
  port_fs_dir_close();
  port_fs_deinit();
}

File FS::open(const String &path, const char *mode, const bool create) {
  return open(path.c_str(), mode, create);
}

File FS::open(const char *path, const char *mode, const bool create) {
  if (!path || !mode) {
    return File();
  }
  fs_file_t file = port_fs_file_open(path, mode, create);

  if(!file) {
    return File();
  }
  return File(file, path);
}

bool FS::exists(const char *path) {
  return port_fs_exists(path);
}

bool FS::exists(const String &path) {
  return exists(path.c_str());
}

bool FS::remove(const char *path) {
  return port_fs_remove(path);
}

bool FS::remove(const String &path) {
  return remove(path.c_str());
}

bool FS::rename(const char *pathFrom, const char *pathTo) {
  return port_fs_rename(pathFrom, pathTo);
}

bool FS::rename(const String &pathFrom, const String &pathTo) {
  return rename(pathFrom.c_str(), pathTo.c_str());
}

bool FS::mkdir(const char *path) {
  return port_fs_mkdir(path);
}

bool FS::mkdir(const String &path) {
  return mkdir(path.c_str());
}

bool FS::rmdir(const char *path) {
  return port_fs_rmdir(path);
}

bool FS::rmdir(const String &path) {
  return rmdir(path.c_str());
}

const char *FS::mountpoint() {
  return "/";
}

fs::FS littleFS;

#endif