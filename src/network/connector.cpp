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
#include <bitcoin/bitcoin/network/connector.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::connector);

namespace libbitcoin {
namespace network {

#define NAME "connector"

using std::placeholders::_1;
using std::placeholders::_2;

connector::connector(threadpool& pool, const settings& settings)
  : pool_(pool),
    settings_(settings),
    resolver_(std::make_shared<asio::resolver>(pool.service())),
    CONSTRUCT_TRACK(connector)
{
}

// This will invoke the handler of all outstanding accept calls.
void connector::cancel()
{
    resolver_->cancel();
}

void connector::connect(const config::endpoint& endpoint,
    connect_handler handler)
{
    connect(endpoint.host(), endpoint.port(), handler);
}

void connector::connect(const config::authority& authority,
    connect_handler handler)
{
    connect(authority.to_hostname(), authority.port(), handler);
}

void connector::connect(const std::string& hostname, uint16_t port,
    connect_handler handler)
{
    const auto handle_connect = synchronize(handler, 1, NAME);
    const auto query = std::make_shared<asio::query>(hostname,
        std::to_string(port));

    resolver_->async_resolve(*query,
        std::bind(&connector::handle_resolve,
            shared_from_this(), _1, _2, handle_connect));
}

void connector::handle_resolve(const boost_code& ec, asio::iterator iterator,
    connect_handler handler)
{
    if (ec)
    {
        handler(error::resolve_failed, nullptr);
        return;
    }

    const auto timeout = settings_.connect_timeout();
    const auto timer = std::make_shared<deadline>(pool_, timeout);
    const auto socket = std::make_shared<asio::socket>(pool_.service());

    timer->start(
        std::bind(&connector::handle_timer,
            shared_from_this(), _1, socket, handler));

    boost::asio::async_connect(*socket, iterator,
        std::bind(&connector::handle_connect,
            shared_from_this(), _1, _2, socket, timer, handler));
}

void connector::handle_timer(const code& ec, asio::socket_ptr socket,
   connect_handler handler)
{
    if (ec)
        handler(ec, nullptr);
    else
        handler(error::channel_timeout, nullptr);

    socket->cancel();
}

void connector::handle_connect(const boost_code& ec, asio::iterator,
    asio::socket_ptr socket, deadline::ptr timer, connect_handler handler)
{
    if (ec)
        handler(error::boost_to_error_code(ec), nullptr);
    else
        handler(error::success,
            std::make_shared<channel>(pool_, socket, settings_));

    timer->cancel();
}

} // namespace network
} // namespace libbitcoin
