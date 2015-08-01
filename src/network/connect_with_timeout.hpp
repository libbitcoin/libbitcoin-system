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
#include <memory>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/network.hpp>

namespace libbitcoin {
namespace network {

using boost::asio::ip::tcp;
using boost::posix_time::time_duration;

class connect_with_timeout
  : public std::enable_shared_from_this<connect_with_timeout>
{
public:
    connect_with_timeout(threadpool& pool,
        const timeout& timeouts=timeout::defaults);

    void start(tcp::resolver::iterator endpoint_iterator,
        network::connect_handler handle_connect);

private:
    void call_handle_connect(const boost::system::error_code& ec,
        tcp::resolver::iterator, network::connect_handler handle_connect);

    void handle_timer(const boost::system::error_code& ec,
        network::connect_handler handle_connect);

    socket_ptr socket_;
    channel::channel_proxy_ptr proxy_;
    boost::asio::deadline_timer timer_;
    boost::posix_time::time_duration connection_timeout_;
};

} // namespace network
} // namespace libbitcoin
