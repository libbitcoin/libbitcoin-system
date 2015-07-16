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
#include <cstdint>
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

network::network(threadpool& pool, uint32_t timeout_seconds)
  : pool_(pool), timeout_(seconds(timeout_seconds))
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
    connect->start(endpoint_iterator, timeout_, handle_connect);
}

void network::connect(const std::string& hostname, uint16_t port,
    connect_handler handle_connect)
{
    const auto resolver = std::make_shared<tcp::resolver>(pool_.service());
    const auto query = std::make_shared<tcp::resolver::query>(hostname,
        std::to_string(port));

    resolver->async_resolve(*query,
        std::bind(&network::resolve_handler,
            this, _1, _2, handle_connect, resolver, query));
}

void network::listen(uint16_t port, listen_handler handle_listen)
{
    using namespace boost::asio;
    boost::system::error_code boost_ec;
    tcp::endpoint endpoint(tcp::v4(), port);
    const auto tcp_accept = std::make_shared<tcp::acceptor>(pool_.service());

    tcp_accept->open(endpoint.protocol(), boost_ec);

    if (!boost_ec)
        tcp_accept->set_option(tcp::acceptor::reuse_address(true), boost_ec);

    if (!boost_ec)
        tcp_accept->bind(endpoint, boost_ec);

    if (!boost_ec)
        tcp_accept->listen(socket_base::max_connections, boost_ec);

    const auto ec = bc::error::boost_to_error_code(boost_ec);
    const auto accept = ec ? nullptr :
        std::make_shared<acceptor>(pool_, tcp_accept);

    handle_listen(ec, accept);
}

} // namespace network
} // namespace libbitcoin
