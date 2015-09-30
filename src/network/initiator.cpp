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
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

const uint32_t initiator::network = 0xd9b4bef9;

initiator::initiator(threadpool& pool, uint32_t network_magic,
    const timeout& timeouts)
  : pool_(pool), magic_(network_magic), timeouts_(timeouts)
{
}

void initiator::resolve_handler(const boost_code& ec,
    asio::iterator endpoint_iterator, connector::handler handle_connect,
    asio::resolver_ptr, asio::query_ptr /* query */)
{
    if (ec)
    {
        // TODO: log query info.
        // TODO: pass query to connector for logging.
        handle_connect(error::resolve_failed, nullptr);
        return;
    }

    const auto outbound = std::make_shared<connector>(pool_, magic_,
        timeouts_);

    outbound->connect(endpoint_iterator, handle_connect);
}

// TODO: add stop().

// TODO: make cancellable (via acceptor).
void initiator::listen(uint16_t port, acceptor::listen_handler handle_listen)
{
    const auto accept = std::make_shared<asio::acceptor>(pool_.service());

    boost_code result;
    asio::endpoint endpoint(asio::tcp::v4(), port);
    accept->open(endpoint.protocol(), result);

    if (!result)
        accept->set_option(asio::acceptor::reuse_address(true), result);

    if (!result)
        accept->bind(endpoint, result);

    if (!result)
        accept->listen(asio::max_connections, result);

    const auto ec = bc::error::boost_to_error_code(result);
    const auto inbound = ec ? nullptr :
        std::make_shared<acceptor>(pool_, accept, magic_, timeouts_);

    handle_listen(ec, inbound);
}

// TODO: make cancellable (via connector).
void initiator::connect(const std::string& hostname, uint16_t port,
    connector::handler handle_connect)
{
    const auto resolve = std::make_shared<asio::resolver>(pool_.service());

    const auto text_port = std::to_string(port);
    const auto query = std::make_shared<asio::query>(hostname, text_port);
    resolve->async_resolve(*query,
        std::bind(&initiator::resolve_handler,
            this, _1, _2, handle_connect, resolve, query));
}

} // namespace network
} // namespace libbitcoin
