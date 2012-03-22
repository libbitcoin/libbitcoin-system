#include <bitcoin/blockchain/blockchain.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

typedef blockchain_fetch_handler_block handler_block;

// Chain of flow
void load_transactions(const std::error_code& ec,
    const message::block& blk, handler_block handle_fetch);

void fetch_block(blockchain_ptr chain, size_t depth,
    handler_block handle_fetch)
{
    chain->fetch_block_header(depth,
        std::bind(load_transactions, _1, _2, handle_fetch));
}

void fetch_block(blockchain_ptr chain, const hash_digest& block_hash,
    handler_block handle_fetch)
{
    chain->fetch_block_header(block_hash,
        std::bind(load_transactions, _1, _2, handle_fetch));
}

void load_transactions(const std::error_code& ec,
    const message::block& blk, handler_block handle_fetch)
{
}

typedef blockchain_fetch_handler_block_locator handler_locator;

void fetch_block_locator(blockchain_ptr chain, handler_locator handle_fetch)
{
}

} // namespace libbitcoin

