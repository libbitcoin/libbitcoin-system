/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/network/session_outbound.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>

INITIALIZE_TRACK(bc::network::session_outbound);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

session_outbound::session_outbound(threadpool& pool, p2p& network,
    const settings& settings)
  : session(pool, network, settings, false, false),
    CONSTRUCT_TRACK(session_outbound, LOG_NETWORK)
{
}

void session_outbound::start()
{
    // If we ever allow restart we need to isolate start.
    if (!stopped())
        return;

    if (settings_.outbound_connections == 0)
    {
        log_debug(LOG_PROTOCOL)
            << "No configured outbound connections.";
        return;
    }

    session::start();
    const auto connect = create_connector();
    for (size_t peer = 0; peer < settings_.outbound_connections; ++peer)
        new_connection(connect);
}

void session_outbound::new_connection(connector::ptr connect)
{
    fetch_address(
        dispatch_.ordered_delegate(&session_outbound::start_connect,
            shared_from_base<session_outbound>(), _1, _2, connect));
}

void session_outbound::start_connect(const code& ec, const authority& host,
    connector::ptr connect)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure fetching address [" << host << "] "
            << ec.message();
        new_connection(connect);
        return;
    }

    if (blacklisted(host))
    {
        log_debug(LOG_PROTOCOL)
            << "Fetched blacklisted address [" << host << "] ";
        new_connection(connect);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Connecting to channel [" << host << "]";

    // OUTBOUND CONNECT
    connect->connect(host,
        dispatch_.ordered_delegate(&session_outbound::handle_connect,
            shared_from_base<session_outbound>(), _1, _2, host, connect));
}

void session_outbound::handle_connect(const code& ec, channel::ptr channel,
    const authority& host, connector::ptr connect)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure connecting [" << host << "] outbound: "
            << ec.message();
        new_connection(connect);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Connected to outbound channel [" << channel->address() << "]";

    register_channel(channel, 
        std::bind(&session_outbound::handle_channel_start,
            shared_from_base<session_outbound>(), _1, channel),
        std::bind(&session_outbound::handle_channel_stop,
            shared_from_base<session_outbound>(), _1, connect));
}

void session_outbound::handle_channel_start(const code& ec,
    channel::ptr channel)
{
    if (ec)
        return;

    attach<protocol_ping>(channel);
    attach<protocol_address>(channel);
}

void session_outbound::handle_channel_stop(const code& ec,
    connector::ptr connect)
{
    if (ec != error::service_stopped)
        new_connection(connect);
}

} // namespace network
} // namespace libbitcoin
