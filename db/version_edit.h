// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_VERSION_EDIT_H_
#define STORAGE_LEVELDB_DB_VERSION_EDIT_H_

#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb {

class VersionSet;

// sstable文件的原信息封装成FileMetaData
struct FileMetaData {
  int refs;                   // 引用计数
  int allowed_seeks;          // Seeks allowed until compaction(compact之前允许的seek次数（参见Version）)
  uint64_t number;
  uint64_t file_size;         // File size in bytes
  InternalKey smallest;       // Smallest internal key served by table （sstable文件的最小key）
  InternalKey largest;        // Largest internal key served by table  （sstable文件的最大key）

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) { }
};

// compact过程中会有一系列改变当前Version的操作(FileNumber增加， 删除input的sstable， 增加输出的sstable····)
// 为了缩小Version切换的时间点，将这些操作封装成VersionEdit， compact完成时，将VersionEdit中的操作一次应用到当前
// Version,即可得到最新状态的Version
class VersionEdit {
 public:
  VersionEdit() { Clear(); }
  ~VersionEdit() { }

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }
  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back(std::make_pair(level, key));
  }

  // Add the specified file at the specified number.
  // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
  // REQUIRES: "smallest" and "largest" are smallest and largest keys in file
  void AddFile(int level, uint64_t file,
               uint64_t file_size,
               const InternalKey& smallest,
               const InternalKey& largest) {
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
    new_files_.push_back(std::make_pair(level, f));
  }

  // Delete the specified "file" from the specified "level".
  void DeleteFile(int level, uint64_t file) {
    deleted_files_.insert(std::make_pair(level, file));
  }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(const Slice& src);

  std::string DebugString() const;

 private:
  friend class VersionSet;

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  // db一旦创建，排序的逻辑就必须保持兼容，用comparator的名字做凭证
  std::string comparator_;
  // log的FileNumber
  uint64_t log_number_;
  // 辅助log的FileNumber
  uint64_t prev_log_number_;
  // 下一个可用FileNumber
  uint64_t next_file_number_;
  // 用过的最后一个SequenceNumber
  SequenceNumber last_sequence_;
  // 标识是否存在，验证使用
  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

  // 要更新的level==> compact_pointer
  std::vector< std::pair<int, InternalKey> > compact_pointers_;
  // 要删除的sstable文件(compact的input)
  DeletedFileSet deleted_files_;
  // 新的文件(compact的output)
  std::vector< std::pair<int, FileMetaData> > new_files_;
};
// 每次compact之后，都会讲对应的VersionEdit encode入manifest文件

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
