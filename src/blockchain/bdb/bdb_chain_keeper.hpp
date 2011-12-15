#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_CHAIN_KEEPER_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_CHAIN_KEEPER_H

#include <bitcoin/blockchain/organizer.hpp>

#include DB_CXX_HEADER

#include <bitcoin/blockchain/bdb_blockchain.hpp>

#include "txn_guard.hpp"

namespace libbitcoin {

class bdb_chain_keeper
 : public chain_keeper
{
public:
    bdb_chain_keeper(bdb_common_ptr common, DbEnv* env,
        Db* db_blocks, Db* db_blocks_hash);

    void start();
    void stop();

    void add(block_detail_ptr incoming_block);
    int find_index(const hash_digest& search_block_hash);
    big_number end_slice_difficulty(size_t slice_begin_index);
    blocks_list end_slice(size_t slice_begin_index);
private:
    txn_guard_ptr txn_;

    bdb_common_ptr common_;

    DbEnv* env_;
    Db* db_blocks_;
    Db* db_blocks_hash_;
};

} // libbitcoin

#endif

