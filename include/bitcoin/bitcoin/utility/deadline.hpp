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

class BC_API deadline
  : public std::enable_shared_from_this<deadline>
{
public:
    typedef std::function<void(const std::error_code&)> handler;

    deadline(threadpool& pool, boost::posix_time::time_duration duration,
        handler handle_expiration);

    static bool canceled(const std::error_code& ec);

    void start();
    void stop();

private:
    boost::posix_time::time_duration duration_;
    boost::asio::deadline_timer timer_;
    handler handler_;

    void handle_timer(const boost::system::error_code& ec);
};

} // namespace libbitcoin

#endif
