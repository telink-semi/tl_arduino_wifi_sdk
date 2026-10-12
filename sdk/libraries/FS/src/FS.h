/********************************************************************************************************
 * @file    FS.h
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
#ifndef FS_H
#define FS_H

#include "port_config.h"
#if PORT_CONFIG_FS_SUPPORTED

#include <Arduino.h>

namespace fs {

#define FILE_READ   "r"
#define FILE_WRITE  "w"
#define FILE_APPEND "a"

class File;

enum SeekMode {
  SeekSet = 0,
  SeekCur = 1,
  SeekEnd = 2
};

class File : public Stream {
protected:
  fs_file_t file;
  String file_path;
public:
  bool isOpen() { return file != NULL; }
  File(fs_file_t _file=NULL, const char *_path="");
  size_t write(uint8_t) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  size_t read(uint8_t *buf, size_t size);
  size_t readBytes(char *buffer, size_t length) {
    return read((uint8_t *)buffer, length);
  }

  bool seek(uint32_t pos, SeekMode mode);
  bool seek(uint32_t pos) {
    return seek(pos, SeekSet);
  }
  size_t position() const;  // returns (size_t)-1 on error
  size_t size() const;
  bool setBufferSize(size_t size);
  void close();
  operator bool() const;
  time_t getLastWrite();
  const char *path() const;
  const char *name() const;

  boolean isDirectory(void);
  boolean seekDir(long position);
  File openNextFile(const char *mode = FILE_READ);
  String getNextFileName(void);
  String getNextFileName(boolean *isDir);
  void rewindDirectory(void);

};

class FS {
public:
  FS();
  ~FS();
  File open(const char *path, const char *mode = FILE_READ, const bool create = false);
  File open(const String &path, const char *mode = FILE_READ, const bool create = false);

  bool exists(const char *path);
  bool exists(const String &path);

  bool remove(const char *path);
  bool remove(const String &path);

  bool rename(const char *pathFrom, const char *pathTo);
  bool rename(const String &pathFrom, const String &pathTo);

  bool mkdir(const char *path);
  bool mkdir(const String &path);

  bool rmdir(const char *path);
  bool rmdir(const String &path);

  const char *mountpoint();

};

}  // namespace fs

#ifndef FS_NO_GLOBALS
using fs::File;
using fs::FS;
using fs::SeekCur;
using fs::SeekEnd;
using fs::SeekMode;
using fs::SeekSet;
#endif  //FS_NO_GLOBALS

extern fs::FS littleFS;

#endif
#endif  //FS_H
