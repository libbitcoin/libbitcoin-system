#ifndef LIBBITCOIN_BLOCKCHAIN_LEVELDB_CHAIN_KEEPER_H
#define LIBBITCOIN_BLOCKCHAIN_LEVELDB_CHAIN_KEEPER_H

#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/blockchain/leveldb_blockchain.hpp>

#include "leveldb_common.hpp"

namespace libbitcoin {

class leveldb_chain_keeper
  : public chain_keeper
{
public:
    leveldb_chain_keeper(leveldb_common_ptr common,
        leveldb::DB* db_blocks, leveldb::DB* db_blocks_hash,
        leveldb::DB* db_txs, leveldb::DB* db_spends, leveldb::DB* db_address);

    void start();
    void stop();

    void add(block_detail_ptr incoming_block);
    int find_index(const hash_digest& search_block_hash);
    big_number end_slice_difficulty(size_t slice_begin_index);
    bool end_slice(size_t slice_begin_index,
        block_detail_list& sliced_blocks);

private:
    bool clear_transaction_data(leveldb_transaction_batch& batch,
        const transaction_type& remove_tx);
    bool remove_address(leveldb::WriteBatch& batch,
        const script& output_script, const output_point& outpoint);

    leveldb_common_ptr common_;

    leveldb::DB* db_blocks_;
    leveldb::DB* db_blocks_hash_;
    leveldb::DB* db_txs_;
    leveldb::DB* db_spends_;
    leveldb::DB* db_address_;
};

typedef std::shared_ptr<leveldb_chain_keeper> leveldb_chain_keeper_ptr;

} // namespace libbitcoin

#endif

