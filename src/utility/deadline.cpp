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
#include <bitcoin/bitcoin/utility/deadline.hpp>

#include <functional>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::deadline);

namespace libbitcoin {

using std::placeholders::_1;

// Use these to make sude codes stay in sync.
static const auto std_timeout = error::channel_timeout;
static const auto std_cancel = error::operation_failed;
static const auto boost_cancel = asio::error::operation_aborted;

// This can be dereferenced with an outstanding callback because the timer
// closure captures an instance of this class and the callback.
// This is guaranteed to call handler exactly once (unless prepetually reset).
deadline::deadline(threadpool& pool, const asio::duration duration)
  : duration_(duration), timer_(pool.service()),
    track("deadline", LOG_NETWORK)
{
}

// static, allows caller to test for timer cancellation in handler.
bool deadline::canceled(const code& ec)
{
    return ec == std_cancel;
}

// static, allows caller to test for timer expiration in handler.
bool deadline::expired(const code& ec)
{
    return ec == std_timeout;
}

void deadline::start(handler handle)
{
    start(handle, duration_);
}

void deadline::start(handler handle, const asio::duration duration)
{
    cancel();
    timer_.expires_from_now(duration);
    timer_.async_wait(
        std::bind(&deadline::handle_timer,
            shared_from_this(), _1, handle));
}

// Ddestruct will not happen until the timer is canceled or expires.
// Cancellation calls handle_timer with asio::error::operation_aborted.
void deadline::cancel()
{
    boost_code ec;
    timer_.cancel(ec);

    // If cancellation fails the call to handle_timer would be delayed until
    // the timer actually fires. So there is no need to handle this error, it
    // just results in a slower completion (and is very unlikely to occur).
    BITCOIN_ASSERT(!ec);
}

// A boost success code implies that the timer fired.
void deadline::handle_timer(const boost_code& ec, handler handle) const
{
    if (!ec)
        handle(std_timeout);
    else if (boost_cancel)
        handle(std_cancel);
    else
        handle(error::boost_to_error_code(ec));
}

} // namespace libbitcoin
