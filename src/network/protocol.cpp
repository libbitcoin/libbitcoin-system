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
#include <bitcoin/bitcoin/network/protocol.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/seeder.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::filesystem::path;
using boost::format;
using boost::posix_time::time_duration;
using boost::posix_time::seconds;

// These are not configurable.
static const size_t sweep_connection_limit = 10;
static const auto sweep_reset_interval = seconds(1);

protocol::protocol(threadpool& pool, hosts& peers, handshake& shake,
    network& net, const hosts::list& seeds, uint16_t port, size_t max_outbound,
    size_t max_inbound)
  : strand_(pool),
    host_pool_(peers),
    handshake_(shake),
    network_(net),
    inbound_port_(port),
    max_inbound_(max_inbound),
    max_outbound_(max_outbound),
    sweep_timer_(pool.service()),
    sweep_count_(0),
    channel_subscribe_(std::make_shared<channel_subscriber_type>(pool)),
    seeds_(seeds)
{
}

void protocol::start(completion_handler handle_complete)
{
    // handshake.start doesn't accept an error code so we prevent its
    // execution in case of start failure, using this lambda wrapper.
    const auto start_handshake = [this, handle_complete]
        (const std::error_code& ec)
    {
        if (ec)
        {
            handle_complete(ec);
            return;
        }

        strand_.wrap(&handshake::start,
            &handshake_, handle_complete)();
    };

    const auto run_protocol =
        strand_.wrap(&protocol::handle_start,
            this, _1, start_handshake);
    
    host_pool_.load(
        strand_.wrap(&protocol::fetch_count,
            this, _1, run_protocol));
}

void protocol::handle_start(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure fetching height: " << ec.message();
        handle_complete(ec);
        return;
    }

    // TODO: handle run startup failure.
    handle_complete(ec);
    run();
}

void protocol::stop(completion_handler handle_complete)
{
    host_pool_.save(
        strand_.wrap(&protocol::handle_stop,
            this, _1, handle_complete));
}

void protocol::handle_stop(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure saving hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }

    channel_subscribe_->relay(error::service_stopped, nullptr);
    handle_complete(error::success);
}

void protocol::fetch_count(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure loading hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }

    host_pool_.fetch_count(
        strand_.wrap(&protocol::start_seeds,
            this, _1, _2, handle_complete));
}

void protocol::start_seeds(const std::error_code& ec, size_t hosts_count,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure checking existing hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }

    if (hosts_count == 0)
    {
        seeder_ = std::make_shared<seeder>(this, seeds_, handle_complete);
        seeder_->start();
        return;
    }

    handle_complete(error::success); 
}

std::string protocol::state_to_string(connect_state state) const
{
    switch (state)
    {
        case connect_state::finding_peer:
            return "finding peer";
        case connect_state::connecting:
            return "connecting to peer";
        case connect_state::established:
            return "established connection";
        case connect_state::stopped:
            return "stopped";
    }

    // Unhandled state!
    BITCOIN_ASSERT(false);
    return "";
}

void protocol::modify_slot(slot_index slot, connect_state state)
{
    connect_states_[slot] = state;
    log_debug(LOG_PROTOCOL)
        << "Outbound connection on slot ["
        << slot << "] " << state_to_string(state) << ".";
}

void protocol::run()
{
    strand_.queue(
        std::bind(&protocol::start_connecting,
            this));

    // Not accepting connections.
    if (inbound_port_ == 0 || max_inbound_ == 0)
        return;

    // Unhandled startup failure condition.
    network_.listen(inbound_port_,
        strand_.wrap(&protocol::handle_listen,
            this, _1, _2));
}

void protocol::start_connecting()
{
    // Initialize the connection slots.
    BITCOIN_ASSERT(outbound_connections_.empty());
    BITCOIN_ASSERT(connect_states_.empty());
    connect_states_.resize(max_outbound_);

    for (size_t slot = 0; slot < connect_states_.size(); ++slot)
        modify_slot(slot, connect_state::stopped);

    // Start the main outbound connect loop.
    start_stopped_connects();
    start_sweep_reset_timer();
}

void protocol::start_stopped_connects()
{
    for (size_t slot = 0; slot < connect_states_.size(); ++slot)
        if (connect_states_[slot] == connect_state::stopped)
            try_connect_once(slot);
}

void protocol::try_connect_once(slot_index slot)
{
    ++sweep_count_;
    if (sweep_count_ > sweep_connection_limit)
        return;

    BITCOIN_ASSERT(slot <= connect_states_.size());
    BITCOIN_ASSERT(connect_states_[slot] == connect_state::stopped);

    // Begin connection flow: finding_peer -> connecting -> established.
    // Failures end with connect_state::stopped and loop back here again.
    modify_slot(slot, connect_state::finding_peer);
    host_pool_.fetch_address(
        strand_.wrap(&protocol::attempt_connect, 
            this, _1, _2, slot));
}

void protocol::start_sweep_reset_timer()
{
    // This timer just loops continuously at fixed intervals
    // resetting the sweep_count_ variable and starting stopped slots.
    const auto reset_sweep = [this](const boost::system::error_code& ec)
    {
        if (ec)
        {
            if (ec != boost::asio::error::operation_aborted)
                log_error(LOG_PROTOCOL)
                    << "Failure resetting sweep timer: " << ec.message();

            BITCOIN_ASSERT(ec == boost::asio::error::operation_aborted);
            return;
        }

        if (sweep_count_ > sweep_connection_limit)
            log_debug(LOG_PROTOCOL)
                << "Resuming connection attempts.";

        // Perform the reset, reallowing connection attempts.
        sweep_count_ = 0;
        start_stopped_connects();

        // Looping timer...
        start_sweep_reset_timer();
    };

    sweep_timer_.expires_from_now(sweep_reset_interval);
    sweep_timer_.async_wait(strand_.wrap(reset_sweep, _1));
}

// Determine if the address is banned.
bool protocol::is_banned(const config::authority& address)
{
    for (const auto& banned: banned_connections_)
    {
        if ((banned.port == 0 || banned.port == address.port) &&
            banned.host == address.host)
            return true;
    }

    return false;
}

// Determine if the connection is manual.
bool protocol::is_manual(channel_ptr node)
{
    auto it = std::find(manual_connections_.begin(), manual_connections_.end(), node);
    return it != manual_connections_.end();
}

// Determine if we are connected to the address for any reason.
bool protocol::is_connected(const config::authority& address)
{
    for (const auto node: outbound_connections_)
        if (node->address() == address)
            return true;

    for (const auto node: inbound_connections_)
        if (node->address() == address)
            return true;

    for (const auto node: manual_connections_)
        if (node->address() == address)
            return true;

    return false;
}

void protocol::attempt_connect(const std::error_code& ec,
    const config::authority& peer, slot_index slot)
{
    BITCOIN_ASSERT(connect_states_[slot] == connect_state::finding_peer);
    modify_slot(slot, connect_state::connecting);

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure randomly selecting a peer address for slot ["
            << slot << "] " << ec.message();
        return;
    }

    // It is possible for this guard to be violated by a race.
    if (is_connected(peer))
    {
        log_debug(LOG_PROTOCOL)
            << "Already connected to selected peer [" 
            << peer.to_string() << "]";

        // Retry another connection, still in same strand.
        modify_slot(slot, connect_state::stopped);
        try_connect_once(slot);
        return;
    }

    if (is_banned(peer))
    {
        log_debug(LOG_PROTOCOL)
            << "Selected banned peer [" << peer.to_string() << "]";

        // Retry another connection, still in same strand.
        modify_slot(slot, connect_state::stopped);
        try_connect_once(slot);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to peer [" << peer.to_string()
        << "] on slot [" << slot << "]";

    // OUTBOUND CONNECT WITH TIMEOUT
    bc::network::connect(handshake_, network_, peer.host, peer.port,
        strand_.wrap(&protocol::handle_connect,
            this, _1, _2, peer, slot));
}

void protocol::handle_connect(const std::error_code& ec, channel_ptr node,
    const config::authority& peer, slot_index slot)
{
    BITCOIN_ASSERT(connect_states_[slot] == connect_state::connecting);

    if (ec || !node)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting to peer ["
            << peer.to_string() << "] on slot [" << slot << "] "
            << ec.message();

        // Retry another connection, still in same strand.
        modify_slot(slot, connect_state::stopped);
        try_connect_once(slot);
        return;
    }

    modify_slot(slot, connect_state::established);
    BITCOIN_ASSERT(outbound_connections_.size() <= max_outbound_);
    outbound_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connected to peer ["
        << peer.to_string() << "] on slot [" << slot << "] ("
        << outbound_connections_.size() << " total)";

    // Remove channel from list of connections.
    node->subscribe_stop(
        strand_.wrap(&protocol::outbound_channel_stopped,
            this, _1, node, slot));

    setup_new_channel(node);
}

void protocol::ban_connection(const std::string& hostname, uint16_t port)
{
    banned_connections_.push_back({ hostname, port });
}

void protocol::maintain_connection(const std::string& hostname, uint16_t port)
{
    // MANUAL CONNECT WITH TIMEOUT
    bc::network::connect(handshake_, network_, hostname, port,
        strand_.wrap(&protocol::handle_manual_connect,
            this, _1, _2, hostname, port));
}

void protocol::handle_manual_connect(const std::error_code& ec,
    channel_ptr node, const std::string& hostname, uint16_t port)
{
    if (ec || !node)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting manually to peer [" 
            << config::authority(hostname, port).to_string() << "] "
            << ec.message();

        // Retry connect.
        maintain_connection(hostname, port);
        return;
    }

    manual_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connection to peer established manually ["
        << config::authority(hostname, port).to_string() << "]";

    // Subscript to channel stop notifications.
    node->subscribe_stop(
        strand_.wrap(&protocol::manual_channel_stopped,
            this, _1, node, hostname, port));

    setup_new_channel(node);
}

void protocol::handle_listen(const std::error_code& ec, acceptor_ptr accept)
{
    if (!accept)
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Error while starting listener: " << ec.message();
        return;
    }

    // ACCEPT INCOMING CONNECTIONS (NO TIMEOUT)
    accept->accept(
        strand_.wrap(&protocol::handle_accept,
            this, _1, _2, accept));
}

void protocol::handle_accept(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
{
    if (!accept)
        return;

    // Relisten for connections.
    accept->accept(
        strand_.wrap(&protocol::handle_accept,
            this, _1, _2, accept));

    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Failure accepting connection from [" 
                << node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Failure accepting connection: " << ec.message();

        return;
    }

    // TODO: we would prefer to limit using slots and stop the listener.
    if (inbound_connections_.size() >= max_inbound_)
    {
        log_info(LOG_PROTOCOL)
            << "Inbound connection limit blocked ["
            << node->address().to_string() << "]";

        node->stop();
        return;
    }

    if (is_banned(node->address()))
    {
        log_info(LOG_PROTOCOL)
            << "Blocked banned connection from ["
            << node->address().to_string() << "]";

        node->stop();
        return;
    }

    inbound_connections_.push_back(node);

    // Accepted!
    log_info(LOG_PROTOCOL)
        << "Accepted connection from [" << node->address().to_string() << "] ("
        << inbound_connections_.size() << " total)";

    const auto handshake_complete = [this, node](const std::error_code& ec)
    {
        if (!node)
            return;

        if (ec)
        {
            log_debug(LOG_PROTOCOL) << "Failure in handshake from ["
                << node->address().to_string() << "] " << ec.message();
            return;
        }

        setup_new_channel(node);
    };

    // The handshake may not complete until the caller disconnects.
    // So this registration cannot happen in the completion handler.
    // Otherwise the termination filaure would prevent the subscription.
    node->subscribe_stop(
        strand_.wrap(&protocol::inbound_channel_stopped,
            this, _1, node));

    handshake_.ready(node, handshake_complete);
}

void protocol::setup_new_channel(channel_ptr node)
{
    if (!node)
        return;

    const auto handle_send = [node](const std::error_code& ec)
    {
        if (!node)
            return;

        if (ec)
        {
            log_debug(LOG_PROTOCOL) << "Send error ["
                << node->address().to_string() << "] " << ec.message();
        }
    };
    
    // Subscribe to address messages.
    node->subscribe_address(
        strand_.wrap(&protocol::handle_address_message,
            this, _1, _2, node));

    node->send(get_address_type(), handle_send);

    // Notify subscribers
    channel_subscribe_->relay(error::success, node);
}

void protocol::remove_connection(channel_ptr_list& connections,
    channel_ptr node)
{
    auto it = std::find(connections.begin(), connections.end(), node);
    BITCOIN_ASSERT(it != connections.end());
    connections.erase(it);
}

void protocol::outbound_channel_stopped(const std::error_code& ec,
    channel_ptr node, slot_index slot)
{
    // We must always attempt a reconnection.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Outbound channel stopped (outbound) ["
                << node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Outbound channel stopped (outbound): " << ec.message();
    }

    // Erase this channel from our list and then attempt a reconnection.
    remove_connection(outbound_connections_, node);

    BITCOIN_ASSERT(connect_states_[slot] == connect_state::established);
    modify_slot(slot, connect_state::stopped);

    // Attempt reconnection.
    try_connect_once(slot);
}

void protocol::manual_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& hostname, uint16_t port)
{
    // We must always attempt a reconnection.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Manual channel stopped (manual) [" 
                << config::authority(hostname, port).to_string() << "] "
                << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Manual channel stopped (manual): " << ec.message();
    }

    // Remove from accepted connections.
    // Timeout logic would go here if we ever need it.
    remove_connection(manual_connections_, node);

    // Attempt reconnection.
    maintain_connection(hostname, port);
}

void protocol::inbound_channel_stopped(const std::error_code& ec,
    channel_ptr node)
{
    // We do not attempt to reconnect inbound connections.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Inbound channel stopped (inbound) ["
                << node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Inbound channel stopped (inbound): " << ec.message();
    }

    // Remove from accepted connections (no reconnect).
    remove_connection(inbound_connections_, node);
}

void protocol::handle_address_message(const std::error_code& ec,
    const address_type& packet, channel_ptr node)
{
    if (!node)
        return;

    if (ec)
    {
        // TODO: reset the connection.
        log_debug(LOG_PROTOCOL)
            << "Failure getting addresses from ["
            << node->address().to_string() << "] " << ec.message();
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from [" << node->address().to_string() << "]";

    for (const auto& net_address: packet.addresses)
        host_pool_.store(net_address,
            strand_.wrap(&protocol::handle_store_address,
                this, _1));

    // Subscribe to address messages.
    node->subscribe_address(
        strand_.wrap(&protocol::handle_address_message,
            this, _1, _2, node));
}

void protocol::handle_store_address(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL) << "Failed to store address: "
            << ec.message();
}

// Deprecated, unreasonable to queue this, use total_connections.
void protocol::fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    strand_.queue(
        std::bind(&protocol::do_fetch_connection_count,
            this, handle_fetch));
}

// Deprecated, unreasonable to queue this, use total_connections.
void protocol::do_fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    handle_fetch(error::success, outbound_connections_.size());
}

void protocol::subscribe_channel(channel_handler handle_channel)
{
    channel_subscribe_->subscribe(handle_channel);
}

size_t protocol::total_connections() const
{
    return outbound_connections_.size() + manual_connections_.size() +
        inbound_connections_.size();
}

void protocol::set_max_outbound(size_t max_outbound)
{
    max_outbound_ = max_outbound;
}

void protocol::set_hosts_filename(const std::string& hosts_path)
{
    host_pool_.file_path_ = hosts_path;
}

// Deprecated, this is problematic because there is no enabler.
void protocol::disable_listener()
{
    inbound_port_ = 0;
}

// Deprecated, should be private.
void protocol::bootstrap(completion_handler handle_complete)
{
}

} // namespace network
} // namespace libbitcoin
