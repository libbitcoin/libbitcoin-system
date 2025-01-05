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
#ifndef LIBBITCOIN_SYSTEM_FUNCLETS_HPP
#define LIBBITCOIN_SYSTEM_FUNCLETS_HPP

#include <type_traits>
#include <bitcoin/system/literals.hpp>

/// Simple type safe functions over value parameter.
/// All values converted and specified domain and reconverted to domain.
/// This isolates integral promotion, and caller controls operating domain.

namespace libbitcoin {

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

/// Conditions.

template <typename Type>
constexpr bool is_zero(Type value) noexcept
{
    return value == static_cast<Type>(0);
}

template <typename Type>
constexpr bool is_nonzero(Type value) noexcept
{
    return !is_zero(value);
}

template <typename Type>
constexpr bool is_one(Type value) noexcept
{
    return value == static_cast<Type>(1);
}

template <typename Type>
constexpr Type lo_bit(Type value) noexcept
{
    static_assert(static_cast<Type>(1) % 2 == 1);
    static_assert(static_cast<Type>(2) % 2 == 0);
    return static_cast<Type>(value % 2);
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
constexpr bool is_null(Type value) noexcept
{
    return value == nullptr;
}

// This is future-proofing against larger integrals or language features that
// promote 3, 5, 6, 7 byte-sized types to integral (see std::is_integral).
constexpr bool is_byte_sized(size_t value) noexcept
{
    return !is_zero(value) && is_zero(value % 8);
}

// non-numbered integrals have unreliable sizes (as do least/fast types).
// s/u char are one byte. bool and wchar_t are of unspecified size, as are
// int, long, and long long. only minimum and relative sizes are assured.
constexpr bool is_integral_sized(size_t bytes) noexcept
{
    return bytes == sizeof(uint8_t)
        || bytes == sizeof(uint16_t)
        || bytes == sizeof(uint32_t)
        || bytes == sizeof(uint64_t);
}

/// Conversions.
/// Native operators promote all operands to at least signed/unsigned int.
/// Overflows do not cause promotion, so native 8/16 bit operations do not
/// overflow, but 32 (and 64) do. Forcing promotion to 64 bit makes this
/// unexpected behavior more consistent, producing overflow only at the
/// maximal integral domain (64), as opposed to 32/64 but not 8/16.
/// See also: bc::system::promote.

template <typename Type>
constexpr Type add1(Type value) noexcept
{
    static_assert(static_cast<Type>(0) + 1 == 1);
    return static_cast<Type>(value + to_promoted_type<Type>{1});
}

template <typename Type>
constexpr Type sub1(Type value) noexcept
{
    static_assert(static_cast<Type>(1) - 1 == 0);
    return static_cast<Type>(value - to_promoted_type<Type>{1});
}

template <typename Type>
constexpr Type to_bits(Type bytes) noexcept
{
    static_assert(static_cast<Type>(1) * 8 == 8);
    return static_cast<Type>(bytes * to_promoted_type<Type>{8});
}

constexpr uint8_t to_byte(char value) noexcept
{
    static_assert(is_one(sizeof(char)));
    return static_cast<uint8_t>(value);
}

/// Floored halving.
template <typename Type>
constexpr Type to_half(Type value) noexcept
{
    return static_cast<Type>(value / to_promoted_type<Type>{2});
}

template <typename Type = int>
constexpr Type to_int(bool value) noexcept
{
    static_assert(static_cast<bool>(1) == true);
    static_assert(static_cast<Type>(0) == false);
    return static_cast<Type>(value ? 1 : 0);
}

template <typename Type>
constexpr bool to_bool(Type value) noexcept
{
    static_assert(is_one(static_cast<Type>(true)));
    static_assert(is_zero(static_cast<Type>(false)));
    return is_nonzero(value);
}

/// Determine the bitcoin variable-serialized size of a given value.
template <typename Unsigned,
    std::enable_if_t<std::is_unsigned_v<Unsigned>, bool> = true>
constexpr size_t variable_size(Unsigned value) noexcept
{
    if (value < varint_two_bytes)
        return sizeof(uint8_t);

    if (value <= max_uint16)
        return sizeof(uint8_t) + sizeof(uint16_t);

    if (value <= max_uint32)
        return sizeof(uint8_t) + sizeof(uint32_t);

    return sizeof(uint8_t) + sizeof(uint64_t);
}

/// Determine the size of the bitcoin variable size from its prefix byte.
constexpr size_t size_variable(uint8_t prefix) noexcept
{
    switch (prefix)
    {
        case varint_eight_bytes:
            return sizeof(uint8_t) + sizeof(uint64_t);
        case varint_four_bytes:
            return sizeof(uint8_t) + sizeof(uint32_t);
        case varint_two_bytes:
            return sizeof(uint8_t) + sizeof(uint16_t);
        default:
            return sizeof(uint8_t);
    }
}

BC_POP_WARNING()

} // namespace libbitcoin

#endif
