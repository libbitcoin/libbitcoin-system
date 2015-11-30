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
#include <bitcoin/bitcoin/network/protocol_events.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define CLASS protocol_events
using std::placeholders::_1;

protocol_events::protocol_events(threadpool& pool, channel::ptr channel,
    const std::string& name)
  : protocol(pool, channel, name),
    stopped_(true),
    event_handler_(nullptr)
{
}

// Properties.
// ----------------------------------------------------------------------------

// protected:
bool protocol_events::stopped() const
{
    // This is a weak indication, but sufficient for shutdown initiation.
    return stopped_;
}

// Start sequence.
// ----------------------------------------------------------------------------

// protected:
void protocol_events::start(event_handler handler)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(event_mutex_);

        event_handler_ = handler;
    }
    ///////////////////////////////////////////////////////////////////////////

    stopped_ = false;
    SUBSCRIBE_STOP1(handle_stopped, _1);
}

// Stop (invoked by stop handler only).
// ----------------------------------------------------------------------------

// private:
void protocol_events::handle_stopped(const code& ec)
{
    log::debug(LOG_PROTOCOL)
        << "Stop protocol_" << name() << " on [" << authority() << "] "
        << ec.message();
    
    // Event handlers can depend on this code for channel stop.
    set_event(error::channel_stopped);
}

// Set event.
// ----------------------------------------------------------------------------

// protected:
void protocol_events::set_event(const code& ec)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(event_mutex_);

    if (!event_handler_)
        return;

    // This will deadlock if event_handler_ invokes this set_event.
    event_handler_(ec);

    // A lock is required in order to clear the closure on stop.
    // A boolean stopped is used to avoid a read lock on each stop test.
    if (ec == error::channel_stopped)
    {
        stopped_ = true;
        event_handler_ = nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace network
} // namespace libbitcoin
