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

/// Values.

template <size_t Value>
using if_odd = std::enable_if_t<is_odd(Value), bool>;

template <size_t Value>
using if_even = std::enable_if_t<is_even(Value), bool>;

template <size_t Value>
using if_non_zero = std::enable_if_t<!is_zero(Value), bool>;

template <size_t Value, size_t Size>
using if_equal = std::enable_if_t<Value == Size, bool>;

template <size_t Left, size_t Right>
using if_greater = std::enable_if_t<(Left > Right), bool>;

template <size_t Left, size_t Right>
using if_not_greater = std::enable_if_t<!(Left > Right), bool>;

template <size_t Left, size_t Right>
using if_lesser = std::enable_if_t<(Left < Right), bool>;

template <size_t Left, size_t Right>
using if_not_lesser = std::enable_if_t<!(Left < Right), bool>;

/// Types.

template <typename Left, typename Right>
using if_same = std::is_same<Left, Right>;

template <typename Type>
using if_byte = std::enable_if_t<!(width<Type>() > width<uint8_t>()), bool>;

template <typename Type>
using if_bytes = std::enable_if_t<(width<Type>() > width<uint8_t>()), bool>;

template <typename Type, size_t Size>
using if_size_of = std::enable_if_t<sizeof(Type) == Size, bool>;

template <typename Type>
using if_const = std::enable_if_t<std::is_const<Type>::value, bool>;

template <typename Type>
using if_non_const = std::enable_if_t<!std::is_const<Type>::value, bool>;

template <typename Base, typename Type>
using if_base_of = std::enable_if_t<
    std::is_base_of<Base, Type>::value, bool>;

template <typename Type>
using if_byte_insertable = std::enable_if_t<
    std::is_base_of<std::vector<uint8_t>, Type>::value ||
    std::is_base_of<std::string, Type>::value, bool>;

template <typename Type>
using if_default_constructible = std::enable_if_t<
    std::is_default_constructible<Type>::value, bool>;

template <typename Type>
using if_unique_object_representations = std::enable_if_t<
    std::has_unique_object_representations<Type>::value, bool>;

template <typename Left, typename Right>
using if_same_width = std::enable_if_t<width<Left>() == width<Right>(), bool>;

template <typename Left, typename Right>
using if_lesser_width = std::enable_if_t<width<Left>() < width<Right>(), bool>;

template <typename Left, typename Right>
using if_not_lesser_width = std::enable_if_t<width<Left>() >= width<Right>(),
    bool>;

// Integer types (specializable, non-floating math, no bool).

template <typename Type>
using if_integer = std::enable_if_t<is_integer<Type>(), bool>;

template <typename Type>
using if_signed_integer = std::enable_if_t<is_integer<Type>() &&
    std::is_signed<Type>::value, bool>;

template <typename Type>
using if_unsigned_integer = std::enable_if_t<is_integer<Type>() &&
    !std::is_signed<Type>::value, bool>;

template <typename Left, typename Right>
using if_same_signed_integer = std::enable_if_t<
    is_integer<Left>() && is_integer<Right>() &&
    (std::is_signed<Left>::value == std::is_signed<Right>::value), bool>;

template <typename Left, typename Right>
using if_not_same_signed_integer = std::enable_if_t<
    is_integer<Left>() && is_integer<Right>() &&
    (std::is_signed<Left>::value != std::is_signed<Right>::value), bool>;

// Integral integer types (native, non-floating math, no bool).

template <typename Type>
using if_integral_integer = std::enable_if_t<is_integer<Type>() &&
    std::is_integral<Type>::value, bool>;

template <typename Type>
using if_non_integral_integer = std::enable_if_t<is_integer<Type>() &&
    !std::is_integral<Type>::value, bool>;

/// Type determination by required byte width and sign.

template <size_t Bytes, if_not_greater<Bytes, sizeof(int64_t)> = true>
using signed_type =
    std::conditional_t<Bytes == 0, signed_size_t,
        std::conditional_t<Bytes == 1, int8_t,
            std::conditional_t<Bytes == 2, int16_t,
                std::conditional_t<Bytes <= 4, int32_t, int64_t>>>>;

template <size_t Bytes, if_not_greater<Bytes, sizeof(uint64_t)> = true>
using unsigned_type =
    std::conditional_t< Bytes == 0, size_t,
        std::conditional_t<Bytes == 1, uint8_t,
            std::conditional_t<Bytes == 2, uint16_t,
                std::conditional_t<Bytes <= 4, uint32_t, uint64_t>>>>;

} // namespace system
} // namespace libbitcoin

#endif
