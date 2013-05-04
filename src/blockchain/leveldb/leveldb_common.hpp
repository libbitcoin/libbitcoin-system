#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_COMMON_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_COMMON_H

#include <memory>
#include <leveldb/db.h>

#include <bitcoin/messages.hpp>
#include <bitcoin/utility/serializer.hpp>

#include "data_type.hpp"
#include "txn_guard.hpp"
#include "protobuf_wrapper.hpp"

namespace libbitcoin {

class leveldb_common
{
public:
    leveldb_common(leveldb::DB* db_blocks, leveldb::DB* db_blocks_hash,
        leveldb::DB* db_txs, leveldb::DB* db_spends, leveldb::DB* db_address);

    uint32_t find_last_block_depth(txn_guard_ptr txn);
    bool fetch_spend(txn_guard_ptr txn,
        const message::output_point& spent_output,
        message::input_point& input_spend);

    bool save_block(uint32_t depth, const message::block& serial_block);

    protobuf::Block fetch_proto_block(uint32_t depth);
    protobuf::Block fetch_proto_block(const hash_digest& block_hash);
    protobuf::Transaction fetch_proto_transaction(txn_guard_ptr txn,
        const hash_digest& tx_hash);

    bool reconstruct_block(txn_guard_ptr txn,
        const protobuf::Block& proto_block_header,
        message::block& result_block);

private:
    bool save_transaction(txn_guard_ptr txn, uint32_t block_depth,
        uint32_t tx_index, const hash_digest& tx_hash,
        const message::transaction& block_tx);
    bool dupli_save(txn_guard_ptr txn, const hash_digest& tx_hash,
        uint32_t block_depth, uint32_t tx_index);
    bool mark_spent_outputs(txn_guard_ptr txn,
        const message::output_point& previous_output,
        const message::input_point& current_input);
    // returns false only on database failure. It may or may not add an entry
    bool add_address(txn_guard_ptr txn, const script& output_script,
        const message::output_point& outpoint);
    bool rewrite_transaction(txn_guard_ptr txn, const hash_digest& tx_hash,
        const protobuf::Transaction& replace_proto_tx);

    leveldb::DB* db_blocks_;
    leveldb::DB* db_blocks_hash_;
    leveldb::DB* db_txs_;
    leveldb::DB* db_spends_;
    leveldb::DB* db_address_;
};

typedef std::shared_ptr<leveldb_common> leveldb_common_ptr;

// Used also by leveldb_chain_keeper when deleting spends + addresses
template <typename Point>
data_chunk create_spent_key(const Point& point)
{
    serializer serial_spend;
    serial_spend.write_hash(point.hash);
    serial_spend.write_4_bytes(point.index);
    return serial_spend.data();
}

data_chunk create_address_key(const script& output_script);

} // namespace libbitcoin

#endif

