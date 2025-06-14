/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// IPv6 supports embedding of an IPv4 address (4 bytes) into IPv6 encodings
/// (16 bytes). The two formats, "compatible" and "mapped" embed the same
/// address differently, with the distinction being the level of support of the
/// device. This is problematic for addresses, as they are device independent.
/// P2P protocol is 16 bytes and allows for either encoding, however the
/// "compatible" concoding is deprecated, so we produce only mapped encoding
/// for P2P serialization. However, as both formats are send via P2P we decode
/// from all three IPv6 encodings (native, compatible, mapped). For human
/// readability we serialize addresses as text, for both logging and shutdown
/// persistence. We refer to this format as denormalized, as it supports only
/// native IPv4 and native IPv6 serialization. IPv6 host names are "bracketed".
/// This provides distinction from the port number (otherwise conflating ":").
/// This form is referred to as "literal" IPv6 encoding (from IPv6 URIs). All
/// text addresses are literal encodings, and all host names are serialized as
/// non-literal, and deserialized as either literal or non-literal.

/// datatracker.ietf.org/doc/html/rfc4291
constexpr size_t ipv4_size = 4;
constexpr size_t ipv6_size = 16;
static constexpr data_array<ipv6_size - ipv4_size> ip_map_prefix
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff
};

/// Member if subnet addresses contain host.
BC_API bool is_member(const asio::address& ip, const asio::address& subnet,
    uint8_t cidr) NOEXCEPT;

/// Unmap IPv6-mapped addresses.
BC_API asio::address denormalize(const asio::address& ip) NOEXCEPT;

/// Denormalizes to IPv4 (unmapped), literal emits unbracketed.
BC_API std::string to_host(const asio::address& ip) NOEXCEPT;
BC_API std::string to_literal(const asio::address& ip) NOEXCEPT;
BC_API asio::address from_host(const std::string& host) THROWS;

/// Parsers.
BC_API bool parse_authority(asio::address& ip, uint16_t& port,
    uint8_t& cidr, const std::string& value) NOEXCEPT;
BC_API bool parse_endpoint(std::string& scheme, std::string& host,
    uint16_t& port, const std::string& value) NOEXCEPT;

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
