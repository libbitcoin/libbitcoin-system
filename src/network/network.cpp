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
#include <algorithm>
#include <functional>
#include <iostream>
#include <bitcoin/network/network.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::ip::tcp;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;

const time_duration connect_timeout = seconds(5);

acceptor::acceptor(threadpool& pool, tcp_acceptor_ptr tcp_accept)
  : pool_(pool), tcp_accept_(tcp_accept)
{
}
void acceptor::accept(accept_handler handle_accept)
{
    socket_ptr socket =
        std::make_shared<tcp::socket>(pool_.service());
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

class perform_connect_with_timeout
  : public std::enable_shared_from_this<perform_connect_with_timeout>
{
public:
    perform_connect_with_timeout(threadpool& pool)
      : timer_(pool.service())
    {
        socket_ = std::make_shared<tcp::socket>(pool.service());
        proxy_ = std::make_shared<channel_proxy>(pool, socket_);
    }

    void start(tcp::resolver::iterator endpoint_iterator,
        time_duration timeout, network::connect_handler handle_connect)
    {
        timer_.expires_from_now(timeout);
        timer_.async_wait(std::bind(
            &perform_connect_with_timeout::close, shared_from_this(), _1));

        boost::asio::async_connect(*socket_, endpoint_iterator,
            std::bind(&perform_connect_with_timeout::call_connect_handler,
                shared_from_this(), _1, _2, handle_connect));
    }

private:
    void call_connect_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator, network::connect_handler handle_connect)
    {
        if (ec)
        {
            handle_connect(error::network_unreachable, nullptr);
            return;
        }
        timer_.cancel();
        proxy_->start();
        channel_ptr channel_object = std::make_shared<channel>(proxy_);
        handle_connect(std::error_code(), channel_object);
    }

    void close(const boost::system::error_code& ec)
    {
        // ec should be boost::asio::error::operation_aborted or nothing.
        BITCOIN_ASSERT(!ec || ec == boost::asio::error::operation_aborted);
        if (!ec)
            proxy_->stop();
    }

    socket_ptr socket_;
    channel::channel_proxy_ptr proxy_;
    boost::asio::deadline_timer timer_;
};

network::network(threadpool& pool)
  : pool_(pool)
{
}

void network::resolve_handler(const boost::system::error_code& ec,
    tcp::resolver::iterator endpoint_iterator,
    connect_handler handle_connect, resolver_ptr, query_ptr)
{
    if (ec)
    {
        handle_connect(error::resolve_failed, nullptr);
        return;
    }
    auto connect =
        std::make_shared<perform_connect_with_timeout>(pool_);
    connect->start(endpoint_iterator, connect_timeout, handle_connect);
}

void network::connect(const std::string& hostname, uint16_t port,
    connect_handler handle_connect)
{
    resolver_ptr resolver =
        std::make_shared<tcp::resolver>(pool_.service());
    query_ptr query =
        std::make_shared<tcp::resolver::query>(hostname, std::to_string(port));
    resolver->async_resolve(*query,
        std::bind(&network::resolve_handler,
            this, _1, _2, handle_connect, resolver, query));
}

// I personally don't like how exceptions mess with the program flow
bool listen_error(const boost::system::error_code& ec,
    network::listen_handler handle_listen)
{
    if (ec == boost::system::errc::address_in_use)
    {
        handle_listen(error::address_in_use, nullptr);
        return true;
    }
    else if (ec)
    {
        handle_listen(error::listen_failed, nullptr);
        return true;
    }
    return false;
}
void network::listen(uint16_t port, listen_handler handle_listen)
{
    tcp::endpoint endpoint(tcp::v4(), port);
    acceptor::tcp_acceptor_ptr tcp_accept =
        std::make_shared<tcp::acceptor>(pool_.service());
    // Need to check error codes for functions
    boost::system::error_code ec;
    tcp_accept->open(endpoint.protocol(), ec);
    if (listen_error(ec, handle_listen))
        return;
    tcp_accept->set_option(tcp::acceptor::reuse_address(true), ec);
    if (listen_error(ec, handle_listen))
        return;
    tcp_accept->bind(endpoint, ec);
    if (listen_error(ec, handle_listen))
        return;
    tcp_accept->listen(boost::asio::socket_base::max_connections, ec);
    if (listen_error(ec, handle_listen))
        return;

    acceptor_ptr accept =
        std::make_shared<acceptor>(pool_, tcp_accept);
    handle_listen(std::error_code(), accept);
}

} // namespace libbitcoin

