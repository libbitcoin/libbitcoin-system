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
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/initiator.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/network/seeder.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
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

// TODO: make protocol an aggregator over the derived classes of network_base.
// TODO: parameterize sequencer, connections and hosts as same object for each.
// TODO: pass config reference into each class.
// TODO: implement protocol_version (handshake) protocol_ping, protocol_address.
protocol::protocol(threadpool& pool, hosts& hosts, handshake& shake,
    initiator& network, uint16_t port, bool relay, size_t max_outbound,
    size_t max_inbound, const config::endpoint::list& seeds,
    const network_address_type& self, const timeout& timeouts)
  : sequence_(pool),
    pool_(pool),
    hosts_(hosts),
    handshake_(shake),
    network_(network),
    channel_subscriber_(std::make_shared<channel_subscriber>(pool)),
    seeds_(seeds),
    self_(self),
    timeouts_(timeouts),
    relay_(relay),
    inbound_port_(port),
    max_inbound_(max_inbound),
    max_outbound_(max_outbound)
{
}

// TODO: add seed connections to protocol_seed (i.e. connect once).
// TODO: add manual connections via config in protocol_manual.

void protocol::start(completion_handler handle_complete)
{
    // Start inbound connection listener if configured.
    if (inbound_port_ > 0 && max_inbound_ > 0)
        start_accepting();

    // TODO: move seeding to node start.
    if (hosts_.capacity() > 0 && max_outbound_ > 0)
        start_seeding(handle_complete);
    else
        // if (!hosts_.empty() && max_outbound_ > 0)
        start_connecting(error::success, handle_complete);
}

void protocol::stop(completion_handler handle_complete)
{
    // Stop protocol subscribers.
    channel_subscriber_->relay(error::service_stopped, nullptr);

    // TODO: use single list (simple).
    // Notify all channels to stop.
    for (const auto node: outbound_connections_)
        node->stop(error::service_stopped);

    for (const auto node: manual_connections_)
        node->stop(error::service_stopped);

    for (const auto node: inbound_connections_)
        node->stop(error::service_stopped);

    hosts_.save(handle_complete);
}

void protocol::start_seeding(completion_handler handle_complete)
{
    const auto complete_seeding =
        std::bind(&protocol::start_connecting,
            this, _1, handle_complete);

    // TODO: handle_complete must be passed in and invoked upon completion.
    std::make_shared<seeder>(pool_, hosts_, handshake_, network_, seeds_,
        self_)->start(complete_seeding);
}

// TODO: implement on protocol_outbound.

void protocol::start_connecting(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Error seeding hosts " << ec.message();
        handle_complete(ec);
        return;
    }

    // Start outbound connection attempts (not concurrent because of strand).
    for (auto channel = 0; channel < max_outbound_; ++channel)
        new_connection();

    handle_complete(error::success);
}

void protocol::new_connection()
{
    hosts_.fetch_address(
        sequence_.sync(&protocol::start_connect,
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

    if (is_blacklisted(peer))
    {
        handle_connect(error::address_blocked, nullptr, peer);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to peer [" << peer.to_string() << "]";

    // OUTBOUND CONNECT (sequential)
    network_.connect(peer.to_hostname(), peer.port(),
        sequence_.sync(&protocol::handle_connect,
            this, _1, _2, peer));
}

void protocol::handle_connect(const std::error_code& ec, channel_ptr node,
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
        sequence_.sync(&protocol::outbound_channel_stopped,
            this, _1, node, peer.to_string());

    start_talking(node, stop_handler, relay_);
}

// TODO: implement on protocol_manual.

void protocol::retry_manual_connection(const config::endpoint& address,
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

void protocol::maintain_connection(const std::string& hostname, uint16_t port,
    bool relay, size_t retry)
{
    // MANUAL CONNECT
    network_.connect(hostname, port,
        sequence_.sync(&protocol::handle_manual_connect,
            this, _1, _2, hostname, port, relay, retry));
}

void protocol::handle_manual_connect(const std::error_code& ec,
    channel_ptr node, const std::string& hostname, uint16_t port, bool relay,
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
        sequence_.sync(&protocol::manual_channel_stopped,
            this, _1, node, peer.to_string(), relay, retries);

    start_talking(node, stop_handler, relay);
}

// TODO: implement on protocol_inbound.

void protocol::start_accepting()
{
    network_.listen(inbound_port_,
        sequence_.sync(&protocol::start_accept,
            this, _1, _2));
}

void protocol::start_accept(const std::error_code& ec, acceptor_ptr accept)
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
        sequence_.sync(&protocol::handle_accept,
            this, _1, _2, accept));
}

void protocol::handle_accept(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
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
        sequence_.sync(&protocol::inbound_channel_stopped,
            this, _1, node, address.to_string());

    start_talking(node, stop_handler, relay_);
}

// TODO: virtual base method.

void protocol::start_talking(channel_ptr node,
    channel_proxy::stop_handler handle_stop, bool relay)
{
    node->subscribe_stop(handle_stop);

    // Notify protocol subscribers of new channel.
    channel_subscriber_->relay(error::success, node);

    // Subscribe to events and start talking on the socket.
    handshake_.start(node,
        sequence_.sync(&protocol::handle_handshake,
            this, _1, node), relay);

    // Start reading from the socket (causing subscription events).
    node->start();
}

// TODO: virtual base method, override in protocol_seed to use no protocol_ping
// and invoke protocol_address using callback and timer invoacation.

void protocol::handle_handshake(const std::error_code& ec, channel_ptr node)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL) << "Failure in peer handshake ["
            << node->address() << "] " << ec.message();
        node->stop(ec);
        return;
    }

    // Attach ping protocol to the new connection.
    std::make_shared<protocol_ping>(node, pool_, timeouts_)->start();

    // Attach address protocol to the new connection.
    std::make_shared<protocol_address>(node, pool_, hosts_, self_)->start();
}

// TODO: abstract base method, implement in derived.

void protocol::outbound_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (outbound) [" << address << "] "
        << ec.message();

    remove_connection(outbound_connections_, node);

    // If not shutdown we always create a replacement oubound connection.
    if (ec != error::service_stopped)
        new_connection();
}

void protocol::manual_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address, bool relay, size_t retries)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (manual) [" << address << "] "
        << ec.message();

    remove_connection(manual_connections_, node);

    // If not shutdown we always attempt to reconnect manual connections.
    if (ec != error::service_stopped)
        retry_manual_connection(address, relay, retries);
}

void protocol::inbound_channel_stopped(const std::error_code& ec,
    channel_ptr node, const std::string& address)
{
    log_debug(LOG_PROTOCOL)
        << "Channel stopped (inbound) [" << address << "] "
        << ec.message();

    // We never attempt to reconnect inbound connections.
    remove_connection(inbound_connections_, node);
}

// TODO: retain these on protocol base.

void protocol::blacklist(const config::authority& peer)
{
    blacklisted_.push_back(peer);
}

void protocol::remove_connection(channel_ptr_list& connections,
    channel_ptr node)
{
    auto it = std::find(connections.begin(), connections.end(), node);
    if (it != connections.end())
        connections.erase(it);
}

void protocol::subscribe_channel(channel_handler handle_channel)
{
    channel_subscriber_->subscribe(handle_channel);
}

// TODO: consolidate three lists using node type enumeration.
// inbound, outbound, manual, seed (?).
// THese are not currently thread safe accessors, should be stranded.

size_t protocol::connection_count() const
{
    return outbound_connections_.size() + manual_connections_.size() +
        inbound_connections_.size();
}

bool protocol::is_blacklisted(const config::authority& peer) const
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

bool protocol::is_connected(const config::authority& peer) const
{
    // TODO: add connection_type to node so we only need one connection pool.
    const auto& inn = inbound_connections_;
    const auto& out = outbound_connections_;
    const auto& man = manual_connections_;
    const auto found = [&peer](const channel_ptr& node)
    {
        return (node->address() == peer);
    };
    return
        (std::find_if(inn.begin(), inn.end(), found) != inn.end()) ||
        (std::find_if(out.begin(), out.end(), found) != out.end()) ||
        (std::find_if(man.begin(), man.end(), found) != man.end());
}

bool protocol::is_loopback(channel_ptr node) const
{
    const auto& outbound = outbound_connections_;
    const auto nonce = node->nonce();
    const auto found = [node, nonce](const channel_ptr& entry)
    {
        return (entry != node) && (entry->nonce() == nonce);
    };
    auto it = std::find_if(outbound.begin(), outbound.end(), found);
    return it != outbound.end();
    return true;
}

} // namespace network
} // namespace libbitcoin
