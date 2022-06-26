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

static_assert(is_zero(0));
static_assert(!is_zero(1u));
static_assert(!is_zero(0xff));
static_assert(std::is_same_v<decltype(is_zero<int16_t>(0)), bool>);

static_assert(!is_nonzero(0));
static_assert(is_nonzero(1u));
static_assert(is_nonzero(0xff));
static_assert(std::is_same_v<decltype(is_nonzero<int16_t>(0)), bool>);

static_assert(is_one(1));
static_assert(!is_one(0u));
static_assert(!is_one(0xff));
static_assert(std::is_same_v<decltype(is_one<int16_t>(0)), bool>);

static_assert(lo_bit(0) == 0);
static_assert(lo_bit(2u) == 0);
static_assert(lo_bit(42) == 0);
static_assert(lo_bit(max_uint32 - 1u) == 0);
static_assert(lo_bit(1) == 1);
static_assert(lo_bit(3u) == 1);
static_assert(lo_bit(5) == 1);
static_assert(lo_bit(max_uint32) == 1);
static_assert(std::is_same_v<decltype(lo_bit<int16_t>(0)), int16_t>);

static_assert(is_even(0));
static_assert(is_even(2u));
static_assert(is_even(42));
static_assert(is_even(max_uint32 - 1u));
static_assert(!is_even(1));
static_assert(!is_even(3u));
static_assert(!is_even(0xff));
static_assert(!is_even(max_uint32));
static_assert(std::is_same_v<decltype(is_even<int16_t>(0)), bool>);

static_assert(is_odd(1));
static_assert(is_odd(3u));
static_assert(is_odd(5));
static_assert(is_odd(max_uint32));
static_assert(!is_odd(0u));
static_assert(!is_odd(2));
static_assert(!is_odd(10));
static_assert(!is_odd(max_uint32 - 1u));
static_assert(std::is_same_v<decltype(is_odd<int16_t>(0)), bool>);

static_assert(is_null(nullptr));
static_assert(is_null(std::nullptr_t{}));
static_assert(std::is_same_v<decltype(is_null(nullptr)), bool>);

static_assert(to_bits(0) == 0 * 8);
static_assert(to_bits(1u) == 1u * 8u);
static_assert(to_bits(-42) == -42 * 8);
static_assert(to_bits(0xff) == 0xff * 8);
static_assert(std::is_same_v<decltype(to_bits<int16_t>(0)), int16_t>);

static_assert(to_byte('\0') == uint8_t{ 0 });
static_assert(to_byte('x') == uint8_t{ 'x' });
static_assert(to_byte('\xff') == uint8_t{ 255 });
static_assert(std::is_same_v<decltype(to_byte('\0')), uint8_t>);

static_assert(to_half(0) == 0 / 2);
static_assert(to_half(1u) == 1u / 2u);
static_assert(to_half(-42) == -42 / 2);
static_assert(to_half(0xff) == 0xff / 2);
static_assert(std::is_same_v<decltype(to_half<int16_t>(0)), int16_t>);

static_assert(to_int(true) == 1);
static_assert(to_int(false) == 0);
static_assert(std::is_same_v<decltype(to_int<int16_t>(0)), int16_t>);

static_assert(to_bool(1));
static_assert(to_bool(-1));
static_assert(to_bool(-42));
static_assert(to_bool(42));
static_assert(!to_bool(0));
static_assert(std::is_same_v<decltype(to_bool<int16_t>(0)), bool>);

static_assert(add1(0) == 0 + 1);
static_assert(add1(1u) == 1u + 1u);
static_assert(add1(-42) == -42 + 1);
static_assert(add1(0xff) == 0xff + 1);
static_assert(std::is_same_v<decltype(add1<int16_t>(0)), int16_t>);

static_assert(sub1(0xff) == 0xff - 1);
static_assert(sub1(1u) == 1u - 1u);
static_assert(sub1(-42) == -42 - 1);
static_assert(sub1(0xff) == 0xff - 1);
static_assert(std::is_same_v<decltype(sub1<int16_t>(0)), int16_t>);

// These may be unexpected, which is why we generally avoid them.
static_assert(width<bool>() >= 8u);
static_assert(width<char>() >= 8u);
static_assert(width<signed char>() >= 8u);
static_assert(width<unsigned char>() >= 8u);
static_assert(width<wchar_t>() >= 16u);
static_assert(width<short>() >= 16u);
static_assert(width<signed short>() >= 16u);
static_assert(width<unsigned short>() >= 16u);
static_assert(width<int>() >= 16u);
static_assert(width<signed int>() >= 16u);
static_assert(width<unsigned int>() >= 16u);
static_assert(width<long>() >= 32u);
static_assert(width<signed long>() >= 32u);
static_assert(width<unsigned long>() >= 32u);
static_assert(width<long long>() >= 64);
static_assert(width<signed long long>() >= 64u);
static_assert(width<unsigned long long>() >= 64u);
static_assert(width<int_fast8_t>() >= 8u);
static_assert(width<uint_fast8_t>() >= 8u);
static_assert(width<int_fast16_t>() >= 16u);
static_assert(width<uint_fast16_t>() >= 16u);
static_assert(width<int_fast32_t>() >= 32u);
static_assert(width<uint_fast32_t>() >= 32u);
static_assert(width<int_fast64_t>() >= 64u);
static_assert(width<uint_fast64_t>() >= 64u);
static_assert(width<int_least8_t>() >= 8u);
static_assert(width<uint_least8_t>() >= 8u);
static_assert(width<int_least16_t>() >= 16u);
static_assert(width<uint_least16_t>() >= 16u);
static_assert(width<int_least32_t>() >= 32u);
static_assert(width<uint_least32_t>() >= 32u);
static_assert(width<int_least64_t>() >= 64u);
static_assert(width<uint_least64_t>() >= 64u);

constexpr uint32_t value42 = 42;
static_assert(width<int8_t>() == 8u);
static_assert(width<uint8_t>() == 8u);
static_assert(width<int16_t>() == 16u);
static_assert(width<uint16_t>() == 16u);
static_assert(width<int32_t>() == 32u);
static_assert(width<uint32_t>() == 32u);
static_assert(width<int64_t>() == 64u);
static_assert(width<uint64_t>() == 64u);
static_assert(variable_size(zero) == 1u);
static_assert(variable_size(1) == 1u);
static_assert(variable_size(0xfe) == 3u);
static_assert(variable_size(0x10000) == 5u);
static_assert(variable_size(0x100000000) == 9u);
static_assert(variable_size(max_uint8) == 3u);
static_assert(variable_size(max_uint16) == 3u);
static_assert(variable_size(max_uint32) == 5u);
static_assert(variable_size(max_uint32) == 5u);
static_assert(variable_size(max_uint64) == 9u);
static_assert(std::is_same_v<decltype(variable_size(zero)), size_t>);

class base {};
class not_derived {};
class derived : base {};

// These are implementation defined.
////static_assert(is_same<unsigned char, uint8_t>());
////static_assert(is_same<signed char, int8_t>());
////static_assert(is_same<int, int32_t>());
////static_assert(is_same<unsigned int, uint32_t>());
////static_assert(!is_same<char, int8_t>());

static_assert(is_same<uint8_t, uint8_t>());
static_assert(!is_same<int8_t, uint8_t>());
static_assert(!is_same<uint8_t, int8_t>());
static_assert(!is_same<uint16_t, int8_t>());
static_assert(!is_same<uint32_t, int8_t>());
static_assert(!is_same<uint64_t, int8_t>());
static_assert(!is_same<size_t, int8_t>());
static_assert(!is_same<int32_t, int8_t>());
static_assert(!is_same<int32_t, int8_t>());
static_assert(!is_same<int64_t, int8_t>());
static_assert(!is_same<char, int8_t>());
static_assert(!is_same<wchar_t, int8_t>());
static_assert(!is_same<bool, int8_t>());
static_assert(!is_same<std::string, int8_t>());
static_assert(!is_same<std::string, int>());
static_assert(!is_same<base, derived>());
static_assert(!is_same<base, not_derived>());
static_assert(std::is_same_v<decltype(is_same<int32_t, int32_t>()), bool>);

// These are implementation defined.
////static_assert(is_signed<char>());
////static_assert(!is_signed<wchar_t>());

// bool is arithmetic:
static_assert(int{ true } == 1 && int{ false } == 0);

// and bool is not signed, because:
////static_assert(!(bool{ -1 } < bool{ 0 }));

static_assert(!is_signed<bool>());
static_assert(is_signed<int8_t>());
static_assert(is_signed<int16_t>());
static_assert(is_signed<int32_t>());
static_assert(is_signed<int64_t>());
static_assert(is_signed<signed char>());
static_assert(!is_signed<unsigned char>());
static_assert(!is_signed<uint8_t>());
static_assert(!is_signed<uint16_t>());
static_assert(!is_signed<uint32_t>());
static_assert(!is_signed<uint64_t>());
static_assert(!is_signed<size_t>());
static_assert(!is_signed<std::string>());
static_assert(std::is_same_v<decltype(is_signed<int32_t>()), bool>);

static_assert(is_same_size<bool, bool>());
static_assert(is_same_size<int8_t, uint8_t>());
static_assert(is_same_size<int16_t, uint16_t>());
static_assert(is_same_size<int32_t, int32_t>());
static_assert(is_same_size<signed char, signed char>());
static_assert(is_same_size<uint8_t, uint8_t>());
static_assert(is_same_size<uint16_t, uint16_t>());
static_assert(is_same_size<uint32_t, uint32_t>());
static_assert(is_same_size<uint64_t, uint64_t>());
static_assert(is_same_size<size_t, signed_size_t>());
static_assert(!is_same_size<wchar_t, bool>());
static_assert(!is_same_size<std::string, bool>());
static_assert(!is_same_size<int16_t, uint256_t>());
static_assert(std::is_same_v<decltype(is_same_size<int32_t, int32_t>()), bool>);

static_assert(is_integer<uint8_t>());
static_assert(is_integer<uint16_t>());
static_assert(is_integer<uint32_t>());
static_assert(is_integer<uint64_t>());
static_assert(is_integer<size_t>());
static_assert(is_integer<int8_t>());
static_assert(is_integer<int16_t>());
static_assert(is_integer<int32_t>());
static_assert(is_integer<int64_t>());
static_assert(is_integer<char>());
static_assert(is_integer<wchar_t>());
static_assert(!is_integer<bool>());
static_assert(!is_integer<std::string>());
static_assert(std::is_same_v<decltype(is_integer<int32_t>()), bool>);

static_assert(is_integral<uint8_t>());
static_assert(is_integral<uint16_t>());
static_assert(is_integral<uint32_t>());
static_assert(is_integral<uint64_t>());
static_assert(is_integral<size_t>());
static_assert(is_integral<int8_t>());
static_assert(is_integral<int16_t>());
static_assert(is_integral<int32_t>());
static_assert(is_integral<int64_t>());
static_assert(is_integral<char>());
static_assert(is_integral<wchar_t>());
static_assert(!is_integral<bool>());
static_assert(!is_integral<std::string>());
static_assert(std::is_same_v<decltype(is_integral<int32_t>()), bool>);

static_assert(!is_integral_size(0));
static_assert(is_integral_size(1));
static_assert(is_integral_size(2));
static_assert(!is_integral_size(3));
static_assert(is_integral_size(4));
static_assert(!is_integral_size(5));
static_assert(!is_integral_size(6));
static_assert(!is_integral_size(7));
static_assert(is_integral_size(8));
static_assert(!is_integral_size(9));
static_assert(!is_integral_size(64));
static_assert(std::is_same_v<decltype(is_integral_size(0)), bool>);

struct two_bytes { uint8_t foo; uint8_t bar; };
union four_bytes { uint32_t foo; uint8_t bar; };

static_assert(is_integral_size<uint8_t>());
static_assert(is_integral_size<uint16_t>());
static_assert(is_integral_size<uint32_t>());
static_assert(is_integral_size<uint64_t>());
static_assert(is_integral_size<two_bytes>());
static_assert(is_integral_size<four_bytes>());
static_assert(!is_integral_size<uint128_t>());
static_assert(!is_integral_size<uint256_t>());
static_assert(!is_integral_size<uint512_t>());
static_assert(std::is_same_v<decltype(is_integral_size<int>()), bool>);

static_assert(!is_bytes_width(0));
static_assert(is_bytes_width(8));
static_assert(is_bytes_width(16));
static_assert(is_bytes_width(24));
static_assert(is_bytes_width(32));
static_assert(is_bytes_width(40));
static_assert(!is_bytes_width(42));
static_assert(is_bytes_width(48));
static_assert(is_bytes_width(56));
static_assert(is_bytes_width(64));
static_assert(is_bytes_width(128));
static_assert(is_bytes_width(256));
static_assert(std::is_same_v<decltype(is_bytes_width(0)), bool>);
