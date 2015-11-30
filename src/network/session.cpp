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
#include <bitcoin/bitcoin/network/session.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
#define NAME "session"

using std::placeholders::_1;
using std::placeholders::_2;

session::session(threadpool& pool, p2p& network, const settings& settings,
    bool outgoing, bool persistent)
  : stopped_(true),
    incoming_(!outgoing),
    notify_(persistent),
    pool_(pool),
    network_(network),
    dispatch_(pool, NAME),
    settings_(settings)
{
}

session::~session()
{
    BITCOIN_ASSERT_MSG(stopped(), "The session was not stopped.");
}

// Properties.
// ----------------------------------------------------------------------------

// protected:
void session::address_count(count_handler handler)
{
    network_.address_count(handler);
}

// protected:
void session::fetch_address(host_handler handler)
{
    network_.fetch_address(handler);
}

// protected:
void session::connection_count(count_handler handler)
{
    network_.connected_count(handler);
}

// protected:
bool session::blacklisted(const authority& authority) const
{
    const auto& blocked = settings_.blacklists;
    const auto it = std::find(blocked.begin(), blocked.end(), authority);
    return it != blocked.end();
}

// Socket creators.
// ----------------------------------------------------------------------------

// protected:
acceptor::ptr session::create_acceptor()
{
    const auto accept = std::make_shared<acceptor>(pool_, settings_);

    // Stop on the subscriber thread.
    subscribe_stop(
        std::bind(&session::do_stop_acceptor,
            shared_from_this(), accept));

    return accept;
}

void session::do_stop_acceptor(acceptor::ptr accept)
{
    accept->stop();
}

// protected:
connector::ptr session::create_connector()
{
    const auto connect = std::make_shared<connector>(pool_, settings_);

    // Stop on the subscriber thread.
    subscribe_stop(
        std::bind(&session::do_stop_connector,
            shared_from_this(), connect));

    return connect;
}

void session::do_stop_connector(connector::ptr connect)
{
    connect->stop();
}

// Start sequence.
// ----------------------------------------------------------------------------

// public:
void session::start(result_handler handler)
{
    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    stopped_ = false;

    // Must subscribe on the start thread.
    subscribe_stop(
        std::bind(&session::do_stop_session,
            shared_from_this()));

    // This is the end of the start sequence.
    handler(error::success);
}

void session::do_stop_session()
{
    stopped_ = true;
}

bool session::stopped() const
{
    return stopped_;
}

// Subscribe Stop sequence.
// ----------------------------------------------------------------------------

// public:
void session::subscribe_stop(stop_handler handler)
{
    // Must resubscribe on the subscriber thread.
    network_.subscribe(
        std::bind(&session::handle_channel_event,
            shared_from_this(), _1, _2, handler));
}

void session::handle_channel_event(const code& ec, channel::ptr,
    stop_handler handler)
{
    // This is the end of the subscribe stop sequence.
    if (ec == error::service_stopped)
        handler();
    else
        subscribe_stop(handler);
}

// Registration sequence.
// ----------------------------------------------------------------------------
// BUGBUG: there is a race here between completion of handshake and start of
// other protocols. Initial incoming messages (ping, address, get-headers)
// initiated by peer are being missed. To resolve this we would need to suspend
// and restart talk or pre-register for the messages. Stop messages are never
// missed since protocol::start is ordered on the channel strand.

// protected:
void session::register_channel(channel::ptr channel,
    result_handler handle_started, result_handler handle_stopped)
{
    // Place invocation of stop handler on ordered delegate.
    const auto stop_handler =
        dispatch_.concurrent_delegate(&session::do_remove,
            shared_from_this(), _1, channel, handle_stopped);

    // Place invocation of start handler on ordered delegate.
    auto start_handler =
        dispatch_.concurrent_delegate(&session::handle_started,
            shared_from_this(), _1, channel, handle_started, stop_handler);

    if (stopped())
    {
        start_handler(error::service_stopped);
        return;
    }

    if (incoming_)
    {
        dispatch_.concurrent(&session::handle_pend,
            shared_from_this(), error::success, channel, start_handler);
        return;
    }
    
    channel->set_nonce(nonzero_pseudo_random());

    const auto unpend_handler =
        dispatch_.concurrent_delegate(&session::do_unpend,
            shared_from_this(), _1, channel, start_handler);

    network_.pend(channel,
        dispatch_.concurrent_delegate(&session::handle_pend,
            shared_from_this(), _1, channel, unpend_handler));
}

void session::handle_pend(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    if (ec)
    {
        handle_started(ec);
        return;
    }

    // The channel starts, invokes the handler, then starts the read cycle.
    channel->start(
        dispatch_.concurrent_delegate(&session::handle_channel_start,
            shared_from_this(), _1, channel, handle_started));
}

void session::handle_channel_start(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    attach<protocol_version>(channel)->start(settings_, network_.height(),
        dispatch_.concurrent_delegate(&session::handle_handshake,
            shared_from_this(), _1, channel, handle_started));
}

void session::handle_handshake(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    if (ec)
    {
        log::debug(LOG_NETWORK)
            << "Failure in handshake with [" << channel->authority()
            << "] " << ec.message();
        handle_started(ec);
        return;
    }
    
    auto handler = 
        dispatch_.concurrent_delegate(&session::handle_is_pending,
            shared_from_this(), _1, channel, handle_started);

    // The loopback test is for incoming channels only.
    if (incoming_)
        network_.pent(channel->version().nonce, handler);
    else
        handler(false);
}

void session::handle_is_pending(bool pending, channel::ptr channel,
    result_handler handle_started)
{
    if (pending)
    {
        log::debug(LOG_NETWORK)
            << "Rejected connection from [" << channel->authority()
            << "] as loopback.";
        handle_started(error::accept_failed);
        return;
    }

    const auto version = channel->version();
    if (version.value < bc::peer_minimum_version)
    {
        log::debug(LOG_NETWORK)
            << "Peer version (" << version.value << ") below minimum ("
            << bc::peer_minimum_version << ") [" 
            << channel->authority() << "]";
        handle_started(error::accept_failed);
        return;
    }

    network_.store(channel,
        dispatch_.concurrent_delegate(&session::handle_stored,
            shared_from_this(), _1, channel, handle_started));
}

void session::handle_stored(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    // Connection-in-use indicated here by error::address_in_use.
    handle_started(ec);

    // Don't notify of channel creation if we are seeding or syncing.
    if (!ec && notify_)
        network_.relay(error::success, channel);
}

void session::handle_started(const code& ec, channel::ptr channel,
    result_handler handle_started, result_handler handle_stopped)
{
    // We must either stop or subscribe the channel for stop before returning.
    if (ec)
        channel->stop(ec);
    else
        channel->subscribe_stop(handle_stopped);

    ////if (ec)
    ////    log::debug(LOG_NETWORK)
    ////        << "Failed to start [" << channel->authority() << "] "
    ////        << ec.message();
    ////else
    ////    log::debug(LOG_NETWORK)
    ////        << "Started [" << channel ->authority() << "]";

    // This is the end of the registration sequence.
    handle_started(ec);
}

void session::do_unpend(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    channel->set_nonce(0);
    handle_started(ec);
    network_.unpend(channel,
        dispatch_.concurrent_delegate(&session::handle_unpend,
            shared_from_this(), _1));
}

void session::do_remove(const code& ec, channel::ptr channel,
    result_handler handle_stopped)
{
    handle_stopped(ec);
    network_.remove(channel,
        dispatch_.concurrent_delegate(&session::handle_remove,
            shared_from_this(), _1));
}

void session::handle_unpend(const code& ec)
{
    if (ec)
        log::debug(LOG_NETWORK)
            << "Failed to unpend a channel: " << ec.message();
}

void session::handle_remove(const code& ec)
{
    if (ec)
        log::debug(LOG_NETWORK)
            << "Failed to remove a channel: " << ec.message();
}

} // namespace network
} // namespace libbitcoin
