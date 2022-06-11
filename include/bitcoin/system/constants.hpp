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
#ifndef LIBBITCOIN_SYSTEM_CONSTANTS_HPP
#define LIBBITCOIN_SYSTEM_CONSTANTS_HPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/integrals.hpp>

namespace libbitcoin {

// Avoid typed casts due to circular header inclusion.
// Avoid integral constants due to pre-cast integral conversion.

template <typename Type>
constexpr bool is_zero(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return value == static_cast<Type>(0);
    BC_POP_WARNING()
}

template <typename Type>
constexpr bool is_nonzero(Type value) noexcept
{
    return !is_zero(value);
}

template <typename Type>
constexpr bool is_one(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return value == static_cast<Type>(1);
    BC_POP_WARNING()
}

template <typename Type>
constexpr Type lo_bit(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(value % 2);
    BC_POP_WARNING()
}

template <typename Type>
constexpr bool is_even(Type value) noexcept
{
    return is_zero(lo_bit(value));
}

template <typename Type>
constexpr bool is_odd(Type value) noexcept
{
    return !is_even(value);
}

template <typename Type>
constexpr bool is_integer()
{
    return std::numeric_limits<Type>::is_integer &&
        !std::is_same<bool, Type>::value;
}

template <typename Type>
constexpr bool is_null(Type value) noexcept
{
    return value == nullptr;
}

template <typename Type>
constexpr Type to_bits(Type bytes) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(bytes * 8);
    BC_POP_WARNING()
}

constexpr uint8_t to_byte(char character) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<uint8_t>(character);
    BC_POP_WARNING()
}

template <typename Type>
constexpr Type to_bytes(Type bits) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(bits / 8);
    BC_POP_WARNING()
}

template <typename Type>
constexpr Type to_half(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(value / 2);
    BC_POP_WARNING()
}

template <typename Type = int>
constexpr Type to_int(bool value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(value ? 1 : 0);
    BC_POP_WARNING()
}

template <typename Type>
constexpr bool to_bool(Type value) noexcept
{
    return !is_zero(value);
}

template <typename Type>
constexpr Type add1(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(value + 1);
    BC_POP_WARNING()
}

template <typename Type>
constexpr Type sub1(Type value) noexcept
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Type>(value - 1);
    BC_POP_WARNING()
}

template <typename Type>
constexpr size_t width() noexcept
{
    // This is not always a logical size for non-integral types.
    return to_bits(sizeof(Type));
}

template <typename Type>
constexpr size_t width(Type value) noexcept
{
    // This is not always a logical size for non-integral types.
    return to_bits(sizeof(value));
}

/// Determine the bitcoin variable-serialized size of a given value.
constexpr size_t variable_size(uint64_t value) noexcept
{
    // C++11: single return required for constexpr.
    return (value < varint_two_bytes) ? sizeof(uint8_t) :
        ((value <= max_uint16) ? sizeof(uint8_t) + sizeof(uint16_t) :
            ((value <= max_uint32) ? sizeof(uint8_t) + sizeof(uint32_t) :
                sizeof(uint8_t) + sizeof(uint64_t)));
}

} // namespace libbitcoin

#endif
