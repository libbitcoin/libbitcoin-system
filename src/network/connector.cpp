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
#include <bitcoin/bitcoin/network/connector.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/peer_to_peer.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::ip::tcp;

std::mutex callback_handled_mutex;

connector::connector(threadpool& pool, const timeout& timeouts)
  : pool_(pool), timeouts_(timeouts), timer_(pool.service())
{
}

void connector::start(tcp::resolver::iterator endpoint_iterator,
    connect_handler handle_connect)
{
    const auto socket = std::make_shared<tcp::socket>(pool_.service());

    // Handle one callback before calling handle_connect.
    const auto complete = synchronizer<connect_handler>(handle_connect, 1,
        "connector");

    timer_.expires_from_now(timeouts_.connect);
    timer_.async_wait(
        std::bind(&connector::handle_timer,
            shared_from_this(), _1, complete));

    boost::asio::async_connect(*socket, endpoint_iterator,
        std::bind(&connector::call_handle_connect,
            shared_from_this(), _1, _2, socket, complete));
}

void connector::call_handle_connect(const boost::system::error_code& ec,
    tcp::resolver::iterator, socket_ptr socket, connect_handler handle_connect)
{
    // Speed up the demise of the timer.
    boost::system::error_code code;
    timer_.cancel(code);

    if (ec)
    {
        handle_connect(error::boost_to_error_code(ec), nullptr);
        return;
    }

    // It is possible for the node to get created here after a timeout, but it
    // will subsequently self-destruct following rejection by the synchronizer.
    const auto node = std::make_shared<channel>(pool_, socket, timeouts_);
    handle_connect(error::success, node);
}

void connector::handle_timer(const boost::system::error_code& ec,
    connect_handler handle_connect)
{
    // A success code implies that the timer fired.
    if (!ec)
        handle_connect(error::channel_timeout, nullptr);
}

} // namespace network
} // namespace libbitcoin
