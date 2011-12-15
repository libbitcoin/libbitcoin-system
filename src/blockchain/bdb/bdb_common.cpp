#include "bdb_common.hpp"

#include DB_CXX_HEADER

#include <bitcoin/util/logger.hpp>

#include "data_type.hpp"
#include "txn_guard.hpp"
#include "protobuf_wrapper.hpp"

namespace libbitcoin {

bdb_common::bdb_common(DbEnv* env, Db* db_blocks, Db* db_blocks_hash,
    Db* db_txs, Db* db_txs_hash)
 : env_(env), db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
    db_txs_(db_txs), db_txs_hash_(db_txs_hash)
{
}

uint32_t bdb_common::find_last_block_depth(txn_guard_ptr txn)
{
    Dbc* cursor;
    db_blocks_->cursor(txn->get(), &cursor, 0);
    Dbt key, data;
    if (cursor->get(&key, &data, DB_LAST) == DB_NOTFOUND)
        return std::numeric_limits<uint32_t>::max();
    BITCOIN_ASSERT(key.get_size() == 4);
    data_chunk raw_depth;
    extend_data(raw_depth, std::string(
        reinterpret_cast<const char*>(key.get_data()), key.get_size()));
    uint32_t last_block_depth = cast_chunk<uint32_t>(raw_depth);
    cursor->close();
    return last_block_depth;
}

bool bdb_common::save_block(txn_guard_ptr txn,
    size_t depth, const message::block serial_block)
{
    protobuf::Block proto_block =
        block_header_to_protobuf(depth, serial_block);
    for (const message::transaction& block_tx: serial_block.transactions)
    {
        uint32_t block_tx_id = save_transaction(txn, block_tx);
        if (block_tx_id == 0)
        {
            log_fatal() << "Could not save transaction";
            return false;
        }
        proto_block.add_transactions(block_tx_id);
    }
    std::ostringstream oss;
    if (!proto_block.SerializeToOstream(&oss))
    {
        log_fatal() << "Protobuf serialization failed";
        return false;
    }
    readable_data_type key, value;
    key.set(depth);
    value.set(oss.str());
    if (db_blocks_->put(txn->get(), key.get(), value.get(), 0) != 0)
    {
        log_fatal() << "bdb put() failed";
        return false;
    }
    return true;
}

uint32_t bdb_common::save_transaction(txn_guard_ptr txn,
    const message::transaction& block_tx)
{
    uint32_t block_tx_id = rand();
    BITCOIN_ASSERT(block_tx_id != 0);
    protobuf::Transaction proto_tx = transaction_to_protobuf(block_tx);
    std::ostringstream oss;
    if (!proto_tx.SerializeToOstream(&oss))
        return 0;
    readable_data_type key, value;
    key.set(block_tx_id);
    value.set(oss.str());
    // Checks for duplicates first
    db_txs_->put(txn->get(), key.get(), value.get(), DB_NOOVERWRITE);
    return block_tx_id;
}

} // libbitcoin

