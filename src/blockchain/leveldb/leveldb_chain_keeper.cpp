#include "leveldb_chain_keeper.hpp"

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>

#include "leveldb_common.hpp"
#include "data_type.hpp"
#include "protobuf_wrapper.hpp"

namespace libbitcoin {

leveldb_chain_keeper::leveldb_chain_keeper(leveldb_common_ptr common,
    DbEnv* env, Db* db_blocks, Db* db_blocks_hash,
    Db* db_txs, Db* db_spends, Db* db_address)
  : common_(common), env_(env),
    db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
    db_txs_(db_txs), db_spends_(db_spends), db_address_(db_address)
{
}

void leveldb_chain_keeper::start()
{
}
void leveldb_chain_keeper::stop()
{
}

void leveldb_chain_keeper::add(block_detail_ptr incoming_block)
{
    uint32_t last_block_depth = common_->find_last_block_depth();
    const message::block& actual_block = incoming_block->actual();
    if (!common_->save_block(last_block_depth + 1, actual_block))
        log_fatal() << "Saving block in organizer failed";
}

int leveldb_chain_keeper::find_index(const hash_digest& search_block_hash)
{
    readable_data_type key;
    key.set(search_block_hash);
    writable_data_type primary_key;
    empty_data_type ignore_data;
    if (db_blocks_hash_->pget(nullptr, key.get(),
        primary_key.get(), ignore_data.get(), 0) != 0)
    {
        return -1;
    }
    uint32_t depth = cast_chunk<uint32_t>(primary_key.data());
    return depth;
}

big_number leveldb_chain_keeper::end_slice_difficulty(size_t slice_begin_index)
{
    big_number total_work;
    Dbc* cursor;
    //db_blocks_->cursor(txn_->get(), &cursor, 0);
    // Our key/value pair
    readable_data_type key;
    key.set(slice_begin_index);
    writable_data_type_ptr value;
    // Position cursor
    value = std::make_shared<writable_data_type>();
    if (cursor->get(key.get(), value->get(), DB_SET) != 0)
        return 0;
    do
    {
        std::stringstream ss;
        data_chunk raw_object = value->data();
        std::copy(raw_object.begin(), raw_object.end(),
            std::ostream_iterator<byte>(ss));
        protobuf::Block proto_block;
        proto_block.ParseFromIstream(&ss);
        total_work += block_work(proto_block.bits());
        value = std::make_shared<writable_data_type>();
    }
    while (cursor->get(key.get(), value->get(), DB_NEXT) == 0);
    return total_work;
}

bool leveldb_chain_keeper::end_slice(size_t slice_begin_index,
    block_detail_list& sliced_blocks)
{
    Dbc* cursor;
    //db_blocks_->cursor(txn_->get(), &cursor, 0);
    readable_data_type key;
    key.set(slice_begin_index);
    writable_data_type_ptr value;
    // Position cursor
    value = std::make_shared<writable_data_type>();
    if (cursor->get(key.get(), value->get(), DB_SET) != 0)
        return true;
    do
    {
        // Read raw value from leveldb
        data_chunk raw_object = value->data();
        // Construct protobuf header from stringstream
        std::stringstream ss;
        std::copy(raw_object.begin(), raw_object.end(),
            std::ostream_iterator<byte>(ss));
        protobuf::Block proto_block;
        proto_block.ParseFromIstream(&ss);
        // Convert protobuf block header into actual block
        message::block sliced_block;
        if (!common_->reconstruct_block(proto_block, sliced_block))
            return false;
        // Add to list of sliced blocks
        block_detail_ptr sliced_detail =
            std::make_shared<block_detail>(sliced_block);
        sliced_blocks.push_back(sliced_detail);
        // Delete current item
        if (cursor->del(0) != 0)
            return false;
        // Remove txs + spends + addresses too
        for (const message::transaction& block_tx: sliced_block.transactions)
            if (!clear_transaction_data(block_tx))
                return false;
        // New value object ready to read next block
        value = std::make_shared<writable_data_type>();
    }
    while (cursor->get(key.get(), value->get(), DB_NEXT) == 0);
    return true;
}

bool leveldb_chain_keeper::clear_transaction_data(
    const message::transaction& remove_tx)
{
    const hash_digest& tx_hash = hash_transaction(remove_tx);
    readable_data_type del_tx_key;
    del_tx_key.set(tx_hash);
    //if (db_txs_->del(txn_->get(), del_tx_key.get(), 0) != 0)
        return false;
    // Remove spends
    // ... spends don't exist for coinbase txs.
    if (!is_coinbase(remove_tx))
        for (uint32_t input_index = 0; input_index < remove_tx.inputs.size();
            ++input_index)
        {
            const message::transaction_input& input = 
                remove_tx.inputs[input_index];
            const message::input_point inpoint{tx_hash, input_index};
            if (!remove_spend(input.previous_output, inpoint))
                return false;
        }
    // Remove addresses
    for (uint32_t output_index = 0; output_index < remove_tx.outputs.size();
        ++output_index)
    {
        const message::transaction_output& output =
            remove_tx.outputs[output_index];
        if (!remove_address(output.output_script, {tx_hash, output_index}))
            return false;
    }
    return true;
}

bool leveldb_chain_keeper::remove_spend(
    const message::output_point& previous_output,
    const message::input_point& current_input)
{
    readable_data_type spent_key;
    spent_key.set(create_spent_key(previous_output));
    //int ret = db_spends_->del(txn_->get(), spent_key.get(), 0);
    int ret = 0;
    if (ret != 0 && ret != DB_NOTFOUND)
        return false;
    return true;
}

bool leveldb_chain_keeper::remove_address(const script& output_script,
    const message::output_point& outpoint)
{
    data_chunk raw_address = create_address_key(output_script);
    if (raw_address.empty())
        return true;
    readable_data_type address_key, output_value;
    address_key.set(raw_address);
    output_value.set(create_spent_key(outpoint));
    // Perform the actual delete
    Dbc* cursor;
    //db_address_->cursor(txn_->get(), &cursor, 0);
    BITCOIN_ASSERT(cursor != nullptr);
    if (cursor->get(address_key.get(), output_value.get(), DB_GET_BOTH) != 0)
        return false;
    if (cursor->del(0) != 0)
        return false;
    cursor->close();
    return true;
}

} // namespace libbitcoin

