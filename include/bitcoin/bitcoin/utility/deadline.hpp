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
#ifndef LIBBITCOIN_DEADLINE_HPP
#define LIBBITCOIN_DEADLINE_HPP

#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

/**
 * Class wrapper for boost::asio::deadline_timer.
 * This simplifies invocation, eliminates boost-speific error handling and makes
 * timer firing and cancellation conditions safer.
 */
class BC_API deadline
  : public std::enable_shared_from_this<deadline>
{
public:
    typedef std::shared_ptr<deadline> ptr;
    typedef std::function<void(const std::error_code&)> handler;
    
    /**
     * Construct a deadline timer.
     * @param[in]  pool      The thread pool used by the timer.
     * @param[in]  duration  The default time period from start to expiration.
     */
    deadline(threadpool& pool,
        const boost::posix_time::time_duration duration);

    /// This class is not copyable.
    deadline(const deadline&) = delete;
    void operator=(const deadline&) = delete;

    /**
     * Test handler error code for indication that the timer was stopped.
     * @param[in]  ec  The error code passed in the handler invocation.
     */
    static bool canceled(const std::error_code& ec);

    /**
     * Test handler error code for indication that the timer fired.
     * @param[in]  ec  The error code passed in the handler invocation.
     */
    static bool expired(const std::error_code& ec);

    /**
     * Start the timer.
     * Use expired(ec) in handler to test for expiration.
     * @param[in]  handle_expiration  Will be invoked upon expire or cancel.
     */
    void start(handler handle_expiration);

    /**
     * Start the timer.
     * Use expired(ec) in handler to test for expiration.
     * @param[in]  handle_expiration  Will be invoked upon expire or cancel.
     * @param[in]  duration           The time period from start to expiration.
     */
    void start(handler handle_expiration,
        const boost::posix_time::time_duration duration);

    /**
     * Cancel the timer.
     * Use canceled(ec) in handler to test for cancellation.
     */
    void cancel();

private:
    void handle_timer(const boost::system::error_code& ec,
        handler handle_expiration) const;

    boost::posix_time::time_duration duration_;
    boost::asio::deadline_timer timer_;
};

} // namespace libbitcoin

#endif
