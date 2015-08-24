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

#include <memory>
#include <functional>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;

// Require three callbacks (or any error) before calling complete.
protocol_seed::protocol_seed(channel::ptr peer, threadpool& pool,
    const asio::duration& timeout, handler complete, hosts& hosts,
    const message::network_address& self)
  : hosts_(hosts), self_({ { self } }),
    protocol_base(peer, pool, timeout, synchronize(complete, 3, "seed"))
{
    BITCOIN_ASSERT(!self_.addresses.empty());
    if (self_.addresses.front().port != 0)
        callback(error::success);
    else
        send(self_, &protocol_seed::handle_send_address,
            shared_from_base<protocol_seed>(), _1);

    if (hosts_.capacity() == 0)
    {
        // Error return ends synchronized completion callback.
        callback(error::not_found);
        return;
    }

    ////accept<address>(&protocol_seed::handle_receive_address, _1, _2);
    ////send(get_address(), &protocol_seed::handle_send_get_address, _1);
}

void protocol_seed::handle_receive_address(const code& ec,
    const address& message)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure receiving addresses from seed [" << peer() << "] "
            << ec.message();
        stop(error::bad_stream);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from seed [" << peer() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage timestamps (active peers are connected < 3 hours ago).
    hosts_.store(message.addresses,
        std::bind(&protocol_seed::handle_store_addresses,
            shared_from_base<protocol_seed>(), _1));
}

void protocol_seed::handle_send_address(const code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending address to seed [" << peer() << "] "
            << ec.message();

    // 1 of 3
    callback(ec);
}

void protocol_seed::handle_send_get_address(const code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending get_address to seed [" << peer() << "] "
            << ec.message();

    // 2 of 3
    callback(ec);
}

void protocol_seed::handle_store_addresses(const code& ec) const
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from seed [" << peer() << "] "
            << ec.message();

    // 3 of 3
    callback(ec);
}

} // namespace network
} // namespace libbitcoin
