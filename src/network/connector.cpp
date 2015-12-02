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
#include <mutex>
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

// Stop sequence.
// ----------------------------------------------------------------------------

// public:
void connector::stop()
{
    safe_stop();
    pending_.clear();
}

void connector::safe_stop()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    // This will asynchronously invoke the handler of each pending resolve.
    resolver_->cancel();
    stopped_ = true;
    ///////////////////////////////////////////////////////////////////////////
}

bool connector::stopped()
{
    return stopped_;
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
    if (stopped())
    {
        // We preserve the asynchronous contract of the async_resolve.
        dispatch_.unordered(handler, error::service_stopped, nullptr);
        return;
    }

    auto query = std::make_shared<asio::query>(hostname, std::to_string(port));

    safe_resolve(query, handler);
}

void connector::safe_resolve(asio::query_ptr query, connect_handler handler)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    // async_resolve will not invoke the handler within this function.
    resolver_->async_resolve(*query,
        std::bind(&connector::handle_resolve,
            shared_from_this(), _1, _2, handler));
    ///////////////////////////////////////////////////////////////////////////
}

void connector::handle_resolve(const boost_code& ec, asio::iterator iterator,
    connect_handler handler)
{
    if (stopped())
    {
        dispatch_.unordered(handler, error::service_stopped, nullptr);
        return;
    }

    if (ec)
    {
        dispatch_.concurrent(handler, error::resolve_failed, nullptr);
        return;
    }

    const auto timeout = settings_.connect_timeout();
    const auto timer = std::make_shared<deadline>(pool_, timeout);
    const auto socket = std::make_shared<asio::socket>(pool_.service());

    // Retain a socket reference until connected, allowing connect cancelation.
    pending_.store(socket);

    // Manage the socket-timer race.
    const auto handle_connect = synchronize(handler, 1, NAME);

    // This is branch #1 of the connnect sequence.
    timer->start(
        std::bind(&connector::handle_timer,
            shared_from_this(), _1, socket, handle_connect));

    // This is branch #2 of the connnect sequence.
    boost::asio::async_connect(*socket, iterator,
        std::bind(&connector::handle_connect,
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

    // BUGBUG: asio::socket is not thread safe, so this will cause a failure in
    // the case where the channel has been created and a method on the socket
    // is  executing concurrently with this close call. The only way to guard
    // against this issue is to create a socket wrapper class (lighter than
    // channel).
    proxy::close(socket);
}

// Connect sequence.
// ----------------------------------------------------------------------------

// private:
void connector::handle_connect(const boost_code& ec, asio::iterator,
    asio::socket_ptr socket, deadline::ptr timer, connect_handler handler)
{
    pending_.remove(socket);

    // This is the end of the connect sequence.
    if (ec)
        handler(error::boost_to_error_code(ec), nullptr);
    else
        handler(error::success, new_channel(socket));

    timer->stop();
}

std::shared_ptr<channel> connector::new_channel(asio::socket_ptr socket)
{
    return std::make_shared<channel>(pool_, socket, settings_);
}

} // namespace network
} // namespace libbitcoin
