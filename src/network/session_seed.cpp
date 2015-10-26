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
#include <memory>
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
    // If we ever allow restart we need to isolate start.
    if (!stopped())
        return;

    if (settings_.outbound_connections == 0)
        handler(error::success);

    address_count(
        dispatch_.ordered_delegate(&session_seed::handle_count,
            shared_from_base<session_seed>(), _1, handler));
}

void session_seed::handle_count(size_t start_size, result_handler handler)
{
    if (start_size != 0)
        handler(error::success);

    if (settings_.seeds.empty())
    {
        log::info(LOG_NETWORK)
            << "Seeding is required but no seed channels are configured.";
        handler(bc::error::operation_failed);
        return;
    }

    session::start();
    const auto connect = create_connector();
    start_seeding(start_size, connect, handler);
}

void session_seed::start_seeding(size_t start_size, connector::ptr connect,
    result_handler handler)
{
    const auto& seeds = settings_.seeds;
    auto multiple =
        dispatch_.ordered_delegate(&session_seed::handle_complete,
            shared_from_base<session_seed>(), start_size, handler);

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
        return;

    log::info(LOG_NETWORK)
        << "Contacting seed [" << seed << "]";

    // OUTBOUND CONNECT
    connect->connect(seed,
        dispatch_.ordered_delegate(&session_seed::handle_connect,
            shared_from_base<session_seed>(), _1, _2, seed, handler));
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

    if (blacklisted(channel->address()))
    {
        log::debug(LOG_NETWORK)
            << "Seed is on blacklisted address [" << seed << "] ";
        handler(error::address_blocked);
        return;
    }

    log::info(LOG_NETWORK)
        << "Connected seed [" << seed << "] as " << channel->address();

    register_channel(channel, 
        std::bind(&session_seed::handle_channel_start,
            shared_from_base<session_seed>(), _1, channel, handler),
        std::bind(&session_seed::handle_channel_stop,
            shared_from_base<session_seed>(), _1));
}

void session_seed::handle_channel_start(const code& ec, channel::ptr channel,
    result_handler handler)
{
    if (ec)
        return;

    attach<protocol_ping>(channel);
    attach<protocol_seed>(channel, handler);
};

void session_seed::handle_channel_stop(const code&)
{
}

// This accepts no error code because individual seed errors are suppressed.
void session_seed::handle_complete(size_t start_size, result_handler handler)
{
    address_count(
        dispatch_.ordered_delegate(&session_seed::handle_final_count,
            shared_from_base<session_seed>(), _1, start_size, handler));
}


// We succeed only if there is a host count increase.
void session_seed::handle_final_count(size_t current_size, size_t start_size,
    result_handler handler)
{
    const auto result = current_size > start_size ? bc::error::success :
        bc::error::operation_failed;

    handler(result);
}

} // namespace network
} // namespace libbitcoin
