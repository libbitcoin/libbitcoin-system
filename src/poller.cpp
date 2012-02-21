#include <bitcoin/poller.hpp>

#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

poller::poller(blockchain_ptr chain)
  : chain_(chain)
{
}

void poller::query(channel_ptr node)
{
    chain_->fetch_block_locator(
        std::bind(&poller::initial_ask_blocks,
            shared_from_this(), _1, _2, node));
}

void poller::initial_ask_blocks(const std::error_code& ec,
    const message::block_locator& locator, channel_ptr node)
{
    if (ec)
        log_error(log_domain::poller)
            << "Fetching initial block locator: " << ec.message();
}

} // libbitcoin

