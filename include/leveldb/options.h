// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
#define STORAGE_LEVELDB_INCLUDE_OPTIONS_H_

#include <stddef.h>
#include "leveldb/export.h"

namespace leveldb {

class Cache;
class Comparator;
class Env;
class FilterPolicy;
class Logger;
class Snapshot;

// DB contents are stored in a set of blocks, each of which holds a
// sequence of key,value pairs.  Each block may be compressed before
// being stored in a file.  The following enum describes which
// compression method (if any) is used to compress a block.
enum CompressionType {
  // NOTE: do not change the values of existing entries, as these are
  // part of the persistent format on disk.
  kNoCompression     = 0x0,
  kSnappyCompression = 0x1
};

// Options to control the behavior of a database (passed to DB::Open)
struct LEVELDB_EXPORT Options {
  // -------------------
  // Parameters that affect behavior

  // Comparator used to define the order of keys in the table.
  // Default: a comparator that uses lexicographic byte-wise ordering
  //
  // REQUIRES: The client must ensure that the comparator supplied
  // here has the same name and orders keys *exactly* the same as the
  // comparator provided to previous open calls on the same DB.
  const Comparator* comparator;

  // If true, the database will be created if it is missing.
  // Default: false
  // 默认值是false，如果为true，则会创建数据库
  bool create_if_missing;

  // If true, an error is raised if the database already exists.
  // Default: false
  // 默认为false，如果为true，则会抛出一个错误
  bool error_if_exists;

  // If true, the implementation will do aggressive checking of the
  // data it is processing and will stop early if it detects any
  // errors.  This may have unforeseen ramifications: for example, a
  // corruption of one DB entry may cause a large number of entries to
  // become unreadable or for the entire DB to become unopenable.
  // Default: false
  // 如果为真，程序执行将会对正在执行的数据进行非常严格的检查，如果检查到任何的错误，将会及早的停止
  // 这个将会有不可不可预见的分支：例如....(不太会翻译)
  bool paranoid_checks;

  // Use the specified object to interact with the environment,
  // e.g. to read/write files, schedule background work, etc.
  // Default: Env::Default()
  Env* env;

  // Any internal progress/error information generated by the db will
  // be written to info_log if it is non-NULL, or to a file stored
  // in the same directory as the DB contents if info_log is NULL.
  // Default: NULL
  // 任何内部产生的进度信息和错误信息将会被写入info_log（如果info_log是非Null的）
  // 或者如果info_log是NULL，那么则把信息作为文件存储在相同的目录下，作为数据库的内容
  Logger* info_log;

  // -------------------
  // Parameters that affect performance
  // 影响性能的参数

  // Amount of data to build up in memory (backed by an unsorted log
  // on disk) before converting to a sorted on-disk file.
  // Larger values increase performance, especially during bulk loads.
  // Up to two write buffers may be held in memory at the same time,
  // so you may wish to adjust this parameter to control memory usage.
  // Also, a larger write buffer will result in a longer recovery time
  // the next time the database is opened.
  // 在转换为一个有序的磁盘文件之前，大量的数据建立在内存中（在磁盘中会有一个无序的log文件存储）
  // 更大的值会增加性能，尤其是在大块的载入时
  // 在同一个时间内，在内存中可以支持同时有两个写入缓冲区
  // 所以你可能希望通过调整参数去控制内存的使用
  // 当然，一个更大的写缓冲区将会导致一个更长的恢复时间在下一次将数据库打开时
  //
  // Default: 4MB
  size_t write_buffer_size;

  // Number of open files that can be used by the DB.  You may need to
  // increase this if your database has a large working set (budget
  // one open file per 2MB of working set).
  // 在数据库可以被使用的最大的打开的文件数量。（每个打开的文件有2M的工作集？）
  // 如果你的数据库有更大的工作集，需要去提高这个默认值
  //
  // Default: 1000
  int max_open_files;

  // Control over blocks (user data is stored in a set of blocks, and
  // a block is the unit of reading from disk).
  // 通过块控制（使用者数据存储在一些列的块当中，并且块是从磁盘读取的单元）

  // If non-NULL, use the specified cache for blocks.
  // If NULL, leveldb will automatically create and use an 8MB internal cache.
  // 如果是non—NULL,那么对blocks使用指定的缓存
  // 如果是NULL，leveldb将会自动的创建和使用一个8M的内部缓存
  // Default: NULL
  Cache* block_cache;

  // Approximate size of user data packed per block.  Note that the
  // block size specified here corresponds to uncompressed data.  The
  // actual size of the unit read from disk may be smaller if
  // compression is enabled.  This parameter can be changed dynamically.
  // 用户数据包装的每个块的近似大小，注意：这里指定的块的大小对应的是没有经过压缩的数据
  // 如果允许压缩，从磁盘读取的单元的实际大小会更小， 这个参数可以动态的改变

  // Default: 4K
  size_t block_size;

  // Number of keys between restart points for delta encoding of keys.
  // This parameter can be changed dynamically.  Most clients should
  // leave this parameter alone.
  //
  // Default: 16
  int block_restart_interval;

  // Leveldb will write up to this amount of bytes to a file before
  // switching to a new one.
  // Most clients should leave this parameter alone.  However if your
  // filesystem is more efficient with larger files, you could
  // consider increasing the value.  The downside will be longer
  // compactions and hence longer latency/performance hiccups.
  // Another reason to increase this parameter might be when you are
  // initially populating a large database.
  //
  // Default: 2MB
  size_t max_file_size;

  // Compress blocks using the specified compression algorithm.  This
  // parameter can be changed dynamically.
  //
  // Default: kSnappyCompression, which gives lightweight but fast
  // compression.
  //
  // Typical speeds of kSnappyCompression on an Intel(R) Core(TM)2 2.4GHz:
  //    ~200-500MB/s compression
  //    ~400-800MB/s decompression
  // Note that these speeds are significantly faster than most
  // persistent storage speeds, and therefore it is typically never
  // worth switching to kNoCompression.  Even if the input data is
  // incompressible, the kSnappyCompression implementation will
  // efficiently detect that and will switch to uncompressed mode.
  CompressionType compression;

  // EXPERIMENTAL: If true, append to existing MANIFEST and log files
  // when a database is opened.  This can significantly speed up open.
  //
  // Default: currently false, but may become true later.
  bool reuse_logs;

  // If non-NULL, use the specified filter policy to reduce disk reads.
  // Many applications will benefit from passing the result of
  // NewBloomFilterPolicy() here.
  // 如果是non—NULL， 使用指定的过滤策略去降低磁盘读取.
  // 许多的应用将会通过NewBloomFilterPolicy()获益
  //
  // Default: NULL
  const FilterPolicy* filter_policy;

  // Create an Options object with default values for all fields.
  // 构造函数
  Options();
};

// Options that control read operations
// 关于控制读取操作的一些选项
struct LEVELDB_EXPORT ReadOptions {
  // If true, all data read from underlying storage will be
  // verified against corresponding checksums.
  // Default: false
  // 是否对读到的block做校验
  bool verify_checksums;

  // Should the data read for this iteration be cached in memory?
  // Callers may wish to set this field to false for bulk scans.
  // Default: true
  // 读到的block是否加入到cache
  bool fill_cache;

  // If "snapshot" is non-NULL, read as of the supplied snapshot
  // (which must belong to the DB that is being read and which must
  // not have been released).  If "snapshot" is NULL, use an implicit
  // snapshot of the state at the beginning of this read operation.
  // Default: NULL
  // 指定读取的snapshot
  const Snapshot* snapshot;

  ReadOptions()
      : verify_checksums(false),
        fill_cache(true),
        snapshot(NULL) {
  }
};

// Options that control write operations
struct LEVELDB_EXPORT WriteOptions {
  // If true, the write will be flushed from the operating system
  // buffer cache (by calling WritableFile::Sync()) before the write
  // is considered complete.  If this flag is true, writes will be
  // slower.
  //
  // If this flag is false, and the machine crashes, some recent
  // writes may be lost.  Note that if it is just the process that
  // crashes (i.e., the machine does not reboot), no writes will be
  // lost even if sync==false.
  //
  // In other words, a DB write with sync==false has similar
  // crash semantics as the "write()" system call.  A DB write
  // with sync==true has similar crash semantics to a "write()"
  // system call followed by "fsync()".
  //
  // Default: false
  // write时，记binlog后，是否对binlog做sync
  bool sync;

  WriteOptions()
      : sync(false) {
  }
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
