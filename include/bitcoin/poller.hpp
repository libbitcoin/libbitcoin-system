#ifndef LIBBITCOIN_BLOCKS_POLLER_H
#define LIBBITCOIN_BLOCKS_POLLER_H

#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class poller
  : public std::enable_shared_from_this<poller>
{
public:
    poller(blockchain_ptr chain);
    void query(channel_ptr node);

private:
    void initial_ask_blocks(const std::error_code& ec,
        const message::block_locator& locator, channel_ptr node);

    void receive_inv(const std::error_code& ec,
        const message::inventory& packet, channel_ptr node);
    void receive_block(const std::error_code& ec,
        const message::block& blk, channel_ptr node);

    void handle_store(const std::error_code& ec, block_info info,
        const hash_digest& block_hash, channel_ptr node);
    void ask_blocks(const std::error_code& ec,
        const message::block_locator& locator,
        const hash_digest& hash_stop, channel_ptr node);

    blockchain_ptr chain_;
};

} // libbitcoin

#endif

