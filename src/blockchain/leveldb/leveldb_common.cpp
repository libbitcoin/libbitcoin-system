#include "leveldb_common.hpp"

#include <bitcoin/block.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

template <typename Data>
leveldb::Slice slice(const Data& data)
{
    return leveldb::Slice(
        reinterpret_cast<const char*>(data.data()), data.size());
}

leveldb_common::leveldb_common(leveldb::DB* db_blocks,
    leveldb::DB* db_blocks_hash, leveldb::DB* db_txs,
    leveldb::DB* db_spends, leveldb::DB* db_address)
  : db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
    db_txs_(db_txs), db_spends_(db_spends), db_address_(db_address)
{
}

uint32_t leveldb_common::find_last_block_depth()
{
    leveldb::Iterator* it = db_blocks_->NewIterator(leveldb::ReadOptions());
    it->SeekToLast();
    if (!it->Valid() || !it->status().ok())
        return std::numeric_limits<uint32_t>::max();
    BITCOIN_ASSERT(it->key().size() == 4);
    const uint8_t* start = reinterpret_cast<const uint8_t*>(it->key().data());
    const uint8_t* end = start + it->key().size();
    return cast_chunk<uint32_t>(data_chunk(start, end));
}

bool leveldb_common::fetch_spend(const message::output_point& spent_output,
    message::input_point& input_spend)
{
#ifdef FOOOO
    readable_data_type search_spend;
    search_spend.set(create_spent_key(spent_output));
    writable_data_type raw_spend;
    //if (db_spends_->get(txn->get(), search_spend.get(),
    //        raw_spend.get(), 0) != 0)
    //    return false;
    const data_chunk raw_spend_data = raw_spend.data();
    deserializer deserial(raw_spend_data);
    input_spend.hash = deserial.read_hash();
    input_spend.index = deserial.read_4_bytes();
#endif
    return true;
}

bool leveldb_common::save_block(
    uint32_t depth, const message::block& serial_block)
{
    leveldb::WriteOptions options;
    protobuf::Block proto_block =
        block_header_to_protobuf(depth, serial_block);
    for (uint32_t tx_index = 0;
        tx_index < serial_block.transactions.size(); ++tx_index)
    {
        const message::transaction& block_tx =
            serial_block.transactions[tx_index];
        const hash_digest& tx_hash = hash_transaction(block_tx);
        //if (!save_transaction(txn, depth, tx_index, tx_hash, block_tx))
        //{
        //    log_fatal() << "Could not save transaction";
        //    return false;
        //}
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
    // Write block to database.
    db_blocks_->Put(options, slice(raw_depth), slice(raw_data));
    db_blocks_hash_->Put(options, slice(block_hash), slice(raw_depth));
    return true;
}

bool leveldb_common::save_transaction(uint32_t block_depth, uint32_t tx_index,
    const hash_digest& tx_hash, const message::transaction& block_tx)
{
#ifdef FOOOOOOOOOO
    if (dupli_save(tx_hash, block_depth, tx_index))
        return true;
    // Actually add block
    protobuf::Transaction proto_tx = transaction_to_protobuf(block_tx);
    proto_tx.set_is_coinbase(is_coinbase(block_tx));
    // Add parent block to transaction
    protobuf::Transaction_BlockPointer* proto_parent = proto_tx.add_parent();
    proto_parent->set_depth(block_depth);
    proto_parent->set_index(tx_index);
    // Save tx to leveldb
    std::ostringstream oss;
    if (!proto_tx.SerializeToOstream(&oss))
        return false;
    readable_data_type key, value;
    key.set(tx_hash);
    value.set(oss.str());
    // Checks for duplicates first
    //if (db_txs_->put(txn->get(), key.get(), value.get(), DB_NOOVERWRITE) != 0)
        return false;
    // Coinbase inputs do not spend anything.
    if (!is_coinbase(block_tx))
        for (uint32_t input_index = 0; input_index < block_tx.inputs.size();
            ++input_index)
        {
            const message::transaction_input& input = 
                block_tx.inputs[input_index];
            const message::input_point inpoint{tx_hash, input_index};
            if (!mark_spent_outputs(input.previous_output, inpoint))
                return false;
        }
    for (uint32_t output_index = 0; output_index < block_tx.outputs.size();
        ++output_index)
    {
        const message::transaction_output& output =
            block_tx.outputs[output_index];
        if (!add_address(output.output_script, {tx_hash, output_index}))
            return false;
    }
#endif
    return true;
}

bool leveldb_common::dupli_save(const hash_digest& tx_hash,
    uint32_t block_depth, uint32_t tx_index)
{
#ifdef FOOOOOOOO
    protobuf::Transaction proto_tx = fetch_proto_transaction(tx_hash);
    if (!proto_tx.IsInitialized())
        return false;
    BITCOIN_ASSERT(block_depth == 91842 || block_depth == 91880);
    protobuf::Transaction::BlockPointer* parent = proto_tx.add_parent();
    parent->set_depth(block_depth);
    parent->set_index(tx_index);
    return rewrite_transaction(tx_hash, proto_tx);
#endif
    return true;
}

bool leveldb_common::mark_spent_outputs(
    const message::output_point& previous_output,
    const message::input_point& current_input)
{
#ifdef FOOOOOOOOO
    readable_data_type spent_key, spend_value;
    spent_key.set(create_spent_key(previous_output));
    spend_value.set(create_spent_key(current_input));
    //if (db_spends_->put(txn->get(), spent_key.get(), spend_value.get(),
    //        DB_NOOVERWRITE) != 0)
    //    return false;
#endif
    return true;
}

bool leveldb_common::add_address(
    const script& output_script, const message::output_point& outpoint)
{
#ifdef FOOOOOO
    data_chunk raw_address = create_address_key(output_script);
    if (raw_address.empty())
        return true;
    readable_data_type address_key, output_value;
    address_key.set(raw_address);
    output_value.set(create_spent_key(outpoint));
    if (db_address_->put(txn->get(), address_key.get(),
            output_value.get(), 0) != 0)
        return false;
#endif
    return true;
}

bool leveldb_common::rewrite_transaction(
    const hash_digest& tx_hash, const protobuf::Transaction& replace_proto_tx)
{
#ifdef FOOOOO
    // Now rewrite tx
    // First delete old
    readable_data_type tx_key;
    tx_key.set(tx_hash);
    if (db_txs_->del(txn->get(), tx_key.get(), 0) != 0)
        return false;
    // Save tx to leveldb
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
#endif
    return true;
}

template<typename Index, typename ProtoType>
bool proto_read(Db* database, const Index& index, ProtoType& proto_object)
{
    readable_data_type key;
    key.set(index);
    writable_data_type data;
    //if (database->get(txn->get(), key.get(), data.get(), 0) != 0)
    //    return false;
    std::stringstream ss;
    data_chunk raw_object(data.data());
    std::copy(raw_object.begin(), raw_object.end(),
        std::ostream_iterator<byte>(ss));
    proto_object.ParseFromIstream(&ss);
    return true;
}

protobuf::Block leveldb_common::fetch_proto_block(uint32_t depth)
{
    data_chunk raw_depth = uncast_type(depth);
    std::string value;
    leveldb::Status status = db_blocks_->Get(
        leveldb::ReadOptions(), slice(raw_depth), &value);
    if (!status.ok())
    {
        if (!status.IsNotFound())
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
    std::string value;
    leveldb::Status status = db_blocks_hash_->Get(
        leveldb::ReadOptions(), slice(block_hash), &value);
    if (!status.ok())
    {
        if (!status.IsNotFound())
            log_fatal() << "Error fetch_proto_block(" << block_hash << "): "
                << status.ToString();
        return protobuf::Block();
    }
    uint32_t depth =
        cast_chunk<uint32_t>(data_chunk(value.begin(), value.end()));
    return fetch_proto_block(depth);
}

protobuf::Transaction leveldb_common::fetch_proto_transaction(
    const hash_digest& tx_hash)
{
    protobuf::Transaction proto_tx;
    //if (!proto_read(db_txs_, txn, tx_hash, proto_tx))
    //    return protobuf::Transaction();
    return proto_tx;
}

bool leveldb_common::reconstruct_block(
    const protobuf::Block& proto_block_header,
    message::block& result_block)
{
#ifdef FOOOOO
    result_block = protobuf_to_block_header(proto_block_header);
    for (const std::string& raw_tx_hash: proto_block_header.transactions())
    {
        protobuf::Transaction proto_tx;
        if (!proto_read(db_txs_, raw_tx_hash, proto_tx))
            return false;
        result_block.transactions.push_back(protobuf_to_transaction(proto_tx));
    }
#endif
    return true;
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

