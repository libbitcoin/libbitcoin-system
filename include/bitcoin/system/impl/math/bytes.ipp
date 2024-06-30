/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_IPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/logarithm.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// Byte width.
// ----------------------------------------------------------------------------
// See also std::bit_width (C++20).

// Called by chain::compact (for validation).
template <typename Integral, if_unsigned_integer<Integral>>
constexpr size_t byte_width(Integral value) NOEXCEPT
{
    // (zero-based position of msb) + 7 / 8.
    // (bit_width(value) + 7) / 8
    // (ceilinged_log2(value) + 7) / 8
    return ceilinged_log256(value);
}

// Called by machine::number (for little-endian chunk sizing).
template <typename Integral, if_signed_integer<Integral>>
constexpr size_t byte_width(Integral value) NOEXCEPT
{
    return is_negative(value) ? sizeof(Integral) :
        byte_width(to_unsigned(value));
}

// Bit count to byte count.
// ----------------------------------------------------------------------------

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer to_ceilinged_bytes(Integer bits) NOEXCEPT
{
    return ceilinged_divide(bits, byte_bits);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer to_floored_bytes(Integer bits) NOEXCEPT
{
    return floored_divide(bits, byte_bits);
}

// Byte of integral by logical index.
// ----------------------------------------------------------------------------

template <size_t Index, typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>,
    if_lesser<Index, sizeof(Integral)>>
constexpr Byte byte(Integral value) NOEXCEPT
{
    constexpr auto byte_mask = unmask_right<Integral>(byte_bits);
    const auto byte = bit_and(shift_right(value, to_bits(Index)), byte_mask);
    return possible_narrow_and_sign_cast<Byte>(byte);
}

// Byte Negation.
// ----------------------------------------------------------------------------
// ****************************************************************************
// CONSENSUS: byte negation is consensus behavior, see machine::number.
// ****************************************************************************

template <typename Integer, if_integer<Integer>>
constexpr bool is_negated(Integer value) NOEXCEPT
{
    // Guard precludes zero appearing negated: ((0 % 8) + 1) == 1.
    return is_nonzero(value) && is_one(add1(bit_width(value) % byte_bits));
}

template <typename Integer, if_integer<Integer>>
constexpr Integer to_negated(Integer value) NOEXCEPT
{
    return set_left(value);
}

template <typename Integer, if_signed_integer<Integer>>
constexpr Integer to_unnegated(Integer value) NOEXCEPT
{
    // negate(minimum) is undefined behavior, but precluded by clearing a bit.
    return !is_negated(value) ? value : negate(
        set_right(value, sub1(bit_width(value)), false));
}

} // namespace system
} // namespace libbitcoin

#endif
