#include "leveldb_chain_keeper.hpp"

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>

#include "protobuf_wrapper.hpp"

namespace libbitcoin {

leveldb_chain_keeper::leveldb_chain_keeper(leveldb_common_ptr common,
    leveldb::DB* db_blocks, leveldb::DB* db_blocks_hash,
    leveldb::DB* db_txs, leveldb::DB* db_spends, leveldb::DB* db_address)
  : common_(common), db_blocks_(db_blocks), db_blocks_hash_(db_blocks_hash),
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
    const block_type& actual_block = incoming_block->actual();
    if (!common_->save_block(last_block_depth + 1, actual_block))
        log_fatal(LOG_BLOCKCHAIN) << "Saving block in organizer failed";
}

int leveldb_chain_keeper::find_index(const hash_digest& search_block_hash)
{
    uint32_t depth = common_->fetch_block_depth(search_block_hash);
    if (depth == std::numeric_limits<uint32_t>::max())
        return -1;
    return depth;
}

big_number leveldb_chain_keeper::end_slice_difficulty(size_t slice_begin_index)
{
    big_number total_work = 0;
    leveldb_iterator it(db_blocks_->NewIterator(leveldb::ReadOptions()));
    data_chunk raw_depth = uncast_type(slice_begin_index);
    for (it->Seek(slice(raw_depth)); it->Valid(); it->Next())
    {
        std::stringstream ss;
        ss.str(it->value().ToString());
        protobuf::Block proto_block;
        proto_block.ParseFromIstream(&ss);
        total_work += block_work(proto_block.bits());
    }
    return total_work;
}

bool reconstruct_block(leveldb_common_ptr common,
    const protobuf::Block& proto_block_header,
    block_type& result_block)
{
    result_block = protobuf_to_block_header(proto_block_header);
    for (const std::string& raw_tx_hash: proto_block_header.transactions())
    {
        // Convert protobuf hash string into internal hash format.
        hash_digest tx_hash;
        BITCOIN_ASSERT(raw_tx_hash.size() == tx_hash.max_size());
        std::copy(raw_tx_hash.begin(), raw_tx_hash.end(), tx_hash.begin());
        // Fetch the actual transaction.
        protobuf::Transaction proto_tx =
            common->fetch_proto_transaction(tx_hash);
        if (!proto_tx.IsInitialized())
            return false;
        result_block.transactions.push_back(protobuf_to_transaction(proto_tx));
    }
    return true;
}

bool leveldb_chain_keeper::end_slice(size_t slice_begin_index,
    block_detail_list& sliced_blocks)
{
    leveldb::WriteBatch blk_batch, blk_hash_batch;
    leveldb_transaction_batch tx_batch;
    leveldb_iterator it(db_blocks_->NewIterator(leveldb::ReadOptions()));
    data_chunk raw_depth = uncast_type(slice_begin_index);
    for (it->Seek(slice(raw_depth)); it->Valid(); it->Next())
    {
        std::stringstream ss;
        ss.str(it->value().ToString());
        protobuf::Block proto_block;
        proto_block.ParseFromIstream(&ss);
        // Convert protobuf block header into actual block
        block_type sliced_block;
        if (!reconstruct_block(common_, proto_block, sliced_block))
            return false;
        // Add to list of sliced blocks
        block_detail_ptr sliced_detail =
            std::make_shared<block_detail>(sliced_block);
        sliced_blocks.push_back(sliced_detail);
        // Make sure to delete hash secondary index too.
        hash_digest block_hash = hash_block_header(sliced_block);
        // Delete block header...
        blk_batch.Delete(it->key());
        // And it's secondary index.
        blk_hash_batch.Delete(slice_block_hash(block_hash));
        // Remove txs + spends + addresses too
        for (const transaction_type& block_tx: sliced_block.transactions)
            if (!clear_transaction_data(tx_batch, block_tx))
                return false;
    }
    leveldb::WriteOptions options;
    // Execute batches.
    db_blocks_->Write(options, &blk_batch);
    db_blocks_hash_->Write(options, &blk_hash_batch);
    db_txs_->Write(options, &tx_batch.tx_batch);
    db_spends_->Write(options, &tx_batch.spends_batch);
    db_address_->Write(options, &tx_batch.address_batch);
    return true;
}

bool leveldb_chain_keeper::clear_transaction_data(
    leveldb_transaction_batch& batch, const transaction_type& remove_tx)
{
    const hash_digest& tx_hash = hash_transaction(remove_tx);
    batch.tx_batch.Delete(slice(tx_hash));
    // Remove spends
    // ... spends don't exist for coinbase txs.
    if (!is_coinbase(remove_tx))
        for (uint32_t input_index = 0; input_index < remove_tx.inputs.size();
            ++input_index)
        {
            const transaction_input_type& input =
                remove_tx.inputs[input_index];
            // We could check if the spend matches the inpoint for safety.
            //const input_point inpoint{tx_hash, input_index};
            // Recreate the key...
            data_chunk spent_key = create_spent_key(input.previous_output);
            // ... Perform the delete.
            batch.spends_batch.Delete(slice(spent_key));
        }
    // Remove addresses
    for (uint32_t output_index = 0; output_index < remove_tx.outputs.size();
        ++output_index)
    {
        const transaction_output_type& output =
            remove_tx.outputs[output_index];
        if (!remove_address(batch.address_batch,
                output.output_script, {tx_hash, output_index}))
            return false;
    }
    return true;
}

bool leveldb_chain_keeper::remove_address(leveldb::WriteBatch& batch,
    const script& output_script, const output_point& outpoint)
{
    data_chunk raw_address = create_address_key(output_script);
    if (raw_address.empty())
        return true;
    data_chunk outpoint_value = create_spent_key(outpoint);
    bool is_found = false;
    leveldb_iterator it(address_iterator(db_address_, raw_address));
    for (; valid_address_iterator(it, raw_address); it->Next())
    {
        if (slice_to_output_point(it->value()) != outpoint)
            continue;
        // We found the address entry we were looking for.
        BITCOIN_ASSERT(!is_found);
        is_found = true;
        // Put changes into batch finally.
        batch.Delete(it->key());
        break;
    }
    BITCOIN_ASSERT(it->status().ok());
    if (!is_found)
        return false;
    return true;
}

} // namespace libbitcoin

