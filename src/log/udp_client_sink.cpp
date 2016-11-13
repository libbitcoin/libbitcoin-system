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
#include <bitcoin/bitcoin/log/udp_client_sink.hpp>

#include <cstddef>
#include <string>
#include <boost/make_shared.hpp>

namespace libbitcoin {
namespace log {

using namespace boost::asio;
using namespace boost::log;

udp_client_sink::udp_client_sink(socket_ptr socket, endpoint_ptr endpoint)
  : socket_(socket), endpoint_(endpoint)
{
}

void udp_client_sink::consume(const record_view& record,
    const std::string& message)
{
    send(message);
}

void udp_client_sink::send(const std::string& message)
{
    if (socket_ && endpoint_)
    {
        const auto message_ptr = boost::make_shared<std::string>(message);
        const auto handler = [message_ptr](const boost_code&, size_t)
        {
            // This holds the message in scope until the send is completed.
        };

        socket_->async_send_to(buffer(*message_ptr), *endpoint_, handler);
    }
}

} // namespace log
} // namespace libbitcoin
