#include "leveldb_common.hpp"

#include <bitcoin/block.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/address.hpp>
#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

void leveldb_databases::write(leveldb_transaction_batch& batch)
{
    leveldb::WriteOptions options;
    // Begin commiting changes to database.
    block->Write(options, &batch.block);
    block_hash->Write(options, &batch.block_hash);
    tx->Write(options, &batch.tx);
    spend->Write(options, &batch.spend);
    credit->Write(options, &batch.credit);
    debit->Write(options, &batch.debit);
}

bool mark_spent_outputs(leveldb::WriteBatch& spend_batch,
    const output_point& previous_output, const input_point& spent_inpoint);
// Both functions return false only on database failure.
// They may or may not add an entry
bool add_debit(leveldb::WriteBatch& batch,
    const transaction_input_type& input, const input_point& inpoint);
bool add_credit(leveldb::WriteBatch& batch,
    const transaction_output_type& output, const output_point& outpoint,
    uint32_t block_depth);

leveldb_common::leveldb_common(leveldb_databases db)
  : db_(db)
{
}

uint32_t leveldb_common::find_last_block_depth()
{
    leveldb_iterator it(db_.block->NewIterator(leveldb::ReadOptions()));
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
    leveldb::Status status = db_.spend->Get(
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
    // Write block header
    batch.block.Put(slice(raw_depth), slice(raw_block_data));
    batch.block_hash.Put(slice_block_hash(block_hash), slice(raw_depth));
    // Execute batches.
    db_.write(batch);
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
    batch.tx.Put(slice(tx_hash), slice(tx_data));
    // Add inputs to spends database.
    // Coinbase inputs do not spend anything.
    if (!is_coinbase(block_tx))
        for (uint32_t input_index = 0; input_index < block_tx.inputs.size();
            ++input_index)
        {
            const transaction_input_type& input =
                block_tx.inputs[input_index];
            const input_point inpoint{tx_hash, input_index};
            if (!mark_spent_outputs(batch.spend,
                    input.previous_output, inpoint))
                return false;
            if (!add_debit(batch.debit, input, {tx_hash, input_index}))
                return false;
        }
    // Save address -> output mappings.
    for (uint32_t output_index = 0; output_index < block_tx.outputs.size();
        ++output_index)
    {
        const transaction_output_type& output =
            block_tx.outputs[output_index];
        if (!add_credit(batch.credit,
                output, {tx_hash, output_index}, block_depth))
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

bool mark_spent_outputs(leveldb::WriteBatch& spend_batch,
    const output_point& previous_output, const input_point& spent_inpoint)
{
    data_chunk spent_key = create_spent_key(previous_output),
        spend_value = create_spent_key(spent_inpoint);
    spend_batch.Put(slice(spent_key), slice(spend_value));
    return true;
}

bool input_has_pubkey(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code == opcode::special &&
        ops[1].code == opcode::special;
}
bool extract_input_address(
    payment_address& address, const script& input_script)
{
    const operation_stack& ops = input_script.operations();
    if (!input_has_pubkey(ops))
        return false;
    BITCOIN_ASSERT(ops.size() == 2);
    const data_chunk& pubkey = ops[1].data;
    if (!set_public_key(address, pubkey))
        return false;
    return true;
}

bool add_debit(leveldb::WriteBatch& batch,
    const transaction_input_type& input, const input_point& inpoint)
{
    payment_address address;
    // Not a Bitcoin address so skip this output.
    if (!extract_input_address(address, input.input_script))
        return true;
    data_chunk addr_key = create_address_key(address, input.previous_output);
    // inpoint
    data_chunk row_info(36);
    auto serial = make_serializer(row_info.begin());
    // inpoint
    serial.write_hash(inpoint.hash);
    serial.write_4_bytes(inpoint.index);
    BITCOIN_ASSERT(
        std::distance(row_info.begin(), serial.iterator()) == 36);
    batch.Put(slice(addr_key), slice(row_info));
    return true;
}

bool add_credit(leveldb::WriteBatch& batch,
    const transaction_output_type& output, const output_point& outpoint,
    uint32_t block_depth)
{
    payment_address address;
    // Not a Bitcoin address so skip this output.
    if (!extract(address, output.output_script))
        return true;
    data_chunk addr_key = create_address_key(address, outpoint);
    // outpoint, value, block_depth
    data_chunk row_info(36 + 8 + 4);
    auto serial = make_serializer(row_info.begin());
    // outpoint
    serial.write_hash(outpoint.hash);
    serial.write_4_bytes(outpoint.index);
    // value
    serial.write_8_bytes(output.value);
    // block_depth
    // We add the block depth for reordering.
    serial.write_4_bytes(block_depth);
    BITCOIN_ASSERT(
        std::distance(row_info.begin(), serial.iterator()) == 36 + 8 + 4);
    batch.Put(slice(addr_key), slice(row_info));
    return true;
}

uint32_t leveldb_common::get_block_depth(const hash_digest& block_hash)
{
    std::string value;
    leveldb::Status status = db_.block_hash->Get(
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
    leveldb::Status status = db_.block->Get(
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
    leveldb::Status status = db_.tx->Get(
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

uint32_t addr_key_checksum(const output_point& outpoint)
{
    data_chunk chksum_data(hash_digest_size + 4);
    auto serial = make_serializer(chksum_data.begin());
    serial.write_hash(outpoint.hash);
    serial.write_4_bytes(outpoint.index);
    BITCOIN_ASSERT(
        std::distance(chksum_data.begin(), serial.iterator()) ==
        hash_digest_size + 4);
    return generate_sha256_checksum(chksum_data);
}
data_chunk create_address_key(
    const payment_address& address, const output_point& outpoint)
{
    data_chunk result(1 + short_hash_size + 4);
    auto serial = make_serializer(result.begin());
    serial.write_byte(address.version());
    serial.write_short_hash(address.hash());
    serial.write_4_bytes(addr_key_checksum(outpoint));
    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator()) ==
        1 + short_hash_size + 4);
    return result;
}

} // namespace libbitcoin

