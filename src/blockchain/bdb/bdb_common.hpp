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
        uint32_t depth, const message::block serial_block);
    uint32_t save_transaction(txn_guard_ptr txn,
        const message::transaction& block_tx);

    template<typename Index, typename ProtoType>
    static bool proto_read(Db* database, txn_guard_ptr txn,
        const Index& index, ProtoType& proto_object)
    {
        readable_data_type key;
        key.set(index);
        std::stringstream ss;
        if (!database_read(database, txn->get(), key.get(), ss))
            return false;
        proto_object.ParseFromIstream(&ss);
        return true;
    }

    bool reconstruct_block(txn_guard_ptr txn,
        const protobuf::Block& proto_block_header,
        message::block& result_block);
private:
    static bool database_read(Db* database, DbTxn* txn, Dbt* key,
        std::stringstream& ss);

    DbEnv* env_;
    Db* db_blocks_;
    Db* db_blocks_hash_;
    Db* db_txs_;
    Db* db_txs_hash_;
};

typedef std::shared_ptr<bdb_common> bdb_common_ptr;

} // libbitcoin

#endif

