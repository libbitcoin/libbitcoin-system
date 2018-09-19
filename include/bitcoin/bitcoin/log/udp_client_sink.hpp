/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_LOG_UDP_CLIENT_SINK_HPP
#define LIBBITCOIN_LOG_UDP_CLIENT_SINK_HPP

#include <string>
#include <boost/asio.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/shared_ptr.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
namespace log {

class BC_API udp_client_sink
  : public boost::log::sinks::basic_formatted_sink_backend<char,
        boost::log::sinks::synchronized_feeding>
{
public:
    using udp = boost::asio::ip::udp;
    typedef boost::shared_ptr<udp::socket> socket_ptr;
    typedef boost::shared_ptr<udp::endpoint> endpoint_ptr;

    udp_client_sink(socket_ptr socket, endpoint_ptr endpoint);

    void consume(const boost::log::record_view& record,
        const std::string& message);

protected:
    typedef boost::shared_ptr<std::string> message_ptr;

    void send(const std::string& message);
    void handle_send(const boost_code&, size_t, message_ptr payload);

private:
    socket_ptr socket_;
    const endpoint_ptr endpoint_;
};

} // namespace log
} // namespace libbitcoin

#endif
