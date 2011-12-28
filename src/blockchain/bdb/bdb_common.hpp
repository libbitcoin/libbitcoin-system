#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_COMMON_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_COMMON_H

#include <memory>

#include <bitcoin/messages.hpp>

#include "data_type.hpp"
#include "txn_guard.hpp"
#include "protobuf_wrapper.hpp"

class DbEnv;
class Db;

namespace libbitcoin {

class bdb_common
{
public:
    bdb_common(DbEnv* env, Db* db_blocks, Db* db_blocks_hash,
        Db* db_txs, Db* db_txs_hash);

    uint32_t find_last_block_depth(txn_guard_ptr txn);

    bool save_block(txn_guard_ptr txn,
        uint32_t depth, const message::block& serial_block);

    protobuf::Block fetch_proto_block(txn_guard_ptr txn, uint32_t depth);
    protobuf::Block fetch_proto_block(txn_guard_ptr txn,
        const hash_digest& block_hash);
    protobuf::Transaction fetch_proto_transaction(txn_guard_ptr txn,
        const hash_digest& tx_hash);

    bool reconstruct_block(txn_guard_ptr txn,
        const protobuf::Block& proto_block_header,
        message::block& result_block);

private:
    uint32_t save_transaction(txn_guard_ptr txn, uint32_t block_depth,
        uint32_t tx_index, const message::transaction& block_tx);

    DbEnv* env_;
    Db* db_blocks_;
    Db* db_blocks_hash_;
    Db* db_txs_;
    Db* db_txs_hash_;
};

typedef std::shared_ptr<bdb_common> bdb_common_ptr;

} // libbitcoin

#endif

