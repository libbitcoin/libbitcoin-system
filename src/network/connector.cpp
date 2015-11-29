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
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::connector);

namespace libbitcoin {
namespace network {

#define NAME "connector"
    
using namespace bc::config;
using std::placeholders::_1;
using std::placeholders::_2;

// The resolver_, pending_, and stopped_ members are protected.

connector::connector(threadpool& pool, const settings& settings)
  : stopped_(false),
    pool_(pool),
    settings_(settings),
    dispatch_(pool, NAME),
    resolver_(std::make_shared<asio::resolver>(pool.service())),
    CONSTRUCT_TRACK(connector)
{
}

connector::~connector()
{
    BITCOIN_ASSERT_MSG(stopped(), "The connector was not stopped.");
}

// Stop sequence.
// ----------------------------------------------------------------------------

// public:
void connector::stop()
{
    // BUGBUG: unsafe stop, second context change.
    dispatch_.ordered(&connector::do_stop,
        shared_from_this());
}

void connector::do_stop()
{
    if (stopped())
        return;

    stopped_ = true;

    // This will invoke the handlers of all outstanding resolve calls.
    resolver_->cancel();

    // This will invoke the handlers of all outstanding connect calls.
    cancel();
}

bool connector::stopped()
{
    return stopped_;
}

// Pending connect clearance.
// ----------------------------------------------------------------------------

// private:
void connector::pend(asio::socket_ptr socket)
{
    pending_.push_back(socket);
}

void connector::unpend(asio::socket_ptr socket)
{
    auto it = std::find(pending_.begin(), pending_.end(), socket);

    // Cancel pending can occur before unpend.
    if (it != pending_.end())
        pending_.erase(it);
}

void connector::cancel()
{
    // The pending collection exists to allow for connect cancelation.
    // If we did not cancel connect attempts their closures would leak.
    for (auto socket: pending_)
        socket->cancel();

    pending_.clear();
}

// Connect sequence.
// ----------------------------------------------------------------------------

// public:
void connector::connect(const endpoint& endpoint, connect_handler handler)
{
    connect(endpoint.host(), endpoint.port(), handler);
}

// public:
void connector::connect(const authority& authority, connect_handler handler)
{
    connect(authority.to_hostname(), authority.port(), handler);
}

// public:
void connector::connect(const std::string& hostname, uint16_t port,
    connect_handler handler)
{
    dispatch_.ordered(&connector::do_connect,
        shared_from_this(), hostname, port, handler);
}

void connector::do_connect(const std::string& hostname, uint16_t port,
    connect_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped, nullptr);
        return;
    }

    const auto port_text = std::to_string(port);
    const auto query = std::make_shared<asio::query>(hostname, port_text);

    resolver_->async_resolve(*query, 
        dispatch_.ordered_delegate(&connector::handle_resolve,
            shared_from_this(), _1, _2, handler));
}

void connector::handle_resolve(const boost_code& ec, asio::iterator iterator,
    connect_handler handler)
{
    if (stopped())
    {
        handler(error::service_stopped, nullptr);
        return;
    }

    if (ec)
    {
        handler(error::resolve_failed, nullptr);
        return;
    }

    const auto timeout = settings_.connect_timeout();
    const auto timer = std::make_shared<deadline>(pool_, timeout);
    const auto socket = std::make_shared<asio::socket>(pool_.service());

    // Retain a socket reference until connected, allowing connect cancelation.
    pend(socket);

    // Manage the socket-timer race.
    const auto handle_connect = synchronize(handler, 1, NAME);

    // This is end #1 of the connnect sequence.
    timer->start(
        std::bind(&connector::handle_timer,
            shared_from_this(), _1, socket, handle_connect));

    // This is end #2 of the connnect sequence.
    boost::asio::async_connect(*socket, iterator,
        dispatch_.ordered_delegate(&connector::handle_connect,
            shared_from_this(), _1, _2, socket, timer, handle_connect));
}

// Timer sequence.
// ----------------------------------------------------------------------------

// private:
void connector::handle_timer(const code& ec, asio::socket_ptr socket,
   connect_handler handler)
{
    // This is the end of the timer sequence.
    if (ec)
        handler(ec, nullptr);
    else
        handler(error::channel_timeout, nullptr);
      
    socket->cancel();
}

// Connect sequence.
// ----------------------------------------------------------------------------

// private:
void connector::handle_connect(const boost_code& ec, asio::iterator,
    asio::socket_ptr socket, deadline::ptr timer, connect_handler handler)
{
    unpend(socket);

    // This is the end of the connect sequence.
    if (ec)
        handler(error::boost_to_error_code(ec), nullptr);
    else
        handler(error::success,
            std::make_shared<channel>(pool_, socket, settings_));

    timer->stop();
}

} // namespace network
} // namespace libbitcoin
