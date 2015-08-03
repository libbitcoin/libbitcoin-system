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
#include <thread>
#include <vector>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
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

protocol::protocol(threadpool& pool, hosts& hosts, handshake& shake,
    network& net, const config::endpoint::list& seeds, uint16_t port,
    size_t max_outbound, size_t max_inbound)
  : strand_(pool),
    host_pool_(hosts),
    handshake_(shake),
    network_(net),
    seeder_(pool, hosts, shake, net, seeds),
    channel_subscriber_(pool),
    inbound_port_(port),
    max_inbound_(max_inbound),
    max_outbound_(max_outbound)
{
}

void protocol::stop(completion_handler handle_complete)
{
    host_pool_.save(
        strand_.wrap(&protocol::handle_hosts_save,
            this, _1, handle_complete));
}

void protocol::handle_hosts_save(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure saving hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }

    channel_subscriber_.relay(error::service_stopped, nullptr);
    handle_complete(error::success);
}

void protocol::start(completion_handler handle_complete)
{
    if (max_outbound_ == 0)
    {
        // Skip load/seed hosts if we aren't connecting outbound.
        start_connecting(handle_complete);
        return;
    }

    host_pool_.load(
        strand_.wrap(&protocol::handle_hosts_load,
            this, _1, handle_complete));
}

void protocol::handle_hosts_load(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure loading hosts: " << ec.message();
        handle_complete(ec);
        return;
    }

    host_pool_.fetch_count(
        strand_.wrap(&protocol::handle_hosts_count,
            this, _1, _2, handle_complete));
}

void protocol::handle_hosts_count(const std::error_code& ec,
    size_t hosts_count, completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure getting hosts count: " << ec.message();
        handle_complete(ec);
        return;
    }

    if (hosts_count > 0)
    {
        // Bypass seeding if we have a non-empty hosts cache.
        start_connecting(handle_complete);
        return;
    }

    seeder_.start(
        strand_.wrap(&protocol::handle_seeder_start,
            this, _1, handle_complete));
}

void protocol::handle_seeder_start(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failed to seed hosts: " << ec.message();
        handle_complete(ec);
        return;
    }

    start_connecting(handle_complete);
}

void protocol::start_connecting(completion_handler handle_complete)
{
    // Start inbound connection listener.
    if (inbound_port_ > 0 && max_inbound_ > 0)
        accept_connections();

    handle_complete(error::success);

    // Start outbound connection attempts.
    for (auto channel = 0; channel < max_outbound_; ++channel)
        new_connection();
}

void protocol::accept_connections()
{
    network_.listen(inbound_port_,
        strand_.wrap(&protocol::start_accept,
            this, _1, _2));
}

void protocol::new_connection()
{
    // Throttle outbound connections to a rate of 10 per second.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    host_pool_.fetch_address(
        strand_.wrap(&protocol::start_connect,
            this, _1, _2));
}

void protocol::start_connect(const std::error_code& ec,
    const config::authority& peer)
{
    if (ec)
    {
        handle_connect(ec, nullptr, peer);
        return;
    }

    if (is_connected(peer))
    {
        handle_connect(error::address_in_use, nullptr, peer);
        return;
    }

    if (is_banned(peer))
    {
        handle_connect(error::address_blocked, nullptr, peer);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to peer [" << peer.to_string() << "]";

    // OUTBOUND CONNECT WITH TIMEOUT
    const auto relay = true;
    bc::network::connect(handshake_, network_, peer.to_hostname(), peer.port(),
        strand_.wrap(&protocol::handle_connect,
            this, _1, _2, peer), relay);
}

void protocol::handle_connect(const std::error_code& ec, channel_ptr node,
    const config::authority& peer)
{
    if (ec || !node)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting [" << peer << "] " << ec.message();

        // Restart connection attempt.
        new_connection();
        return;
    }

    outbound_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connected to peer [" << peer.to_string() << "] (" 
        << outbound_connections_.size() << " total)";

    // Subscribe to remove channel from list of connections when it stops.
    node->subscribe_stop(
        strand_.wrap(&protocol::outbound_channel_stopped,
            this, _1, node, peer.to_string()));

    setup_new_channel(node);
}

void protocol::ban_connection(const config::authority& peer)
{
    banned_connections_.push_back(peer);
}

void protocol::maintain_connection(const config::endpoint& address, bool relay)
{
    maintain_connection(address.host(), address.port(), relay);
}

// Deprecated.
void protocol::maintain_connection(const std::string& hostname, uint16_t port,
    bool relay)
{
    // MANUAL CONNECT WITH TIMEOUT
    bc::network::connect(handshake_, network_, hostname, port,
        strand_.wrap(&protocol::handle_manual_connect,
            this, _1, _2, hostname, port, relay));
}

void protocol::handle_manual_connect(const std::error_code& ec,
    channel_ptr node, const std::string& hostname, uint16_t port, bool relay)
{
    const config::endpoint peer(hostname, port);

    if (ec || !node)
    {
        // Warn because we are supposed to maintain this connection.
        log_warning(LOG_PROTOCOL)
            << "Failure connecting [" << peer << "] manually: "
            << ec.message();

        // Retry connection.
        maintain_connection(hostname, port, relay);
        return;
    }

    manual_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connected to peer [" << peer << "] manually";

    // Subscribe to channel stop notifications.
    node->subscribe_stop(
        strand_.wrap(&protocol::manual_channel_stopped,
            this, _1, node, peer.to_string(), relay));

    setup_new_channel(node);
}

void protocol::start_accept(const std::error_code& ec, acceptor_ptr accept)
{
    if (ec || !accept)
    {
        log_error(LOG_PROTOCOL)
            << "Error starting listener: " << ec.message();
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
    // Relisten for connections.
    start_accept(ec, accept);

    if (ec || !node)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Failure accepting connection from [" 
                << node->address() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Failure accepting connection: " << ec.message();

        return;
    }

    if (inbound_connections_.size() >= max_inbound_)
    {
        node->stop(error::connection_limit);
        return;
    }

    const auto address = node->address();

    if (is_banned(address))
    {
        node->stop(error::address_blocked);
        return;
    }

    if (is_loopback(node))
    {
        node->stop(error::connection_to_self);
        return;
    }

    inbound_connections_.push_back(node);

    // Accepted!
    log_info(LOG_PROTOCOL)
        << "Accepted connection from [" << address << "] ("
        << inbound_connections_.size() << " total)";

    const auto handshake_complete = [this, node](const std::error_code& ec)
    {
        if (!node)
            return;

        if (ec)
        {
            log_debug(LOG_PROTOCOL) << "Failure in handshake from ["
                << node->address() << "] " << ec.message();
            return;
        }

        setup_new_channel(node);
    };

    // The handshake may not complete until the caller disconnects.
    // So this registration cannot happen in the completion handler.
    // Otherwise the termination failure would prevent the subscription.
    node->subscribe_stop(
        strand_.wrap(&protocol::inbound_channel_stopped,
            this, _1, node, address.to_string()));

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
            log_debug(LOG_PROTOCOL)
                << "Send error [" << node->address() << "] " << ec.message();
        }
    };

    // Don't ask for or subscribe to addresses if host pool is zero-sized.
    if (host_pool_.size() > 0)
    {
        // Subscribe to address messages.
        node->subscribe_address(
            strand_.wrap(&protocol::handle_address_message,
                this, _1, _2, node));

        // Ask for addresses.
        node->send(get_address_type(), handle_send);
    }

    // Notify subscribers
    channel_subscriber_.relay(error::success, node);
}

void protocol::remove_connection(channel_ptr_list& connections,
    channel_ptr node)
{
    auto it = std::find(connections.begin(), connections.end(), node);
    if (it != connections.end())
        connections.erase(it);
}

void protocol::outbound_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Channel stopped (outbound) [" << address << "] "
            << ec.message();
    }

    // We always create a replacement oubound connection.
    remove_connection(outbound_connections_, node);
    new_connection();
}

void protocol::manual_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address, bool relay)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Channel stopped (manual) [" << address << "] "
            << ec.message();
    }

    // We always attempt to reconnect manual connections.
    remove_connection(manual_connections_, node);
    maintain_connection(config::endpoint(address), relay);
}

void protocol::inbound_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Channel stopped (inbound) [" << address << "] "
            << ec.message();
    }

    // We never attempt to reconnect inbound connections.
    remove_connection(inbound_connections_, node);
}

void protocol::handle_address_message(const std::error_code& ec,
    const address_type& message, channel_ptr node)
{
    if (!node)
        return;

    if (ec)
    {
        // TODO: reset the connection.
        log_debug(LOG_PROTOCOL)
            << "Failure getting addresses from ["
            << node->address() << "] " << ec.message();
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from [" << node->address() << "]";

    for (const auto& net_address: message.addresses)
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

void protocol::subscribe_channel(channel_handler handle_channel)
{
    channel_subscriber_.subscribe(handle_channel);
}

size_t protocol::total_connections() const
{
    return outbound_connections_.size() + manual_connections_.size() +
        inbound_connections_.size();
}

// Determine if the address is banned.
bool protocol::is_banned(const config::authority& peer) const
{
    const auto& banned = banned_connections_;
    const auto predicate = [&peer](const config::authority& node)
    {
        return (node.port() == 0 || node.port() == peer.port()) &&
            (node.ip() == peer.ip());
    };
    auto it = std::find_if(banned.begin(), banned.end(), predicate);
    return it != banned.end();
    return true;
}

// Determine if we are connected to the address for any reason.
bool protocol::is_connected(const config::authority& peer) const
{
    // TODO: add connection_type to node so we only need one connection pool.
    const auto& inn = inbound_connections_;
    const auto& out = outbound_connections_;
    const auto& man = manual_connections_;
    const auto predicate = [&peer](const channel_ptr& node)
    {
        return (node->address() == peer);
    };
    return
        (std::find_if(inn.begin(), inn.end(), predicate) != inn.end()) ||
        (std::find_if(out.begin(), out.end(), predicate) != out.end()) ||
        (std::find_if(man.begin(), man.end(), predicate) != man.end());
}

// Determine if connection matches the nonce of one of our own outbounds.
bool protocol::is_loopback(channel_ptr peer) const
{
    const auto& outbound = outbound_connections_;
    const auto peer_nonce = peer->nonce();
    const auto predicate = [peer, peer_nonce](const channel_ptr& node)
    {
        return (node != peer) && (node->nonce() == peer_nonce);
    };
    auto it = std::find_if(outbound.begin(), outbound.end(), predicate);
    return it != outbound.end();
    return true;
}

void protocol::set_max_outbound(size_t max_outbound)
{
    max_outbound_ = max_outbound;
}

void protocol::set_hosts_filename(const std::string& hosts_path)
{
    host_pool_.file_path_ = hosts_path;
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
