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
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(resolver_mutex_);

        // This will invoke the handlers of all outstanding resolve calls.
        resolver_->cancel();
    }
    ///////////////////////////////////////////////////////////////////////////

    clear();
    stopped_ = true;
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
        handler(error::service_stopped, nullptr);
        return;
    }

    const auto port_text = std::to_string(port);
    const auto query = std::make_shared<asio::query>(hostname, port_text);

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(resolver_mutex_);

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

// Pending connect.
// ----------------------------------------------------------------------------
// TODO: move this to independent class (see proxy pending).
// The pending collection exists to allow for connect cancelation.
// If we did not cancel connect attempts their closures would leak.
// The clear method invokes the handlers of all outstanding connect calls.

// private:
void connector::pend(asio::socket_ptr socket)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(pending_mutex_);

    pending_.push_back(socket);
    ///////////////////////////////////////////////////////////////////////////
}

void connector::unpend(asio::socket_ptr socket)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(pending_mutex_);

    auto it = std::find(pending_.begin(), pending_.end(), socket);

    // clear can occur before unpend, so we may not find the entry.
    if (it != pending_.end())
        pending_.erase(it);
    ///////////////////////////////////////////////////////////////////////////
}

void connector::clear()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(pending_mutex_);

    // TODO: ensure socket cancel will not invoke member handler in scope.
    // BUGBUG: socket::cancel fails with error::operation_not_supported
    // on Windows XP and Windows Server 2003, but handler invocation required.
    // We should enable BOOST_ASIO_ENABLE_CANCELIO and BOOST_ASIO_DISABLE_IOCP
    // on these platforms only. See: bit.ly/1YC0p9e
    for (auto socket : pending_)
        socket->cancel();

    pending_.clear();
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace network
} // namespace libbitcoin
