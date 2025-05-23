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
#include <bitcoin/system/unicode/ascii.hpp>

#include <algorithm>
#include <iterator>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/normalization.hpp>

namespace libbitcoin {
namespace system {

// TODO: move to .ipp and make constexpr/inline.

// char32_t functions.

bool is_ascii_character(char32_t character) NOEXCEPT
{
    return is_zero(character & 0xffffff80);
}

bool is_ascii_number(char32_t point) NOEXCEPT
{
    return '0' <= point && point <= '9';
}

bool is_ascii_separator(char32_t point) NOEXCEPT
{
    return is_ascii_character(point) && is_separator(point);
}

bool is_ascii_whitespace(char32_t point) NOEXCEPT
{
    return is_ascii_character(point) && is_whitespace(point);
}

// string functions.

bool is_ascii(const std::string& text) NOEXCEPT
{
    return std::all_of(text.begin(), text.end(), is_ascii_character);
}

bool is_ascii_numeric(const std::string& text) NOEXCEPT
{
    if (text.empty())
        return true;

    const auto negative = (text.front() == '-');
    const auto start = std::next(text.begin(), negative ? 1 : 0);
    return std::all_of(start, text.end(), is_ascii_number);
}

std::string ascii_to_lower(const std::string& text) NOEXCEPT
{
    std::string copy{ text};

    // C++17: parallel policy for std::transform.
    std::transform(text.begin(), text.end(), copy.begin(),
        [](char value) NOEXCEPT
        {
            return narrow_sign_cast<uint8_t>(
                'A' <= value && value <= 'Z' ? value + ('a' - 'A') : value);
        });
    return copy;
}

std::string ascii_to_upper(const std::string& text) NOEXCEPT
{
    std::string copy{ text };

    // C++17: parallel policy for std::transform.
    std::transform(text.begin(), text.end(), copy.begin(),
        [](char value) NOEXCEPT
        {
            return narrow_sign_cast<uint8_t>(
                'a' <= value && value <= 'z' ? value + ('A' - 'a') : value);
        });

    return copy;
}

bool has_ascii_whitespace(const std::string& text) NOEXCEPT
{
    // C++17: parallel policy for std::any_of.
    return std::any_of(text.begin(), text.end(),
        [](char character) NOEXCEPT
        {
            return is_ascii_whitespace(character);
        });
}

bool has_mixed_ascii_case(const std::string& text) NOEXCEPT
{
    auto lower = false;
    auto upper = false;

    // non-parallel (side effect).
    std::for_each(text.begin(), text.end(),
        [&](char character) NOEXCEPT
        {
            lower |= ('a' <= character && character <= 'z');
            upper |= ('A' <= character && character <= 'Z');
        });

    return lower && upper;
}

} // namespace system
} // namespace libbitcoin
