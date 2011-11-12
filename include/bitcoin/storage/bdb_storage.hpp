#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_STORAGE_H

#include <bitcoin/storage/storage.hpp>

#include DB_CXX_HEADER

#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

class bdb_storage
 : public storage,
    public threaded_service,
    public std::enable_shared_from_this<bdb_storage>
{
public:
    bdb_storage();

    void store(const message::block& block, store_block_handler handle_store);

    void fetch_block_locator(fetch_handler_block_locator handle_fetch);
    void fetch_balance(const short_hash& pubkey_hash,
        fetch_handler_balance handle_fetch);
};

} // libbitcoin

#endif

