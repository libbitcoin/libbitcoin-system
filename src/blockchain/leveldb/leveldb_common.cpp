#include "leveldb_common.hpp"

#include <bitcoin/block.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/satoshi_serialize.hpp>
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
        log_fatal(LOG_BLOCKCHAIN) << "fetch_spend: " << status.ToString();
        return false;
    }
    const data_chunk raw_spend_data(raw_spend.begin(), raw_spend.end());
    auto deserial = make_deserializer(
        raw_spend_data.begin(), raw_spend_data.end());
    input_spend.hash = deserial.read_hash();
    input_spend.index = deserial.read_4_bytes();
    return true;
}

bool leveldb_common::save_block(
    uint32_t depth, const block_type& serial_block)
{
    leveldb_transaction_batch batch;
    // Write block header + tx hashes
    data_chunk raw_block_data(
        80 + 4 + serial_block.transactions.size() * hash_digest_size);
    // Downcast to base header type so serializer selects that.
    auto header_end = satoshi_save(
        serial_block.header, raw_block_data.begin());
    BITCOIN_ASSERT(std::distance(raw_block_data.begin(), header_end) == 80);
    auto serial_hashes = make_serializer(header_end);
    // Write the number of transactions...
    serial_hashes.write_4_bytes(serial_block.transactions.size());
    // ... And now the tx themselves.
    for (uint32_t tx_index = 0;
        tx_index < serial_block.transactions.size(); ++tx_index)
    {
        const transaction_type& block_tx =
            serial_block.transactions[tx_index];
        const hash_digest& tx_hash = hash_transaction(block_tx);
        if (!save_transaction(batch, depth, tx_index, tx_hash, block_tx))
        {
            log_fatal(LOG_BLOCKCHAIN) << "Could not save transaction";
            return false;
        }
        serial_hashes.write_hash(tx_hash);
    }
    BITCOIN_ASSERT(
        std::distance(raw_block_data.begin(), serial_hashes.iterator()) ==
        80 + 4 + serial_block.transactions.size() * hash_digest_size);
    data_chunk raw_depth = uncast_type(depth);
    hash_digest block_hash = hash_block_header(serial_block.header);
    // Begin commiting changes to database.
    leveldb::WriteOptions options;
    // Write block to database.
    db_blocks_->Put(options, slice(raw_depth), slice(raw_block_data));
    db_blocks_hash_->Put(options,
        slice_block_hash(block_hash), slice(raw_depth));
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
    data_chunk tx_data(8 + satoshi_raw_size(block_tx));
    // Serialize tx.
    auto serial = make_serializer(tx_data.begin());
    serial.write_4_bytes(block_depth);
    serial.write_4_bytes(tx_index);
    // Actual tx data.
    auto end_iter = satoshi_save(block_tx, serial.iterator());
    BITCOIN_ASSERT(
        std::distance(tx_data.begin(), end_iter) ==
        8 + satoshi_raw_size(block_tx));
    // Save tx to leveldb
    batch.tx_batch.Put(slice(tx_hash), slice(tx_data));
    // Add inputs to spends database.
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
    // Save address -> output mappings.
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
    leveldb_tx_info tx;
    if (!get_transaction(tx, tx_hash, false, false))
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
    // Count the number of outpoints for this address.
    uint32_t counter = rand();
    // Add counter to raw_address key because leveldb
    // doesn't support duplicate keys.
    data_chunk raw_counter = uncast_type(counter);
    // Use 3 bytes so key size is 24 bytes for alignment.
    raw_counter.pop_back();
    BITCOIN_ASSERT(raw_counter.size() == 3);
    extend_data(raw_address, raw_counter);
    BITCOIN_ASSERT(raw_address.size() == (1 + 20 + 3));
    // Must be a multiple of (32 + 4)
    data_chunk raw_outpoint = create_spent_key(outpoint);
    BITCOIN_ASSERT(raw_outpoint.size() == (32 + 4));
    address_batch.Put(slice(raw_address), slice(raw_outpoint));
    return true;
}

uint32_t leveldb_common::get_block_depth(const hash_digest& block_hash)
{
    std::string value;
    leveldb::Status status = db_blocks_hash_->Get(
        leveldb::ReadOptions(), slice_block_hash(block_hash), &value);
    if (status.IsNotFound())
        return std::numeric_limits<uint32_t>::max();
    else if (!status.ok())
    {
        log_fatal(LOG_BLOCKCHAIN) << "fetch_block_depth("
            << block_hash << "): " << status.ToString();
        return std::numeric_limits<uint32_t>::max();
    }
    return recreate_depth(value);
}

bool leveldb_common::get_block(leveldb_block_info& blk_info,
    uint32_t depth, bool read_header, bool read_tx_hashes)
{
    // First we try to read the bytes from the database.
    data_chunk raw_depth = uncast_type(depth);
    std::string value;
    leveldb::Status status = db_blocks_->Get(
        leveldb::ReadOptions(), slice(raw_depth), &value);
    if (status.IsNotFound())
        return false;
    else if (!status.ok())
    {
        log_fatal(LOG_BLOCKCHAIN) << "fetch_proto_block("
            << depth << "): " << status.ToString();
        return false;
    }
    return deserialize_block(blk_info, value, read_header, read_tx_hashes);
}
bool leveldb_common::deserialize_block(leveldb_block_info& blk_info,
    const std::string& raw_data, bool read_header, bool read_tx_hashes)
{
    // Read the header (if neccessary).
    // There is always at least one tx in a block.
    BITCOIN_ASSERT(raw_data.size() >= 80 + 4 + hash_digest_size);
    BITCOIN_ASSERT((raw_data.size() - 84) % hash_digest_size == 0);
    if (read_header)
        satoshi_load(raw_data.begin(), raw_data.begin() + 80, blk_info.header);
    if (!read_tx_hashes)
        return true;
    // Read the tx hashes for this block (if neccessary).
    auto deserial = make_deserializer(raw_data.begin() + 80, raw_data.end());
    uint32_t tx_count = deserial.read_4_bytes();
    for (size_t i = 0; i < tx_count; ++i)
    {
        const hash_digest& tx_hash = deserial.read_hash();
        blk_info.tx_hashes.push_back(tx_hash);
    }
    return true;
}

bool leveldb_common::get_transaction(leveldb_tx_info& tx_info,
    const hash_digest& tx_hash, bool read_parent, bool read_tx)
{
    // First we try to read the bytes from the database.
    std::string value;
    leveldb::Status status = db_txs_->Get(
        leveldb::ReadOptions(), slice(tx_hash), &value);
    if (status.IsNotFound())
        return false;
    else if (!status.ok())
    {
        log_fatal(LOG_BLOCKCHAIN) << "get_transaction("
            << tx_hash << "): " << status.ToString();
        return false;
    }
    // Read the parent block depth and our index in that block (if neccessary).
    BITCOIN_ASSERT(value.size() > 8);
    if (read_parent)
    {
        auto deserial = make_deserializer(value.begin(), value.begin() + 8);
        tx_info.depth = deserial.read_4_bytes();
        tx_info.index = deserial.read_4_bytes();
    }
    if (!read_tx)
        return true;
    // Read the actual transaction (if neccessary).
    try
    {
        BITCOIN_ASSERT(value.size() > 8);
        satoshi_load(value.begin() + 8, value.end(), tx_info.tx);
    }
    catch (end_of_stream)
    {
        return false;
    }
    BITCOIN_ASSERT(satoshi_raw_size(tx_info.tx) + 8 == value.size());
    BITCOIN_ASSERT(hash_transaction(tx_info.tx) == tx_hash);
    return true;
}

leveldb::Slice slice_block_hash(const hash_digest& block_hash)
{
    // Cut first 16 bytes of block hash
    BITCOIN_ASSERT(block_hash.size() == 32);
    return leveldb::Slice(
        reinterpret_cast<const char*>(block_hash.data() + 16), 16);
}

output_point slice_to_output_point(const leveldb::Slice& out_slice)
{
    // We need a copy not a temporary
    data_chunk raw_outpoint;
    const uint8_t* value_start =
        reinterpret_cast<const uint8_t*>(out_slice.data());
    raw_outpoint.insert(raw_outpoint.end(),
        value_start, value_start + out_slice.size());
    // Then read the value off
    auto deserial = make_deserializer(raw_outpoint.begin(), raw_outpoint.end());
    output_point outpoint;
    outpoint.hash = deserial.read_hash();
    outpoint.index = deserial.read_4_bytes();
    return outpoint;
}

data_chunk create_address_key(const script& output_script)
{
    payment_address address;
    if (!extract(address, output_script))
        return data_chunk();
    data_chunk result(1 + short_hash_size);
    auto serial = make_serializer(result.begin());
    serial.write_byte(address.version());
    serial.write_short_hash(address.hash());
    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator()) ==
        1 + short_hash_size);
    return result;
}

leveldb::Iterator* address_iterator(leveldb::DB* db_address,
    const data_chunk& raw_address)
{
    leveldb::Iterator* it = db_address->NewIterator(leveldb::ReadOptions());
    it->Seek(slice(raw_address));
    return it;
}
bool valid_address_iterator(leveldb_iterator& it,
    const data_chunk& raw_address)
{
    return it->Valid() && it->key().starts_with(slice(raw_address));
}

} // namespace libbitcoin

