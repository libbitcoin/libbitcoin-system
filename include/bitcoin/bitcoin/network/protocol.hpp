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
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/initiator.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/network/seeder.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API protocol
{
public:
    typedef std::function<void(const std::error_code&)>
        completion_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        fetch_connection_count_handler;
    typedef std::function<void (const std::error_code&, channel_ptr)>
        channel_handler;
    typedef std::function<void (const std::error_code&, channel_ptr)>
        broadcast_handler;

    protocol(threadpool& pool, hosts& hosts, initiator& network,
        uint16_t port=bc::protocol_port, bool relay=true,
        size_t max_outbound=8, size_t max_inbound=8,
        const config::endpoint::list& seeds=seeder::defaults,
        const network_address_type& self=bc::unspecified_network_address,
        const timeout& timeouts=timeout::defaults);
    
    /// This class is not copyable.
    protocol(const protocol&) = delete;
    void operator=(const protocol&) = delete;

    void start(completion_handler handle_complete);
    void stop(completion_handler handle_complete);
    void blacklist(const config::authority& peer);
    void subscribe_channel(channel_handler handle_channel);
    void maintain_connection(const std::string& hostname, uint16_t port,
        bool relay=true, size_t retries=0);

    size_t connection_count() const;

    template <typename Message>
    void broadcast(const Message& packet, broadcast_handler handle_send)
    {
        dispatch_.queue(
            &protocol::do_broadcast<Message>,
                this, packet, handle_send);
    }

private:
    typedef std::vector<channel_ptr> channel_ptr_list;
    typedef subscriber<const std::error_code&, channel_ptr> channel_subscriber;
    typedef channel_subscriber::ptr channel_subscriber_ptr;

    // Common to all connection types.
    void handle_handshake(const std::error_code& ec, channel_ptr node);
    void start_talking(channel_ptr node,
        channel_proxy::stop_handler handle_stop, bool relay);
    void remove_connection(channel_ptr_list& connections, channel_ptr node);

    // Inbound connections.
    void start_accepting();
    void start_accept(const std::error_code& ec, acceptor_ptr accept);
    void handle_accept(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept);

    // Outbound connections.
    void new_connection();
    void start_seeding(completion_handler handle_complete);
    void start_connecting(const std::error_code& ec,
        completion_handler handle_complete);
    void start_connect(const std::error_code& ec,
        const config::authority& peer);
    void handle_connect(const std::error_code& ec, channel_ptr node,
        const config::authority& peer);

    // Manual connections.
    void handle_manual_connect(const std::error_code& ec, channel_ptr node,
        const std::string& hostname, uint16_t port, bool relay, size_t retry);
    void retry_manual_connection(const config::endpoint& address,
        bool relay, size_t retry);

    // Remove channels from lists when disconnected.
    void inbound_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname);
    void outbound_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname);
    void manual_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname, bool relay,
        size_t retries);

    // Channel metadata.
    bool is_blacklisted(const config::authority& peer) const;
    bool is_connected(const config::authority& peer) const;
    bool is_loopback(channel_ptr node) const;

    template <typename Message>
    void do_broadcast(const Message& packet, broadcast_handler handle_send)
    {
        for (const auto node: outbound_connections_)
            node->send(packet,
                [=](const std::error_code& ec){ handle_send(ec, node); });

        for (const auto node: manual_connections_)
            node->send(packet,
                [=](const std::error_code& ec){ handle_send(ec, node); });

        for (const auto node: inbound_connections_)
            node->send(packet,
                [=](const std::error_code& ec){ handle_send(ec, node); });
    }

    dispatcher dispatch_;
    threadpool& pool_;
    hosts& hosts_;
    initiator& network_;
    protocol_version::ptr handshake_;
    channel_subscriber_ptr channel_subscriber_;

    // Configuration.
    const config::endpoint::list& seeds_;
    const network_address_type& self_;
    const timeout& timeouts_;
    uint16_t inbound_port_;
    size_t max_inbound_;
    size_t max_outbound_;
    bool relay_;

    channel_ptr_list manual_connections_;
    channel_ptr_list inbound_connections_;
    channel_ptr_list outbound_connections_;

    config::authority::list blacklisted_;
};

} // namespace network
} // namespace libbitcoin

#endif

