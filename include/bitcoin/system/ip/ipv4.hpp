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
#ifndef LIBBITCOIN_SYSTEM_IP_IPV4_HPP
#define LIBBITCOIN_SYSTEM_IP_IPV4_HPP

#include <compare>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

/// IPv4 address (rfc791).
class BC_API ipv4
{
public:
    static constexpr size_t size = 4;
    typedef data_array<size> bytes_type;

    /// Parse dotted decimal, four octets without leading zeros.
    static bool from_string(ipv4& out, const std::string_view& text) NOEXCEPT;

    /// 0.0.0.0
    static ipv4 any() NOEXCEPT;

    /// 127.0.0.1
    static ipv4 loopback() NOEXCEPT;

    DEFAULT_COPY_MOVE(ipv4);

    /// Unspecified (0.0.0.0).
    ipv4() NOEXCEPT;
    ipv4(const bytes_type& bytes) NOEXCEPT;

    /// Network byte order.
    const bytes_type& to_bytes() const NOEXCEPT;

    /// Dotted decimal.
    std::string to_string() const NOEXCEPT;

    /// 127.0.0.0/8 (rfc1122).
    bool is_loopback() const NOEXCEPT;

    /// 0.0.0.0
    bool is_unspecified() const NOEXCEPT;

    bool operator==(const ipv4& other) const NOEXCEPT = default;
    std::strong_ordering operator<=>(const ipv4& other) const NOEXCEPT = default;

private:
    bytes_type bytes_;
};

/// Text form (to_string).
BC_API std::ostream& operator<<(std::ostream& stream,
    const ipv4& ip) NOEXCEPT;

} // namespace ip
} // namespace system
} // namespace libbitcoin

#endif
