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
        Db* db_blocks, Db* db_blocks_hash,
        Db* db_txs, Db* db_spends, Db* db_address);

    void start();
    void stop();

    void add(block_detail_ptr incoming_block);
    int find_index(const hash_digest& search_block_hash);
    big_number end_slice_difficulty(size_t slice_begin_index);
    block_detail_list end_slice(size_t slice_begin_index);

    txn_guard_ptr txn();

private:
    bool clear_transaction_data(const message::transaction& remove_tx);
    bool remove_spend(const message::output_point& previous_output,
        const message::input_point& current_input);
    bool remove_address(const script& output_script,
        const message::output_point& outpoint);

    txn_guard_ptr txn_;

    bdb_common_ptr common_;

    DbEnv* env_;
    Db* db_blocks_;
    Db* db_blocks_hash_;
    Db* db_txs_;
    Db* db_spends_;
    Db* db_address_;
};

typedef std::shared_ptr<bdb_chain_keeper> bdb_chain_keeper_ptr;

} // libbitcoin

#endif

