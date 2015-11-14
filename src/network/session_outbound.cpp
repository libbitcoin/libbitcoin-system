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
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>

INITIALIZE_TRACK(bc::network::session_outbound);

namespace libbitcoin {
namespace network {

#define CLASS session_outbound

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
    if (!stopped())
        return;

    if (settings_.outbound_connections == 0)
    {
        log::info(LOG_NETWORK)
            << "Not configured for generating outbound connections.";
        return;
    }

    session::start();
    const auto connect = create_connector();
    for (size_t peer = 0; peer < settings_.outbound_connections; ++peer)
        new_connection(connect);
}

void session_outbound::new_connection(connector::ptr connect)
{
    fetch_address(ORDERED3(start_connect, _1, _2, connect));
}

void session_outbound::start_connect(const code& ec, const authority& host,
    connector::ptr connect)
{
    if (stopped())
        return;

    // This prevents a tight loop in an unusual circumstance.
    // TODO: rebuild connection count once addresses are found.
    if (ec == error::not_found)
    {
        log::error(LOG_NETWORK)
            << "The address pool is empty, suspending outbound session.";
        return;
    }

    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Failure fetching new address: " << ec.message();
        new_connection(connect);
        return;
    }

    // This could create a tight loop in the case of a small pool.
    if (blacklisted(host))
    {
        log::debug(LOG_NETWORK)
            << "Fetched blacklisted address [" << host << "] ";
        new_connection(connect);
        return;
    }

    log::debug(LOG_NETWORK)
        << "Connecting to channel [" << host << "]";

    // OUTBOUND CONNECT
    connect->connect(host, ORDERED4(handle_connect, _1, _2, host, connect));
}

void session_outbound::handle_connect(const code& ec, channel::ptr channel,
    const authority& host, connector::ptr connect)
{
    if (ec)
    {
        log::debug(LOG_NETWORK)
            << "Failure connecting [" << host << "] outbound: "
            << ec.message();
        new_connection(connect);
        return;
    }

    log::info(LOG_NETWORK)
        << "Connected to outbound channel [" << channel->authority() << "]";

    register_channel(channel, 
        BIND3(handle_channel_start, _1, connect, channel),
        BIND2(handle_channel_stop, _1, connect));
}

void session_outbound::handle_channel_start(const code& ec,
    connector::ptr connect, channel::ptr channel)
{
    // Treat a start failure just like a stop.
    if (ec)
    {
        handle_channel_stop(ec, connect);
        return;
    }

    attach<protocol_ping>(channel, settings_);
    attach<protocol_address>(channel, settings_);
}

void session_outbound::handle_channel_stop(const code& ec,
    connector::ptr connect)
{
    if (ec != error::service_stopped)
        new_connection(connect);
}

} // namespace network
} // namespace libbitcoin
