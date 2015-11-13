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
    event_handler_(nullptr)
{
}

void protocol_events::set_event(const code& ec)
{
    CALL1(do_set_event, ec);
}

void protocol_events::start()
{
    const auto unhandled = [](const code) {};
    start(unhandled);
}

void protocol_events::start(event_handler handler)
{
    if (event_handler_ || stopped())
    {
        handler(error::channel_stopped);
        return;
    }

    event_handler_ = handler;
    SUBSCRIBE_STOP1(handle_stop, _1);
}

void protocol_events::do_set_event(const code& ec)
{
    if (event_handler_)
        event_handler_(ec);

    if (ec == error::channel_stopped)
        event_handler_ = nullptr;
}

void protocol_events::handle_stop(const code& ec)
{
    log::debug(LOG_PROTOCOL)
        << "Stopped protocol_" << name() << " on [" << authority() << "] "
        << ec.message();
    
    // Event handlers can depend on this code for channel stop.
    set_event(error::channel_stopped);
}

} // namespace network
} // namespace libbitcoin
