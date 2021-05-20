/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_DEADLINE_HPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_DEADLINE_HPP

#include <memory>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/concurrency/asio.hpp>
#include <bitcoin/system/concurrency/enable_shared_from_base.hpp>
#include <bitcoin/system/concurrency/thread.hpp>
#include <bitcoin/system/concurrency/threadpool.hpp>
////#include <bitcoin/system/concurrency/track.hpp>
#include <bitcoin/system/noncopyable.hpp>

namespace libbitcoin {
namespace system {

/**
 * Class wrapper for boost::asio::deadline_timer, thread safe.
 * This simplifies invocation, eliminates boost-specific error handling and
 * makes timer firing and cancellation conditions safer.
 */
class BC_API deadline
  : public enable_shared_from_base<deadline>,
    noncopyable
    /*, track<deadline>*/
{
public:
    typedef std::shared_ptr<deadline> ptr;
    typedef std::function<void(const code&)> handler;

    /**
     * Construct a deadline timer with a zero duration.
     * @param[in]  pool      The thread pool used by the timer.
     */
    deadline(threadpool& pool);

    /**
     * Construct a deadline timer.
     * @param[in]  pool      The thread pool used by the timer.
     * @param[in]  duration  The default time period from start to expiration.
     */
    deadline(threadpool& pool, const asio::duration duration);

    /**
     * Start or restart the timer.
     * The handler will not be invoked within the scope of this call.
     * Use expired(ec) in handler to test for expiration.
     * @param[in]  handle  Callback invoked upon expire or cancel.
     */
    void start(handler handle);

    /**
     * Start or restart the timer.
     * The handler will not be invoked within the scope of this call.
     * Use expired(ec) in handler to test for expiration.
     * @param[in]  handle    Callback invoked upon expire or cancel.
     * @param[in]  duration  The time period from start to expiration.
     */
    void start(handler handle, const asio::duration duration);

    /**
     * Cancel the timer. The handler will be invoked.
     */
    void stop();

private:
    void handle_timer(const boost_code& ec, handler handle) const;

    asio::timer timer_;
    asio::duration duration_;
    mutable shared_mutex mutex_;
};

} // namespace system
} // namespace libbitcoin

#endif
