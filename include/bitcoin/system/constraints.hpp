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
#ifndef LIBBITCOIN_SYSTEM_CONSTRAINTS_HPP
#define LIBBITCOIN_SYSTEM_CONSTRAINTS_HPP

#include <bitcoin/system/typelets.hpp>

namespace libbitcoin {

template <bool Expression>
using bool_if = std::enable_if_t<Expression, bool>;

/// Sizes.
/// These were originally typed to size_t, but this causes implicit narrow
/// casting from uint64_t when sizeof(size_t) == 32. This results in truncated
/// constant warnings and unexpected template matching failures. So these are
/// now typed to the largest integral size, since they do not return values.

template <uint64_t Value>
using if_odd = bool_if<
    is_odd(Value)>;

template <uint64_t Value>
using if_even = bool_if<
    is_even(Value)>;

template <uint64_t Value>
using if_zero = bool_if<
    is_zero(Value)>;

template <uint64_t Value>
using if_nonzero = bool_if<
    is_nonzero(Value)>;

template <uint64_t Left, uint64_t Right>
using if_equal = bool_if<
    (Left == Right)>;

template <uint64_t Left, uint64_t Right>
using if_not_equal = bool_if<
    (Left != Right)>;

template <uint64_t Left, uint64_t Right>
using if_greater = bool_if<
    (Left > Right)>;

template <uint64_t Left, uint64_t Right>
using if_not_greater = bool_if<
    !(Left > Right)>;

template <uint64_t Left, uint64_t Right>
using if_lesser = bool_if<
    (Left < Right)>;

template <uint64_t Left, uint64_t Right>
using if_not_lesser = bool_if<
    !(Left < Right)>;

template <uint64_t Bits>
using if_bytes_width = bool_if<
    is_byte_sized(Bits)>;

template <uint64_t Bytes>
using if_integral_size = bool_if<
    is_integral_sized(Bytes)>;

/// Types.

template <typename Left, typename Right>
using if_same = bool_if<
    is_same_type<Left, Right>>;

template <typename Left, typename Right>
using if_not_same = bool_if<
    !is_same_type<Left, Right>>;

template <typename Type>
using if_one_byte = bool_if<
    is_same_size<Type, uint8_t>>;

template <typename Type>
using if_not_one_byte = bool_if<
    !is_same_size<Type, uint8_t>>;

template <typename Type, uint64_t Size>
using if_size_of = bool_if<
    sizeof(Type) == Size>;

template <typename Type>
using if_const = bool_if<
    std::is_const<Type>::value>;

template <typename Type>
using if_non_const = bool_if<
    !std::is_const<Type>::value>;

template <typename Base, typename Type>
using if_base_of = bool_if<
    std::is_base_of<Base, Type>::value>;

template <typename Base, typename Type>
using if_not_base_of = bool_if<
    !std::is_base_of<Base, Type>::value>;

template <typename Left, typename Right>
using if_same_size = bool_if<
    is_same_size<Left, Right>>;

template <typename Left, typename Right>
using if_not_same_size = bool_if<
    !is_same_size<Left, Right>>;

template <typename Left, typename Right>
using if_lesser_size = bool_if<
    is_lesser_size<Left, Right>>;

template <typename Left, typename Right>
using if_not_lesser_size = bool_if<
    !is_lesser_size<Left, Right>>;

template <typename Left, typename Right>
using if_greater_size = bool_if<
    is_greater_size<Left, Right>>;

template <typename Left, typename Right>
using if_not_greater_size = bool_if<
    !is_greater_size<Left, Right>>;

template <typename Type>
using if_common = bool_if<
    is_common_size<Type>>;

template <typename Type>
using if_default_constructible = bool_if<
    std::is_default_constructible<Type>::value>;

template <typename Type>
using if_trivially_constructible = bool_if<
    std::is_trivially_constructible<Type>::value>;

template <typename Type>
using if_unique_object_representations = bool_if<
    std::has_unique_object_representations<Type>::value>;

/// Integer types (specializable, non-floating math, non-bool).

template <typename Type>
using if_integer = bool_if<
    is_integer<Type>>;

template <typename Type>
using if_non_integer = bool_if<
    !is_integer<Type>>;

template <typename Type>
using if_signed_integer = bool_if<
    is_integer<Type> &&
    is_signed<Type>>;

template <typename Type>
using if_unsigned_integer = bool_if<
    is_integer<Type> &&
    !is_signed<Type>>;

template <typename Left, typename Right>
using if_same_signed_integer = bool_if<
    is_integer<Left> &&
    is_integer<Right> &&
    (is_signed<Left> == is_signed<Right>)>;

template <typename Left, typename Right>
using if_not_same_signed_integer = bool_if<
    is_integer<Left> &&
    is_integer<Right> &&
    (is_signed<Left> != is_signed<Right>)>;

template <typename Type>
using if_uintx = bool_if<
    is_uintx<Type>>;

template <typename Type>
using if_not_uintx = bool_if<
    !is_uintx<Type>>;

/// Integral integer types (native, non-floating point, non-bool).
/// All integrals are integers but not all integers are integral.

template <typename Type>
using if_integral = bool_if<
    is_integral<Type>>;

template <typename Type>
using if_non_integral = bool_if<
    !is_integral<Type>>;

// Alias for if_integral.
template <typename Type>
using if_integral_integer = bool_if<
    is_integral_integer<Type>>;

template <typename Type>
using if_non_integral_integer = bool_if<
    !is_integral<Type>  &&
    is_integer<Type>>;

template <typename Type>
using if_signed_integral_integer = bool_if<
    is_integral_integer<Type> &&
    is_signed<Type>>;

template <typename Type>
using if_unsigned_integral_integer = bool_if<
    is_integral_integer<Type> &&
    !is_signed<Type>>;

template <typename Left, typename Right>
using if_same_signed_integral_integer = bool_if<
    is_integral_integer<Left> &&
    is_integral_integer<Right> &&
    (is_signed<Left> == is_signed<Right>)>;

template <typename Left, typename Right>
using if_not_same_signed_integral_integer = bool_if<
    is_integral_integer<Left> &&
    is_integral_integer<Right> &&
    (is_signed<Left> != is_signed<Right>)>;

template <typename Integer>
using if_big_endian_integral_integer = bool_if<
    is_integral_integer<Integer> &&
    is_big_endian>;

template <typename Integer>
using if_little_endian_integral_integer = bool_if<
    is_integral_integer<Integer> &&
    is_little_endian>;

/// std::array/std::vector

template <typename Type>
using if_std_array = bool_if<
    is_std_array<Type>>;

template <typename Type>
using if_not_std_array = bool_if<
    !is_std_array<Type>>;

template <typename Type>
using if_integral_array = bool_if<
    is_std_array<Type> &&
    is_integral_integer<typename Type::value_type>>;

template <typename Type>
using if_byte_insertable = bool_if<
    std::is_base_of<std::string, Type>::value ||
    std::is_base_of<std_vector<uint8_t>, Type>::value ||
    std::is_base_of<std::vector<uint8_t>, Type>::value>;

} // namespace libbitcoin

#endif
