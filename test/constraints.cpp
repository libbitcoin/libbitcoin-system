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
#include "test.hpp"

#include <string>
#include <type_traits>
#include <vector>

// helpers

typedef bool is_non_constant;
typedef const bool is_constant;

class base {};
class not_derived {};
class derived : base {};
////class not_default_constructible
////{
////    not_default_constructible(int)
////    {
////    }
////};

template <typename Type>
constexpr bool is_true()
{
    // The msvc++ compiler dislikes std::is_same outside of template arguments.
    // But visual studio intellisense properly evaluations these assertions.
    return !std::is_same<Type, std::enable_if<false, bool>>::value;
}

// Type alias.
static_assert(std::is_same<std::make_signed<size_t>::type, signed_size_t>::value, "");

// values

static_assert(is_true<if_odd<1>>(), "");
static_assert(is_true<if_odd<3>>(), "");
static_assert(is_true<if_odd<5u>>(), "");
static_assert(is_true<if_odd<0xff>>(), "");
////static_assert(!is_true<if_odd<0>>(), "");
////static_assert(!is_true<if_odd<2>>(), "");
////static_assert(!is_true<if_odd<42u>>(), "");
////static_assert(!is_true<if_odd<0xfe>>(), "");

static_assert(is_true<if_even<0>>(), "");
static_assert(is_true<if_even<2>>(), "");
static_assert(is_true<if_even<42u>>(), "");
static_assert(is_true<if_even<0xfe>>(), "");
////static_assert(!is_true<if_even<1>>(), "");
////static_assert(!is_true<if_even<3>>(), "");
////static_assert(!is_true<if_even<5u>>(), "");
////static_assert(!is_true<if_even<0xff>>(), "");

static_assert(is_true<if_non_zero<1>>(), "");
////static_assert(!is_true<if_non_zero<0>>(), "");

static_assert(is_true<if_size<0, 0>>(), "");
static_assert(is_true<if_size<1, 1>>(), "");
static_assert(is_true<if_size<42, 42>>(), "");
////static_assert(!is_true<if_size<0, 1>>(), "");
////static_assert(!is_true<if_size<1, 42>>(), "");
////static_assert(!is_true<if_size<42, 1>>(), "");

static_assert(is_true<if_greater<1, 0>>(), "");
////static_assert(!is_true<if_greater<0, 0>>(), "");
////static_assert(!is_true<if_greater<0, 1>>(), "");

static_assert(is_true<if_not_greater<0, 0>>(), "");
static_assert(is_true<if_not_greater<0, 1>>(), "");
////static_assert(!is_true<if_not_greater<1, 0>>(), "");

static_assert(is_true<if_lesser<0, 1>>(), "");
////static_assert(!is_true<if_lesser<1, 0>>(), "");
////static_assert(!is_true<if_lesser<0, 0>>(), "");

static_assert(is_true<if_not_lesser<0, 0>>(), "");
static_assert(is_true<if_not_lesser<1, 0>>(), "");
////static_assert(!is_true<if_not_lesser<0, 1>>(), "");

// types

static_assert(is_true<if_same<bool, bool>>(), "");
static_assert(is_true<if_same<uint8_t, uint8_t>>(), "");
static_assert(is_true<if_same<uint8_t, unsigned char>>(), "");
static_assert(is_true<if_same<unsigned char, uint8_t>>(), "");
static_assert(is_true<if_same<base, base>>(), "");
////static_assert(!is_true<if_same<base, derived>>(), "");
////static_assert(!is_true<if_same<bool, char>>(), "");
////static_assert(!is_true<if_same<char, bool>>(), "");

static_assert(is_true<if_byte<bool>>(), "");
static_assert(is_true<if_byte<char>>(), "");
static_assert(is_true<if_byte<uint8_t>>(), "");
////static_assert(!is_true<if_byte<float>>(), "");
////static_assert(!is_true<if_byte<double>>(), "");
////static_assert(!is_true<if_byte<int16_t>>(), "");
////static_assert(!is_true<if_byte<uint32_t>>(), "");
////static_assert(!is_true<if_byte<int64_t>>(), "");
////static_assert(!is_true<if_byte<size_t>>(), "");
////static_assert(!is_true<if_byte<base>>(), "");

static_assert(is_true<if_bytes<int16_t>>(), "");
static_assert(is_true<if_bytes<uint32_t>>(), "");
static_assert(is_true<if_bytes<int64_t>>(), "");
static_assert(is_true<if_bytes<size_t>>(), "");
static_assert(is_true<if_bytes<float>>(), "");
static_assert(is_true<if_bytes<double>>(), "");
////static_assert(!is_true<if_bytes<bool>>(), "");
////static_assert(!is_true<if_bytes<char>>(), "");
////static_assert(!is_true<if_bytes<uint8_t>>(), "");
////static_assert(!is_true<if_bytes<base>>(), "");

static_assert(is_true<if_const<is_constant>>(), "");
////static_assert(!is_true<if_const<is_non_constant>>(), "");

static_assert(is_true<if_non_const<is_non_constant>>(), "");
////static_assert(!is_true<if_non_const<is_constant>>(), "");

static_assert(is_true<if_base_of<base, base>>(), "");
static_assert(is_true<if_base_of<base, derived>>(), "");
////static_assert(!is_true<if_base_of<base, not_derived>>(), "");
////static_assert(!is_true<if_base_of<uint8_t, uint8_t>>(), "");
////static_assert(!is_true<if_base_of<uint8_t, uint32_t>>(), "");
////static_assert(!is_true<if_base_of<float, double>>(), "");

static_assert(is_true<if_byte_insertable<std::string>>(), "");
static_assert(is_true<if_byte_insertable<std::vector<uint8_t>>>(), "");
////static_assert(!is_true<if_byte_insertable<std::u32string>>(), "");
////static_assert(!is_true<if_byte_insertable<std::vector<uint32_t>>>(), "");
////static_assert(!is_true<if_byte_insertable<uint32_t>>(), "");

static_assert(is_true<if_default_constructible<bool>>(), "");
static_assert(is_true<if_default_constructible<std::string>>(), "");
static_assert(is_true<if_default_constructible<std::vector<bool>>>(), "");
static_assert(is_true<if_default_constructible<std::array<bool, 42>>>(), "");
////static_assert(!is_true<if_default_constructible<not_default_constructible>>(), "");

// if_same_width
static_assert(is_true<if_same_width<bool, bool>>(), "");
static_assert(is_true<if_same_width<int, int>>(), "");
static_assert(is_true<if_same_width<int, uint32_t>>(), "");
static_assert(is_true<if_same_width<int32_t, int>>(), "");
static_assert(is_true<if_same_width<int8_t, int8_t>>(), "");
static_assert(is_true<if_same_width<uint8_t, uint8_t>>(), "");
static_assert(is_true<if_same_width<int16_t, uint16_t>>(), "");
static_assert(is_true<if_same_width<uint16_t, int16_t>>(), "");
static_assert(is_true<if_same_width<int32_t, uint32_t>>(), "");
static_assert(is_true<if_same_width<uint32_t, int32_t>>(), "");
static_assert(is_true<if_same_width<int64_t, uint64_t>>(), "");
static_assert(is_true<if_same_width<uint64_t, int64_t>>(), "");
////static_assert(!is_true<if_same_width<bool, int>>(), "");
////static_assert(!is_true<if_same_width<int, bool>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int16_t>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int32_t>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int64_t>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int16_t>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int32_t>>(), "");
////static_assert(!is_true<if_same_width<int8_t, int64_t>>(), "");
////static_assert(!is_true<if_same_width<int16_t, int32_t>>(), "");
////static_assert(!is_true<if_same_width<int16_t, int64_t>>(), "");
////static_assert(!is_true<if_same_width<int32_t, int64_t>>(), "");

// if_lesser_width
////static_assert(!is_true<if_lesser_width<bool, bool>>(), "");
////static_assert(!is_true<if_lesser_width<int, int>>(), "");
////static_assert(!is_true<if_lesser_width<int, uint32_t>>(), "");
////static_assert(!is_true<if_lesser_width<int32_t, int>>(), "");
////static_assert(!is_true<if_lesser_width<int8_t, int8_t>>(), "");
////static_assert(!is_true<if_lesser_width<uint8_t, uint8_t>>(), "");
////static_assert(!is_true<if_lesser_width<int16_t, uint16_t>>(), "");
////static_assert(!is_true<if_lesser_width<uint16_t, int16_t>>(), "");
////static_assert(!is_true<if_lesser_width<int32_t, uint32_t>>(), "");
////static_assert(!is_true<if_lesser_width<uint32_t, int32_t>>(), "");
////static_assert(!is_true<if_lesser_width<int64_t, uint64_t>>(), "");
////static_assert(!is_true<if_lesser_width<uint64_t, int64_t>>(), "");
static_assert(is_true<if_lesser_width<bool, int>>(), "");
////static_assert(!is_true<if_lesser_width<int, bool>>(), "");
static_assert(is_true<if_lesser_width<int8_t, int16_t>>(), "");
static_assert(is_true<if_lesser_width<int8_t, int32_t>>(), "");
static_assert(is_true<if_lesser_width<int8_t, int64_t>>(), "");
static_assert(is_true<if_lesser_width<int16_t, int32_t>>(), "");
static_assert(is_true<if_lesser_width<int16_t, int64_t>>(), "");
static_assert(is_true<if_lesser_width<int32_t, int64_t>>(), "");

// if_not_lesser_width
static_assert(is_true<if_not_lesser_width<bool, bool>>(), "");
static_assert(is_true<if_not_lesser_width<int, int>>(), "");
static_assert(is_true<if_not_lesser_width<int, uint32_t>>(), "");
static_assert(is_true<if_not_lesser_width<int32_t, int>>(), "");
static_assert(is_true<if_not_lesser_width<int8_t, int8_t>>(), "");
static_assert(is_true<if_not_lesser_width<uint8_t, uint8_t>>(), "");
static_assert(is_true<if_not_lesser_width<int16_t, uint16_t>>(), "");
static_assert(is_true<if_not_lesser_width<uint16_t, int16_t>>(), "");
static_assert(is_true<if_not_lesser_width<int32_t, uint32_t>>(), "");
static_assert(is_true<if_not_lesser_width<uint32_t, int32_t>>(), "");
static_assert(is_true<if_not_lesser_width<int64_t, uint64_t>>(), "");
static_assert(is_true<if_not_lesser_width<uint64_t, int64_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<bool, int>>(), "");
////static_assert(is_true<if_not_lesser_width<int, bool>>(), "");
////static_assert(!is_true<if_not_lesser_width<int8_t, int16_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<int8_t, int32_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<int8_t, int64_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<int16_t, int32_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<int16_t, int64_t>>(), "");
////static_assert(!is_true<if_not_lesser_width<int32_t, int64_t>>(), "");

// integer types

static_assert(is_true<if_integer<char>>(), "");
static_assert(is_true<if_integer<int>>(), "");
static_assert(is_true<if_integer<uint8_t>>(), "");
static_assert(is_true<if_integer<uint32_t>>(), "");
static_assert(is_true<if_integer<int32_t>>(), "");
static_assert(is_true<if_integer<uintx>>(), "");
////static_assert(!is_true<if_integer<bool>>(), "");
////static_assert(!is_true<if_integer<float>>(), "");
////static_assert(!is_true<if_integer<double>>(), "");
////static_assert(!is_true<if_integer<base>>(), "");

static_assert(is_true<if_signed_integer<char>>(), "");
static_assert(is_true<if_signed_integer<int>>(), "");
static_assert(is_true<if_signed_integer<int16_t>>(), "");
static_assert(is_true<if_signed_integer<int32_t>>(), "");
static_assert(is_true<if_signed_integer<int64_t>>(), "");
////static_assert(!is_true<if_signed_integer<bool>>(), "");
////static_assert(!is_true<if_signed_integer<float>>(), "");
////static_assert(!is_true<if_signed_integer<double>>(), "");
////static_assert(!is_true<if_signed_integer<uint16_t>>(), "");
////static_assert(!is_true<if_signed_integer<size_t>>(), "");
////static_assert(!is_true<if_signed_integer<base>>(), "");

static_assert(is_true<if_unsigned_integer<uint16_t>>(), "");
static_assert(is_true<if_unsigned_integer<size_t>>(), "");
////static_assert(!is_true<if_unsigned_integer<bool>>(), "");
////static_assert(!is_true<if_unsigned_integer<char>>(), "");
////static_assert(!is_true<if_unsigned_integer<int>>(), "");
////static_assert(!is_true<if_unsigned_integer<float>>(), "");
////static_assert(!is_true<if_unsigned_integer<double>>(), "");
////static_assert(!is_true<if_unsigned_integer<int16_t>>(), "");
////static_assert(!is_true<if_unsigned_integer<int32_t>>(), "");
////static_assert(!is_true<if_unsigned_integer<int64_t>>(), "");
////static_assert(!is_true<if_unsigned_integer<base>>(), "");

static_assert(is_true<if_same_signed_integer<int, char>>(), "");
static_assert(is_true<if_same_signed_integer<uint16_t, uint8_t>>(), "");
////static_assert(!is_true<if_same_signed_integer<uint16_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<size_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<bool, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<int, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<float, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<double, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<int16_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<int32_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<int64_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<base, bool>>(), "");

static_assert(is_true<if_same_signed_integer<char, char>>(), "");
static_assert(is_true<if_same_signed_integer<char, int>>(), "");
static_assert(is_true<if_same_signed_integer<char, int16_t>>(), "");
static_assert(is_true<if_same_signed_integer<char, int32_t>>(), "");
static_assert(is_true<if_same_signed_integer<char, int64_t>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, uint16_t>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, size_t>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, float>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, double>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, base>>(), "");

static_assert(is_true<if_same_signed_integer<int, int>>(), "");
static_assert(is_true<if_same_signed_integer<char, char>>(), "");
static_assert(is_true<if_same_signed_integer<uint8_t, uint8_t>>(), "");
static_assert(is_true<if_same_signed_integer<uint32_t, uint32_t>>(), "");
static_assert(is_true<if_same_signed_integer<uint64_t, uint64_t>>(), "");
////static_assert(is_true<if_same_signed_integer<bool, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<float, float>>(), "");
////static_assert(!is_true<if_same_signed_integer<double, double>>(), "");
////static_assert(!is_true<if_same_signed_integer<base, base>>(), "");

static_assert(is_true<if_same_signed_integer<int, char>>(), "");
static_assert(is_true<if_same_signed_integer<uint16_t, uint8_t>>(), "");
static_assert(is_true<if_same_signed_integer<uint8_t, unsigned char>>(), "");
static_assert(is_true<if_same_signed_integer<unsigned char, uint8_t>>(), "");
////static_assert(!is_true<if_same_signed_integer<int16_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<uint16_t, bool>>(), "");
////static_assert(!is_true<if_same_signed_integer<char, unsigned char>>(), "");

////static_assert(!is_true<if_not_same_signed_integer<int, char>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<uint16_t, uint8_t>>(), "");
static_assert(is_true<if_not_same_signed_integer<uint16_t, int>>(), "");
static_assert(is_true<if_not_same_signed_integer<size_t, int>>(), "");
static_assert(is_true<if_not_same_signed_integer<int, uint16_t>>(), "");
static_assert(is_true<if_not_same_signed_integer<int, size_t>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<bool, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<char, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<int, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<float, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<double, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<int16_t, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<int32_t, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<int64_t, int>>(), "");
////static_assert(!is_true<if_not_same_signed_integer<base, int>>(), "");

// integral integer types

static_assert(is_true<if_integral_integer<char>>(), "");
static_assert(is_true<if_integral_integer<int>>(), "");
static_assert(is_true<if_integral_integer<uint8_t>>(), "");
static_assert(is_true<if_integral_integer<uint32_t>>(), "");
static_assert(is_true<if_integral_integer<int32_t>>(), "");
////static_assert(!is_true<if_integral_integer<bool>>(), "");
////static_assert(!is_true<if_integral_integer<uintx>>(), "");
////static_assert(!is_true<if_integral_integer<float>>(), "");
////static_assert(!is_true<if_integral_integer<double>>(), "");
////static_assert(!is_true<if_integral_integer<base>>(), "");

static_assert(is_true<if_non_integral_integer<uintx>>(), "");
static_assert(is_true<if_non_integral_integer<uint256_t>>(), "");
////static_assert(!is_true<if_non_integral_integer<bool>>(), "");
////static_assert(!is_true<if_non_integral_integer<char>>(), "");
////static_assert(!is_true<if_non_integral_integer<int>>(), "");
////static_assert(!is_true<if_non_integral_integer<uint8_t>>(), "");
////static_assert(!is_true<if_non_integral_integer<uint32_t>>(), "");
////static_assert(!is_true<if_non_integral_integer<int32_t>>(), "");
////static_assert(!is_true<if_non_integral_integer<float>>(), "");
////static_assert(!is_true<if_non_integral_integer<double>>(), "");
////static_assert(!is_true<if_non_integral_integer<base>>(), "");

// Unsigned type determination by byte width.
static_assert(std::is_same<signed_type<0>, signed_size_t>::value, "");
static_assert(std::is_same<signed_type<1>, int8_t>::value, "");
static_assert(std::is_same<signed_type<2>, int16_t>::value, "");
static_assert(std::is_same<signed_type<3>, int32_t>::value, "");
static_assert(std::is_same<signed_type<4>, int32_t>::value, "");
static_assert(std::is_same<signed_type<5>, int64_t>::value, "");
static_assert(std::is_same<signed_type<6>, int64_t>::value, "");
static_assert(std::is_same<signed_type<7>, int64_t>::value, "");
static_assert(std::is_same<signed_type<8>, int64_t>::value, "");

// Signed type determination by byte width.
static_assert(std::is_same<unsigned_type<0>, size_t>::value, "");
static_assert(std::is_same<unsigned_type<1>, uint8_t>::value, "");
static_assert(std::is_same<unsigned_type<2>, uint16_t>::value, "");
static_assert(std::is_same<unsigned_type<3>, uint32_t>::value, "");
static_assert(std::is_same<unsigned_type<4>, uint32_t>::value, "");
static_assert(std::is_same<unsigned_type<5>, uint64_t>::value, "");
static_assert(std::is_same<unsigned_type<6>, uint64_t>::value, "");
static_assert(std::is_same<unsigned_type<7>, uint64_t>::value, "");
static_assert(std::is_same<unsigned_type<8>, uint64_t>::value, "");
