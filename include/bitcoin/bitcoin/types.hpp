/*
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
#ifndef LIBBITCOIN_TYPES_HPP
#define LIBBITCOIN_TYPES_HPP

#include <iomanip>
#include <memory>
#include <sstream>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

namespace network {

    #define DECLARE_PTR_TYPE(nametype) \
        class nametype; \
        typedef std::shared_ptr<nametype> nametype##_ptr

    DECLARE_PTR_TYPE(acceptor);
    DECLARE_PTR_TYPE(channel);

} // namespace network

#undef DECLARE_PTR_TYPE

typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

// A list of indices. Used for creating block_locator objects or
// Storing list of unconfirmed input indexes in tx pool.
typedef std::vector<size_t> index_list;

struct BC_API node_address
{
    std::string hostname;
    uint16_t port;
};

} // namespace libbitcoin

#endif

