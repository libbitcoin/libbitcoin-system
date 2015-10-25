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
#include <bitcoin/bitcoin/network/protocol.hpp>

#include <functional>
#include <memory>
#include <string>
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

protocol::protocol(threadpool& pool, channel::ptr channel,
    const std::string& name, completion_handler handler)
  : name_(name),
    dispatch_(pool),
    channel_(channel),
    completion_handler_(handler)
{
    start_ = [this]()
    {
        subscribe_stop();
    };
}

protocol::protocol(threadpool& pool, channel::ptr channel,
    const asio::duration& timeout, const std::string& name,
    completion_handler handler)
  : protocol(pool, channel, name, handler)
{
    start_ = [this, &pool, &timeout]()
    {
        subscribe_timer(pool, timeout);
    };
}

config::authority protocol::authority() const
{
    return stopped() ? config::authority() : channel_->address();
}

void protocol::complete(const code& ec) const
{
    if (completion_handler_ != nullptr)
        completion_handler_(ec);
}

// This must only be called from start() and before any subscriptions.
// Ideally avoid this, but it works around no self-closure in construct.
void protocol::set_handler(completion_handler handler)
{
    BITCOIN_ASSERT_MSG(handler == nullptr, "The callback cannot be reset.");
    if (completion_handler_ == nullptr)
        completion_handler_ = handler;
}

void protocol::set_version(const message::version& value)
{
    if (!stopped())
        channel_->set_version(value);
}

// Startup is deferred until after construct in order to use shared_from_this.
// We could simplify this by using boost::enable_shared_from_this which can be
// called from construct, but that requires use of boost::share_ptr as well.
void protocol::start()
{
    BITCOIN_ASSERT_MSG(start_ != nullptr, "Protocol cannot be restarted.");

    if (start_ == nullptr)
        return;

    start_();
    start_ = nullptr;
}

// Protocols either stop themselves or stop on channel stop.
void protocol::stop(const code& ec)
{
    if (stopped())
        return;

    channel_->stop(ec);
}

bool protocol::stopped() const
{
    return start_ != nullptr || channel_ == nullptr;
}

void protocol::subscribe_stop()
{
    if (stopped())
        return;

    channel_->subscribe_stop(
        dispatch_.unordered_delegate(&protocol::handle_stop,
            shared_from_this(), _1));
}

void protocol::subscribe_timer(threadpool& pool,
    const asio::duration& timeout)
{
    if (stopped())
        return;

    deadline_ = std::make_shared<deadline>(pool, timeout);
    deadline_->start(
        dispatch_.unordered_delegate(&protocol::handle_timer,
            shared_from_this(), _1));
}
    
void protocol::handle_stop(const code& ec)
{
    // All dynamic calls must be stranded in order to protect this resource.
    channel_ = nullptr;

    log_debug(LOG_PROTOCOL)
        << "Stopped " << name_ << " protocol on [" << authority() << "] "
        << ec.message();

    if (deadline_)
        deadline_->cancel();

    complete(ec);
}

void protocol::handle_timer(const code& ec)
{
    if (stopped())
        return;

    log_debug(LOG_PROTOCOL)
        << "Fired " << name_ << " protocol timer on [" << authority() << "] "
        << ec.message();

    complete(error::channel_timeout);
}

} // namespace network
} // namespace libbitcoin
