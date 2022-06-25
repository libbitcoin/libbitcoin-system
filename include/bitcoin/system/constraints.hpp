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
#ifndef LIBBITCOIN_SYSTEM_CONSTRAINTS_HPP
#define LIBBITCOIN_SYSTEM_CONSTRAINTS_HPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// C++11: std::conditional
// C++14: std::conditional_t
// C++17: std::conjunction/conjunction_v
// C++17: std::disjunction/disjunction_v
// C++17: std::negation/negation_v
// C++11: std::integral_constant

template <bool Condition>
using bool_unless = std::enable_if_t<Condition, bool>;

/// Sizes.

template <size_t Value>
using if_odd = bool_unless<
    is_odd(Value)>;

template <size_t Value>
using if_even = bool_unless<
    is_even(Value)>;

template <size_t Value>
using if_zero = bool_unless<
    is_zero(Value)>;

template <size_t Value>
using if_nonzero = bool_unless<
    is_nonzero(Value)>;

template <size_t Left, size_t Right>
using if_equal = bool_unless<
    (Left == Right)>;

template <size_t Left, size_t Right>
using if_greater = bool_unless<
    (Left > Right)>;

template <size_t Left, size_t Right>
using if_not_greater = bool_unless<
    !(Left > Right)>;

template <size_t Left, size_t Right>
using if_lesser = bool_unless<
    (Left < Right)>;

template <size_t Left, size_t Right>
using if_not_lesser = bool_unless<
    !(Left < Right)>;

template <size_t Bits>
using if_bytes_width = bool_unless<
    is_bytes_width(Bits)>;

template <size_t Bytes>
using if_integral_size = bool_unless<
    is_integral_size(Bytes)>;

/// Types.

template <typename Left, typename Right>
using if_same = bool_unless<
    is_same<Left, Right>()>;

template <typename Left, typename Right>
using if_not_same = bool_unless<
    !is_same<Left, Right>()>;

template <typename Type>
using if_one_byte = bool_unless<
    sizeof(Type) == sizeof(uint8_t)>;

template <typename Type>
using if_not_one_byte = bool_unless<
    sizeof(Type) != sizeof(uint8_t)>;

template <typename Type, size_t Size>
using if_size_of = bool_unless<
    sizeof(Type) == Size>;

template <typename Type>
using if_const = bool_unless<
    std::is_const<Type>::value>;

template <typename Type>
using if_non_const = bool_unless<
    !std::is_const<Type>::value>;

template <typename Base, typename Type>
using if_base_of = bool_unless<
    std::is_base_of<Base, Type>::value>;

template <typename Left, typename Right>
using if_same_width = bool_unless<
    width<Left>() == width<Right>()>;

template <typename Left, typename Right>
using if_lesser_width = bool_unless<
    width<Left>() < width<Right>()>;

template <typename Left, typename Right>
using if_not_lesser_width = bool_unless<
    width<Left>() >= width<Right>()>;

template <typename Type>
using if_default_constructible = bool_unless<
    std::is_default_constructible<Type>::value>;

template <typename Type>
using if_trivially_constructible = bool_unless<
    std::is_trivially_constructible<Type>::value>;

template <typename Type>
using if_unique_object_representations = bool_unless<
    std::has_unique_object_representations<Type>::value>;

template <typename Type>
using if_byte_insertable = bool_unless<
    std::is_base_of<std::string, Type>::value ||
    std::is_base_of<std::vector<uint8_t>, Type>::value>;

/// Integer types (specializable, non-floating math, non-bool).

template <typename Type>
using if_integer = bool_unless<
    is_integer<Type>()>;

template <typename Type>
using if_signed_integer = bool_unless<
    is_signed<Type>() &&
    is_integer<Type>()>;

template <typename Type>
using if_unsigned_integer = bool_unless<
    !is_signed<Type>() &&
    is_integer<Type>()>;

template <typename Left, typename Right>
using if_same_signed_integer = bool_unless<
    (is_signed<Left>() == is_signed<Right>()) &&
    (is_integer<Left>() && is_integer<Right>())>;

template <typename Left, typename Right>
using if_not_same_signed_integer = bool_unless<
    (is_signed<Left>() != is_signed<Right>()) &&
    (is_integer<Left>() && is_integer<Right>())>;

/// Integral integer types (native, non-floating math, non-bool).

template <typename Type>
using if_integral_integer = bool_unless<
    is_integral<Type>() &&
    is_integer<Type>()>;

template <typename Type>
using if_non_integral_integer = bool_unless<
    !is_integral<Type>() &&
    is_integer<Type>()>;

template <typename Type>
using if_signed_integral_integer = bool_unless<
    is_signed<Type>() &&
    is_integral<Type>() &&
    is_integer<Type>()>;

template <typename Type>
using if_unsigned_integral_integer = bool_unless<
    !is_signed<Type>() &&
    is_integral<Type>() &&
    is_integer<Type>()>;

/// Type determination by required byte width and sign.

template <size_t Bytes, if_not_greater<Bytes, sizeof(int64_t)> = true>
using signed_type =
    std::conditional_t<Bytes == 0, signed_size_t,
        std::conditional_t<Bytes == 1, int8_t,
            std::conditional_t<Bytes == 2, int16_t,
                std::conditional_t<Bytes <= 4, int32_t,
                    int64_t>>>>;

template <size_t Bytes, if_not_greater<Bytes, sizeof(uint64_t)> = true>
using unsigned_type =
    std::conditional_t<Bytes == 0, size_t,
        std::conditional_t<Bytes == 1, uint8_t,
            std::conditional_t<Bytes == 2, uint16_t,
                std::conditional_t<Bytes <= 4, uint32_t,
                    uint64_t>>>>;

/// Use instead of std::make_signed.
template <typename Type>
using to_signed_type = signed_type<sizeof(Type)>;

/// Use instead of std::make_unsigned.
template <typename Type>
using to_unsigned_type = unsigned_type<sizeof(Type)>;

/// Alias for -> decltype(dividend / divisor).
template <typename Left, typename Right>
using to_common_type = typename std::common_type<Left, Right>::type;

/// Endianness.

constexpr auto is_big_endian = std::endian::native == std::endian::big;
constexpr auto is_little_endian = std::endian::native == std::endian::little;
constexpr auto is_unknown_endian = !is_big_endian && !is_little_endian;
static_assert(!is_unknown_endian, "unsupported integer representation");

template <typename Integer>
using if_big_endian_integral_integer = bool_unless<
    is_big_endian &&
    is_integral<Integer>() &&
    is_integer<Integer>()>;

template <typename Integer>
using if_little_endian_integral_integer = bool_unless<
    is_little_endian &&
    is_integral<Integer>() &&
    is_integer<Integer>()>;

/// uintx_t.

/// Template for constructing uintx types.
/// There is no dynamic memory allocation when minBits == maxBits.
template <uint32_t Bits>
using uintx_t = boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<Bits, Bits,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>>;

/// Cannot generalize because no boost support for unsigned arbitrary precision.
/// Otherwise uintx_t<0> would suffice. uintx can construct from uintx_t types
/// but is not a base type. Use of signed types here would also not generalize
/// as boost uses a different allocator for arbitrary precision. So we are stuck
/// with this seam, requiring template specialization for uintx.
typedef boost::multiprecision::cpp_int uintx;

// C++11: use std::integral_constant (up to primitives limit).
typedef uintx_t<5> uint5_t;
typedef uintx_t<11> uint11_t;
typedef uintx_t<48> uint48_t;
typedef uintx_t<128> uint128_t;
typedef uintx_t<160> uint160_t;
typedef uintx_t<256> uint256_t;
typedef uintx_t<512> uint512_t;

template <size_t Bytes>
using unsigned_extended_type =
    std::conditional_t<Bytes == 0, size_t,
        std::conditional_t<Bytes == 1, uint8_t,
            std::conditional_t<Bytes == 2, uint16_t,
                std::conditional_t<Bytes <= 4, uint32_t,
                    std::conditional_t<Bytes <= 8, uint64_t,
                        uintx_t<Bytes>>>>>>;

} // namespace system
} // namespace libbitcoin

#endif
