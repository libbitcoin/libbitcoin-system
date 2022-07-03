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

typedef bool non_constant;
typedef const bool constant;
class base {};
class not_derived {};
class derived : base {};
////class not_default_constructible { not_default_constructible(int) {}};

template <typename>
constexpr bool is_defined = true;

// sizes

static_assert(is_defined<if_odd<1>>);
static_assert(is_defined<if_odd<3>>);
static_assert(is_defined<if_odd<5u>>);
static_assert(is_defined<if_odd<0xff>>);
////static_assert(!is_defined<if_odd<0>>);
////static_assert(!is_defined<if_odd<2>>);
////static_assert(!is_defined<if_odd<42u>>);
////static_assert(!is_defined<if_odd<0xfe>>);

static_assert(is_defined<if_even<0>>);
static_assert(is_defined<if_even<2>>);
static_assert(is_defined<if_even<42u>>);
static_assert(is_defined<if_even<0xfe>>);
////static_assert(!is_defined<if_even<1>>);
////static_assert(!is_defined<if_even<3>>);
////static_assert(!is_defined<if_even<5u>>);
////static_assert(!is_defined<if_even<0xff>>);

static_assert(is_defined<if_zero<0>>);
////static_assert(!is_defined<if_zero<1>>);

static_assert(is_defined<if_nonzero<1>>);
////static_assert(!is_defined<if_nonzero<0>>);

static_assert(is_defined<if_equal<0, 0>>);
static_assert(is_defined<if_equal<1, 1>>);
static_assert(is_defined<if_equal<42, 42>>);
////static_assert(!is_defined<if_equal<0, 1>>);
////static_assert(!is_defined<if_equal<1, 42>>);
////static_assert(!is_defined<if_equal<42, 1>>);

static_assert(is_defined<if_greater<1, 0>>);
////static_assert(!is_defined<if_greater<0, 0>>);
////static_assert(!is_defined<if_greater<0, 1>>);

static_assert(is_defined<if_not_greater<0, 0>>);
static_assert(is_defined<if_not_greater<0, 1>>);
////static_assert(!is_defined<if_not_greater<1, 0>>);

static_assert(is_defined<if_lesser<0, 1>>);
////static_assert(!is_defined<if_lesser<1, 0>>);
////static_assert(!is_defined<if_lesser<0, 0>>);

static_assert(is_defined<if_not_lesser<0, 0>>);
static_assert(is_defined<if_not_lesser<1, 0>>);
////static_assert(!is_defined<if_not_lesser<0, 1>>);

static_assert(is_defined<if_bytes_width<8>>);
static_assert(is_defined<if_bytes_width<16>>);
static_assert(is_defined<if_bytes_width<24>>);
static_assert(is_defined<if_bytes_width<32>>);
static_assert(is_defined<if_bytes_width<40>>);
static_assert(is_defined<if_bytes_width<48>>);
static_assert(is_defined<if_bytes_width<56>>);
static_assert(is_defined<if_bytes_width<64>>);
static_assert(is_defined<if_bytes_width<128>>);
static_assert(is_defined<if_bytes_width<256>>);
////static_assert(!is_defined<if_bytes_width<0>>);
////static_assert(!is_defined<if_bytes_width<42>>);

static_assert(is_defined<if_integral_size<1>>);
static_assert(is_defined<if_integral_size<2>>);
static_assert(is_defined<if_integral_size<4>>);
static_assert(is_defined<if_integral_size<8>>);
////static_assert(!is_defined<if_integral_size<0>>);
////static_assert(!is_defined<if_integral_size<16>>);

// types

static_assert(is_defined<if_same<bool, bool>>);
static_assert(is_defined<if_same<uint8_t, uint8_t>>);
static_assert(is_defined<if_same<uint8_t, unsigned char>>);
static_assert(is_defined<if_same<unsigned char, uint8_t>>);
static_assert(is_defined<if_same<base, base>>);
////static_assert(!is_defined<if_same<base, derived>>);
////static_assert(!is_defined<if_same<bool, char>>);
////static_assert(!is_defined<if_same<char, bool>>);

static_assert(is_defined<if_not_same<base, derived>>);
static_assert(is_defined<if_not_same<bool, char>>);
static_assert(is_defined<if_not_same<char, bool>>);
////static_assert(!is_defined<if_not_same<bool, bool>>);
////static_assert(!is_defined<if_not_same<uint8_t, uint8_t>>);
////static_assert(!is_defined<if_not_same<uint8_t, unsigned char>>);
////static_assert(!is_defined<if_not_same<unsigned char, uint8_t>>);
////static_assert(!is_defined<if_not_same<base, base>>);

static_assert(is_defined<if_one_byte<int8_t>>);
static_assert(is_defined<if_one_byte<uint8_t>>);
////static_assert(is_defined<if_one_byte<char>>);
////static_assert(is_defined<if_one_byte<bool>>);
////static_assert(!is_defined<if_one_byte<uint256_t>>);
////static_assert(!is_defined<if_one_byte<float>>);
////static_assert(!is_defined<if_one_byte<double>>);
////static_assert(!is_defined<if_one_byte<int16_t>>);
////static_assert(!is_defined<if_one_byte<uint32_t>>);
////static_assert(!is_defined<if_one_byte<int64_t>>);
////static_assert(!is_defined<if_one_byte<size_t>>);
////static_assert(!is_defined<if_one_byte<base>>);

static_assert(is_defined<if_not_one_byte<int16_t>>);
static_assert(is_defined<if_not_one_byte<uint32_t>>);
static_assert(is_defined<if_not_one_byte<int64_t>>);
static_assert(is_defined<if_not_one_byte<size_t>>);
static_assert(is_defined<if_not_one_byte<wchar_t>>);
static_assert(is_defined<if_not_one_byte<float>>);
static_assert(is_defined<if_not_one_byte<double>>);
static_assert(is_defined<if_not_one_byte<uint256_t>>);
////static_assert(!is_defined<if_not_one_byte<bool>>);
////static_assert(!is_defined<if_not_one_byte<char>>);
////static_assert(!is_defined<if_not_one_byte<uint8_t>>);
////static_assert(!is_defined<if_not_one_byte<base>>);

static_assert(is_defined<if_size_of<bool, sizeof(bool)>>);
static_assert(is_defined<if_size_of<int, sizeof(int)>>);
static_assert(is_defined<if_size_of<int8_t, sizeof(uint8_t)>>);
static_assert(is_defined<if_size_of<int16_t, sizeof(uint16_t)>>);
static_assert(is_defined<if_size_of<int32_t, sizeof(uint32_t)>>);
static_assert(is_defined<if_size_of<int64_t, sizeof(uint64_t)>>);
static_assert(is_defined<if_size_of<signed_size_t, sizeof(size_t)>>);
////static_assert(!is_defined<if_size_of<bool, sizeof(int)>>);
////static_assert(!is_defined<if_size_of<int, sizeof(bool)>>);

static_assert(is_defined<if_const<constant>>);
////static_assert(!is_defined<if_const<is_non_constant>>);

static_assert(is_defined<if_non_const<non_constant>>);
////static_assert(!is_defined<if_non_const<is_constant>>);

static_assert(is_defined<if_base_of<base, base>>);
static_assert(is_defined<if_base_of<base, derived>>);
////static_assert(!is_defined<if_base_of<base, not_derived>>);
////static_assert(!is_defined<if_base_of<uint8_t, uint8_t>>);
////static_assert(!is_defined<if_base_of<uint8_t, uint32_t>>);
////static_assert(!is_defined<if_base_of<float, double>>);

static_assert(is_defined<if_same_size<bool, bool>>);
static_assert(is_defined<if_same_size<int, int>>);
static_assert(is_defined<if_same_size<int8_t, int8_t>>);
static_assert(is_defined<if_same_size<uint8_t, uint8_t>>);
static_assert(is_defined<if_same_size<int16_t, uint16_t>>);
static_assert(is_defined<if_same_size<uint16_t, int16_t>>);
static_assert(is_defined<if_same_size<int32_t, uint32_t>>);
static_assert(is_defined<if_same_size<uint32_t, int32_t>>);
static_assert(is_defined<if_same_size<int64_t, uint64_t>>);
static_assert(is_defined<if_same_size<uint64_t, int64_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int16_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int32_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int64_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int16_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int32_t>>);
////static_assert(!is_defined<if_same_size<int8_t, int64_t>>);
////static_assert(!is_defined<if_same_size<int16_t, int32_t>>);
////static_assert(!is_defined<if_same_size<int16_t, int64_t>>);
////static_assert(!is_defined<if_same_size<int32_t, int64_t>>);
////static_assert(is_defined<if_same_size<int, uint32_t>>);
////static_assert(is_defined<if_same_size<int32_t, int>>);
////static_assert(!is_defined<if_same_size<bool, int>>);
////static_assert(!is_defined<if_same_size<int, bool>>);

static_assert(is_defined<if_not_same_size<int8_t, int16_t>>);
static_assert(is_defined<if_not_same_size<int8_t, int32_t>>);
static_assert(is_defined<if_not_same_size<int8_t, int64_t>>);
static_assert(is_defined<if_not_same_size<int8_t, int16_t>>);
static_assert(is_defined<if_not_same_size<int8_t, int32_t>>);
static_assert(is_defined<if_not_same_size<int8_t, int64_t>>);
static_assert(is_defined<if_not_same_size<int16_t, int32_t>>);
static_assert(is_defined<if_not_same_size<int16_t, int64_t>>);
static_assert(is_defined<if_not_same_size<int32_t, int64_t>>);
////static_assert(is_defined<if_not_same_size<bool, bool>>);
////static_assert(is_defined<if_not_same_size<int, int>>);
////static_assert(is_defined<if_not_same_size<int8_t, int8_t>>);
////static_assert(is_defined<if_not_same_size<uint8_t, uint8_t>>);
////static_assert(is_defined<if_not_same_size<int16_t, uint16_t>>);
////static_assert(is_defined<if_not_same_size<uint16_t, int16_t>>);
////static_assert(is_defined<if_not_same_size<int32_t, uint32_t>>);
////static_assert(is_defined<if_not_same_size<uint32_t, int32_t>>);
////static_assert(is_defined<if_not_same_size<int64_t, uint64_t>>);
////static_assert(is_defined<if_not_same_size<uint64_t, int64_t>>);
////static_assert(is_defined<if_not_same_size<int, uint32_t>>);
////static_assert(is_defined<if_not_same_size<int32_t, int>>);
////static_assert(is_defined<if_not_same_size<bool, int>>);
////static_assert(is_defined<if_not_same_size<int, bool>>);

static_assert(is_defined<if_lesser_size<int8_t, int16_t>>);
static_assert(is_defined<if_lesser_size<int8_t, int32_t>>);
static_assert(is_defined<if_lesser_size<int8_t, int64_t>>);
static_assert(is_defined<if_lesser_size<int16_t, int32_t>>);
static_assert(is_defined<if_lesser_size<int16_t, int64_t>>);
static_assert(is_defined<if_lesser_size<int32_t, int64_t>>);
////static_assert(is_defined<if_lesser_size<bool, int>>);
////static_assert(!is_defined<if_lesser_size<bool, bool>>);
////static_assert(!is_defined<if_lesser_size<int, int>>);
////static_assert(!is_defined<if_lesser_size<int, uint32_t>>);
////static_assert(!is_defined<if_lesser_size<int32_t, int>>);
////static_assert(!is_defined<if_lesser_size<int8_t, int8_t>>);
////static_assert(!is_defined<if_lesser_size<uint8_t, uint8_t>>);
////static_assert(!is_defined<if_lesser_size<int16_t, uint16_t>>);
////static_assert(!is_defined<if_lesser_size<uint16_t, int16_t>>);
////static_assert(!is_defined<if_lesser_size<int32_t, uint32_t>>);
////static_assert(!is_defined<if_lesser_size<uint32_t, int32_t>>);
////static_assert(!is_defined<if_lesser_size<int64_t, uint64_t>>);
////static_assert(!is_defined<if_lesser_size<uint64_t, int64_t>>);
////static_assert(!is_defined<if_lesser_size<int, bool>>);

static_assert(is_defined<if_not_lesser_size<bool, bool>>);
static_assert(is_defined<if_not_lesser_size<bool, uint8_t>>);
static_assert(is_defined<if_not_lesser_size<char, uint8_t>>);
static_assert(is_defined<if_not_lesser_size<wchar_t, uint16_t>>);
static_assert(is_defined<if_not_lesser_size<int, uint16_t>>);
static_assert(is_defined<if_not_lesser_size<int, short>>);
static_assert(is_defined<if_not_lesser_size<short, uint16_t>>);
static_assert(is_defined<if_not_lesser_size<long, int>>);
static_assert(is_defined<if_not_lesser_size<long long, long>>);
static_assert(is_defined<if_not_lesser_size<long, uint32_t>>);
static_assert(is_defined<if_not_lesser_size<int32_t, int>>);
static_assert(is_defined<if_not_lesser_size<int8_t, int8_t>>);
static_assert(is_defined<if_not_lesser_size<uint8_t, uint8_t>>);
static_assert(is_defined<if_not_lesser_size<int16_t, uint16_t>>);
static_assert(is_defined<if_not_lesser_size<uint16_t, int16_t>>);
static_assert(is_defined<if_not_lesser_size<int32_t, uint32_t>>);
static_assert(is_defined<if_not_lesser_size<uint32_t, int32_t>>);
static_assert(is_defined<if_not_lesser_size<int64_t, uint64_t>>);
static_assert(is_defined<if_not_lesser_size<uint64_t, int64_t>>);
////static_assert(!is_defined<if_not_lesser_size<bool, int>>);
////static_assert(is_defined<if_not_lesser_size<int, bool>>);
////static_assert(!is_defined<if_not_lesser_size<int8_t, int16_t>>);
////static_assert(!is_defined<if_not_lesser_size<int8_t, int32_t>>);
////static_assert(!is_defined<if_not_lesser_size<int8_t, int64_t>>);
////static_assert(!is_defined<if_not_lesser_size<int16_t, int32_t>>);
////static_assert(!is_defined<if_not_lesser_size<int16_t, int64_t>>);
////static_assert(!is_defined<if_not_lesser_size<int32_t, int64_t>>);

static_assert(is_defined<if_default_constructible<bool>>);
static_assert(is_defined<if_default_constructible<std::vector<bool>>>);
static_assert(is_defined<if_default_constructible<std::string>>);
static_assert(is_defined<if_default_constructible<std::array<bool, 42>>>);
////static_assert(!is_defined<if_default_constructible<not_default_constructible>>);

static_assert(is_defined<if_trivially_constructible<uint32_t>>);
////static_assert(is_defined<if_trivially_constructible<std::vector<uint8_t>>>);
////static_assert(!is_defined<if_trivially_constructible<std::string>>);
static_assert(is_defined<if_trivially_constructible<std::array<uint8_t, 42>>>);
////static_assert(!is_defined<if_trivially_constructible<not_default_constructible>>);

static_assert(is_defined<if_unique_object_representations<bool>>);
static_assert(is_defined<if_unique_object_representations<size_t>>);
//static_assert(!is_defined<if_unique_object_representations<std::string>>);
////static_assert(!is_defined<if_unique_object_representations<base>>);
////static_assert(!is_defined<if_unique_object_representations<derived>>);

static_assert(is_defined<if_byte_insertable<std::string>>);
static_assert(is_defined<if_byte_insertable<std::vector<uint8_t>>>);
////static_assert(is_defined<if_byte_insertable<std::array<uint8_t, 42>>>);
////static_assert(!is_defined<if_byte_insertable<std::u32string>>);
////static_assert(!is_defined<if_byte_insertable<std::vector<uint32_t>>>);
////static_assert(!is_defined<if_byte_insertable<uint32_t>>);

// integer types

static_assert(is_defined<if_integer<char>>);
static_assert(is_defined<if_integer<int>>);
static_assert(is_defined<if_integer<uint8_t>>);
static_assert(is_defined<if_integer<uint32_t>>);
static_assert(is_defined<if_integer<int32_t>>);
static_assert(is_defined<if_integer<uintx>>);
////static_assert(!is_defined<if_integer<bool>>);
////static_assert(!is_defined<if_integer<float>>);
////static_assert(!is_defined<if_integer<double>>);
////static_assert(!is_defined<if_integer<base>>);

static_assert(is_defined<if_signed_integer<signed char>>);
static_assert(is_defined<if_signed_integer<int>>);
static_assert(is_defined<if_signed_integer<int16_t>>);
static_assert(is_defined<if_signed_integer<int32_t>>);
static_assert(is_defined<if_signed_integer<int64_t>>);
////static_assert(!is_defined<if_signed_integer<bool>>);
////static_assert(!is_defined<if_signed_integer<float>>);
////static_assert(!is_defined<if_signed_integer<double>>);
////static_assert(!is_defined<if_signed_integer<uint16_t>>);
////static_assert(!is_defined<if_signed_integer<size_t>>);
////static_assert(!is_defined<if_signed_integer<base>>);

static_assert(is_defined<if_unsigned_integer<uint16_t>>);
static_assert(is_defined<if_unsigned_integer<size_t>>);
////static_assert(!is_defined<if_unsigned_integer<bool>>);
////static_assert(!is_defined<if_unsigned_integer<char>>);
////static_assert(!is_defined<if_unsigned_integer<int>>);
////static_assert(!is_defined<if_unsigned_integer<float>>);
////static_assert(!is_defined<if_unsigned_integer<double>>);
////static_assert(!is_defined<if_unsigned_integer<int16_t>>);
////static_assert(!is_defined<if_unsigned_integer<int32_t>>);
////static_assert(!is_defined<if_unsigned_integer<int64_t>>);
////static_assert(!is_defined<if_unsigned_integer<base>>);

static_assert(is_defined<if_same_signed_integer<int, signed char>>);
static_assert(is_defined<if_same_signed_integer<uint16_t, uint8_t>>);
////static_assert(!is_defined<if_same_signed_integer<uint16_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<size_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<bool, bool>>);
////static_assert(!is_defined<if_same_signed_integer<char, bool>>);
////static_assert(!is_defined<if_same_signed_integer<int, bool>>);
////static_assert(!is_defined<if_same_signed_integer<float, bool>>);
////static_assert(!is_defined<if_same_signed_integer<double, bool>>);
////static_assert(!is_defined<if_same_signed_integer<int16_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<int32_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<int64_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<base, bool>>);

static_assert(is_defined<if_same_signed_integer<char, char>>);
static_assert(is_defined<if_same_signed_integer<int, int>>);
static_assert(is_defined<if_same_signed_integer<int, int16_t>>);
static_assert(is_defined<if_same_signed_integer<int, int32_t>>);
static_assert(is_defined<if_same_signed_integer<int, int64_t>>);
////static_assert(!is_defined<if_same_signed_integer<char, uint16_t>>);
////static_assert(!is_defined<if_same_signed_integer<char, size_t>>);
////static_assert(!is_defined<if_same_signed_integer<char, float>>);
////static_assert(!is_defined<if_same_signed_integer<char, double>>);
////static_assert(!is_defined<if_same_signed_integer<char, base>>);

static_assert(is_defined<if_same_signed_integer<int, int>>);
static_assert(is_defined<if_same_signed_integer<unsigned char, uint8_t>>);
static_assert(is_defined<if_same_signed_integer<uint8_t, uint8_t>>);
static_assert(is_defined<if_same_signed_integer<uint32_t, uint32_t>>);
static_assert(is_defined<if_same_signed_integer<uint64_t, uint64_t>>);
////static_assert(is_defined<if_same_signed_integer<bool, bool>>);
////static_assert(!is_defined<if_same_signed_integer<float, float>>);
////static_assert(!is_defined<if_same_signed_integer<double, double>>);
////static_assert(!is_defined<if_same_signed_integer<base, base>>);

static_assert(is_defined<if_same_signed_integer<uint16_t, uint8_t>>);
static_assert(is_defined<if_same_signed_integer<uint8_t, unsigned char>>);
static_assert(is_defined<if_same_signed_integer<unsigned char, uint8_t>>);
////static_assert(is_defined<if_same_signed_integer<int, char>>);
////static_assert(!is_defined<if_same_signed_integer<int16_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<uint16_t, bool>>);
////static_assert(!is_defined<if_same_signed_integer<char, unsigned char>>);

static_assert(is_defined<if_not_same_signed_integer<uint8_t, signed char>>);
static_assert(is_defined<if_not_same_signed_integer<uint16_t, int>>);
static_assert(is_defined<if_not_same_signed_integer<size_t, int>>);
static_assert(is_defined<if_not_same_signed_integer<int, uint16_t>>);
static_assert(is_defined<if_not_same_signed_integer<int, size_t>>);
////static_assert(!is_defined<if_not_same_signed_integer<int, char>>);
////static_assert(!is_defined<if_not_same_signed_integer<uint16_t, uint8_t>>);
////static_assert(!is_defined<if_not_same_signed_integer<bool, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<char, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<int, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<float, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<double, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<int16_t, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<int32_t, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<int64_t, int>>);
////static_assert(!is_defined<if_not_same_signed_integer<base, int>>);

// integral integer types

static_assert(is_defined<if_integral_integer<char>>);
static_assert(is_defined<if_integral_integer<int>>);
static_assert(is_defined<if_integral_integer<uint8_t>>);
static_assert(is_defined<if_integral_integer<uint32_t>>);
static_assert(is_defined<if_integral_integer<int32_t>>);
////static_assert(!is_defined<if_integral_integer<bool>>);
////static_assert(!is_defined<if_integral_integer<uintx>>);
////static_assert(!is_defined<if_integral_integer<float>>);
////static_assert(!is_defined<if_integral_integer<double>>);
////static_assert(!is_defined<if_integral_integer<base>>);

static_assert(is_defined<if_non_integral_integer<uintx>>);
static_assert(is_defined<if_non_integral_integer<uint256_t>>);
////static_assert(!is_defined<if_non_integral_integer<bool>>);
////static_assert(!is_defined<if_non_integral_integer<char>>);
////static_assert(!is_defined<if_non_integral_integer<int>>);
////static_assert(!is_defined<if_non_integral_integer<uint8_t>>);
////static_assert(!is_defined<if_non_integral_integer<uint32_t>>);
////static_assert(!is_defined<if_non_integral_integer<int32_t>>);
////static_assert(!is_defined<if_non_integral_integer<float>>);
////static_assert(!is_defined<if_non_integral_integer<double>>);
////static_assert(!is_defined<if_non_integral_integer<base>>);

static_assert(is_defined<if_signed_integral_integer<signed char>>);
static_assert(is_defined<if_signed_integral_integer<int8_t>>);
////static_assert(!is_defined<if_signed_integral_integer<uint8_t>>);
////static_assert(!is_defined<if_signed_integral_integer<bool>>);
////static_assert(!is_defined<if_signed_integral_integer<uintx>>);

static_assert(is_defined<if_unsigned_integral_integer<unsigned char>>);
static_assert(is_defined<if_unsigned_integral_integer<uint8_t>>);
////static_assert(!is_defined<if_unsigned_integral_integer<char>>);
////static_assert(!is_defined<if_unsigned_integral_integer<bool>>);
////static_assert(!is_defined<if_unsigned_integral_integer<uintx>>);
