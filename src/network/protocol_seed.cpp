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
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
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

protocol_base::handler protocol_seed::synchronizer(handler complete)
{
    return synchronize(complete, 3, NAME);
}

// Require three callbacks (or any error) before calling complete.
protocol_seed::protocol_seed(channel::ptr peer, threadpool& pool,
    const asio::duration& timeout, handler complete, hosts& hosts,
    const config::authority& self)
  : hosts_(hosts),
    self_(self),
    protocol_base(peer, pool, timeout, NAME, synchronizer(complete)),
    CONSTRUCT_TRACK(protocol_seed, LOG_NETWORK)
{
}

void protocol_seed::start()
{
    protocol_base::start();

    if (self_.port() == 0)
    {
        callback(error::success);
    }
    else
    {
        address self({ { self_.to_network_address() } });
        send<protocol_seed>(self, &protocol_seed::handle_send_address, _1);
    }

    if (hosts_.capacity() == 0)
    {
        // Stops channel and ends callback synchronization.
        stop(error::not_found);
        return;
    }

    subscribe<protocol_seed, address>(&protocol_seed::handle_receive_address, _1, _2);
    send<protocol_seed>(get_address(), &protocol_seed::handle_send_get_address, _1);
}

void protocol_seed::handle_receive_address(const code& ec,
    const address& message)
{
    if (stopped())
        return;

    if (ec)
    {
        // We are getting here with channel stopped because this session
        // doesn't register a stop handler. We may be getting stopped due to
        // failure to handle ping on this session.
        log_debug(LOG_PROTOCOL)
            << "Failure receiving addresses from seed [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from seed [" << authority() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage timestamps (active peers are connected < 3 hours ago).
    hosts_.store(message.addresses, 
        bind<protocol_seed>(&protocol_seed::handle_store_addresses, _1));
}

void protocol_seed::handle_send_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending address to seed [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 1 of 3
    callback(error::success);
}

void protocol_seed::handle_send_get_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending get_address to seed [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 2 of 3
    callback(error::success);
}

void protocol_seed::handle_store_addresses(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from seed [" << authority() << "] "
            << ec.message();
        stop(ec);
        return;
    }

    // 3 of 3
    callback(error::success);
    stop(error::channel_stopped);
}

} // namespace network
} // namespace libbitcoin
