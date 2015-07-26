/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include "connect_with_timeout.hpp"

#include <functional>
#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::ip::tcp;
using boost::posix_time::time_duration;

connect_with_timeout::connect_with_timeout(threadpool& pool,
    const timeout& timeouts)
  : timer_(pool.service()),
    connection_timeout_(timeouts.connection),
    socket_(std::make_shared<tcp::socket>(pool.service())),
    proxy_(std::make_shared<channel_proxy>(pool, socket_, timeouts))
{
}

void connect_with_timeout::start(tcp::resolver::iterator endpoint_iterator,
    network::connect_handler connect_handler)
{
    timer_.expires_from_now(connection_timeout_);
    timer_.async_wait(
        std::bind(&connect_with_timeout::handle_timer,
            shared_from_this(), _1));

    boost::asio::async_connect(*socket_, endpoint_iterator,
        std::bind(&connect_with_timeout::handle_connect,
            shared_from_this(), _1, _2, connect_handler));
}

void connect_with_timeout::handle_connect(
    const boost::system::error_code& ec, tcp::resolver::iterator, 
    network::connect_handler connect_handler)
{
    timer_.cancel();

    if (ec)
    {
        connect_handler(error::boost_to_error_code(ec), nullptr);
        return;
    }

    proxy_->start();
    const auto channel_object = std::make_shared<channel>(proxy_);
    connect_handler(error::success, channel_object);
}

void connect_with_timeout::handle_timer(const boost::system::error_code& ec)
{
    // If there is no error the timer fired because of expiration.
    // Otherwise we canceled the timer explicitly due to another error.
    if (!ec)
        proxy_->stop(error::channel_timeout);
}

} // namespace network
} // namespace libbitcoin
