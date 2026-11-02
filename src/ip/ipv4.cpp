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
#include <bitcoin/system/ip/ipv4.hpp>

#include <charconv>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

// Statics.
// ----------------------------------------------------------------------------

bool ipv4::from_string(ipv4& out, const std::string_view& text) NOEXCEPT
{
    bytes_type bytes{};
    auto position = text.data();
    const auto end = std::next(position, text.size());

    for (auto octet = bytes.begin(); octet != bytes.end(); ++octet)
    {
        if (octet != bytes.begin())
        {
            if (position == end || *position != '.')
                return false;

            position = std::next(position);
        }

        const auto start = position;
        const auto [next, ec] = std::from_chars(start, end, *octet);
        const auto digits = std::distance(start, next);
        if (ec != std::errc{} || (digits > 1 && *start == '0'))
            return false;

        position = next;
    }

    if (position != end)
        return false;

    out = { bytes };
    return true;
}

ipv4 ipv4::any() NOEXCEPT
{
    return {};
}

ipv4 ipv4::loopback() NOEXCEPT
{
    return { { 127, 0, 0, 1 } };
}

// Constructors.
// ----------------------------------------------------------------------------

ipv4::ipv4() NOEXCEPT
  : bytes_{}
{
}

ipv4::ipv4(const bytes_type& bytes) NOEXCEPT
  : bytes_{ bytes }
{
}

// Properties.
// ----------------------------------------------------------------------------

const ipv4::bytes_type& ipv4::to_bytes() const NOEXCEPT
{
    return bytes_;
}

std::string ipv4::to_string() const NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::string out{};
    for (const auto octet: bytes_)
    {
        if (!out.empty())
            out.push_back('.');

        out.append(std::to_string(octet));
    }

    return out;
    BC_POP_WARNING()
}

bool ipv4::is_loopback() const NOEXCEPT
{
    return bytes_.front() == 127_u8;
}

bool ipv4::is_unspecified() const NOEXCEPT
{
    return bytes_ == bytes_type{};
}

// Operators.
// ----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const ipv4& ip) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << ip.to_string();
    BC_POP_WARNING()
    return stream;
}

} // namespace ip
} // namespace system
} // namespace libbitcoin
