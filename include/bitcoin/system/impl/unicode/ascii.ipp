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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_ASCII_IPP
#define LIBBITCOIN_SYSTEM_UNICODE_ASCII_IPP

#include <algorithm>
#include <iterator>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// char32_t functions.

constexpr bool is_ascii_character(char32_t character) NOEXCEPT
{
    return is_zero(character & 0xffffff80);
}

constexpr bool is_ascii_number(char32_t point) NOEXCEPT
{
    return '0' <= point && point <= '9';
}

constexpr bool is_ascii_lower(char32_t point) NOEXCEPT
{
    return 'a' <= point && point <= 'z';
}

constexpr bool is_ascii_upper(char32_t point) NOEXCEPT
{
    return 'A' <= point && point <= 'Z';
}

constexpr bool is_ascii_alpha(char32_t point) NOEXCEPT
{
    return is_ascii_lower(point) || is_ascii_upper(point);
}

// see char32_separators[]
constexpr bool is_ascii_separator(char32_t point) NOEXCEPT
{
    // space or no-break space.
    return point == 0x20 || point == 0xa0;
}

// see char_whitespace[]
constexpr bool is_ascii_whitespace(char32_t point) NOEXCEPT
{
    // space or \t, \n, \v, \f, \r
    return point == 0x20 || point == '\t' || ('\n' <= point && point <= '\r');
}

} // namespace system
} // namespace libbitcoin

#endif
