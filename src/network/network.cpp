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
#include <bitcoin/bitcoin/network/network.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include "connect_with_timeout.hpp"

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::ip::tcp;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;

// TODO: parameterize for config access.
static const auto connect_timeout = seconds(5);

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

    const auto connect = std::make_shared<connect_with_timeout>(pool_);
    connect->start(endpoint_iterator, connect_timeout, handle_connect);
}

void network::connect(const std::string& hostname, uint16_t port,
    connect_handler handle_connect)
{
    const auto resolver = std::make_shared<tcp::resolver>(pool_.service());
    const auto query = std::make_shared<tcp::resolver::query>(
        hostname, std::to_string(port));

    resolver->async_resolve(*query,
        std::bind(&network::resolve_handler,
            this, _1, _2, handle_connect, resolver, query));
}

// I personally don't like how exceptions mess with the program flow.
static bool listen_error(const boost::system::error_code& ec,
    network::listen_handler handle_listen)
{
    if (ec == boost::system::errc::address_in_use)
    {
        handle_listen(error::address_in_use, nullptr);
        return true;
    }

    if (ec)
    {
        handle_listen(error::listen_failed, nullptr);
        return true;
    }

    return false;
}

void network::listen(uint16_t port, listen_handler handle_listen)
{
    tcp::endpoint endpoint(tcp::v4(), port);
    const auto tcp_accept =std::make_shared<tcp::acceptor>(pool_.service());

    // Need to check error codes for functions.
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

    const auto accept = std::make_shared<acceptor>(pool_, tcp_accept);
    handle_listen(error::success, accept);
}

} // namespace network
} // namespace libbitcoin
