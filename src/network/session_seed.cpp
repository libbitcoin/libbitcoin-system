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
#include <bitcoin/bitcoin/network/session_seed.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_ping.hpp>
#include <bitcoin/bitcoin/network/protocol_seed.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>

INITIALIZE_TRACK(bc::network::session_seed);

namespace libbitcoin {
namespace network {

#define CLASS session_seed

using std::placeholders::_1;
using std::placeholders::_2;

session_seed::session_seed(threadpool& pool, p2p& network,
    const settings& settings)
  : session(pool, network, settings, false, true),
    CONSTRUCT_TRACK(session_seed, LOG_NETWORK)
{
}

void session_seed::start(result_handler handler)
{
    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    if (settings_.host_pool_capacity == 0)
    {
        log::info(LOG_NETWORK)
            << "Not configured to populate an address pool.";
        handler(error::success);
        return;
    }

    address_count(ORDERED2(handle_count, _1, handler));
}

void session_seed::handle_count(size_t start_size, result_handler handler)
{
    if (start_size != 0)
    {
        log::debug(LOG_NETWORK)
            << "Seeding is not required because there are " 
            << start_size << " cached addresses.";
        handler(error::success);
        return;
    }

    if (settings_.seeds.empty())
    {
        log::error(LOG_NETWORK)
            << "Seeding is required but no seeds are configured.";
        handler(error::operation_failed);
        return;
    }

    session::start();
    start_seeding(start_size, create_connector(), handler);
}

void session_seed::start_seeding(size_t start_size, connector::ptr connect,
    result_handler handler)
{
    const auto& seeds = settings_.seeds;
    auto multiple = ORDERED2(handle_complete, start_size, handler);

    // Require all seed callbacks before calling session_seed::handle_stopped.
    auto single = synchronize(multiple, seeds.size(), "session_seed", true);

    // Require one callback per channel before calling single.
    // We don't use parallel here because connect is itself asynchronous.
    for (const auto& seed: seeds)
        start_seed(seed, connect, synchronize(single, 1, seed.to_string()));
}

void session_seed::start_seed(const config::endpoint& seed,
    connector::ptr connect, result_handler handler)
{
    if (stopped())
    {
        handler(error::channel_stopped);
        return;
    }

    log::info(LOG_NETWORK)
        << "Contacting seed [" << seed << "]";

    // OUTBOUND CONNECT
    connect->connect(seed, ORDERED4(handle_connect, _1, _2, seed, handler));
}

void session_seed::handle_connect(const code& ec, channel::ptr channel,
    const config::endpoint& seed, result_handler handler)
{
    if (ec)
    {
        log::info(LOG_NETWORK)
            << "Failure contacting seed [" << seed << "] " << ec.message();
        handler(ec);
        return;
    }

    if (blacklisted(channel->authority()))
    {
        log::debug(LOG_NETWORK)
            << "Seed [" << seed << "] on blacklisted address ["
            << channel->authority() << "]";
        handler(error::address_blocked);
        return;
    }

    log::info(LOG_NETWORK)
        << "Connected seed [" << seed << "] as " << channel->authority();

    register_channel(channel, 
        BIND3(handle_channel_start, _1, channel, handler),
        BIND1(handle_channel_stop, _1));
}

void session_seed::handle_channel_start(const code& ec, channel::ptr channel,
    result_handler handler)
{
    if (ec)
    {
        handler(ec);
        return;
    }

    attach<protocol_ping>(channel, settings_);
    attach<protocol_seed>(channel, settings_, handler);
};

void session_seed::handle_channel_stop(const code&)
{
}

// This accepts no error code because individual seed errors are suppressed.
void session_seed::handle_complete(size_t start_size, result_handler handler)
{
    address_count(ORDERED3(handle_final_count, _1, start_size, handler));
}

// We succeed only if there is a host count increase.
void session_seed::handle_final_count(size_t current_size, size_t start_size,
    result_handler handler)
{
    const auto result = current_size > start_size ? error::success :
        error::operation_failed;

    handler(result);
}

} // namespace network
} // namespace libbitcoin
