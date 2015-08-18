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
#include <bitcoin/bitcoin/network/initiator.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::ip::tcp;

initiator::initiator(threadpool& pool, const timeout& timeouts)
  : pool_(pool), timeouts_(timeouts)
{
}

void initiator::resolve_handler(const boost::system::error_code& ec,
    tcp::resolver::iterator endpoint_iterator, 
    connector::connect_handler handle_connect, resolver_ptr,
    query_ptr /* query */)
{
    if (ec)
    {
        // TODO: log query info.
        // TODO: pass query to connector for logging.
        handle_connect(error::resolve_failed, nullptr);
        return;
    }

    const auto outbound = std::make_shared<connector>(pool_, timeouts_);
    outbound->connect(endpoint_iterator, handle_connect);
}

void initiator::listen(uint16_t port, acceptor::listen_handler handle_listen)
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
    const auto inbound = ec ? nullptr :
        std::make_shared<acceptor>(pool_, tcp_accept, timeouts_);

    handle_listen(ec, inbound);
}

void initiator::connect(const std::string& hostname, uint16_t port,
    connector::connect_handler handle_connect)
{
    const auto resolver = std::make_shared<tcp::resolver>(pool_.service());
    const auto query = std::make_shared<tcp::resolver::query>(hostname,
        std::to_string(port));

    resolver->async_resolve(*query,
        std::bind(&initiator::resolve_handler,
            this, _1, _2, handle_connect, resolver, query));
}

} // namespace network
} // namespace libbitcoin
