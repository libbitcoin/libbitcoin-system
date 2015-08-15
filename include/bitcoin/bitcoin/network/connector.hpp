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
#ifndef LIBBITCOIN_CONNECTOR_HPP
#define LIBBITCOIN_CONNECTOR_HPP

#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

using boost::asio::ip::tcp;
using boost::posix_time::time_duration;

class connector
  : public std::enable_shared_from_this<connector>
{
public:
    typedef std::function<void(const std::error_code&, channel_ptr)>
        connect_handler;

    connector(threadpool& pool, const timeout& timeouts=timeout::defaults);

    void start(tcp::resolver::iterator endpoint_iterator,
        connect_handler handle_connect);

private:
    void call_handle_connect(const boost::system::error_code& ec,
        tcp::resolver::iterator, socket_ptr socket,
        connect_handler handle_connect);
    void handle_timer(const boost::system::error_code& ec,
        connect_handler handle_connect);

    threadpool& pool_;
    const timeout& timeouts_;
    boost::asio::deadline_timer timer_;
};

} // namespace network
} // namespace libbitcoin

#endif
