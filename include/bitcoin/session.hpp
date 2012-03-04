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

class session
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;

    session();
    void start(completion_handler handle_complete);

private:
    void download_blockchain(channel_ptr node);
    void new_channel(channel_ptr node);

    void inventory(const std::error_code& ec,
        const message::inventory& packet, channel_ptr node);
    void get_data(const std::error_code& ec,
        const message::get_data& packet, channel_ptr node);
    void get_blocks(const std::error_code& ec,
        const message::get_blocks& packet, channel_ptr node);

    async_service network_service_;

    hosts_ptr hosts_;
    handshake_ptr handshake_;
    network_ptr network_;
    protocol_ptr protocol_;

    async_service disk_service_;

    blockchain_ptr chain_;
    poller_ptr poll_;

    async_service mempool_service_;

    transaction_pool_ptr tx_pool_;
};

} // libbitcoin

#endif

