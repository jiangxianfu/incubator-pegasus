// Copyright (c) 2017, Xiaomi, Inc.  All rights reserved.
// This source code is licensed under the Apache License Version 2.0, which
// can be found in the LICENSE file in the root directory of this source tree.

#pragma once

#include <dsn/dist/replication/replica_base.h>
#include <dsn/perf_counter/perf_counter_wrapper.h>
#include <rrdb/rrdb_types.h>

namespace pegasus {
namespace server {

class hotkey_collector;

class capacity_unit_calculator : public dsn::replication::replica_base
{
public:
    capacity_unit_calculator(replica_base *r,
                             std::shared_ptr<hotkey_collector> read_hotkey_collector,
                             std::shared_ptr<hotkey_collector> write_hotkey_collector);

    void add_get_cu(int32_t status, const dsn::blob &key, const dsn::blob &value);
    void add_multi_get_cu(int32_t status,
                          const dsn::blob &hash_key,
                          const std::vector<::dsn::apps::key_value> &kvs);
    void add_scan_cu(int32_t status, const std::vector<::dsn::apps::key_value> &kvs);
    void add_sortkey_count_cu(int32_t status, const dsn::blob &hash_key);
    void add_ttl_cu(int32_t status, const dsn::blob &key);

    void add_put_cu(int32_t status, const dsn::blob &key, const dsn::blob &value);
    void add_remove_cu(int32_t status, const dsn::blob &key);
    void add_multi_put_cu(int32_t status,
                          const dsn::blob &hash_key,
                          const std::vector<::dsn::apps::key_value> &kvs);
    void add_multi_remove_cu(int32_t status,
                             const dsn::blob &hash_key,
                             const std::vector<::dsn::blob> &sort_keys);
    void add_incr_cu(int32_t status, const dsn::blob &key);
    void add_check_and_set_cu(int32_t status,
                              const dsn::blob &hash_key,
                              const dsn::blob &check_sort_key,
                              const dsn::blob &set_sort_key,
                              const dsn::blob &value);
    void add_check_and_mutate_cu(int32_t status,
                                 const dsn::blob &hash_key,
                                 const dsn::blob &check_sort_key,
                                 const std::vector<::dsn::apps::mutate> &mutate_list);

protected:
    friend class capacity_unit_calculator_test;

#ifdef PEGASUS_UNIT_TEST
    virtual int64_t add_read_cu(int64_t read_data_size);
    virtual int64_t add_write_cu(int64_t write_data_size);
#else
    int64_t add_read_cu(int64_t read_data_size);
    int64_t add_write_cu(int64_t write_data_size);
#endif

private:
    uint64_t _read_capacity_unit_size;
    uint64_t _write_capacity_unit_size;
    uint32_t _log_read_cu_size;
    uint32_t _log_write_cu_size;

    ::dsn::perf_counter_wrapper _pfc_recent_read_cu;
    ::dsn::perf_counter_wrapper _pfc_recent_write_cu;

    ::dsn::perf_counter_wrapper _pfc_get_bytes;
    ::dsn::perf_counter_wrapper _pfc_multi_get_bytes;
    ::dsn::perf_counter_wrapper _pfc_scan_bytes;
    ::dsn::perf_counter_wrapper _pfc_put_bytes;
    ::dsn::perf_counter_wrapper _pfc_multi_put_bytes;
    ::dsn::perf_counter_wrapper _pfc_check_and_set_bytes;
    ::dsn::perf_counter_wrapper _pfc_check_and_mutate_bytes;

    /*
        hotkey capturing weight rules:
            add_get_cu: whether find the key or not, weight = 1(read_collector),
            add_multi_get_cu: weight = returned sortkey count(read_collector),
            add_scan_cu : not capture now,
            add_sortkey_count_cu: weight = 1(read_collector),
            add_ttl_cu: weight = 1(read_collector),
            add_put_cu: weight = 1(write_collector),
            add_remove_cu: weight = 1(write_collector),
            add_multi_put_cu: weight = returned sortkey count(write_collector),
            add_multi_remove_cu: weight = returned sortkey count(write_collector),
            add_incr_cu: if find the key, weight = 1(write_collector),
                         else weight = 1(read_collector)
            add_check_and_set_cu: if find the key, weight = 1(write_collector),
                         else weight = 1(read_collector)
            add_check_and_mutate_cu: if find the key, weight = mutate_list size
                                     else weight = 1
    */
    std::shared_ptr<hotkey_collector> _read_hotkey_collector;
    std::shared_ptr<hotkey_collector> _write_hotkey_collector;
};

} // namespace server
} // namespace pegasus
