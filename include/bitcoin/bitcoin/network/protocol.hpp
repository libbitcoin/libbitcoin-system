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

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class seeder;

class BC_API protocol
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        fetch_connection_count_handler;
    typedef std::function<void (const std::error_code&, channel_ptr)>
        channel_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        broadcast_handler;

    static const size_t default_max_outbound;
    static const hosts::authority_list default_seeds;

    protocol(threadpool& pool, hosts& peers, handshake& shake, network& net,
        const hosts::authority_list& seeds = default_seeds,
        uint16_t port=bc::protocol_port,
        size_t max_outbound=default_max_outbound);
    
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
     * Fetch number of connections maintained by this service.
     * @param[in]  handle_fetch  Completion handler for fetch operation.
     */
    void fetch_connection_count(
        fetch_connection_count_handler handle_fetch);

    /**
     * Create a manual connection to a specific node. If disconnected
     * this service will keep attempting to reconnect until successful.
     */
    void maintain_connection(const std::string& hostname, uint16_t port);

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
     * Not threadsafe. Intended only for diagnostics information.
     */
    size_t total_connections() const;

    /**
     * Determine if the listener is enabled.
     */
    bool get_listening();

    /**
     * Enable or disable the listener.
     */
    void set_listening(bool value=true);

    /**
     * Broadcast a message to all nodes in our connection list.
     * @param[in]   packet      Message packet to broadcast
     * @param[in]   handle_send Called after every send operation.
     */
    template <typename Message>
    void broadcast(const Message& packet, broadcast_handler handle_send)
    {
        // The intermediate variable 'lambda' is a workaround for a
        // limitation of the VC++ CTP_Nov2013 generic lambda support.
        const auto lambda = &protocol::do_broadcast<Message>;
        strand_.queue(lambda, this, packet, handle_send);
    }
    
    /// Deprecated, should be private since it's called from start.
    void bootstrap(completion_handler handle_complete);

    /// Deprecated, should be private since it's called from start.
    void run();

    /// Deprecated, set on construct.
    void set_max_outbound(size_t max_outbound);

    /// Deprecated, construct hosts with path.
    void set_hosts_filename(const std::string& hosts_path);

    /// Deprecated, set on construct or use accessors.
    void disable_listener();

private:
    struct connection_info
    {
        network_address_type address;
        channel_ptr node;
    };

    enum class connect_state
    {
        finding_peer,
        connecting,
        established,
        stopped
    };

    typedef size_t slot_index;
    typedef std::vector<channel_ptr> channel_ptr_list;
    typedef std::vector<connect_state> connect_state_list;
    typedef std::vector<connection_info> connection_list;
    typedef subscriber<const std::error_code&, channel_ptr>
        channel_subscriber_type;

    // start sequence
    void handle_start(const std::error_code& ec,
        completion_handler handle_complete);
    void fetch_count(const std::error_code& ec,
        completion_handler handle_complete);
    void start_seeder(const std::error_code& ec, size_t hosts_count,
        completion_handler handle_complete);

    // stop sequence
    void handle_stop(const std::error_code& ec,
        completion_handler handle_complete);

    std::string state_to_string(connect_state state) const;
    void modify_slot(slot_index slot, connect_state state);

    // run loop
    void start_connecting();

    // Connect outwards
    void start_stopped_connects();

    // This function is called in these places:
    //
    // 1. try_outbound_connects() calls it n times.
    // Called by run() at the start.
    // 2. If we fetch a random node address that we are already
    // connected to in attempt_connect().
    // 3. If we fail to connect to said address in handle_connect().
    // 4. If the channel is stopped manually or there is an error
    // (such as a disconnect). See setup_new_channel() for the
    // subscribe call.
    void try_connect_once(slot_index slot);
    void attempt_connect(const std::error_code& ec,
        const network_address_type& packet, slot_index slot);
    void handle_connect(const std::error_code& ec, channel_ptr node,
        const network_address_type& address, slot_index slot);

    // Periodically call this method to reset the watermark and reallow
    // connections. This prevents too many connection attempts from
    // exhausting resources by putting a limit on connection attempts
    // within a certain time interval.
    void start_watermark_reset_timer();

    // Manual connections
    void handle_manual_connect(const std::error_code& ec, channel_ptr node,
        const std::string& hostname, uint16_t port);

    // Accept inwards connections
    void handle_listen(const std::error_code& ec, acceptor_ptr accept);
    void handle_accept(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept);

    // Channel setup
    void setup_new_channel(channel_ptr node);

    // Remove channels from lists when disconnected.
    void outbound_channel_stopped(const std::error_code& ec,
        channel_ptr node, slot_index slot);
    void manual_channel_stopped(const std::error_code& ec,
        channel_ptr node, const std::string& hostname, uint16_t port);
    void inbound_channel_stopped(const std::error_code& ec,
        channel_ptr node);

    void handle_address_message(const std::error_code& ec,
        const address_type& addr, channel_ptr node);
    void handle_store_address(const std::error_code& ec);

    // fetch methods
    void do_fetch_connection_count(
        fetch_connection_count_handler handle_fetch);

    template <typename Message>
    void do_broadcast(const Message& packet, broadcast_handler handle_send)
    {
        const auto total_nodes = total_connections();
        const auto send_handler =
            std::bind(handle_send, std::placeholders::_1, total_nodes);

        for (const auto& connection: connections_)
            connection.node->send(packet, send_handler);

        for (const auto node: manual_connections_)
            node->send(packet, send_handler);

        for (const auto node: accepted_channels_)
            node->send(packet, send_handler);
    }
    
    async_strand strand_;
    hosts& host_pool_;
    handshake& handshake_;
    network& network_;

    // There's a fixed number of slots that are always trying to reconnect.
    size_t max_outbound_;
    connection_list connections_;

    // Simply a debugging tool to enforce correct state transition behaviour
    // for maintaining connections.
    connect_state_list connect_states_;

    // Used to prevent too many connection attempts from exhausting resources.
    // The watermark is refreshed every interval.
    boost::asio::deadline_timer watermark_timer_;
    size_t watermark_count_;

    // Manual connections created via configuration or user input.
    channel_ptr_list manual_connections_;

    // Inbound connections from the p2p network.
    uint16_t listen_port_;
    channel_ptr_list accepted_channels_;
    channel_subscriber_type::ptr channel_subscribe_;

    boost::filesystem::path hosts_path_;
    const hosts::authority_list& seeds_;
    std::shared_ptr<seeder> seeder_;
    friend class seeder;
};

} // namespace network
} // namespace libbitcoin

#endif

