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

using std::placeholders::_1;
using std::placeholders::_2;

session::session(threadpool& pool, p2p& network, const settings& settings,
    bool incoming, bool temporary)
  : stopped_(true),
    incoming_(incoming),
    notify_(!temporary),
    pool_(pool),
    network_(network),
    dispatch_(pool),
    settings_(settings)
{
}

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

// If we ever allow restart we need to guard start.
void session::start()
{
    if (!stopped())
        return;

    stopped_ = false;
    subscribe_stop(std::bind(&session::handle_stop,
        shared_from_this()));
}

void session::handle_stop()
{
    stopped_ = true;
}

bool session::stopped() const
{
    return stopped_;
}

void session::subscribe_stop(stop_handler handler)
{
    network_.subscribe(
        dispatch_.ordered_delegate(&session::handle_channel,
            shared_from_this(), _1, _2, handler));
}

void session::handle_channel(const code& ec, channel::ptr,
    stop_handler handler)
{
    if (ec == error::service_stopped)
        handler();
    else
        subscribe_stop(handler);
}

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

// The two handlers are always ordered on the session strand.
// This should be the first call after a socket is opened on a channel.
void session::register_channel(channel::ptr channel,
    result_handler handle_started, result_handler handle_stopped)
{
    if (stopped())
    {
        handle_started(error::service_stopped);
        return;
    }

    // After this point we must stop the channel if there is a start error.
    // This is managed in session::handle_started, where all starts terminate.
    channel->start();

    // Place invocation of stop handler on ordered delegate.
    const auto remove_handler =
        dispatch_.ordered_delegate(&session::remove,
            shared_from_this(), _1, channel, handle_stopped);

    // Place invocation of start handler on ordered delegate.
    const auto start_handler = 
        dispatch_.ordered_delegate(&session::handle_started,
            shared_from_this(), _1, channel, handle_started, remove_handler);

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

    // TODO: there is a race here between completion of handshake and start of other protocols.
    // Incoming ping and address messages are being missed.

    const auto handler =
        dispatch_.ordered_delegate(&session::handle_handshake,
            shared_from_this(), _1, channel, handle_started);

    // Subscribe start handler to handshake completion.
    attach<protocol_version>(channel)->
        start(settings_, network_.height(), handler);

    // Start reading messages from the socket.
    channel->talk();
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
    
    if (incoming_)
    {
        network_.pent(channel->version().nonce,
            dispatch_.ordered_delegate(&session::handle_is_pending,
                shared_from_this(), _1, channel, handle_started));
        return;
    }

    // Bypass loopback test for outgoing channels.
    dispatch_.ordered(&session::handle_is_pending,
        shared_from_this(), false, channel, handle_started);
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
        std::bind(&session::handle_stored,
            shared_from_this(), _1, channel, handle_started));
}

void session::handle_stored(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    // Don't notify of channel creation if we are seeding (for example).
    if (notify_)
        network_.relay(error::success, channel);

    handle_started(ec);
}

void session::handle_started(const code& ec, channel::ptr channel,
    result_handler handle_started, result_handler handle_stopped)
{
    // We must either stop or subscribe the channel for stop before returning.
    if (ec)
        channel->stop(ec);
    else
        channel->subscribe_stop(handle_stopped);

    // End of the start sequence.
    handle_started(ec);
}

void session::unpend(const code& ec, channel::ptr channel,
    result_handler handle_started)
{
    channel->set_nonce(0);
    handle_started(ec);
    network_.unpend(channel,
        std::bind(&session::handle_unpend,
            shared_from_this(), _1));
}

void session::remove(const code& ec, channel::ptr channel,
    result_handler handle_stopped)
{
    handle_stopped(ec);
    network_.remove(channel,
        std::bind(&session::handle_remove,
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
