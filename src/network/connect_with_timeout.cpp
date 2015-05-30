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

connect_with_timeout::connect_with_timeout(threadpool& pool)
  : timer_(pool.service()),
    socket_(std::make_shared<tcp::socket>(pool.service())),
    proxy_(std::make_shared<channel_proxy>(pool, socket_))
{
}

void connect_with_timeout::start(tcp::resolver::iterator endpoint_iterator,
    time_duration timeout, network::connect_handler handle_connect)
{
    timer_.expires_from_now(timeout);
    timer_.async_wait(std::bind(
        &connect_with_timeout::close,
            shared_from_this(), _1));

    boost::asio::async_connect(*socket_, endpoint_iterator,
        std::bind(&connect_with_timeout::call_connect_handler,
            shared_from_this(), _1, _2, handle_connect));
}

void connect_with_timeout::call_connect_handler(
    const boost::system::error_code& ec, tcp::resolver::iterator, 
    network::connect_handler handle_connect)
{
    if (ec)
    {
        handle_connect(error::network_unreachable, nullptr);
        return;
    }

    timer_.cancel();
    proxy_->start();
    const auto channel_object = std::make_shared<channel>(proxy_);
    handle_connect(error::success, channel_object);
}

void connect_with_timeout::close(const boost::system::error_code& ec)
{
    // ec should be boost::asio::error::operation_aborted or nothing.
    BITCOIN_ASSERT(!ec || ec == boost::asio::error::operation_aborted);

    if (!ec)
        proxy_->stop();
}

} // namespace network
} // namespace libbitcoin
