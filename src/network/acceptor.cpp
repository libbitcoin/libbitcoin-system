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
#include <iostream>
#include <memory>
#include <mutex>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::acceptor);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;

static const auto reuse_address = asio::acceptor::reuse_address(true);

// The acceptor_ member is protected against concurrent access.
// This is safe against deadlock as handlers are invoked outside of the
// critical sections. We can't use a dispatch guard becuase of context change.
acceptor::acceptor(threadpool& pool, const settings& settings)
  : stopped_(true),
    pool_(pool),
    settings_(settings),
    acceptor_(std::make_shared<asio::acceptor>(pool_.service())),
    CONSTRUCT_TRACK(acceptor)
{
}

acceptor::~acceptor()
{
    BITCOIN_ASSERT_MSG(stopped(), "The acceptor was not stopped.");
}

// Stop sequence.
// ----------------------------------------------------------------------------

// public:
void acceptor::stop()
{
    stopped_ = true;

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(acceptor_mutex_);

    acceptor_->close();
    ///////////////////////////////////////////////////////////////////////////
}

// This is only used to validate shutdown.
bool acceptor::stopped()
{
    return stopped_;
}

// Listen sequence.
// ----------------------------------------------------------------------------

// public:
// This is hardwired to listen on IPv6.
void acceptor::listen(uint16_t port, result_handler handler)
{
    stopped_ = false;
    code ec(error::operation_failed);

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(acceptor_mutex_);

        if (acceptor_->is_open())
        {
            boost_code error;
            asio::endpoint endpoint(asio::tcp::v6(), settings_.inbound_port);

            acceptor_->open(endpoint.protocol(), error);

            if (!error)
                acceptor_->set_option(reuse_address, error);

            if (!error)
                acceptor_->bind(endpoint, error);

            if (!error)
                acceptor_->listen(asio::max_connections, error);

            ec = error::boost_to_error_code(error);
        }
    }
    ///////////////////////////////////////////////////////////////////////////

    // This is the end of the listen sequence.
    handler(ec);
}

// Accept sequence.
// ----------------------------------------------------------------------------

// public:
void acceptor::accept(accept_handler handler)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(acceptor_mutex_);

        if (acceptor_->is_open())
        {
            auto socket = std::make_shared<asio::socket>(pool_.service());

            // async_accept will not invoke the handler within this function.
            acceptor_->async_accept(*socket,
                std::bind(&acceptor::handle_accept,
                    shared_from_this(), _1, socket, handler));
        }

        return;
    }
    ///////////////////////////////////////////////////////////////////////////

    // TODO: this must be concurrent.
    handler(error::service_stopped, nullptr);
}

void acceptor::handle_accept(const boost_code& ec, asio::socket_ptr socket,
    accept_handler handler)
{
    // This is the end of the accept sequence.
    if (ec)
        handler(error::boost_to_error_code(ec), nullptr);
    else
        handler(error::success,
            std::make_shared<channel>(pool_, socket, settings_));
}

} // namespace network
} // namespace libbitcoin
