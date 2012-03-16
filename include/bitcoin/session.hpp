#ifndef LIBBITCOIN_SESSION_H
#define LIBBITCOIN_SESSION_H

#include <bitcoin/network/hosts.hpp>
#include <bitcoin/network/handshake.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/protocol.hpp>
#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/poller.hpp>
#include <bitcoin/transaction_pool.hpp>

namespace libbitcoin {

struct session_params
{
    hosts_ptr hosts;
    handshake_ptr handshake;
    network_ptr network;
    protocol_ptr protocol;
    blockchain_ptr blockchain;
    poller_ptr poller;
    transaction_pool_ptr transaction_pool;
};

class session
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;

    session(const session_params& params);
    void start(completion_handler handle_complete);
    void stop(completion_handler handle_complete);

private:
    void download_blockchain(channel_ptr node);
    void new_channel(channel_ptr node);
    void set_start_depth(const std::error_code& ec, size_t fork_point,
        const blockchain::block_list& new_blocks,
        const blockchain::block_list& replaced_blocks);

    void inventory(const std::error_code& ec,
        const message::inventory& packet, channel_ptr node);
    void get_data(const std::error_code& ec,
        const message::get_data& packet, channel_ptr node);
    void get_blocks(const std::error_code& ec,
        const message::get_blocks& packet, channel_ptr node);

    void request_tx_data(bool tx_exists,
        const hash_digest& tx_hash, channel_ptr node);

    hosts_ptr hosts_;
    handshake_ptr handshake_;
    network_ptr network_;
    protocol_ptr protocol_;

    blockchain_ptr chain_;
    poller_ptr poll_;

    transaction_pool_ptr tx_pool_;
};

} // libbitcoin

#endif

