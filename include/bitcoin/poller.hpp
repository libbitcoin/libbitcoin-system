#ifndef LIBBITCOIN_BLOCKS_POLLER_HPP
#define LIBBITCOIN_BLOCKS_POLLER_HPP

#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class poller
{
public:
    poller(threadpool& pool, blockchain& chain);
    void query(channel_ptr node);
    void monitor(channel_ptr node);

private:
    void initial_ask_blocks(const std::error_code& ec,
        const block_locator_type& locator, channel_ptr node);

    void receive_inv(const std::error_code& ec,
        const inventory_type& packet, channel_ptr node);
    void receive_block(const std::error_code& ec,
        const block_type& blk, channel_ptr node);

    void handle_store(const std::error_code& ec, block_info info,
        const hash_digest& block_hash, channel_ptr node);
    void ask_blocks(const std::error_code& ec,
        const block_locator_type& locator,
        const hash_digest& hash_stop, channel_ptr node);

    io_service::strand strand_;
    blockchain& chain_;

    // Last hash from a block locator
    hash_digest last_hash_end_;
    // Last hash from an inventory packet
    hash_digest last_block_hash_;
};

} // namespace libbitcoin

#endif

