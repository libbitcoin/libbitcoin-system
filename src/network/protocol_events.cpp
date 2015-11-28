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
    return stopped_;
}

// Start sequence.
// ----------------------------------------------------------------------------

// protected:
void protocol_events::start(event_handler handler)
{
    stopped_ = false;

    // This is not cleared until destruct, so cannot contain a self-reference.
    event_handler_ = handler;

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
    if (stopped())
        return;

    event_handler_(ec);

    // We cannot set event handler to null since this method is not protected.
    if (ec == error::channel_stopped)
        stopped_ = true;
}

} // namespace network
} // namespace libbitcoin
