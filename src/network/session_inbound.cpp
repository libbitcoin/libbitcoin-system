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
#include <bitcoin/bitcoin/network/session_inbound.hpp>

#include <cstddef>
#include <functional>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>

INITIALIZE_TRACK(bc::network::session_inbound);

namespace libbitcoin {
namespace network {

#define CLASS session_inbound

using std::placeholders::_1;
using std::placeholders::_2;

session_inbound::session_inbound(threadpool& pool, p2p& network,
    const settings& settings)
  : session(pool, network, settings, true, false),
    CONSTRUCT_TRACK(session_inbound, LOG_NETWORK)
{
}

void session_inbound::start()
{
    if (!stopped())
        return;

    if (settings_.inbound_port == 0 || settings_.inbound_connection_limit == 0)
    {
        log::info(LOG_NETWORK)
            << "Not configured for accepting incoming connections.";
        return;
    }

    session::start();
    const auto accept = create_acceptor();
    const auto port = settings_.inbound_port;

    // START LISTENING ON PORT
    accept->listen(port, ORDERED2(start_accept, _1, accept));
}

void session_inbound::start_accept(const code& ec, acceptor::ptr accept)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Error starting listener: " << ec.message();
        return;
    }

    // ACCEPT THE NEXT INCOMING CONNECTION
    accept->accept(ORDERED3(handle_accept, _1, _2, accept));
}

void session_inbound::handle_accept(const code& ec, channel::ptr channel,
    acceptor::ptr accept)
{
    if (stopped())
        return;

    start_accept(error::success, accept);

    if (ec)
    {
        log::debug(LOG_NETWORK)
            << "Failure accepting connection: " << ec.message();
        return;
    }

    if (blacklisted(channel->authority()))
    {
        log::debug(LOG_NETWORK)
            << "Rejected inbound connection from ["
            << channel->authority() << "] due to blacklisted address.";
        return;
    }

    connection_count(ORDERED2(handle_connection_count, _1, channel));
}

void session_inbound::handle_connection_count(size_t connections,
    channel::ptr channel)
{
    if (connections >= settings_.inbound_connection_limit)
    {
        log::debug(LOG_NETWORK)
            << "Rejected inbound connection from ["
            << channel->authority() << "] due to connection limit.";
        return;
    }
   
    log::info(LOG_NETWORK)
        << "Connected inbound channel [" << channel->authority() << "]";

    register_channel(channel, 
        BIND2(handle_channel_start, _1, channel),
        BIND1(handle_channel_stop, _1));
}

void session_inbound::handle_channel_start(const code& ec,
    channel::ptr channel)
{
    if (ec)
        return;

    attach<protocol_ping>(channel, settings_);
    attach<protocol_address>(channel, settings_);
}

void session_inbound::handle_channel_stop(const code&)
{
}

} // namespace network
} // namespace libbitcoin
