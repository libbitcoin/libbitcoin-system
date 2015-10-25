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
#include <bitcoin/bitcoin/network/session_manual.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>

INITIALIZE_TRACK(bc::network::session_manual);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

session_manual::session_manual(threadpool& pool, p2p& network,
    const settings& settings)
  : connect_(create_connector()),
    session(pool, network, settings, false, false),
    CONSTRUCT_TRACK(session_manual, LOG_NETWORK)
{
}

// Must call start() before connect.
void session_manual::connect(const std::string& hostname, uint16_t port)
{
    const auto unhandled = [](const code&, channel::ptr) {};
    connect(hostname, port, unhandled);
}

// Must call start() before connect.
void session_manual::connect(const std::string& hostname, uint16_t port,
    channel_handler handler)
{
    // This recreates the connector for each new connect, and each time a
    // connection is restarted. Need to split start() and connect() here.
    start_connect(hostname, port, handler, settings_.connect_attempts);
}

void session_manual::start_connect(const std::string& hostname, uint16_t port,
    channel_handler handler, uint16_t retries)
{
    if (stopped())
        return;

    // MANUAL CONNECT OUTBOUND
    connect_->connect(hostname, port,
        dispatch_.ordered_delegate(&session_manual::handle_connect,
            shared_from_base<session_manual>(), _1, _2, hostname, port,
                handler, retries));
}

void session_manual::handle_connect(const code& ec, channel::ptr channel,
    const std::string& hostname, uint16_t port, channel_handler handler,
    uint16_t retries)
{
    if (ec)
    {
        log_warning(LOG_NETWORK)
            << "Failure connecting [" << config::endpoint(hostname, port)
            << "] manually: " << ec.message();

        // Retry logic.
        if (settings_.connect_attempts == 0)
            start_connect(hostname, port, handler, 0);
        else if (retries > 0)
            start_connect(hostname, port, handler, retries - 1);
        else
            handler(ec, nullptr);

        return;
    }

    // We only invoke the callback on the first successful connection.
    handler(ec, channel);

    log_info(LOG_NETWORK)
        << "Connected manual channel [" << config::endpoint(hostname, port)
        << "] as [" << channel->address() << "]";

    register_channel(channel, 
        std::bind(&session_manual::handle_channel_start,
            shared_from_base<session_manual>(), _1, channel),
        std::bind(&session_manual::handle_channel_stop,
            shared_from_base<session_manual>(), _1, hostname, port));
}

void session_manual::handle_channel_start(const code& ec, channel::ptr channel)
{
    if (ec)
        return;

    attach<protocol_ping>(channel);
    attach<protocol_address>(channel);
}

void session_manual::handle_channel_stop(const code& ec,
    const std::string& hostname, uint16_t port)
{
    if (ec != error::service_stopped)
        connect(hostname, port);
}

} // namespace network
} // namespace libbitcoin
