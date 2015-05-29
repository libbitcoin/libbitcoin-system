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
#ifndef LIBBITCOIN_ACCEPTOR_HPP
#define LIBBITCOIN_ACCEPTOR_HPP

#include <functional>
#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class acceptor;

// TODO: move acceptor_ptr into acceptor as public type (interface break).
typedef std::shared_ptr<acceptor> acceptor_ptr;

class BC_API acceptor
  : public std::enable_shared_from_this<acceptor>
{
public:
    typedef std::shared_ptr<boost::asio::ip::tcp::acceptor> tcp_acceptor_ptr;
    typedef std::function<void (const std::error_code&, channel_ptr)>
        accept_handler;

     acceptor(threadpool& pool, tcp_acceptor_ptr tcp_accept);

     /// This class is not copyable.
     acceptor(const acceptor&) = delete;
     void operator=(const acceptor&) = delete;

     void accept(accept_handler handle_accept);

private:
    void call_handle_accept(const boost::system::error_code& ec,
        socket_ptr socket, accept_handler handle_accept);

    threadpool& pool_;
    tcp_acceptor_ptr tcp_accept_;
};

} // namespace network
} // namespace libbitcoin

#endif

