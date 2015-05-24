/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
    connect_with_timeout(threadpool& pool);

    void start(tcp::resolver::iterator endpoint_iterator,
        time_duration timeout, network::connect_handler handle_connect);

private:
    void call_connect_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator, network::connect_handler handle_connect);

    void close(const boost::system::error_code& ec);

    socket_ptr socket_;
    channel::channel_proxy_ptr proxy_;
    boost::asio::deadline_timer timer_;
};

} // namespace network
} // namespace libbitcoin
