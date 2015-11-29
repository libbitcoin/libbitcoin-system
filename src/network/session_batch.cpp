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
#include <bitcoin/bitcoin/network/session_batch.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define CLASS session_batch
#define NAME "session_batch"

using namespace bc::config;
using std::placeholders::_1;
using std::placeholders::_2;

session_batch::session_batch(threadpool& pool, p2p& network,
    const settings& settings, bool persistent)
  : session(pool, network, settings, true, persistent)
{
}

// Connect sequence.
// ----------------------------------------------------------------------------

// protected:
void session_batch::connect(connector::ptr connect, channel_handler handler)
{
    const auto batch = std::max(settings_.connect_batch_size, 1u);
    const auto complete = synchronize(handler, 1, NAME);

    // We can't use dispatch::race here because it doesn't increment the shared
    // pointer reference count.
    for (uint32_t host = 0; host < batch; ++host)
        CONCURRENT2(new_connect, connect, complete);
}

void session_batch::new_connect(connector::ptr connect,
    channel_handler handler)
{
    if (stopped())
    {
        log::debug(LOG_NETWORK)
            << "Suspended outbound connection.";
        return;
    }

    fetch_address(CONCURRENT4(start_connect, _1, _2, connect, handler));
}

void session_batch::start_connect(const code& ec, const authority& host,
    connector::ptr connect, channel_handler handler)
{
    // This termination prevents a tight loop in the empty address pool case.
    if (ec)
    {
        log::error(LOG_NETWORK)
            << "Failure fetching new address: " << ec.message();
        handler(ec, nullptr);
        return;
    }

    // This could create a tight loop in the case of a small pool.
    if (blacklisted(host))
    {
        log::debug(LOG_NETWORK)
            << "Fetched blacklisted address [" << host << "] ";
        new_connect(connect, handler);
        return;
    }

    log::debug(LOG_NETWORK)
        << "Connecting to [" << host << "]";

    // CONNECT
    connect->connect(host, CONCURRENT5(handle_connect, _1, _2, host, connect,
        handler));
}

void session_batch::handle_connect(const code& ec, channel::ptr channel,
    const authority& host, connector::ptr connect, channel_handler handler)
{
    if (ec)
    {
        log::debug(LOG_NETWORK)
            << "Failure connecting to [" << host << "] "
            << ec.message();
        new_connect(connect, handler);
        return;
    }

    log::debug(LOG_NETWORK)
        << "Connected to [" << channel->authority() << "]";

    // This is the end of the connect sequence.
    handler(error::success, channel);
}

} // namespace network
} // namespace libbitcoin
