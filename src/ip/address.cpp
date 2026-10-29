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
#include <bitcoin/system/ip/address.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/ipv4.hpp>
#include <bitcoin/system/ip/ipv6.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

// Statics.
// ----------------------------------------------------------------------------

bool address::from_string(address& out, const std::string_view& text) NOEXCEPT
{
    if (text.find(':') != std::string_view::npos)
    {
        ipv6 ip{};
        if (!ipv6::from_string(ip, text))
            return false;

        out = { ip };
        return true;
    }

    ipv4 ip{};
    if (!ipv4::from_string(ip, text))
        return false;

    out = { ip };
    return true;
}

// Constructors.
// ----------------------------------------------------------------------------

address::address() NOEXCEPT
  : address(ipv4{})
{
}

address::address(const ipv4& ip) NOEXCEPT
  : v6_{ false }, ipv4_{ ip }, ipv6_{}
{
}

address::address(const ipv6& ip) NOEXCEPT
  : v6_{ true }, ipv4_{}, ipv6_{ ip }
{
}

// Properties.
// ----------------------------------------------------------------------------

bool address::is_v4() const NOEXCEPT
{
    return !v6_;
}

bool address::is_v6() const NOEXCEPT
{
    return v6_;
}

const ipv4& address::to_v4() const NOEXCEPT
{
    return ipv4_;
}

const ipv6& address::to_v6() const NOEXCEPT
{
    return ipv6_;
}

std::string address::to_string() const NOEXCEPT
{
    return v6_ ? ipv6_.to_string() : ipv4_.to_string();
}

bool address::is_loopback() const NOEXCEPT
{
    return v6_ ? ipv6_.is_loopback() : ipv4_.is_loopback();
}

bool address::is_unspecified() const NOEXCEPT
{
    return v6_ ? ipv6_.is_unspecified() : ipv4_.is_unspecified();
}

// Operators.
// ----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const address& ip) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << ip.to_string();
    BC_POP_WARNING()
    return stream;
}

} // namespace ip
} // namespace system
} // namespace libbitcoin
