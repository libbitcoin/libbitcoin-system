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
#ifndef LIBBITCOIN_SYSTEM_IP_FUNCTIONS_HPP
#define LIBBITCOIN_SYSTEM_IP_FUNCTIONS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/address.hpp>
#include <bitcoin/system/ip/ipv6.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

/// IPv6 supports embedding IPv4 address (4 bytes) into IPv6 encodings (16
/// bytes). Two formats, "compatible" and "mapped", embed same address
/// differently, distinguished by device level of support. This is problematic
/// for addresses, as they are device independent. P2P protocol is 16 bytes and
/// allows either encoding, but "compatible" encoding is deprecated, so only
/// mapped encoding is produced for P2P serialization. As both formats are sent
/// via P2P, all three IPv6 encodings (native, compatible, mapped) are decoded.
/// For human readability addresses serialize as text, for both logging and
/// shutdown persistence. This format is denormalized, supporting only native
/// IPv4 and native IPv6 serialization. IPv6 host names are "bracketed",
/// distinguishing them from port number (otherwise conflating ":"). This form
/// is "literal" IPv6 encoding (from IPv6 URIs). All text addresses are
/// literal encodings, and all host names serialize as non-literal, and
/// deserialize as either literal or non-literal.

/// Member if subnet addresses contain host.
BC_API bool is_member(const address& ip, const address& subnet,
    uint8_t cidr) NOEXCEPT;

/// Unmap IPv6-mapped addresses.
BC_API address denormalize(const address& ip) NOEXCEPT;

/// Map IPv4 addresses to IPv6.
BC_API ipv6 normalize(const address& ip) NOEXCEPT;

/// Denormalizes to IPv4 (unmapped), literal emits unbracketed.
BC_API std::string to_host(const address& ip) NOEXCEPT;
BC_API std::string to_literal(const address& ip) NOEXCEPT;

} // namespace ip
} // namespace system
} // namespace libbitcoin

#endif
