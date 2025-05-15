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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_MIDSTATE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_MIDSTATE_IPP

// Midstate input/output.
// ============================================================================
// These bypass endianness, used for padding and state reintroduction.

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr void CLASS::
inject_left_half(auto& buffer, const auto& left) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        buffer.at(0) = left.at(0);
        buffer.at(1) = left.at(1);
        buffer.at(2) = left.at(2);
        buffer.at(3) = left.at(3);
        buffer.at(4) = left.at(4);
        buffer.at(5) = left.at(5);
        buffer.at(6) = left.at(6);
        buffer.at(7) = left.at(7);
    }
    else
    {
        using word = array_element<decltype(buffer)>;
        array_cast<word, SHA::state_words>(buffer) = left;
    }
}

TEMPLATE
INLINE constexpr void CLASS::
inject_right_half(auto& buffer, const auto& right) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        buffer.at(8) = right.at(0);
        buffer.at(9) = right.at(1);
        buffer.at(10) = right.at(2);
        buffer.at(11) = right.at(3);
        buffer.at(12) = right.at(4);
        buffer.at(13) = right.at(5);
        buffer.at(14) = right.at(6);
        buffer.at(15) = right.at(7);
    }
    else
    {
        using word = array_element<decltype(buffer)>;
        array_cast<word, SHA::state_words, SHA::state_words>(buffer) = right;
    }
}

TEMPLATE
INLINE constexpr void CLASS::
inject_left_quarter(auto& buffer, const auto& left) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        buffer.at(0) = left.at(0);
        buffer.at(1) = left.at(1);
        buffer.at(2) = left.at(2);
        buffer.at(3) = left.at(3);
    }
    else
    {
        using word = array_element<decltype(buffer)>;
        array_cast<word, to_half(SHA::state_words)>(buffer) = left;
    }
}

TEMPLATE
INLINE constexpr void CLASS::
inject_right_quarter(auto& buffer, const auto& right) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        buffer.at(4) = right.at(0);
        buffer.at(5) = right.at(1);
        buffer.at(6) = right.at(2);
        buffer.at(7) = right.at(3);
    }
    else
    {
        using word = array_element<decltype(buffer)>;
        constexpr auto words = to_half(SHA::state_words);
        array_cast<word, words, words>(buffer) = right;
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
