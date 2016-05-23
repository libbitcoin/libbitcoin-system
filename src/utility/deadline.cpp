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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/deadline.hpp>

#include <functional>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

using std::placeholders::_1;

// This protects timer_ against concurrent access with no chance of deadlock.
// This can be dereferenced with an outstanding callback because the timer
// closure captures an instance of this class and the callback.
// This is guaranteed to call handler exactly once unless canceled or reset.
deadline::deadline(threadpool& pool, const asio::duration duration)
  : duration_(duration),
    timer_(pool.service()),
    CONSTRUCT_TRACK(deadline)
{
}

void deadline::start(handler handle)
{
    start(handle, duration_);
}

void deadline::start(handler handle, const asio::duration duration)
{
    const auto timer_handler =
        std::bind(&deadline::handle_timer,
            shared_from_this(), _1, handle);

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(mutex_);

    timer_.cancel();
    timer_.expires_from_now(duration);

    // async_wait will not invoke the handler within this function.
    timer_.async_wait(timer_handler);
    ///////////////////////////////////////////////////////////////////////////
}

// Cancellation calls handle_timer with asio::error::operation_aborted.
// We do not handle the cancelation result code, which will return success
// in the case of a race in which the timer is already canceled.
// We don't use strand because cancel must not change context.
void deadline::stop()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(mutex_);

    timer_.cancel();
    ///////////////////////////////////////////////////////////////////////////
}

// If the timer expires the callback is fired with a success code.
// If the timer fails the callback is fired with the normalized error code.
// If the timer is canceled no call is made.
void deadline::handle_timer(const boost_code& ec, handler handle) const
{
    if (!ec)
        handle(error::success);
    else if (ec != asio::error::operation_aborted)
        handle(error::boost_to_error_code(ec));
}

} // namespace libbitcoin
