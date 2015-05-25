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
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
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

// Based on http://bitcoinstats.com/network/dns-servers
#ifdef ENABLE_TESTNET
const hosts::authority_list protocol::default_seeds =
{
    { "testnet-seed.alexykot.me", 18333 },
    { "testnet-seed.bitcoin.petertodd.org", 18333 },
    { "testnet-seed.bluematt.me", 18333 },
    { "testnet-seed.bitcoin.schildbach.de", 18333 }
};
#else
const hosts::authority_list protocol::default_seeds =
{
    { "seed.bitnodes.io", 8333 },
    { "seed.bitcoinstats.com", 8333 },
    { "seed.bitcoin.sipa.be", 8333 },
    { "dnsseed.bluematt.me", 8333 },
    { "seed.bitcoin.jonasschnelli.ch", 8333 },
    { "dnsseed.bitcoin.dashjr.org", 8333 }

    // Previously also included:
    // bitseed.xf2.org:8333
    // archivum.info:8333
    // progressbar.sk:8333
    // faucet.bitcoin.st:8333
    // bitcoin.securepayment.cc:8333
};
#endif

const size_t protocol::default_max_outbound = 8;

// TODO: parameterize for config access.
const size_t watermark_connection_limit = 10;
const time_duration watermark_reset_interval = seconds(1);

protocol::protocol(threadpool& pool, hosts& peers, handshake& shake,
    network& net, const hosts::authority_list& seeds, uint16_t port,
    size_t max_outbound)
  : strand_(pool),
    host_pool_(peers),
    handshake_(shake),
    network_(net),
    max_outbound_(max_outbound),
    watermark_timer_(pool.service()),
    watermark_count_(0),
    listen_port_(port),
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
        strand_.wrap(&protocol::start_seeder,
            this, _1, _2, handle_complete));
}

void protocol::start_seeder(const std::error_code& ec, size_t hosts_count,
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

    if (listen_port_ == 0)
        return;

    // Unhandled startup failure condition.
    network_.listen(listen_port_,
        strand_.wrap(&protocol::handle_listen,
            this, _1, _2));
}

void protocol::start_connecting()
{
    // Initialize the connection slots.
    BITCOIN_ASSERT(connections_.empty());
    BITCOIN_ASSERT(connect_states_.empty());
    connect_states_.resize(max_outbound_);

    for (size_t slot = 0; slot < connect_states_.size(); ++slot)
        modify_slot(slot, connect_state::stopped);

    // Start the main outbound connect loop.
    start_stopped_connects();
    start_watermark_reset_timer();
}

void protocol::start_stopped_connects()
{
    for (size_t slot = 0; slot < connect_states_.size(); ++slot)
        if (connect_states_[slot] == connect_state::stopped)
            try_connect_once(slot);
}

void protocol::try_connect_once(slot_index slot)
{
    ++watermark_count_;
    if (watermark_count_ > watermark_connection_limit)
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

void protocol::start_watermark_reset_timer()
{
    // This timer just loops continuously at fixed intervals
    // resetting the watermark_count_ variable and starting stopped slots.
    const auto reset_watermark = [this](const boost::system::error_code& ec)
    {
        if (ec)
        {
            if (ec != boost::asio::error::operation_aborted)
                log_error(LOG_PROTOCOL)
                    << "Failure resetting watermark timer: " << ec.message();

            BITCOIN_ASSERT(ec == boost::asio::error::operation_aborted);
            return;
        }

        if (watermark_count_ > watermark_connection_limit)
            log_debug(LOG_PROTOCOL)
                << "Resuming connection attempts.";

        // Perform the reset, reallowing connection attempts.
        watermark_count_ = 0;
        start_stopped_connects();

        // Looping timer...
        start_watermark_reset_timer();
    };

    watermark_timer_.expires_from_now(watermark_reset_interval);
    watermark_timer_.async_wait(strand_.wrap(reset_watermark, _1));
}

template <typename ConnectionList>
bool already_connected(const message::network_address& address,
    const ConnectionList& connections)
{
    for (const auto& connection: connections)
    {
        if (connection.address.ip == address.ip &&
            connection.address.port == address.port)
        {
            return true;
        }
    }

    return false;
}

void protocol::attempt_connect(const std::error_code& ec,
    const message::network_address& address, slot_index slot)
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

    if (already_connected(address, connections_))
    {
        log_debug(LOG_PROTOCOL)
            << "Already connected to selected peer [" 
            << authority(address).to_string() << "]";

        // Retry another connection, still in same strand.
        modify_slot(slot, connect_state::stopped);
        try_connect_once(slot);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to peer [" << authority(address).to_string()
        << "] on slot [" << slot << "]";

    const authority peer(address);
    connect(handshake_, network_, peer.host, peer.port,
        strand_.wrap(&protocol::handle_connect,
            this, _1, _2, address, slot));
}

void protocol::handle_connect(const std::error_code& ec, channel::pointer node,
    const message::network_address& address, slot_index slot)
{
    BITCOIN_ASSERT(connect_states_[slot] == connect_state::connecting);

    if (ec || !node)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting to peer ["
            << authority(address).to_string() << "] on slot [" << slot << "] "
            << ec.message();

        // Retry another connection, still in same strand.
        modify_slot(slot, connect_state::stopped);
        try_connect_once(slot);
        return;
    }

    modify_slot(slot, connect_state::established);
    BITCOIN_ASSERT(connections_.size() <= max_outbound_);
    connections_.push_back({address, node});

    log_info(LOG_PROTOCOL)
        << "Connected to peer ["
        << authority(address).to_string() << "] on slot [" << slot << "] ("
        << connections_.size() << " total)";

    // Remove channel from list of connections
    node->subscribe_stop(
        strand_.wrap(&protocol::outbound_channel_stopped,
            this, _1, node, slot));

    setup_new_channel(node);
}

void protocol::maintain_connection(const std::string& hostname, uint16_t port)
{
    connect(handshake_, network_, hostname, port,
        strand_.wrap(&protocol::handle_manual_connect,
            this, _1, _2, hostname, port));
}

void protocol::handle_manual_connect(const std::error_code& ec,
    channel::pointer node, const std::string& hostname, uint16_t port)
{
    if (ec || !node)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting manually to peer [" 
            << authority(hostname, port).to_string() << "] " << ec.message();

        // Retry connect.
        maintain_connection(hostname, port);
        return;
    }

    manual_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connection to peer established manually ["
        << authority(hostname, port).to_string() << "]";

    // Subscript to channel stop notifications.
    node->subscribe_stop(
        strand_.wrap(&protocol::manual_channel_stopped,
            this, _1, node, hostname, port));

    setup_new_channel(node);
}

void protocol::handle_listen(const std::error_code& ec,
    acceptor::pointer accept)
{
    if (!accept)
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Error while starting listener: " << ec.message();
        return;
    }

    // Listen for connections.
    accept->accept(
        strand_.wrap(&protocol::handle_accept,
            this, _1, _2, accept));
}

void protocol::handle_accept(const std::error_code& ec, channel::pointer node,
    acceptor::pointer accept)
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

    accepted_channels_.push_back(node);

    log_info(LOG_PROTOCOL)
        << "Accepted connection from [" << node->address().to_string() << "] ("
        << accepted_channels_.size() << " total)";

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

        // Remove channel from list of connections
        node->subscribe_stop(
            strand_.wrap(&protocol::inbound_channel_stopped,
                this, _1, node));

        setup_new_channel(node);
    };

    handshake_.ready(node, handshake_complete);
}

void protocol::setup_new_channel(channel::pointer node)
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

    node->send(message::get_address(), handle_send);

    // Notify subscribers
    channel_subscribe_->relay(error::success, node);
}

template <typename ConnectionsList>
void remove_connection(ConnectionsList& connections, channel::pointer node)
{
    auto it = connections.begin();
    for (; it != connections.end(); ++it)
        if (it->node == node)
            break;

    BITCOIN_ASSERT(it != connections.end());
    connections.erase(it);
}

void protocol::outbound_channel_stopped(const std::error_code& ec,
    channel::pointer node, slot_index slot)
{
    // We must always attempt a reconnection.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (outbound) ["
                << node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (outbound): " << ec.message();
    }

    // Erase this channel from our list and then attempt a reconnection.
    remove_connection(connections_, node);

    BITCOIN_ASSERT(connect_states_[slot] == connect_state::established);
    modify_slot(slot, connect_state::stopped);

    // Attempt reconnection.
    try_connect_once(slot);
}

template <typename ChannelsList>
void remove_channel(ChannelsList& channels, channel::pointer node)
{
    const auto it = std::find(channels.begin(), channels.end(), node);
    BITCOIN_ASSERT(it != channels.end());
    channels.erase(it);
}

void protocol::manual_channel_stopped(const std::error_code& ec,
    channel::pointer node, const std::string& hostname, uint16_t port)
{
    // We must always attempt a reconnection.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (manual) [" 
                << authority(hostname, port).to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (manual): " << ec.message();
    }

    // Remove from accepted connections.
    // Timeout logic would go here if we ever need it.
    remove_channel(manual_connections_, node);

    // Attempt reconnection.
    maintain_connection(hostname, port);
}

void protocol::inbound_channel_stopped(const std::error_code& ec,
    channel::pointer node)
{
    // We do not attempt to reconnect inbound connections.
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (inbound) ["
                << node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Channel stopped (inbound): " << ec.message();
    }

    // Remove from accepted connections (no reconnect).
    remove_channel(accepted_channels_, node);
}

void protocol::handle_address_message(const std::error_code& ec,
    const message::address& packet, channel::pointer node)
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

void protocol::fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    strand_.queue(
        std::bind(&protocol::do_fetch_connection_count,
            this, handle_fetch));
}

void protocol::do_fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    handle_fetch(error::success, connections_.size());
}

void protocol::subscribe_channel(channel_handler handle_channel)
{
    channel_subscribe_->subscribe(handle_channel);
}

size_t protocol::total_connections() const
{
    return connections_.size() + manual_connections_.size() +
        accepted_channels_.size();
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
    listen_port_ = 0;
}

// Deprecated, should be private.
void protocol::bootstrap(completion_handler handle_complete)
{
}

} // namespace network
} // namespace libbitcoin
