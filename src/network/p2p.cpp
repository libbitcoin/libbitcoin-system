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
#include <bitcoin/bitcoin/network/p2p.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/network/session_seed.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::p2p::channel_subscriber);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::filesystem::path;
using boost::format;
using boost::posix_time::time_duration;
using boost::posix_time::seconds;

const uint16_t p2p::mainnet = 8333;

p2p::p2p(threadpool& pool, hosts& hosts, connector& network,
    uint16_t port, bool relay, size_t max_outbound, size_t max_inbound,
    const config::endpoint::list& seeds, const config::authority& self,
    const timeout& timeouts)
  : dispatch_(pool),
    pool_(pool),
    hosts_(hosts),
    network_(network),
    channel_subscriber_(MAKE_SUBSCRIBER(channel, pool, LOG_NETWORK)),
    seeds_(seeds),
    self_(self),
    timeouts_(timeouts),
    relay_(relay),
    stopped_(true),
    inbound_port_(port),
    max_inbound_(max_inbound),
    max_outbound_(max_outbound)
{
}

// TODO: add seed connections to session_seed (i.e. connect once).
// TODO: add manual connections via config in session_manual.

void p2p::start(completion_handler handle_complete)
{
    stopped_ = false;

    // Start inbound connection listener if configured.
    if (inbound_port_ > 0 && max_inbound_ > 0)
        start_accepting();

    hosts_.load(
        std::bind(&p2p::handle_hosts_loaded,
            this, handle_complete));
}

// TODO: move seeding to node start.
void p2p::handle_hosts_loaded(completion_handler handle_complete)
{
    const auto need_seeding = (max_outbound_ > 0 && hosts_.size() == 0 &&
        hosts_.capacity() > 0 && !seeds_.empty());

    if (need_seeding)
        start_seeding(handle_complete);
    else
        start_connecting(error::success, handle_complete);
}

void p2p::stop(completion_handler handle_complete)
{
    stopped_ = true;

    // Stop protocol subscribers.
    channel_subscriber_->relay(error::service_stopped, nullptr);

    // Notify all channels to stop.
    for (const auto node: outbound_connections_)
        node->stop(error::service_stopped);

    for (const auto node: manual_connections_)
        node->stop(error::service_stopped);

    for (const auto node: inbound_connections_)
        node->stop(error::service_stopped);

    hosts_.save(handle_complete);
}

void p2p::start_seeding(completion_handler handle_complete)
{
    const auto complete =
        std::bind(&p2p::start_connecting,
            this, _1, handle_complete);

    // There is presently no way to get stop notification to this.
    // This will always call complete no later than implied by timeouts.
    std::make_shared<session_seed>(pool_, hosts_, timeouts_, network_, seeds_,
        self_.to_network_address())->start(complete);
}

// TODO: implement on session_outbound.

void p2p::start_connecting(const code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Error seeding hosts: " << ec.message();
        handle_complete(ec);
        return;
    }

    // Start outbound connection attempts (not concurrent because of strand).
    for (size_t channel = 0; channel < max_outbound_; ++channel)
        new_connection();

    handle_complete(error::success);
}

void p2p::new_connection()
{
    hosts_.fetch_address(
        dispatch_.concurrent_delegate(&p2p::start_connect,
            this, _1, _2));
}

void p2p::start_connect(const code& ec, const config::authority& peer)
{
    if (stopped_)
        return;

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

    if (is_blacklisted(peer))
    {
        handle_connect(error::address_blocked, nullptr, peer);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to peer [" << peer.to_string() << "]";

    // OUTBOUND CONNECT (sequential)
    // There is presently no way to get stop notification to this.
    // This will always call complete no later than implied by timeout.
    network_.connect(peer.to_hostname(), peer.port(),
        dispatch_.ordered_delegate(&p2p::handle_connect,
            this, _1, _2, peer));
}

void p2p::handle_connect(const code& ec, channel::ptr node,
    const config::authority& peer)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting [" << peer << "] " << ec.message();

        // Restart connection attempt.
        new_connection();
        return;
    }

    // Save the connection as we are now assured of getting stop event.
    outbound_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connected to peer [" << peer.to_string() << "] (" 
        << outbound_connections_.size() << " total)";

    const auto stop_handler =
        dispatch_.ordered_delegate(&p2p::outbound_channel_stopped,
            this, _1, node, peer.to_string());

    start_talking(node, stop_handler, relay_);
}

// TODO: implement on session_manual.

void p2p::retry_manual_connection(const config::endpoint& address,
    bool relay, size_t retry)
{
    const auto done = (retry == 1);
    if (done)
    {
        log_warning(LOG_PROTOCOL)
            << "Failure connecting [" << address << "] retries exceeded";
        return;
    }

    const auto forever = (retry == 0);
    const auto retries = forever ? 0 : retry - 1;
    maintain_connection(address.host(), address.port(), relay, retries);
}

void p2p::maintain_connection(const std::string& hostname, uint16_t port,
    bool relay, size_t retry)
{
    if (stopped_)
        return;

    // MANUAL CONNECT
    // There is presently no way to get stop notification to this.
    // This will always call complete no later than implied by timeout.
    network_.connect(hostname, port,
        dispatch_.ordered_delegate(&p2p::handle_manual_connect,
            this, _1, _2, hostname, port, relay, retry));
}

void p2p::handle_manual_connect(const code& ec,
    channel::ptr node, const std::string& hostname, uint16_t port, bool relay,
    size_t retries)
{
    const config::endpoint peer(hostname, port);

    if (ec)
    {
        // Warn because we are supposed to maintain this connection.
        log_warning(LOG_PROTOCOL)
            << "Failure connecting [" << peer << "] manually: "
            << ec.message();

        // Retry connection.
        const config::endpoint address(hostname, port);
        retry_manual_connection(address, relay, retries);
        return;
    }

    // Save the connection as we are now assured of getting a stop event.
    manual_connections_.push_back(node);

    // Connected!
    log_info(LOG_PROTOCOL)
        << "Connected to peer [" << peer << "] manually ("
        << manual_connections_.size() << " total)";

    const auto stop_handler =
        dispatch_.ordered_delegate(&p2p::manual_channel_stopped,
            this, _1, node, peer.to_string(), relay, retries);

    start_talking(node, stop_handler, relay);
}

// TODO: implement on session_inbound.

void p2p::start_accepting()
{
    network_.listen(inbound_port_,
        dispatch_.ordered_delegate(&p2p::start_accept,
            this, _1, _2));
}

void p2p::start_accept(const code& ec, acceptor::ptr accept)
{
    BITCOIN_ASSERT(accept);

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Error starting listener: " << ec.message();
        return;
    }

    // ACCEPT INCOMING CONNECTIONS
    accept->accept(
        dispatch_.ordered_delegate(&p2p::handle_accept,
            this, _1, _2, accept));
}

void p2p::handle_accept(const code& ec, channel::ptr node,
    acceptor::ptr accept)
{
    // Relisten for connections.
    start_accept(ec, accept);

    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure accepting connection: " << ec.message();
        return;
    }

    if (inbound_connections_.size() >= max_inbound_)
    {
        log_debug(LOG_PROTOCOL)
            << "Rejected inbound connection due to connection limit";
        return;
    }

    const auto address = node->address();
    if (is_blacklisted(node->address()))
    {
        log_debug(LOG_PROTOCOL)
            << "Rejected inbound connection due to blacklisted address";
        return;
    }

    if (is_loopback(node))
    {
        log_debug(LOG_PROTOCOL)
            << "Rejected inbound connection from self";
        return;
    }

    // Save the connection as we are now assured of getting stop event.
    inbound_connections_.push_back(node);

    // Accepted!
    log_info(LOG_PROTOCOL)
        << "Accepted connection from [" << address << "] ("
        << inbound_connections_.size() << " total)";

    const auto stop_handler = 
        dispatch_.ordered_delegate(&p2p::inbound_channel_stopped,
            this, _1, node, address.to_string());

    start_talking(node, stop_handler, relay_);
}

// TODO: virtual base method.

void p2p::start_talking(channel::ptr node,
    proxy::stop_handler handle_stop, bool relay)
{
    node->subscribe_stop(handle_stop);

    // Notify protocol subscribers of new channel.
    channel_subscriber_->relay(error::success, node);

    const auto callback = 
        dispatch_.ordered_delegate(&p2p::handle_handshake,
            this, _1, node);

    // TODO: set height.
    const uint32_t blockchain_height = 0;

    // Attach version protocol to the new connection (until complete).
    std::make_shared<protocol_version>(node, pool_, timeouts_.handshake,
        callback, hosts_, self_, blockchain_height, relay)->start();

    // Start reading from the socket (causing subscription events).
    node->start();
}

void p2p::handle_handshake(const code& ec, channel::ptr node)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL) << "Failure in peer handshake ["
            << node->address() << "] " << ec.message();
        node->stop(ec);
        return;
    }

    // Attach ping protocol to the new connection (until node stop event).
    std::make_shared<protocol_ping>(node, pool_, timeouts_.heartbeat)->start();

    // Attach address protocol to the new connection (until node stop event).
    std::make_shared<protocol_address>(node, pool_, hosts_, self_)->start();
}

// TODO: abstract base method, implement in derived.

void p2p::outbound_channel_stopped(const code& ec,
    channel::ptr node, const std::string& address)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (outbound) [" << address << "] "
        << ec.message();

    remove_connection(outbound_connections_, node);

    // If not shutdown we always create a replacement oubound connection.
    if (ec != error::service_stopped)
        new_connection();
}

void p2p::manual_channel_stopped(const code& ec,
    channel::ptr node, const std::string& address, bool relay, size_t retries)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (manual) [" << address << "] "
        << ec.message();

    remove_connection(manual_connections_, node);

    // If not shutdown we always attempt to reconnect manual connections.
    if (ec != error::service_stopped)
        retry_manual_connection(address, relay, retries);
}

void p2p::inbound_channel_stopped(const code& ec,
    channel::ptr node, const std::string& address)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (inbound) [" << address << "] "
        << ec.message();

    // We never attempt to reconnect inbound connections.
    remove_connection(inbound_connections_, node);
}

// TODO: retain these on protocol base.

void p2p::blacklist(const config::authority& peer)
{
    blacklisted_.push_back(peer);
}

void p2p::remove_connection(channel_ptr_list& connections,
    channel::ptr node)
{
    auto it = std::find(connections.begin(), connections.end(), node);
    if (it != connections.end())
        connections.erase(it);
}

void p2p::subscribe_channel(channel_handler handle_channel)
{
    channel_subscriber_->subscribe(handle_channel);
}

size_t p2p::connection_count() const
{
    return outbound_connections_.size() + manual_connections_.size() +
        inbound_connections_.size();
}

bool p2p::is_blacklisted(const config::authority& peer) const
{
    const auto found = [&peer](const config::authority& host)
    {
        return (host.port() == 0 || host.port() == peer.port()) &&
            (host.ip() == peer.ip());
    };
    auto it = std::find_if(blacklisted_.begin(), blacklisted_.end(), found);
    return it != blacklisted_.end();
    return true;
}

bool p2p::is_connected(const config::authority& peer) const
{
    const auto& inn = inbound_connections_;
    const auto& out = outbound_connections_;
    const auto& man = manual_connections_;
    const auto found = [&peer](const channel::ptr& node)
    {
        return (node->address() == peer);
    };
    return
        (std::find_if(inn.begin(), inn.end(), found) != inn.end()) ||
        (std::find_if(out.begin(), out.end(), found) != out.end()) ||
        (std::find_if(man.begin(), man.end(), found) != man.end());
}

bool p2p::is_loopback(channel::ptr node) const
{
    const auto& outbound = outbound_connections_;
    const auto id = node->identifier();
    const auto found = [node, id](const channel::ptr& entry)
    {
        return (entry != node) && (entry->identifier() == id);
    };
    auto it = std::find_if(outbound.begin(), outbound.end(), found);
    return it != outbound.end();
    return true;
}

} // namespace network
} // namespace libbitcoin
