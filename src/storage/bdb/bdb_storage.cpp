#include <bitcoin/storage/bdb_storage.hpp>

namespace libbitcoin {

bdb_storage::bdb_storage()
{
}

void bdb_storage::store(const message::block& block, 
    store_block_handler handle_store)
{
}

void bdb_storage::fetch_block_locator(fetch_handler_block_locator handle_fetch)
{
}

void bdb_storage::fetch_balance(const short_hash& pubkey_hash,
    fetch_handler_balance handle_fetch)
{
}

} // libbitcoin

