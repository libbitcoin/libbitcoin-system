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
#ifndef LIBBITCOIN_SYSTEM_IP_IPV6_HPP
#define LIBBITCOIN_SYSTEM_IP_IPV6_HPP

#include <compare>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/ipv4.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

/// Prefix of an IPv4-mapped IPv6 address (rfc4291).
constexpr data_array<12> mapped_prefix
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff
};

/// IPv6 address (rfc4291), without zone index.
class BC_API ipv6
{
public:
    static constexpr size_t size = 16;
    typedef data_array<size> bytes_type;

    /// Parse text form (rfc4291), including an embedded IPv4 suffix.
    static bool from_string(ipv6& out, const std::string_view& text) NOEXCEPT;

    /// ::
    static ipv6 any() NOEXCEPT;

    /// ::1
    static ipv6 loopback() NOEXCEPT;

    /// The IPv4-mapped IPv6 address of the IPv4 address.
    static ipv6 mapped(const ipv4& ip) NOEXCEPT;

    DEFAULT_COPY_MOVE(ipv6);

    /// Unspecified (::).
    ipv6() NOEXCEPT;
    ipv6(const bytes_type& bytes) NOEXCEPT;

    /// Network byte order.
    const bytes_type& to_bytes() const NOEXCEPT;

    /// Canonical text (rfc5952), IPv4-mapped and compatible as mixed notation.
    std::string to_string() const NOEXCEPT;

    /// The IPv4 address of the low four bytes (meaningful when mapped).
    ipv4 to_v4() const NOEXCEPT;

    /// ::1
    bool is_loopback() const NOEXCEPT;

    /// ::
    bool is_unspecified() const NOEXCEPT;

    /// ::ffff:0:0/96
    bool is_v4_mapped() const NOEXCEPT;

    bool operator==(const ipv6& other) const NOEXCEPT = default;
    std::strong_ordering operator<=>(const ipv6& other) const NOEXCEPT = default;

private:
    bytes_type bytes_;
};

/// Text form (to_string).
BC_API std::ostream& operator<<(std::ostream& stream,
    const ipv6& ip) NOEXCEPT;

} // namespace ip
} // namespace system
} // namespace libbitcoin

#endif
