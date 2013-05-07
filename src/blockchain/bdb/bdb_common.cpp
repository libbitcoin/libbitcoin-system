#include "bdb_common.hpp"

#include DB_CXX_HEADER

#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/address.hpp>

namespace libbitcoin {

bdb_common::bdb_common(DbEnv* env, Db* db_blocks, Db* db_blocks_hash,
    Db* db_txs, Db* db_spends, Db* db_address)
  : env_(env), db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
    db_txs_(db_txs), db_spends_(db_spends), db_address_(db_address)
{
}

uint32_t bdb_common::find_last_block_depth(txn_guard_ptr txn)
{
    Dbc* cursor;
    db_blocks_->cursor(txn->get(), &cursor, 0);
    BITCOIN_ASSERT(cursor != nullptr);
    writable_data_type key, data;
    if (cursor->get(key.get(), data.get(), DB_LAST) == DB_NOTFOUND)
        return std::numeric_limits<uint32_t>::max();
    BITCOIN_ASSERT(key.get()->get_size() == 4);
    uint32_t last_block_depth = cast_chunk<uint32_t>(key.data());
    cursor->close();
    return last_block_depth;
}

bool bdb_common::fetch_spend(txn_guard_ptr txn,
    const message::output_point& spent_output,
    message::input_point& input_spend)
{
    readable_data_type search_spend;
    search_spend.set(bdb_create_spent_key(spent_output));
    writable_data_type raw_spend;
    if (db_spends_->get(txn->get(), search_spend.get(),
            raw_spend.get(), 0) != 0)
        return false;
    const data_chunk raw_spend_data = raw_spend.data();
    deserializer deserial(raw_spend_data);
    input_spend.hash = deserial.read_hash();
    input_spend.index = deserial.read_4_bytes();
    return true;
}

bool bdb_common::save_block(txn_guard_ptr txn,
    uint32_t depth, const block_type& serial_block)
{
    proto::Block proto_block =
        block_header_to_proto(depth, serial_block);
    for (uint32_t tx_index = 0;
        tx_index < serial_block.transactions.size(); ++tx_index)
    {
        const transaction_type& block_tx =
            serial_block.transactions[tx_index];
        const hash_digest& tx_hash = hash_transaction(block_tx);
        if (!save_transaction(txn, depth, tx_index, tx_hash, block_tx))
        {
            log_fatal() << "Could not save transaction";
            return false;
        }
        proto_block.add_transactions(
            std::string(tx_hash.begin(), tx_hash.end()));
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

bool bdb_common::save_transaction(txn_guard_ptr txn, uint32_t block_depth,
    uint32_t tx_index, const hash_digest& tx_hash,
    const transaction_type& block_tx)
{
    if (dupli_save(txn, tx_hash, block_depth, tx_index))
        return true;
    // Actually add block
    proto::Transaction proto_tx = transaction_to_proto(block_tx);
    proto_tx.set_is_coinbase(is_coinbase(block_tx));
    // Add parent block to transaction
    proto::Transaction_BlockPointer* proto_parent = proto_tx.add_parent();
    proto_parent->set_depth(block_depth);
    proto_parent->set_index(tx_index);
    // Save tx to bdb
    std::ostringstream oss;
    if (!proto_tx.SerializeToOstream(&oss))
        return false;
    readable_data_type key, value;
    key.set(tx_hash);
    value.set(oss.str());
    // Checks for duplicates first
    if (db_txs_->put(txn->get(), key.get(), value.get(), DB_NOOVERWRITE) != 0)
        return false;
    // Coinbase inputs do not spend anything.
    if (!is_coinbase(block_tx))
        for (uint32_t input_index = 0; input_index < block_tx.inputs.size();
            ++input_index)
        {
            const transaction_input_type& input =
                block_tx.inputs[input_index];
            const message::input_point inpoint{tx_hash, input_index};
            if (!mark_spent_outputs(txn, input.previous_output, inpoint))
                return false;
        }
    for (uint32_t output_index = 0; output_index < block_tx.outputs.size();
        ++output_index)
    {
        const transaction_output_type& output =
            block_tx.outputs[output_index];
        if (!add_address(txn, output.output_script, {tx_hash, output_index}))
            return false;
    }
    return true;
}

bool bdb_common::dupli_save(txn_guard_ptr txn, const hash_digest& tx_hash,
    uint32_t block_depth, uint32_t tx_index)
{
    proto::Transaction proto_tx = fetch_proto_transaction(txn, tx_hash);
    if (!proto_tx.IsInitialized())
        return false;
    BITCOIN_ASSERT(block_depth == 91842 || block_depth == 91880);
    proto::Transaction::BlockPointer* parent = proto_tx.add_parent();
    parent->set_depth(block_depth);
    parent->set_index(tx_index);
    return rewrite_transaction(txn, tx_hash, proto_tx);
}

bool bdb_common::mark_spent_outputs(txn_guard_ptr txn,
    const message::output_point& previous_output,
    const message::input_point& current_input)
{
    readable_data_type spent_key, spend_value;
    spent_key.set(bdb_create_spent_key(previous_output));
    spend_value.set(bdb_create_spent_key(current_input));
    if (db_spends_->put(txn->get(), spent_key.get(), spend_value.get(),
            DB_NOOVERWRITE) != 0)
        return false;
    return true;
}

bool bdb_common::add_address(txn_guard_ptr txn,
    const script& output_script, const message::output_point& outpoint)
{
    data_chunk raw_address = bdb_create_address_key(output_script);
    if (raw_address.empty())
        return true;
    readable_data_type address_key, output_value;
    address_key.set(raw_address);
    output_value.set(bdb_create_spent_key(outpoint));
    if (db_address_->put(txn->get(), address_key.get(),
            output_value.get(), 0) != 0)
        return false;
    return true;
}

bool bdb_common::rewrite_transaction(txn_guard_ptr txn,
    const hash_digest& tx_hash, const proto::Transaction& replace_proto_tx)
{
    // Now rewrite tx
    // First delete old
    readable_data_type tx_key;
    tx_key.set(tx_hash);
    if (db_txs_->del(txn->get(), tx_key.get(), 0) != 0)
        return false;
    // Save tx to bdb
    std::ostringstream write_oss;
    if (!replace_proto_tx.SerializeToOstream(&write_oss))
        return false;
    readable_data_type tx_data;
    tx_data.set(write_oss.str());
    // Checks for duplicates first
    if (db_txs_->put(txn->get(), tx_key.get(), tx_data.get(),
        DB_NOOVERWRITE) != 0)
    {
        return false;
    }
    return true;
}

template<typename Index, typename ProtoType>
bool proto_read(Db* database, txn_guard_ptr txn,
    const Index& index, ProtoType& proto_object)
{
    readable_data_type key;
    key.set(index);
    writable_data_type data;
    if (database->get(txn->get(), key.get(), data.get(), 0) != 0)
        return false;
    std::stringstream ss;
    data_chunk raw_object(data.data());
    std::copy(raw_object.begin(), raw_object.end(),
        std::ostream_iterator<byte>(ss));
    proto_object.ParseFromIstream(&ss);
    return true;
}

proto::Block bdb_common::fetch_proto_block(txn_guard_ptr txn,
    uint32_t depth)
{
    proto::Block proto_block;
    if (!proto_read(db_blocks_, txn, depth, proto_block))
        return proto::Block();
    return proto_block;
}

proto::Transaction bdb_common::fetch_proto_transaction(
    txn_guard_ptr txn, const hash_digest& tx_hash)
{
    proto::Transaction proto_tx;
    if (!proto_read(db_txs_, txn, tx_hash, proto_tx))
        return proto::Transaction();
    return proto_tx;
}

proto::Block bdb_common::fetch_proto_block(txn_guard_ptr txn,
    const hash_digest& block_hash)
{
    proto::Block proto_block;
    if (!proto_read(db_blocks_hash_, txn, block_hash, proto_block))
        return proto::Block();
    return proto_block;
}

bool bdb_common::reconstruct_block(txn_guard_ptr txn,
    const proto::Block& proto_block_header,
    block_type& result_block)
{
    result_block = proto_to_block_header(proto_block_header);
    for (const std::string& raw_tx_hash: proto_block_header.transactions())
    {
        proto::Transaction proto_tx;
        if (!proto_read(db_txs_, txn, raw_tx_hash, proto_tx))
            return false;
        result_block.transactions.push_back(proto_to_transaction(proto_tx));
    }
    return true;
}

data_chunk bdb_create_address_key(const script& output_script)
{
    payment_address address;
    if (!extract(address, output_script))
        return data_chunk();
    serializer serial;
    serial.write_byte(address.version());
    serial.write_short_hash(address.hash());
    return serial.data();
}

} // namespace libbitcoin

