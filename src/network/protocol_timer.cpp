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
#include <bitcoin/bitcoin/network/protocol_timer.hpp>

#include <functional>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/protocol_events.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define CLASS protocol_timer
using std::placeholders::_1;

protocol_timer::protocol_timer(threadpool& pool, channel::ptr channel,
    const std::string& name)
  : protocol_events(pool, channel, name)
{
}

void protocol_timer::start(const asio::duration& timeout,
    event_handler handler)
{
    deadline_ = std::make_shared<deadline>(pool(), timeout);
    reset_timer();

    protocol_events::start(BIND2(handle_event, _1, handler));
}

void protocol_timer::cancel_timer()
{
    deadline_->cancel();
}

void protocol_timer::reset_timer()
{
    if (stopped())
        return;

    deadline_->start(BIND1(handle_timer, _1));
}

void protocol_timer::handle_timer(const code& ec)
{
    if (stopped())
        return;

    log::debug(LOG_PROTOCOL)
        << "Fired protocol_" << name() << " timer on [" << authority() << "] "
        << ec.message();

    set_event(error::channel_timeout);
}

void protocol_timer::handle_event(const code& ec, event_handler handler)
{
    if (ec == error::channel_stopped)
        cancel_timer();

    handler(ec);
}

} // namespace network
} // namespace libbitcoin
