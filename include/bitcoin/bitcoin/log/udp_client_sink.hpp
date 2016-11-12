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
#ifndef LIBBITCOIN_LOG_UDP_CLIENT_SINK_HPP
#define LIBBITCOIN_LOG_UDP_CLIENT_SINK_HPP

#include <boost/asio.hpp>
//#include <boost/format.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
//#include <bitcoin/bitcoin/config/authority.hpp>
//#include <bitcoin/bitcoin/log/statistics.hpp>
//#include <bitcoin/bitcoin/log/metrics.hpp>

namespace libbitcoin {
namespace log {

class BC_API udp_client_sink
  : public boost::log::sinks::basic_formatted_sink_backend<
    char, boost::log::sinks::synchronized_feeding>
{
public:
    udp_client_sink(boost::shared_ptr<boost::asio::ip::udp::socket>& socket,
        boost::shared_ptr<boost::asio::ip::udp::endpoint>& endpoint);

    void consume(boost::log::record_view const& record,
        const std::string& message);

//    // counter reported statistics
//    void counter(std::string& metric, int64_t value);
//    void counter(std::string& metric, int64_t value, float rate);
//
//    // gauge reported statistics
//    void gauge(std::string& metric, uint64_t value);
//    void gauge(std::string& metric, uint64_t value, float rate);
//
//    // timer reported statistics
//    void timer(std::string& metric, std::chrono::milliseconds value);
//    void timer(std::string& metric, std::chrono::milliseconds value, float rate);

protected:
//    void send(boost::format& message);
    void send(const std::string& message);

private:
    boost::shared_ptr<boost::asio::ip::udp::socket> socket_;
    boost::shared_ptr<boost::asio::ip::udp::endpoint> endpoint_;
};

} // namespace log
} // namespace libbitcoin

#endif
