/*
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
#include <bitcoin/bitcoin/network/acceptor.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using boost::asio::ip::tcp;

acceptor::acceptor(threadpool& pool, tcp_acceptor_ptr tcp_accept)
  : pool_(pool), tcp_accept_(tcp_accept)
{
}

void acceptor::accept(accept_handler handle_accept)
{
    socket_ptr socket = std::make_shared<tcp::socket>(pool_.service());
    tcp_accept_->async_accept(*socket,
        std::bind(&acceptor::call_handle_accept, shared_from_this(),
            _1, socket, handle_accept));
}

void acceptor::call_handle_accept(const boost::system::error_code& ec,
    socket_ptr socket, accept_handler handle_accept)
{
    if (ec)
    {
        handle_accept(error::accept_failed, nullptr);
        return;
    }

    auto proxy = std::make_shared<channel_proxy>(pool_, socket);
    proxy->start();
    channel_ptr channel_object = std::make_shared<channel>(proxy);
    handle_accept(std::error_code(), channel_object);
}

} // namespace network
} // namespace libbitcoin
