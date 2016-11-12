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
#include <boost/make_shared.hpp>

namespace libbitcoin {
namespace log {

const std::string counter_symbol = "c";
const std::string gauge_symbol = "g";
const std::string timer_symbol = "ms";

const std::string simple_format = "%1s:%2%|%3s\n";
const std::string sampled_format = "%1s:%2%|%3s|@|%4f\n";

udp_client_sink::udp_client_sink(
    boost::shared_ptr<boost::asio::ip::udp::socket>& socket,
    boost::shared_ptr<boost::asio::ip::udp::endpoint>& endpoint)
  : socket_(socket), endpoint_(endpoint)
{
}

void udp_client_sink::consume(boost::log::record_view const& record,
    const std::string& message)
{
    send(message);
}

//void udp_client_sink::counter(std::string& metric,
//    int64_t value)
//{
//    auto message = boost::format(simple_format);
//    return send((message % metric % value % counter_symbol));
//}
//
//void udp_client_sink::counter(std::string& metric,
//    int64_t value, float rate)
//{
//    auto message = boost::format(sampled_format);
//    return send((message % metric % value % counter_symbol % rate));
//}
//
//void udp_client_sink::gauge(std::string& metric,
//    uint64_t value)
//{
//    auto message = boost::format(simple_format);
//    return send((message % metric % value % gauge_symbol));
//}
//
//void udp_client_sink::gauge(std::string& metric,
//    uint64_t value, float rate)
//{
//    auto message = boost::format(sampled_format);
//    return send((message % metric % value % gauge_symbol % rate));
//}
//
//void udp_client_sink::timer(std::string& metric,
//    std::chrono::milliseconds value)
//{
//    auto message = boost::format(simple_format);
//    return send((message % metric % value.count() % timer_symbol));
//}
//
//void udp_client_sink::timer(std::string& metric,
//    std::chrono::milliseconds value,
//    float rate)
//{
//    auto message = boost::format(sampled_format);
//    return send((message % metric % value.count() % timer_symbol % rate));
//}
//
//void udp_client_sink::send(boost::format& message)
//{
//    auto raw_message = message.str();
//    send(raw_message);
//}

void udp_client_sink::send(const std::string& message)
{
    auto data = boost::make_shared<std::string>(message);

    if (socket_ && endpoint_)
    {
        socket_->async_send_to(boost::asio::buffer(*data), *endpoint_,
            [](const boost_code& error, std::size_t bytes_transferred)
            {
            });
    }
}

} // namespace log
} // namespace libbitcoin
