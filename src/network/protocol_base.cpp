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
#define CLASS protocol_base

protocol_base::protocol_base(channel::ptr channel, threadpool& pool,
    handler complete)
  : channel_(channel), dispatch_(pool), callback_(complete), stopped_(false)
{
    start_ = [this]() { subscribe_stop(); };
}

protocol_base::protocol_base(channel::ptr channel, threadpool& pool,
    const asio::duration& timeout, handler complete)
  : protocol_base(channel, pool, complete)
{
    start_ = [this, &pool, &timeout]() { subscribe_timer(pool, timeout); };
}

config::authority protocol_base::authority() const
{
    return channel_->address();
}

void protocol_base::callback(const code& ec) const
{
    if (callback_ != nullptr)
        callback_(ec);
}

void protocol_base::set_callback(handler complete)
{
    BITCOIN_ASSERT_MSG(callback_ == nullptr, "The callback cannot be reset.");
    if (callback_ == nullptr)
        callback_ = complete;
}

// Startup is deferred until after construct in order to use shared_from_this.
// We could simplify this by using boost::enable_shared_from_this which can be
// called from construct, but that requires use of boost::share_ptr as well.
void protocol_base::start()
{
    BITCOIN_ASSERT_MSG(start_ != nullptr, "The protocol cannot be restarted.");
    if (start_ == nullptr)
        return;

    start_();
    start_ = nullptr;
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
    SUBSCRIBE1(stop, handle_stop, _1);
}

void protocol_base::subscribe_timer(threadpool& pool,
    const asio::duration& timeout)
{
    deadline_ = std::make_shared<deadline>(pool, timeout);
    deadline_->start(BIND1(handle_timer, _1));
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

#undef CLASS

} // namespace network
} // namespace libbitcoin
