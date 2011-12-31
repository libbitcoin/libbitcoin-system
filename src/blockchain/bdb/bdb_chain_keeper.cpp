#include "bdb_chain_keeper.hpp"

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/data_helpers.hpp>

#include "bdb_common.hpp"
#include "data_type.hpp"
#include "protobuf_wrapper.hpp"

namespace libbitcoin {

bdb_chain_keeper::bdb_chain_keeper(bdb_common_ptr common, DbEnv* env,
    Db* db_blocks, Db* db_blocks_hash)
  : common_(common), env_(env), db_blocks_(db_blocks),
    db_blocks_hash_(db_blocks_hash)
{
}

void bdb_chain_keeper::start()
{
    txn_ = std::make_shared<txn_guard>(env_);
}
void bdb_chain_keeper::stop()
{
    txn_->commit();
    txn_.reset();
}

void bdb_chain_keeper::add(block_detail_ptr incoming_block)
{
    uint32_t last_block_depth = common_->find_last_block_depth(txn_);
    const message::block& actual_block = incoming_block->actual();
    if (!common_->save_block(txn_, last_block_depth + 1, actual_block))
        log_fatal() << "Saving block in organizer failed";
}

int bdb_chain_keeper::find_index(const hash_digest& search_block_hash)
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

big_number bdb_chain_keeper::end_slice_difficulty(size_t slice_begin_index)
{
    big_number total_work;
    Dbc* cursor;
    db_blocks_->cursor(txn_->get(), &cursor, 0);
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

blocks_list bdb_chain_keeper::end_slice(size_t slice_begin_index)
{
    blocks_list sliced_blocks;
    Dbc* cursor;
    db_blocks_->cursor(txn_->get(), &cursor, 0);
    readable_data_type key;
    key.set(slice_begin_index);
    writable_data_type_ptr value;
    // Position cursor
    value = std::make_shared<writable_data_type>();
    if (cursor->get(key.get(), value->get(), DB_SET) != 0)
        return blocks_list();
    do
    {
        // Read raw value from bdb
        data_chunk raw_object = value->data();
        // Construct protobuf header from stringstream
        std::stringstream ss;
        std::copy(raw_object.begin(), raw_object.end(),
            std::ostream_iterator<byte>(ss));
        protobuf::Block proto_block;
        proto_block.ParseFromIstream(&ss);
        // Convert protobuf block header into actual block
        message::block sliced_block;
        if (!common_->reconstruct_block(txn_, proto_block, sliced_block))
            return blocks_list();
        // Add to list of sliced blocks
        block_detail_ptr sliced_detail =
            std::make_shared<block_detail>(sliced_block);
        sliced_blocks.push_back(sliced_detail);
        // Delete current item
        if (cursor->del(0) != 0)
            return blocks_list();
        // TODO should remove txs + spends too
        value = std::make_shared<writable_data_type>();
    }
    while (cursor->get(key.get(), value->get(), DB_NEXT) == 0);
    return sliced_blocks;
}

txn_guard_ptr bdb_chain_keeper::txn()
{
    return txn_;
}

} // libbitcoin

