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
#include <bitcoin/bitcoin/network/protocol_seed.hpp>

#include <functional>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_timer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::protocol_seed);

namespace libbitcoin {
namespace network {

#define NAME "seed"
#define CLASS protocol_seed

using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;

// Require three callbacks (or any error) before calling complete.
protocol_seed::protocol_seed(threadpool& pool, p2p& network,
    channel::ptr channel)
  : protocol_timer(pool, channel, NAME),
    network_(network),
    CONSTRUCT_TRACK(protocol_seed, LOG_PROTOCOL)
{
}

void protocol_seed::start(const settings& settings, event_handler handler)
{
    if (settings.host_pool_capacity == 0)
    {
        // Stops channel and ends callback synchronization.
        handler(error::not_found);
        return;
    }

    // The synchronizer is the only object that is aware of completion.
    const auto seeding_complete = BIND2(handle_seeding_complete, _1, handler);

    protocol_timer::start(settings.channel_germination(),
        synchronize(seeding_complete, 3, NAME));

    send_own_address(settings);
    SUBSCRIBE2(address, handle_receive_address, _1, _2);
    SEND1(get_address(), handle_send_get_address, _1);
}

void protocol_seed::send_own_address(const settings& settings)
{
    if (settings.self.port() == 0)
    {
        set_event(error::success);
        return;
    }

    address self({ { settings.self.to_network_address() } });
    SEND1(self, handle_send_address, _1);
}

void protocol_seed::handle_seeding_complete(const code& ec,
    event_handler handler)
{
    cancel_timer();
    handler(ec);

    if (ec)
        stop(ec);
}

void protocol_seed::handle_receive_address(const code& ec,
    const address& message)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure receiving addresses from seed [" << authority() << "] "
            << ec.message();
        set_event(ec);
        return;
    }

    log::debug(LOG_PROTOCOL)
        << "Storing addresses from seed [" << authority() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage timestamps (active channels are connected < 3 hours ago).
    network_.store(message.addresses, BIND1(handle_store_addresses, _1));
}

void protocol_seed::handle_send_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure sending address to seed [" << authority() << "] "
            << ec.message();
        set_event(ec);
        return;
    }

    // 1 of 3
    set_event(error::success);
}

void protocol_seed::handle_send_get_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure sending get_address to seed [" << authority() << "] "
            << ec.message();
        set_event(ec);
        return;
    }

    // 2 of 3
    set_event(error::success);
}

void protocol_seed::handle_store_addresses(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure storing addresses from seed [" << authority() << "] "
            << ec.message();
        set_event(ec);
        return;
    }

    log::debug(LOG_PROTOCOL)
        << "Stopping completed seed [" << authority() << "] ";

    // 3 of 3
    set_event(error::channel_stopped);
}

} // namespace network
} // namespace libbitcoin
