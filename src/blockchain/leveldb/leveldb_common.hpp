#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_COMMON_H
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_COMMON_H

#include <memory>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include <bitcoin/format.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/utility/serializer.hpp>

namespace libbitcoin {

struct leveldb_block_info
{
    block_header_type header;
    hash_digest_list tx_hashes;
};
struct leveldb_tx_info
{
    // The info that connects it to block parent.
    size_t depth, index;
    transaction_type tx;
};

typedef std::unique_ptr<leveldb::Iterator> leveldb_iterator;

struct leveldb_transaction_batch
{
    leveldb::WriteBatch tx_batch, spends_batch, address_batch;
};

struct leveldb_databases
{
    leveldb::DB* block;
    leveldb::DB* block_hash;
    leveldb::DB* tx;
    leveldb::DB* spend;
    leveldb::DB* addr;
};

class leveldb_common
{
public:
    leveldb_common(leveldb_databases db);

    uint32_t find_last_block_depth();
    bool fetch_spend(
        const output_point& spent_output,
        input_point& input_spend);

    bool save_block(uint32_t depth, const block_type& serial_block);

    uint32_t get_block_depth(const hash_digest& block_hash);
    bool get_block(leveldb_block_info& blk_info,
        uint32_t depth, bool read_header, bool read_tx_hashes);
    // Used by chain_keeper when iterating through blocks.
    // get_block isn't sufficient by itself.
    bool deserialize_block(leveldb_block_info& blk_info,
        const std::string& raw_data, bool read_header, bool read_tx_hashes);
    bool get_transaction(leveldb_tx_info& tx_info,
        const hash_digest& tx_hash, bool read_parent, bool read_tx);

private:
    bool save_transaction(leveldb_transaction_batch& batch,
        uint32_t block_depth, uint32_t tx_index,
        const hash_digest& tx_hash, const transaction_type& block_tx);
    bool duplicate_exists(const hash_digest& tx_hash,
        uint32_t block_depth, uint32_t tx_index);
    bool mark_spent_outputs(leveldb::WriteBatch& spends_batch,
        const output_point& previous_output,
        const input_point& current_input);
    // returns false only on database failure. It may or may not add an entry
    bool add_address(leveldb::WriteBatch& address_batch,
        const script& output_script,
        const output_point& outpoint);

    leveldb_databases db_;
};

typedef std::shared_ptr<leveldb_common> leveldb_common_ptr;

template <typename Data>
leveldb::Slice slice(const Data& data)
{
    return leveldb::Slice(
        reinterpret_cast<const char*>(data.data()), data.size());
}

template <typename Data>
uint32_t recreate_depth(const Data& raw_data)
{
    const uint8_t* start = reinterpret_cast<const uint8_t*>(raw_data.data());
    const uint8_t* end = start + raw_data.size();
    return cast_chunk<uint32_t>(data_chunk(start, end));
}

leveldb::Slice slice_block_hash(const hash_digest& block_hash);

// Used also by leveldb_chain_keeper when deleting spends + addresses
template <typename Point>
data_chunk create_spent_key(const Point& point)
{
    data_chunk spent_key(hash_digest_size + 4);
    auto serial = make_serializer(spent_key.begin());
    serial.write_hash(point.hash);
    serial.write_4_bytes(point.index);
    return spent_key;
}

output_point slice_to_output_point(const leveldb::Slice& out_slice);

data_chunk create_address_key(const script& output_script);

leveldb::Iterator* address_iterator(leveldb::DB* db_address,
    const data_chunk& raw_address);
bool valid_address_iterator(leveldb_iterator& it,
    const data_chunk& raw_address);

} // namespace libbitcoin

#endif

