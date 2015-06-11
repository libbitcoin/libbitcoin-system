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
#ifndef LIBBITCOIN_AUTHORITY_HPP
#define LIBBITCOIN_AUTHORITY_HPP

#include <cstdint>
#include <string>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>

namespace libbitcoin {
namespace network {

struct BC_API authority
{
    authority();
    authority(const std::string& line);
    authority(const network_address_type& net);
    authority(const std::string& host, uint16_t port);
    authority(const boost::asio::ip::tcp::socket::endpoint_type& endpoint);
    bool operator==(const authority& other) const;
    std::string to_string() const;

    std::string host;
    uint16_t port;
};

} // namespace network
} // namespace libbitcoin

#endif

