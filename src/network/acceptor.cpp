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

#include <cstdint>
#include <memory>
#include <iostream>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

#define NAME "acceptor"

INITIALIZE_TRACK(bc::network::acceptor);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;

acceptor::acceptor(threadpool& pool, const settings& settings)
  : pool_(pool),
    settings_(settings),
    acceptor_(std::make_shared<asio::acceptor>(pool_.service())),
    CONSTRUCT_TRACK(acceptor)
{
}

// This will invoke the handler of all outstanding accept calls.
void acceptor::cancel()
{
    acceptor_->cancel();
}

// This is hardwired to listen on IPv6.
void acceptor::listen(uint16_t port, result_handler handler)
{
    boost_code ec;
    asio::endpoint endpoint(asio::tcp::v6(), settings_.inbound_port);
    acceptor_->open(endpoint.protocol(), ec);

    if (!ec)
        acceptor_->set_option(asio::acceptor::reuse_address(true), ec);

    if (!ec)
        acceptor_->bind(endpoint, ec);

    if (!ec)
        acceptor_->listen(asio::max_connections, ec);

    handler(error::boost_to_error_code(ec));
}

// This can be called repeatedly, but doesn't need to be.
void acceptor::accept(accept_handler handler)
{
    const auto handle_accept = synchronize(handler, 1, NAME);

    const auto socket = std::make_shared<asio::socket>(pool_.service());
    acceptor_->async_accept(*socket,
        std::bind(&acceptor::handle_accept,
            shared_from_this(), _1, socket, handle_accept));
}

void acceptor::handle_accept(const boost_code& ec, asio::socket_ptr socket,
    accept_handler handler)
{
    if (ec)
        handler(error::boost_to_error_code(ec), nullptr);
    else
        handler(error::success, 
            std::make_shared<channel>(pool_, socket, settings_));
}

} // namespace network
} // namespace libbitcoin
