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
#include <bitcoin/bitcoin/network/protocol_base.hpp>

#include <memory>
#include <functional>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using std::placeholders::_1;

protocol_base::protocol_base(channel::ptr channel, threadpool& pool,
    handler complete)
  : channel_(channel), dispatch_(pool), callback_(complete), stopped_(false)
{
    subscribe_stop();
}

protocol_base::protocol_base(channel::ptr channel, threadpool& pool,
    const asio::duration& timeout, handler complete)
  : protocol_base(channel, pool, complete)
{
    subscribe_timer(pool, timeout);
}

void protocol_base::callback(const code& ec) const
{
    if (callback_ != nullptr)
        callback_(ec);
}

config::authority protocol_base::authority() const
{
    return channel_->address();
}

void protocol_base::stop(const code& ec)
{
    if (!stopped())
        channel_->stop(ec);
}

bool protocol_base::stopped() const
{
    return stopped_;
}

void protocol_base::subscribe_stop()
{
    channel_->subscribe_stop(
        dispatch_.sync(&protocol_base::handle_stop,
            shared_from_base<protocol_base>(), _1));
}

void protocol_base::subscribe_timer(threadpool& pool,
    const asio::duration& timeout)
{
    deadline_ = std::make_shared<deadline>(pool, timeout);
    deadline_->start(
        std::bind(&protocol_base::handle_timer,
            shared_from_base<protocol_base>(), _1));
}

void protocol_base::handle_stop(const code& ec)
{
    if (stopped())
        return;

    stopped_ = true;

    if (deadline_)
        deadline_->cancel();

    callback(ec);
}

void protocol_base::handle_timer(const code& ec) const
{
    if (stopped() || deadline::canceled(ec))
        return;

    callback(error::channel_timeout);
}

} // namespace network
} // namespace libbitcoin
