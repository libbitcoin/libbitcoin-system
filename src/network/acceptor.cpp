/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see COPYING.
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
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using boost::asio::ip::tcp;

acceptor::acceptor(threadpool& pool, tcp_acceptor_ptr accept,
    const timeout& timeouts)
  : pool_(pool), timeouts_(timeouts), tcp_acceptor_(accept)
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
        handle_accept(error::boost_to_error_code(ec), nullptr);
        return;
    }

    const auto node = std::make_shared<channel>(pool_, socket, timeouts_);
    handle_accept(error::success, node);
}

} // namespace network
} // namespace libbitcoin
