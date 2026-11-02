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
#ifndef LIBBITCOIN_SYSTEM_IP_ADDRESS_HPP
#define LIBBITCOIN_SYSTEM_IP_ADDRESS_HPP

#include <compare>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/ipv4.hpp>
#include <bitcoin/system/ip/ipv6.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

/// IPv4 or IPv6 address, IPv4 ordered before IPv6.
class BC_API address
{
public:
    /// Parse IPv6 text if it contains a colon, otherwise IPv4.
    static bool from_string(address& out, const std::string_view& text) NOEXCEPT;

    DEFAULT_COPY_MOVE(address);

    /// Unspecified IPv4 (0.0.0.0).
    address() NOEXCEPT;
    address(const ipv4& ip) NOEXCEPT;
    address(const ipv6& ip) NOEXCEPT;

    bool is_v4() const NOEXCEPT;
    bool is_v6() const NOEXCEPT;

    /// The IPv4 address (unspecified if IPv6).
    const ipv4& to_v4() const NOEXCEPT;

    /// The IPv6 address (unspecified if IPv4).
    const ipv6& to_v6() const NOEXCEPT;

    /// Text of the IPv4 or IPv6 address.
    std::string to_string() const NOEXCEPT;

    bool is_loopback() const NOEXCEPT;
    bool is_unspecified() const NOEXCEPT;

    bool operator==(const address& other) const NOEXCEPT = default;
    std::strong_ordering operator<=>(const address& other) const NOEXCEPT = default;

private:
    bool v6_;
    ipv4 ipv4_;
    ipv6 ipv6_;
};

/// Text form (to_string).
BC_API std::ostream& operator<<(std::ostream& stream,
    const address& ip) NOEXCEPT;

} // namespace ip
} // namespace system
} // namespace libbitcoin

#endif
