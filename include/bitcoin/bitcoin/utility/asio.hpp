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
#ifndef LIBBITCOIN_ASIO_HPP
#define LIBBITCOIN_ASIO_HPP

#include <memory>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/compat.hpp>

// Convenience namespace for commonly used boost asio aliases.

namespace libbitcoin {
namespace asio {

namespace error = boost::asio::error;

typedef boost::asio::ip::tcp tcp;
typedef boost::asio::ip::address address;
typedef boost::asio::ip::address_v4 ipv4;
typedef boost::asio::ip::address_v6 ipv6;
typedef boost::asio::ip::tcp::endpoint endpoint;
typedef boost::asio::io_service service;
typedef boost::asio::deadline_timer timer;
typedef boost::posix_time::time_duration duration;

typedef tcp::socket socket;
typedef tcp::acceptor acceptor;
typedef tcp::resolver resolver;
typedef tcp::resolver::query query;
typedef tcp::resolver::iterator iterator;

typedef std::shared_ptr<socket> socket_ptr;
typedef std::shared_ptr<acceptor> acceptor_ptr;
typedef std::shared_ptr<resolver> resolver_ptr;
typedef std::shared_ptr<query> query_ptr;

BC_CONSTEXPR int max_connections = boost::asio::socket_base::max_connections;

} // namespace asio
} // namespace libbitcoin

#endif

