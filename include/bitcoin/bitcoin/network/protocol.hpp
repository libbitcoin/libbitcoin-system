/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// TODO: rationalize include guard names (conflict with libbitcoin-protocol).
#ifndef BC_PROTOCOL_HPP
#define BC_PROTOCOL_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/peer.hpp>
#include <bitcoin/bitcoin/network/seeder.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API protocol
{
public:
    typedef std::function<void (const std::error_code&)>
        completion_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        fetch_connection_count_handler;
    typedef std::function<void (const std::error_code&, channel_ptr)>
        channel_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        broadcast_handler;

    protocol(threadpool& pool, hosts& hosts, handshake& shake, peer& network,
        uint16_t port=bc::protocol_port, bool relay=true,
        size_t max_outbound=8, size_t max_inbound=8, 
        const config::endpoint::list& seeds=seeder::defaults);
    
    /// This class is not copyable.
    protocol(const protocol&) = delete;
    void operator=(const protocol&) = delete;

    /**
     * Perform full initialization sequence.
     * Internally calls bootstrap() and then run().
     * @param[in]  handle_complete  Completion handler for start operation.
     */
    void start(completion_handler handle_complete);

    /**
     * Gracefully close down.
     * @param[in]  handle_complete  Completion handler for start operation.
     */
    void stop(completion_handler handle_complete);

    /**
     * Add a banned connection.
     * @param[in]  peer  The peer to ban.
     */
    void ban_connection(const config::authority& peer);

    /**
     * Create a persistent connection to the specific node. If disconnected
     * this service will keep attempting to reconnect until successful.
     * @param[in]  address  The host address to maintain.
     * @param[in]  relay    Relay transactions (without a bloom filter).
     * @param[in]  retries  Retry connection this many times (zero forever).
     */
    void maintain_connection(const std::string& hostname, uint16_t port,
        bool relay=true, size_t retries=0);

    /**
     * Subscribe to new connections established to other nodes.
     * This method must be called again to stay subscribed as
     * handlers are deregistered after being called.
     * When this protocol service is stopped, any subscribed handlers
     * will be called with the error_code set to error::service_stopped.
     * @param[in]  handle_channel  Handler for new connection.
     */
    void subscribe_channel(channel_handler handle_channel);

    /**
     * Return the number of active connections.
     * The summation is not thread safe. Intended for diagnostics only.
     */
    size_t total_connections() const;

    /**
     * Broadcast a message to all nodes in our connection list.
     * @param[in]   packet      Message packet to broadcast
     * @param[in]   handle_send Called after every send operation.
     */
    template <typename Message>
    void broadcast(const Message& packet, broadcast_handler handle_send)
    {
        // The intermediate variable 'lambda' is a workaround for a
        // limitation of the MSVC++ CTP_Nov2013 generic lambda support.
        const auto lambda = &protocol::do_broadcast<Message>;
        strand_.queue(lambda, this, packet, handle_send);
    }

    /// Deprecated, should be private since it's called from start.
    void bootstrap(completion_handler handle_complete);

    /// Deprecated, set on construct or use accessors.
    void disable_listener();

    /// Deprecated, unreasonable to queue this, use total_connections.
    void fetch_connection_count(fetch_connection_count_handler handle_fetch);

    /// Deprecated, should be private since it's called from start.
    void run() {}

    /// Deprecated, construct hosts with path.
    void set_hosts_filename(const std::string& hosts_path);

    /// Deprecated, set on construct.
    void set_max_outbound(size_t max_outbound);

private:
    typedef std::vector<channel_ptr> channel_ptr_list;
    typedef subscriber<const std::error_code&, channel_ptr> channel_subscriber;

    // Startup sequence
    void handle_hosts_load(const std::error_code& ec,
        completion_handler handle_complete);
    void handle_hosts_count(const std::error_code& ec,
        size_t hosts_count, completion_handler handle_complete);
    void handle_seeder_start(const std::error_code& ec,
        completion_handler handle_complete);
    void handle_hosts_save(const std::error_code& ec,
        completion_handler handle_complete);
    void handle_address_message(const std::error_code& ec,
        const address_type& message, channel_ptr node);
    void handle_store_address(const std::error_code& ec);

    // Start outbound and accepting inbound connections
    void start_connecting(completion_handler handle_complete, bool relay);
    void handle_handshake(const std::error_code& ec, channel_ptr node);

    // Inbound connections
    void accept_connections(bool relay);
    void start_accept(const std::error_code& ec, acceptor_ptr accept,
        bool relay);
    void handle_accept(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept, bool relay);

    // Outbound connections
    void new_connection(bool relay);
    void start_connect(const std::error_code& ec,
        const config::authority& peer, bool relay);
    void handle_connect(const std::error_code& ec, channel_ptr node,
        const config::authority& peer, bool relay);

    // Manual connections
    void handle_manual_connect(const std::error_code& ec, channel_ptr node,
        const std::string& hostname, uint16_t port, bool relay,
        size_t retries);
    void retry_manual_connection(const config::endpoint& address,
        bool relay, size_t retries);

    // Remove channels from lists when disconnected.
    void inbound_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname);
    void outbound_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname, bool relay);
    void manual_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname, bool relay,
        size_t retries);

    // Channel setup
    bool is_banned(const config::authority& peer) const;
    bool is_connected(const config::authority& peer) const;
    bool is_loopback(channel_ptr node) const;
    void notify_stop();
    void remove_connection(channel_ptr_list& connections, channel_ptr node);
    void setup_new_channel(channel_ptr node);

    /// Deprecated, unreasonable to queue this, use total_connections.
    void do_fetch_connection_count(
        fetch_connection_count_handler handle_fetch);

    template <typename Message>
    void do_broadcast(const Message& packet, broadcast_handler handle_send)
    {
        const auto total_nodes = total_connections();
        const auto send_handler =
            std::bind(handle_send,
                std::placeholders::_1, total_nodes);

        for (const auto node: outbound_connections_)
            node->send(packet, send_handler);

        for (const auto node: manual_connections_)
            node->send(packet, send_handler);

        for (const auto node: inbound_connections_)
            node->send(packet, send_handler);
    }
    
    async_strand strand_;
    hosts& host_pool_;
    handshake& handshake_;
    peer& network_;
    seeder seeder_;
    channel_subscriber::ptr channel_subscriber_;

    // Manual connections created via configuration or user input.
    channel_ptr_list manual_connections_;
    config::authority::list banned_connections_;

    // Inbound connections from the p2p network.
    uint16_t inbound_port_;
    size_t max_inbound_;
    channel_ptr_list inbound_connections_;

    // There's a fixed number of connections that are always trying to reconnect.
    size_t max_outbound_;
    channel_ptr_list outbound_connections_;

    bool relay_inbound_and_outbound_;
    boost::filesystem::path hosts_path_;
};

} // namespace network
} // namespace libbitcoin

#endif

