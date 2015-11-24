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
    bool incoming, bool temporary)
  : stopped_(true),
    incoming_(incoming),
    notify_(!temporary),
    pool_(pool),
    network_(network),
    dispatch_(pool, NAME),
    settings_(settings)
{
}

// Properties.
// ----------------------------------------------------------------------------

void session::address_count(count_handler handler)
{
    network_.address_count(handler);
}

void session::fetch_address(host_handler handler)
{
    network_.fetch_address(handler);
}

void session::connection_count(count_handler handler)
{
    network_.connected_count(handler);
}

// This is threadsafe because settings is const.
bool session::blacklisted(const authority& authority) const
{
    const auto& blocked = settings_.blacklists;
    const auto it = std::find(blocked.begin(), blocked.end(), authority);
    return it != blocked.end();
}

// Socket creators.
// ----------------------------------------------------------------------------

acceptor::ptr session::create_acceptor()
{
    const auto accept = std::make_shared<acceptor>(pool_, settings_);
    const auto handle_stop = [accept]()
    {
        accept->cancel();
    };

    subscribe_stop(handle_stop);
    return accept;
}

connector::ptr session::create_connector()
{
    const auto connect = std::make_shared<connector>(pool_, settings_);
    const auto handle_stop = [connect]()
    {
        connect->cancel();
    };

    subscribe_stop(handle_stop);
    return connect;
}

// Start sequence.
// ----------------------------------------------------------------------------

// public:
void session::start(result_handler handler)
{
    dispatch_.ordered(&session::do_start,
        shared_from_this(), handler);
}

void session::do_start(result_handler handler)
{
    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    const auto subscribe_handler =
        dispatch_.ordered_delegate(&session::handle_stop,
            shared_from_this());

    stopped_ = false;
    subscribe_stop(subscribe_handler);

    // This is the end of the start sequence.
    handler(error::success);
}

void session::subscribe_stop(stop_handler handler)
{
    const auto subscribe_handler =
        dispatch_.ordered_delegate(&session::handle_channel_event,
            shared_from_this(), _1, _2, handler);

    network_.subscribe(subscribe_handler);
}

// Stop sequence (initiated by stop handler).
// ----------------------------------------------------------------------------

// private:
void session::handle_channel_event(const code& ec, channel::ptr,
    stop_handler handler)
{
    if (ec == error::service_stopped)
        handler();
    else
        subscribe_stop(handler);
}

void session::handle_stop()
{
    stopped_ = true;
}

bool session::stopped() const
{
    return stopped_;
}

// Channel registration sequence.
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
        dispatch_.ordered_delegate(&session::remove,
            shared_from_this(), _1, channel, handle_stopped);

    // Place invocation of start handler on ordered delegate.
    auto start_handler =
        dispatch_.ordered_delegate(&session::handle_started,
            shared_from_this(), _1, channel, handle_started, stop_handler);

    if (stopped())
    {
        start_handler(error::service_stopped);
        return;
    }

    if (incoming_)
    {
        dispatch_.ordered(&session::handle_pend,
            shared_from_this(), error::success, channel, start_handler);
        return;
    }
    
    channel->set_nonce(nonzero_pseudo_random());

    const auto unpend_handler =
        dispatch_.ordered_delegate(&session::unpend,
            shared_from_this(), _1, channel, start_handler);

    network_.pend(channel,
        dispatch_.ordered_delegate(&session::handle_pend,
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

    const auto handler =
        dispatch_.bound_delegate(&session::handle_channel_start,
            shared_from_this(), _1, channel, handle_started);

    // The channel starts, invokes the handler, then starts the read cycle.
    channel->start(handler);
}

void session::handle_channel_start(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    const auto handler =
        dispatch_.ordered_delegate(&session::handle_handshake,
            shared_from_this(), _1, channel, handle_started);

    attach<protocol_version>(channel)->
        start(settings_, network_.height(), handler);
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
        dispatch_.ordered_delegate(&session::handle_is_pending,
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

    const auto handler = 
        dispatch_.bound_delegate(&session::handle_stored,
            shared_from_this(), _1, channel, handle_started);

    network_.store(channel, handler);
}

void session::handle_stored(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    // Connection-in-use indicated here by error::address_in_use.
    handle_started(ec);

    // Don't notify of channel creation if we are seeding (for example).
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

    // End of the registration sequence.
    handle_started(ec);
}

void session::unpend(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    channel->set_nonce(0);
    handle_started(ec);
    network_.unpend(channel,
        dispatch_.bound_delegate(&session::handle_unpend,
            shared_from_this(), _1));
}

void session::remove(const code& ec, channel::ptr channel,
    result_handler handle_stopped)
{
    handle_stopped(ec);
    network_.remove(channel,
        dispatch_.bound_delegate(&session::handle_remove,
            shared_from_this(), _1));
}

void session::handle_unpend(const code& ec)
{
    if (ec)
        log::warning(LOG_NETWORK)
            << "Failed to unpend a channel: " << ec.message();
}

void session::handle_remove(const code& ec)
{
    if (ec)
        log::warning(LOG_NETWORK)
            << "Failed to remove a channel: " << ec.message();
}

} // namespace network
} // namespace libbitcoin
