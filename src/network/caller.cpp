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
#include <bitcoin/bitcoin/network/caller.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::caller);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

// There is no way to cancel channel creation, must wait for timer.
caller::caller(threadpool& pool, uint32_t network_magic, 
    const timeout& timeouts)
  : pool_(pool), magic_(network_magic), timeouts_(timeouts),
    deadline_(std::make_shared<deadline>(pool, timeouts.connect)),
    CONSTRUCT_TRACK(caller, LOG_NETWORK)
{
}

// Calling this with an outstanding call will cause the outstnding call to
// destruct and invoke stop, canceling the timer and firing the callback.
void caller::connect(asio::iterator endpoint_iterator,
    handler handle_connect)
{
    // Handle one callback before calling handle_connect.
    // TODO: register the complete delegate with an external stop notifier.
    const auto complete = synchronize(handle_connect, 1, "caller");

    // Start the deadline timer with completion handler.
    deadline_->start(
        std::bind(&caller::handle_timer,
            shared_from_this(), _1, complete));

    // Create a socket for the connection.
    const auto socket = std::make_shared<asio::socket>(pool_.service());

    // Start the socket connection with completion handler.
    using namespace boost::asio;
    async_connect(*socket, endpoint_iterator,
        std::bind(&caller::create_channel,
            shared_from_this(), _1, _2, socket, complete));
}

void caller::create_channel(const boost_code& ec, asio::iterator,
    asio::socket_ptr socket, handler complete)
{
    deadline_->cancel();

    if (ec)
    {
        complete(error::boost_to_error_code(ec), nullptr);
        return;
    }

    // It is possible for the node to get created here after a timeout, but it
    // will subsequently self-destruct following rejection by the synchronizer.
    const auto peer = std::make_shared<channel>(pool_, socket, magic_,
        timeouts_);

    complete(error::success, peer);
}

void caller::handle_timer(const code& ec, handler complete)
{
    // The handler must not expect a node when there is an error.
    complete(error::channel_timeout, nullptr);
}

} // namespace network
} // namespace libbitcoin
