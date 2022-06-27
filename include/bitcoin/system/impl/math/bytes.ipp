/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <cstdint>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/external/byte_swap.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// widths
// ----------------------------------------------------------------------------

template <typename Value, if_unsigned_integer<Value>>
constexpr size_t byte_width(Value value) noexcept
{
    return ceilinged_log256(value);
}

template <typename Value, if_signed_integer<Value>>
constexpr size_t byte_width(Value value) noexcept
{
    return is_negative(value) ? sizeof(Value) : byte_width(to_unsigned(value));
}

// ****************************************************************************
// CONSENSUS: negation is consensus behavior, see machine::number.
// ****************************************************************************

template <typename Integer, if_integer<Integer>>
constexpr bool is_negated(Integer value) noexcept
{
    return is_nonzero(value) && add1(bit_width(value) % byte_bits) == one;
};

template <typename Integer, if_integer<Integer>>
constexpr Integer to_negated(Integer value) noexcept
{
    return set_left(value, zero, true);
}

template <typename Integer, if_signed_integer<Integer>>
constexpr Integer to_unnegated(Integer value) noexcept
{
    // -minimum<Integer> (undefined behavior) precluded by set_right.
    return !is_negated(value) ? value : negate(
        set_right(value, sub1(bit_width(value)), false));
}

// native-to-endian integral conversions
// ----------------------------------------------------------------------------

template <typename Integer,
    if_big_endian_integral_integer<Integer>>
constexpr Integer to_big_end(Integer from) noexcept
{
    return from;
}

template <typename Integer,
    if_little_endian_integral_integer<Integer>>
constexpr Integer to_big_end(Integer from) noexcept
{
    return byteswap(from);
}

template <typename Integer,
    if_big_endian_integral_integer<Integer>>
constexpr Integer to_little_end(Integer from) noexcept
{
    return byteswap(from);
}

template <typename Integer,
    if_little_endian_integral_integer<Integer>>
constexpr Integer to_little_end(Integer from) noexcept
{
    return from;
}

// endianness reversals
// ----------------------------------------------------------------------------

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint8_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // no-op for calling consistency across all integral integer types.
    return value;
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint16_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap16_native(to_unsigned(value)) :
        byte_swap16(to_unsigned(value)));
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint32_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap32_native(to_unsigned(value)) :
        byte_swap32(to_unsigned(value)));
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint64_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap64_native(to_unsigned(value)) :
        byte_swap64(to_unsigned(value)));
}

// bits to bytes
// ----------------------------------------------------------------------------

template <size_t Bits, if_bytes_width<Bits>>
constexpr size_t to_bytes() noexcept
{
    return Bits / byte_bits;
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer to_ceilinged_bytes(Integer bits) noexcept
{
    return ceilinged_divide(bits, byte_bits);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer to_floored_bytes(Integer bits) noexcept
{
    return floored_divide(bits, byte_bits);
}

} // namespace system
} // namespace libbitcoin

#endif
