// Copyright (c) 2017, Xiaomi, Inc.  All rights reserved.
// This source code is licensed under the Apache License Version 2.0, which
// can be found in the LICENSE file in the root directory of this source tree.

#pragma once

#include <rocksdb/db.h>
#include <dsn/dist/replication/replica_base.h>

#include <rrdb/rrdb_types.h>

#include "pegasus_server_impl.h"

namespace pegasus {
namespace server {

class pegasus_server_impl;

// Manage meta data of Pegasus, now support
// - pegasus_data_version
// - pegasus_last_flushed_decree
// - pegasus_last_manual_compact_finish_time
class meta_store : public dsn::replication::replica_base
{
public:
    meta_store(pegasus_server_impl *server, rocksdb::DB *db, rocksdb::ColumnFamilyHandle *meta_cf);

    uint64_t get_last_flushed_decree() const;
    uint64_t get_decree_from_readonly_db(rocksdb::DB *db,
                                         rocksdb::ColumnFamilyHandle *meta_cf) const;
    uint32_t get_data_version() const;
    uint64_t get_last_manual_compact_finish_time() const;
    std::string get_usage_scenario() const;

    void set_last_flushed_decree(uint64_t decree) const;
    void set_data_version(uint32_t version) const;
    void set_last_manual_compact_finish_time(uint64_t last_manual_compact_finish_time) const;
    void set_usage_scenario(const std::string &usage_scenario) const;

private:
    ::dsn::error_code
    get_value_from_meta_cf(bool read_flushed_data, const std::string &key, uint64_t *value) const;
    ::dsn::error_code get_string_value_from_meta_cf(bool read_flushed_data,
                                                    const std::string &key,
                                                    std::string *value) const;
    ::dsn::error_code set_value_to_meta_cf(const std::string &key, uint64_t value) const;
    ::dsn::error_code set_string_value_to_meta_cf(const std::string &key,
                                                  const std::string &value) const;

    static ::dsn::error_code get_value_from_meta_cf(rocksdb::DB *db,
                                                    rocksdb::ColumnFamilyHandle *cf,
                                                    bool read_flushed_data,
                                                    const std::string &key,
                                                    uint64_t *value);
    static ::dsn::error_code get_string_value_from_meta_cf(rocksdb::DB *db,
                                                           rocksdb::ColumnFamilyHandle *cf,
                                                           bool read_flushed_data,
                                                           const std::string &key,
                                                           std::string *value);

    friend class pegasus_write_service;

    // Keys of meta data wrote into meta column family.
    static const std::string DATA_VERSION;
    static const std::string LAST_FLUSHED_DECREE;
    static const std::string LAST_MANUAL_COMPACT_FINISH_TIME;

    rocksdb::DB *_db;
    rocksdb::ColumnFamilyHandle *_meta_cf;
    rocksdb::WriteOptions _wt_opts;
};

} // namespace server
} // namespace pegasus
