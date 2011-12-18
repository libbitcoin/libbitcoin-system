#ifndef LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_BLOCKCHAIN_H
#define LIBBITCOIN_BLOCKCHAIN_BERKELEYDB_BLOCKCHAIN_H

#include <bitcoin/blockchain/blockchain.hpp>

#include <bitcoin/blockchain/organizer.hpp>
#include <bitcoin/util/threads.hpp>

class Db;
class DbEnv;

namespace libbitcoin {

class bdb_common;
typedef std::shared_ptr<bdb_common> bdb_common_ptr;

class bdb_blockchain
 : public blockchain, public threaded_service,
    public std::enable_shared_from_this<bdb_blockchain>
{
public:
    bdb_blockchain(const std::string& prefix);
    ~bdb_blockchain();

    static bool setup(const std::string& prefix);

    void store(const message::block& stored_block,
        store_block_handler handle_store);

    void fetch_block(size_t depth, fetch_handler_block handle_fetch);
    void fetch_block(const hash_digest& block_hash,
        fetch_handler_block handle_fetch);
    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch);

private:
    bdb_blockchain();
    bool initialize(const std::string& prefix);

    void do_store(const message::block& store_block,
        store_block_handler handle_store);

    void fetch_block_by_depth(size_t depth, 
        fetch_handler_block handle_fetch);
    void fetch_block_by_hash(const hash_digest& block_hash, 
        fetch_handler_block handle_fetch);
    void do_fetch_block_locator(fetch_handler_block_locator handle_fetch);

    DbEnv* env_;
    Db* db_blocks_;
    Db* db_blocks_hash_;
    Db* db_txs_;
    Db* db_txs_hash_;

    bdb_common_ptr common_;

    // Organize stuff
    orphans_pool_ptr orphans_;
    chain_keeper_ptr chain_;
    organizer_ptr organize_;
};

} // libbitcoin

#endif

