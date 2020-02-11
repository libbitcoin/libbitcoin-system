/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_ASIO_HPP
#define LIBBITCOIN__UTILITY_ASIO_HPP

#include <chrono>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <bitcoin/bitcoin/compat.hpp>

// Convenience namespace for commonly used boost asio aliases.

namespace libbitcoin {
//namespace asio {
namespace api {

namespace error = boost::asio::error;

//typedef std::chrono::hours hours;
class hours { std::chrono::hours value; };
//typedef std::chrono::minutes minutes;
class minutes { std::chrono::minutes value; };
//typedef std::chrono::seconds seconds;
class seconds { std::chrono::seconds value; };
//typedef std::chrono::milliseconds milliseconds;
class milliseconds { std::chrono::milliseconds value; };
//typedef std::chrono::microseconds microseconds;
class microseconds { std::chrono::microseconds value; };

// Steady clock: use for continuity, not time of day determinations.
typedef std::chrono::steady_clock steady_clock;
//class steady_clock { std::chrono::steady_clock value; };
typedef steady_clock::duration duration;
//class duration { std::chrono::steady_clock::duration value; };
class time_point { std::chrono::steady_clock::time_point value; };
class timer { boost::asio::basic_waitable_timer<steady_clock> value; };

//typedef boost::asio::io_service service;
class service { boost::asio::io_service value; };
//typedef boost::asio::ip::address address;
class address { boost::asio::ip::address value; };
//typedef boost::asio::ip::address_v4 ipv4;
class ipv4 { boost::asio::ip::address_v4 value; };
//typedef boost::asio::ip::address_v6 value; };
class ipv6 { boost::asio::ip::address_v6 value; };
//typedef boost::asio::ip::tcp tcp;
class tcp { boost::asio::ip::tcp value; };
//typedef boost::asio::ip::tcp::endpoint endpoint;
class endpoint { boost::asio::ip::tcp::endpoint value; };

//typedef boost::asio::ip::tcp::socket socket;
class socket { boost::asio::ip::tcp::socket value; };
//typedef boost::asio::ip::tcp::acceptor acceptor;
class acceptor { boost::asio::ip::tcp::acceptor value; };
//typedef boost::asio::ip::tcp::resolver resolver;
class resolver { boost::asio::ip::tcp::resolver value; };
//typedef boost::asio::ip::tcp::resolver::query query;
class query { boost::asio::ip::tcp::resolver::query value; };
//typedef boost::asio::ip::tcp::resolver::iterator iterator;
class iterator { boost::asio::ip::tcp::resolver::iterator value; };

// Boost thread is used because of thread_specific_ptr limitation:
// stackoverflow.com/q/22448022/1172329
//typedef boost::thread thread;
class thread { boost::thread value; };

//typedef std::shared_ptr<socket> socket_ptr;
class socket_ptr { std::shared_ptr<socket> value; };

BC_CONSTEXPR int max_connections = boost::asio::socket_base::max_connections;

} // namespace api
//} // namespace asio
} // namespace libbitcoin

#endif
