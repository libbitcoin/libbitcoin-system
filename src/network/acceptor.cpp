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
#include <bitcoin/bitcoin/network/acceptor.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using boost::asio::ip::tcp;

acceptor::acceptor(threadpool& pool, tcp_acceptor_ptr accept,
    const timeout& timeouts)
  : pool_(pool), times_(timeouts), tcp_acceptor_(accept)
{
}

void acceptor::accept(accept_handler handle_accept)
{
    const auto socket = std::make_shared<tcp::socket>(pool_.service());
    tcp_acceptor_->async_accept(*socket,
        std::bind(&acceptor::call_handle_accept,
            shared_from_this(), _1, socket, handle_accept));
}

void acceptor::call_handle_accept(const boost::system::error_code& ec,
    socket_ptr socket, accept_handler handle_accept)
{
    if (ec)
    {
        handle_accept(error::accept_failed, nullptr);
        return;
    }

    const auto proxy = std::make_shared<channel_proxy>(pool_, socket, times_);
    const auto channel_object = std::make_shared<channel>(proxy);
    handle_accept(error::success, channel_object);

    // EKV 7/31/2015: moved this after handle_accept because of a race failure.
    // start->read_header() would process messages before handle_accept() would
    // have called the handshake registrations. So handshake could miss the
    // version message and never complete the handshake. The handshake would
    // hang until a disconnect was forced by the client or the server expired
    // the connection. See also connect_with_timeout::call_handle_connect.
    proxy->start();
}

} // namespace network
} // namespace libbitcoin
