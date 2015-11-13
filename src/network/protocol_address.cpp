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
#include <bitcoin/bitcoin/network/protocol_address.hpp>

#include <functional>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_events.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::protocol_address);

namespace libbitcoin {
namespace network {

#define NAME "address"
#define CLASS protocol_address

using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;

protocol_address::protocol_address(threadpool& pool, p2p& network,
    channel::ptr channel)
  : protocol_events(pool, channel, NAME),
    network_(network),
    CONSTRUCT_TRACK(protocol_address, LOG_PROTOCOL)
{
}

void protocol_address::start(const settings& settings)
{
    if (settings.self.port() != 0)
    {
        self_.addresses.push_back(settings.self.to_network_address());
        SEND1(self_, handle_send_address, _1);
    }

    // If we can't store addresses we don't ask for or receive them.
    if (settings.host_pool_capacity == 0)
        return;

    protocol_events::start();

    SUBSCRIBE2(get_address, handle_receive_get_address, _1, _2);
    SEND1(get_address(), handle_send_get_address, _1);
}

void protocol_address::handle_receive_address(const code& ec,
    const address& message)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure receiving address message from ["
            << authority() << "] " << ec.message();
        stop(ec);
        return;
    }

    // Resubscribe to address messages.
    SUBSCRIBE2(address, handle_receive_address, _1, _2);

    log::debug(LOG_PROTOCOL)
        << "Storing addresses from [" << authority() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage timestamps (active channels are connected < 3 hours ago).
    network_.store(message.addresses, BIND1(handle_store_addresses, _1));
}

void protocol_address::handle_receive_get_address(const code& ec,
    const get_address& message)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure receiving get_address message from ["
            << authority() << "] " << ec.message();
        stop(ec);
        return;
    }

    // TODO: allowing repeated queries can allow a channel to map our history.
    // Resubscribe to get_address messages.
    SUBSCRIBE2(get_address, handle_receive_get_address, _1, _2);

    // TODO: pull active hosts from host cache (currently just resending self).
    // TODO: need to distort for privacy, don't send currently-connected peers.
    if (self_.addresses.empty())
        return;

    log::debug(LOG_PROTOCOL)
        << "Sending addresses to [" << authority() << "] ("
        << self_.addresses.size() << ")";

    SEND1(self_, handle_send_address, _1);
}

void protocol_address::handle_send_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure sending address [" << authority() << "] "
            << ec.message();
        stop(ec);
    }
}

void protocol_address::handle_send_get_address(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::debug(LOG_PROTOCOL)
            << "Failure sending get_address [" << authority() << "] "
            << ec.message();
        stop(ec);
    }
}

void protocol_address::handle_store_addresses(const code& ec)
{
    if (stopped())
        return;

    if (ec)
    {
        log::error(LOG_PROTOCOL)
            << "Failure storing addresses from [" << authority() << "] "
            << ec.message();
        stop(ec);
    }
}

} // namespace network
} // namespace libbitcoin
