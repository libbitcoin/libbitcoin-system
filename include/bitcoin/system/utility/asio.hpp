/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ASIO_HPP
#define LIBBITCOIN_SYSTEM_ASIO_HPP

#include <chrono>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <bitcoin/system/compat.hpp>

// Convenience namespace for commonly used boost asio aliases.

namespace libbitcoin {
namespace system {
namespace asio {

namespace error = boost::asio::error;

typedef std::chrono::hours hours;
typedef std::chrono::minutes minutes;
typedef std::chrono::seconds seconds;
typedef std::chrono::milliseconds milliseconds;
typedef std::chrono::microseconds microseconds;
typedef std::chrono::nanoseconds nanoseconds;

// Steady clock: use for continuity, not time of day determinations.
typedef std::chrono::steady_clock steady_clock;
typedef steady_clock::duration duration;
typedef steady_clock::time_point time_point;
typedef boost::asio::basic_waitable_timer<steady_clock> timer;

typedef boost::asio::io_service service;
typedef boost::asio::ip::address address;
typedef boost::asio::ip::address_v4 ipv4;
typedef boost::asio::ip::address_v6 ipv6;
typedef boost::asio::ip::tcp tcp;
typedef boost::asio::ip::tcp::endpoint endpoint;

typedef tcp::socket socket;
typedef tcp::acceptor acceptor;
typedef tcp::resolver resolver;
typedef tcp::resolver::query query;
typedef tcp::resolver::iterator iterator;

// Boost thread is used because of thread_specific_ptr limitation:
// stackoverflow.com/q/22448022/1172329
typedef boost::thread thread;

typedef std::shared_ptr<socket> socket_ptr;

BC_CONSTEXPR int max_connections = boost::asio::socket_base::max_connections;

} // namespace asio
} // namespace system
} // namespace libbitcoin

#endif
