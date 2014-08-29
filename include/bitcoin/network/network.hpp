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
#ifndef LIBBITCOIN_NETWORK_NETWORK_HPP
#define LIBBITCOIN_NETWORK_NETWORK_HPP

#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <bitcoin/define.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/threadpool.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class acceptor
  : public std::enable_shared_from_this<acceptor>
{
public:
    typedef std::shared_ptr<boost::asio::ip::tcp::acceptor> tcp_acceptor_ptr;

    typedef std::function<
        void (const std::error_code&, channel_ptr)> accept_handler;

    BC_API acceptor(threadpool& pool, tcp_acceptor_ptr tcp_accept);
    BC_API void accept(accept_handler handle_accept);

private:
    void call_handle_accept(const boost::system::error_code& ec,
        socket_ptr socket, accept_handler handle_accept);

    threadpool& pool_;
    tcp_acceptor_ptr tcp_accept_;
};

class network
{
public:
    typedef std::function<
        void (const std::error_code&, channel_ptr)> connect_handler;
    typedef std::function<
        void (const std::error_code&, acceptor_ptr)> listen_handler;

    BC_API network(threadpool& pool);

    network(const network&) = delete;
    void operator=(const network&) = delete;

    BC_API void listen(uint16_t port, listen_handler handle_listen);
    BC_API void connect(const std::string& hostname, uint16_t port,
        connect_handler handle_connect);

private:
    typedef std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
    typedef std::shared_ptr<boost::asio::ip::tcp::resolver::query> query_ptr;

    void resolve_handler(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
        connect_handler handle_connect, resolver_ptr, query_ptr);

    threadpool& pool_;
};

} // namespace libbitcoin

#endif

