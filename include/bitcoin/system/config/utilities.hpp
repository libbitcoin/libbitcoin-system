/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_UTILITIES_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_UTILITIES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/ip.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Rejects ipv6 mapped/compat to ipv4 and unbracketed ipv6.
BC_API ip::address from_host(const std::string& host) THROWS;

/// Parsers.
BC_API bool parse_authority(ip::address& ip, uint16_t& port,
    uint8_t& cidr, const std::string& value) NOEXCEPT;
BC_API bool parse_url(std::string& scheme, std::string& host,
    uint16_t& port, const std::string& value) NOEXCEPT;
BC_API bool parse_endpoint(std::string& host, uint16_t& port,
    const std::string& value) NOEXCEPT;

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
