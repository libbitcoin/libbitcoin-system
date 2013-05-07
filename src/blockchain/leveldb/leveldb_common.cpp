#include "leveldb_common.hpp"

#include <bitcoin/block.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

leveldb_common::leveldb_common(leveldb::DB* db_blocks,
    leveldb::DB* db_blocks_hash, leveldb::DB* db_txs,
    leveldb::DB* db_spends, leveldb::DB* db_address)
  : db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
    db_txs_(db_txs), db_spends_(db_spends), db_address_(db_address)
{
}

uint32_t leveldb_common::find_last_block_depth()
{
    leveldb_iterator it(db_blocks_->NewIterator(leveldb::ReadOptions()));
    it->SeekToLast();
    if (!it->Valid() || !it->status().ok())
        return std::numeric_limits<uint32_t>::max();
    BITCOIN_ASSERT(it->key().size() == 4);
    return recreate_depth(it->key());
}

bool leveldb_common::fetch_spend(const output_point& spent_output,
    input_point& input_spend)
{
    data_chunk spent_key = create_spent_key(spent_output);
    std::string raw_spend;
    leveldb::Status status = db_spends_->Get(
        leveldb::ReadOptions(), slice(spent_key), &raw_spend);
    if (status.IsNotFound())
        return false;
    else if (!status.ok())
    {
        log_fatal() << "Error fetch_spend: " << status.ToString();
        return false;
    }
    const data_chunk raw_spend_data(raw_spend.begin(), raw_spend.end());
    deserializer deserial(raw_spend_data);
    input_spend.hash = deserial.read_hash();
    input_spend.index = deserial.read_4_bytes();
    return true;
}

bool leveldb_common::save_block(
    uint32_t depth, const block_type& serial_block)
{
    leveldb_transaction_batch batch;
    protobuf::Block proto_block =
        block_header_to_protobuf(depth, serial_block);
    for (uint32_t tx_index = 0;
        tx_index < serial_block.transactions.size(); ++tx_index)
    {
        const transaction_type& block_tx =
            serial_block.transactions[tx_index];
        const hash_digest& tx_hash = hash_transaction(block_tx);
        if (!save_transaction(batch, depth, tx_index, tx_hash, block_tx))
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
    data_chunk raw_depth = uncast_type(depth);
    std::string raw_data = oss.str();
    hash_digest block_hash = hash_block_header(serial_block);
    // Begin commiting changes to database.
    leveldb::WriteOptions options;
    // Write block to database.
    db_blocks_->Put(options, slice(raw_depth), slice(raw_data));
    db_blocks_hash_->Put(options, slice(block_hash), slice(raw_depth));
    // Execute batches.
    db_txs_->Write(options, &batch.tx_batch);
    db_spends_->Write(options, &batch.spends_batch);
    db_address_->Write(options, &batch.address_batch);
    return true;
}

bool leveldb_common::save_transaction(leveldb_transaction_batch& batch,
    uint32_t block_depth, uint32_t tx_index,
    const hash_digest& tx_hash, const transaction_type& block_tx)
{
    if (duplicate_exists(tx_hash, block_depth, tx_index))
        return true;
    // Actually add block
    protobuf::Transaction proto_tx = transaction_to_protobuf(block_tx);
    proto_tx.set_is_coinbase(is_coinbase(block_tx));
    // Set tx's parent block.
    proto_tx.mutable_parent()->set_depth(block_depth);
    proto_tx.mutable_parent()->set_index(tx_index);
    // Save tx to leveldb
    std::ostringstream oss;
    if (!proto_tx.SerializeToOstream(&oss))
        return false;
    std::string raw_data = oss.str();
    batch.tx_batch.Put(slice(tx_hash), raw_data);
    // Coinbase inputs do not spend anything.
    if (!is_coinbase(block_tx))
        for (uint32_t input_index = 0; input_index < block_tx.inputs.size();
            ++input_index)
        {
            const transaction_input_type& input =
                block_tx.inputs[input_index];
            const input_point inpoint{tx_hash, input_index};
            if (!mark_spent_outputs(batch.spends_batch,
                    input.previous_output, inpoint))
                return false;
        }
    for (uint32_t output_index = 0; output_index < block_tx.outputs.size();
        ++output_index)
    {
        const transaction_output_type& output =
            block_tx.outputs[output_index];
        if (!add_address(batch.address_batch,
                output.output_script, {tx_hash, output_index}))
            return false;
    }
    return true;
}

bool leveldb_common::duplicate_exists(const hash_digest& tx_hash,
    uint32_t block_depth, uint32_t tx_index)
{
    protobuf::Transaction proto_tx = fetch_proto_transaction(tx_hash);
    if (!proto_tx.IsInitialized())
        return false;
    BITCOIN_ASSERT(block_depth == 91842 || block_depth == 91880);
    return true;
}

bool leveldb_common::mark_spent_outputs(leveldb::WriteBatch& spends_batch,
    const output_point& previous_output,
    const input_point& current_input)
{
    data_chunk spent_key = create_spent_key(previous_output),
        spend_value = create_spent_key(current_input);
    spends_batch.Put(slice(spent_key), slice(spend_value));
    return true;
}

bool leveldb_common::add_address(leveldb::WriteBatch& address_batch,
    const script& output_script, const output_point& outpoint)
{
    data_chunk raw_address = create_address_key(output_script);
    if (raw_address.empty())
        return true;
    std::string outpoints;
    leveldb::Status status = db_address_->Get(
        leveldb::ReadOptions(), slice(raw_address), &outpoints);
    if (!status.ok() && !status.IsNotFound())
        return false;
    // Must be a multiple of (32 + 4)
    BITCOIN_ASSERT(outpoints.size() % (32 + 4) == 0);
    extend_data(outpoints, create_spent_key(outpoint));
    BITCOIN_ASSERT(outpoints.size() % (32 + 4) == 0);
    address_batch.Put(slice(raw_address), outpoints);
    return true;
}

protobuf::Block leveldb_common::fetch_proto_block(uint32_t depth)
{
    data_chunk raw_depth = uncast_type(depth);
    std::string value;
    leveldb::Status status = db_blocks_->Get(
        leveldb::ReadOptions(), slice(raw_depth), &value);
    if (status.IsNotFound())
        return protobuf::Block();
    else if (!status.ok())
    {
        log_fatal() << "Error fetch_proto_block(" << depth << "): "
            << status.ToString();
        return protobuf::Block();
    }
    std::stringstream ss(value);
    protobuf::Block proto_block;
    proto_block.ParseFromIstream(&ss);
    return proto_block;
}

protobuf::Block leveldb_common::fetch_proto_block(const hash_digest& block_hash)
{
    return fetch_proto_block(fetch_block_depth(block_hash));
}

uint32_t leveldb_common::fetch_block_depth(const hash_digest& block_hash)
{
    std::string value;
    leveldb::Status status = db_blocks_hash_->Get(
        leveldb::ReadOptions(), slice(block_hash), &value);
    if (status.IsNotFound())
        return std::numeric_limits<uint32_t>::max();
    else if (!status.ok())
    {
        log_fatal() << "Error fetch_block_depth(" << block_hash << "): "
            << status.ToString();
        return std::numeric_limits<uint32_t>::max();
    }
    return recreate_depth(value);
}

protobuf::Transaction leveldb_common::fetch_proto_transaction(
    const hash_digest& tx_hash)
{
    std::string value;
    leveldb::Status status = db_txs_->Get(
        leveldb::ReadOptions(), slice(tx_hash), &value);
    if (status.IsNotFound())
        return protobuf::Transaction();
    else if (!status.ok())
    {
        log_fatal() << "Error fetch_proto_tx(" << tx_hash << "): "
            << status.ToString();
        return protobuf::Transaction();
    }
    std::stringstream ss(value);
    protobuf::Transaction proto_tx;
    proto_tx.ParseFromIstream(&ss);
    return proto_tx;
}

data_chunk create_address_key(const script& output_script)
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

