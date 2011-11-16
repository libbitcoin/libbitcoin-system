#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_STORAGE_H

#include <bitcoin/storage/storage.hpp>

#include <db_cxx.h>

#include <bitcoin/util/threaded_service.hpp>

#include "bdb_detail.hpp"

namespace libbitcoin {

class bdb_storage
 : public storage, public threaded_service,
    public std::enable_shared_from_this<bdb_storage>
{
public:
    bdb_storage(const std::string& prefix);
    ~bdb_storage();

    static bool setup(const std::string& prefix);

    void store(const message::block& block, store_block_handler handle_store);

    void fetch_block(size_t depth, fetch_handler_block handle_fetch);
    void fetch_block(const hash_digest& block_hash,
        fetch_handler_block handle_fetch);
    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch);

private:
    bdb_storage();
    void initialize(const std::string& prefix);

    void fetch_block_by_depth(size_t depth, 
        fetch_handler_block handle_fetch);
    void fetch_block_by_hash(const hash_digest& block_hash, 
        fetch_handler_block handle_fetch);
    void do_fetch_block_locator(fetch_handler_block_locator handle_fetch);

    bool save_block(size_t depth, const message::block serial_block);
    uint32_t save_transaction(const message::transaction& block_tx);

    bdb_guard<DbEnv> env_;
    bdb_guard<Db> db_blocks_;
    bdb_guard<Db> db_blocks_hash_;
    bdb_guard<Db> db_txs_;
    bdb_guard<Db> db_txs_hash_;
};

} // libbitcoin

#endif

