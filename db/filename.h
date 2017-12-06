// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// File names used by DB code

#ifndef STORAGE_LEVELDB_DB_FILENAME_H_
#define STORAGE_LEVELDB_DB_FILENAME_H_

#include <stdint.h>
#include <string>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "port/port.h"

namespace leveldb {

class Env;

// db中的文件用文件名区分类型，有以下几种类型
// 1. kLogFile, 日志文件：[0-9]+.log
//  leveldb的写流程是先记binlog，然后写sstable，该日志文件即是binlog。前缀数字为FileNumber
// 2. kDBLockFile, lock文件：LOCK
//  一个db同时只能有一个db实例操作，通过对LOCK文件加文件锁（flock）实现主动保护
// 3. kTableFile, sstable文件：[0-9]+.sst
//  保存数据的sstable文件。前缀为FileNumber。
// 4. kDescriptorFile, db元文件信息： MANIFEST-[0-9]+
//  每当db中的状态改变(VersionSet), 会将这次改变(VersionEdit)追加到descriptor文件中
//  后缀数字为FileNumber
// 5. kCurrentFile, : CURRENT
//  CURRENT文件中保存当前使用的descriptor文件的文件名
// 6. kTempFile, 临时文件： [0-9]+.dbtmp
//  对db做修复（Repairer）时，会产生临时文件。前缀为FileNumber.
// 7.kINfoLogFile, db运行时打印日志的文件：LOG
//  db运行时，打印的info日志保存在LOG中。每次重新运行，则会现将LOG文件重命名为LOG.old
enum FileType {
  kLogFile,
  kDBLockFile,
  kTableFile,
  kDescriptorFile,
  kCurrentFile,
  kTempFile,
  kInfoLogFile  // Either the current one, or an old one
};

// Return the name of the log file with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string LogFileName(const std::string& dbname, uint64_t number);

// Return the name of the sstable with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string TableFileName(const std::string& dbname, uint64_t number);

// Return the legacy file name for an sstable with the specified number
// in the db named by "dbname". The result will be prefixed with
// "dbname".
extern std::string SSTTableFileName(const std::string& dbname, uint64_t number);

// Return the name of the descriptor file for the db named by
// "dbname" and the specified incarnation number.  The result will be
// prefixed with "dbname".
extern std::string DescriptorFileName(const std::string& dbname,
                                      uint64_t number);

// Return the name of the current file.  This file contains the name
// of the current manifest file.  The result will be prefixed with
// "dbname".
extern std::string CurrentFileName(const std::string& dbname);

// Return the name of the lock file for the db named by
// "dbname".  The result will be prefixed with "dbname".
extern std::string LockFileName(const std::string& dbname);

// Return the name of a temporary file owned by the db named "dbname".
// The result will be prefixed with "dbname".
extern std::string TempFileName(const std::string& dbname, uint64_t number);

// Return the name of the info log file for "dbname".
extern std::string InfoLogFileName(const std::string& dbname);

// Return the name of the old info log file for "dbname".
extern std::string OldInfoLogFileName(const std::string& dbname);

// If filename is a leveldb file, store the type of the file in *type.
// The number encoded in the filename is stored in *number.  If the
// filename was successfully parsed, returns true.  Else return false.
extern bool ParseFileName(const std::string& filename,
                          uint64_t* number,
                          FileType* type);

// Make the CURRENT file point to the descriptor file with the
// specified number.
extern Status SetCurrentFile(Env* env, const std::string& dbname,
                             uint64_t descriptor_number);


}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_FILENAME_H_
