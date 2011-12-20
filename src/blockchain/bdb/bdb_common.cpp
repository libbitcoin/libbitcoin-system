#include "bdb_common.hpp"

#include DB_CXX_HEADER

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

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
    writable_data_type key, data;
    if (cursor->get(key.get(), data.get(), DB_LAST) == DB_NOTFOUND)
        return std::numeric_limits<uint32_t>::max();
    BITCOIN_ASSERT(key.get()->get_size() == 4);
    uint32_t last_block_depth = cast_chunk<uint32_t>(key.data());
    cursor->close();
    return last_block_depth;
}

bool bdb_common::save_block(txn_guard_ptr txn,
    uint32_t depth, const message::block serial_block)
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


bool bdb_common::database_read(Db* database, DbTxn* txn, Dbt* key,
    std::stringstream& ss)
{
    writable_data_type data;
    if (database->get(txn, key, data.get(), 0) != 0)
        return false;
    data_chunk raw_object(data.data());
    std::copy(raw_object.begin(), raw_object.end(),
        std::ostream_iterator<byte>(ss));
    return true;
}

bool bdb_common::reconstruct_block(txn_guard_ptr txn,
    const protobuf::Block& proto_block_header,
    message::block& result_block)
{
    result_block = protobuf_to_block_header(proto_block_header);
    for (uint32_t tx_index: proto_block_header.transactions())
    {
        protobuf::Transaction proto_tx;
        if (!proto_read(db_txs_, txn, tx_index, proto_tx))
            return false;
        result_block.transactions.push_back(protobuf_to_transaction(proto_tx));
    }
    return true;
}

} // libbitcoin

