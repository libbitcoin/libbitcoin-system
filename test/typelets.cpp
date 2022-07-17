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

class base {};
class not_derived {};
class derived : base {};

// These are implementation defined.
////static_assert(is_same_type<unsigned char, uint8_t>);
////static_assert(is_same_type<signed char, int8_t>);
////static_assert(is_same_type<int, int32_t>);
////static_assert(is_same_type<unsigned int, uint32_t>);
////static_assert(!is_same_type<char, int8_t>);

static_assert(is_same_type<uint8_t, uint8_t>);
static_assert(!is_same_type<int8_t, uint8_t>);
static_assert(!is_same_type<uint8_t, int8_t>);
static_assert(!is_same_type<uint16_t, int8_t>);
static_assert(!is_same_type<uint32_t, int8_t>);
static_assert(!is_same_type<uint64_t, int8_t>);
static_assert(!is_same_type<size_t, int8_t>);
static_assert(!is_same_type<int32_t, int8_t>);
static_assert(!is_same_type<int32_t, int8_t>);
static_assert(!is_same_type<int64_t, int8_t>);
static_assert(!is_same_type<char, int8_t>);
static_assert(!is_same_type<wchar_t, int8_t>);
static_assert(!is_same_type<bool, int8_t>);
static_assert(!is_same_type<std::string, int8_t>);
static_assert(!is_same_type<std::string, int>);
static_assert(!is_same_type<base, derived>);
static_assert(!is_same_type<base, not_derived>);
static_assert(is_same_type<decltype(is_same_type<int32_t, int32_t>), const bool>);

// These are implementation defined.
////static_assert(is_signed<char>);
////static_assert(!is_signed<wchar_t>);

// bool is arithmetic:
static_assert(int{ true } == 1 && int{ false } == 0);

// and bool is not signed, because:
////static_assert(!(bool{ -1 } < bool{ 0 }));

static_assert(!is_signed<bool>);
static_assert(is_signed<int8_t>);
static_assert(is_signed<int16_t>);
static_assert(is_signed<int32_t>);
static_assert(is_signed<int64_t>);
static_assert(is_signed<signed char>);
static_assert(!is_signed<unsigned char>);
static_assert(!is_signed<uint8_t>);
static_assert(!is_signed<uint16_t>);
static_assert(!is_signed<uint32_t>);
static_assert(!is_signed<uint64_t>);
static_assert(!is_signed<size_t>);
static_assert(!is_signed<std::string>);
static_assert(is_same_type<decltype(is_signed<int32_t>), const bool>);

// bool/char sizes are unspecified.

static_assert(is_same_size<bool, bool>);
static_assert(is_same_size<int8_t, uint8_t>);
static_assert(is_same_size<int16_t, uint16_t>);
static_assert(is_same_size<int32_t, int32_t>);
static_assert(is_same_size<signed char, signed char>);
static_assert(is_same_size<uint8_t, uint8_t>);
static_assert(is_same_size<uint16_t, uint16_t>);
static_assert(is_same_size<uint32_t, uint32_t>);
static_assert(is_same_size<uint64_t, uint64_t>);
static_assert(is_same_size<size_t, signed_size_t>);
static_assert(!is_same_size<uint32_t, uint64_t>);
static_assert(!is_same_size<uint16_t, uint32_t>);
static_assert(!is_same_size<uint8_t, uint16_t>);
static_assert(is_same_type<decltype(is_same_size<int32_t, int32_t>), const bool>);

static_assert(!is_lesser_size<bool, bool>);
static_assert(!is_lesser_size<int8_t, uint8_t>);
static_assert(!is_lesser_size<int16_t, uint16_t>);
static_assert(!is_lesser_size<int32_t, int32_t>);
static_assert(!is_lesser_size<signed char, signed char>);
static_assert(!is_lesser_size<uint8_t, uint8_t>);
static_assert(!is_lesser_size<uint16_t, uint16_t>);
static_assert(!is_lesser_size<uint32_t, uint32_t>);
static_assert(!is_lesser_size<uint64_t, uint64_t>);
static_assert(!is_lesser_size<size_t, signed_size_t>);
static_assert(is_lesser_size<uint32_t, uint64_t>);
static_assert(is_lesser_size<uint16_t, uint32_t>);
static_assert(is_lesser_size<uint8_t, uint16_t>);
static_assert(is_same_type<decltype(is_lesser_size<int32_t, int32_t>), const bool>);

static_assert(!is_greater_size<bool, bool>);
static_assert(!is_greater_size<int8_t, uint8_t>);
static_assert(!is_greater_size<int16_t, uint16_t>);
static_assert(!is_greater_size<int32_t, int32_t>);
static_assert(!is_greater_size<signed char, signed char>);
static_assert(!is_greater_size<uint8_t, uint8_t>);
static_assert(!is_greater_size<uint16_t, uint16_t>);
static_assert(!is_greater_size<uint32_t, uint32_t>);
static_assert(!is_greater_size<uint64_t, uint64_t>);
static_assert(!is_greater_size<size_t, signed_size_t>);
static_assert(is_greater_size<uint64_t, uint32_t>);
static_assert(is_greater_size<uint32_t, uint16_t>);
static_assert(is_greater_size<uint16_t, uint8_t>);
static_assert(is_same_type<decltype(is_greater_size<int32_t, int32_t>), const bool>);

static_assert(is_common_size<int>);
static_assert(is_common_size<signed int>);
static_assert(is_common_size<unsigned int>);
static_assert(is_common_size<int32_t>);
static_assert(is_common_size<uint32_t>);
static_assert(is_common_size<int64_t>);
static_assert(is_common_size<uint64_t>);
static_assert(is_common_size<size_t>);
static_assert(is_common_size<signed_size_t>);
////static_assert(!is_common_size<int8_t>);
////static_assert(!is_common_size<uint8_t>);
////static_assert(!is_common_size<int16_t>);
////static_assert(!is_common_size<uint16_t>);
static_assert(is_same_type<decltype(is_common_size<int>), const bool>);

struct two_bytes { uint8_t foo; uint8_t bar; };
union four_bytes { uint32_t foo; uint8_t bar; };

static_assert(!is_integral_size<uintx>);
static_assert(!is_integral_size<uint128_t>);
static_assert(is_integral_size<uint8_t>);
static_assert(is_integral_size<uint16_t>);
static_assert(is_integral_size<uint32_t>);
static_assert(is_integral_size<uint64_t>);
static_assert(is_integral_size<two_bytes>);
static_assert(is_integral_size<four_bytes>);
static_assert(!is_integral_size<uint128_t>);
static_assert(!is_integral_size<uint256_t>);
static_assert(!is_integral_size<uint512_t>);
static_assert(is_same_type<decltype(is_integral_size<int>), const bool>);

static_assert(!is_integral<uintx>);
static_assert(!is_integral<uint128_t>);
static_assert(is_integral<uint8_t>);
static_assert(is_integral<uint16_t>);
static_assert(is_integral<uint32_t>);
static_assert(is_integral<uint64_t>);
static_assert(is_integral<size_t>);
static_assert(is_integral<int8_t>);
static_assert(is_integral<int16_t>);
static_assert(is_integral<int32_t>);
static_assert(is_integral<int64_t>);
static_assert(is_integral<char>);
static_assert(is_integral<wchar_t>);
static_assert(!is_integral<bool>);
static_assert(!is_integral<std::string>);
static_assert(is_same_type<decltype(is_integral<int32_t>), const bool>);

static_assert(is_integer<uintx>);
static_assert(is_integer<uint128_t>);
static_assert(is_integer<uint8_t>);
static_assert(is_integer<uint16_t>);
static_assert(is_integer<uint32_t>);
static_assert(is_integer<uint64_t>);
static_assert(is_integer<size_t>);
static_assert(is_integer<int8_t>);
static_assert(is_integer<int16_t>);
static_assert(is_integer<int32_t>);
static_assert(is_integer<int64_t>);
static_assert(is_integer<char>);
static_assert(is_integer<wchar_t>);
static_assert(!is_integer<bool>);
static_assert(!is_integer<std::string>);
static_assert(is_same_type<decltype(is_integer<int32_t>), const bool>);

static_assert(!is_integral_integer<uintx>);
static_assert(!is_integral_integer<uint128_t>);
static_assert(is_integral_integer<uint8_t>);
static_assert(is_integral_integer<uint16_t>);
static_assert(is_integral_integer<uint32_t>);
static_assert(is_integral_integer<uint64_t>);
static_assert(is_integral_integer<size_t>);
static_assert(is_integral_integer<int8_t>);
static_assert(is_integral_integer<int16_t>);
static_assert(is_integral_integer<int32_t>);
static_assert(is_integral_integer<int64_t>);
static_assert(is_integral_integer<char>);
static_assert(is_integral_integer<wchar_t>);
static_assert(!is_integral_integer<bool>);
static_assert(!is_integral_integer<std::string>);
static_assert(is_same_type<decltype(is_integral_integer<int32_t>), const bool>);

// These may be unexpected, which is why we generally avoid them.
static_assert(sizeof(bool) >= 1u);
////static_assert(bits<bool> >= 1u);
static_assert(bits<char> >= 8u);
static_assert(bits<signed char> >= 8u);
static_assert(bits<unsigned char> >= 8u);
static_assert(bits<wchar_t> >= 16u);
static_assert(bits<short> >= 16u);
static_assert(bits<signed short> >= 16u);
static_assert(bits<unsigned short> >= 16u);
static_assert(bits<int> >= 16u);
static_assert(bits<signed int> >= 16u);
static_assert(bits<unsigned int> >= 16u);
static_assert(bits<long> >= 32u);
static_assert(bits<signed long> >= 32u);
static_assert(bits<unsigned long> >= 32u);
static_assert(bits<long long> >= 64);
static_assert(bits<signed long long> >= 64u);
static_assert(bits<unsigned long long> >= 64u);
static_assert(bits<int_fast8_t> >= 8u);
static_assert(bits<uint_fast8_t> >= 8u);
static_assert(bits<int_fast16_t> >= 16u);
static_assert(bits<uint_fast16_t> >= 16u);
static_assert(bits<int_fast32_t> >= 32u);
static_assert(bits<uint_fast32_t> >= 32u);
static_assert(bits<int_fast64_t> >= 64u);
static_assert(bits<uint_fast64_t> >= 64u);
static_assert(bits<int_least8_t> >= 8u);
static_assert(bits<uint_least8_t> >= 8u);
static_assert(bits<int_least16_t> >= 16u);
static_assert(bits<uint_least16_t> >= 16u);
static_assert(bits<int_least32_t> >= 32u);
static_assert(bits<uint_least32_t> >= 32u);
static_assert(bits<int_least64_t> >= 64u);
static_assert(bits<uint_least64_t> >= 64u);

static_assert(bits<int8_t> == 8u);
static_assert(bits<uint8_t> == 8u);
static_assert(bits<int16_t> == 16u);
static_assert(bits<uint16_t> == 16u);
static_assert(bits<int32_t> == 32u);
static_assert(bits<uint32_t> == 32u);
static_assert(bits<int64_t> == 64u);
static_assert(bits<uint64_t> == 64u);
static_assert(is_same_type<decltype(bits<int8_t>), const size_t>);

static_assert(bytes<8u> == 1u);
static_assert(bytes<16u> == 2u);
static_assert(bytes<24u> == 3u);
static_assert(bytes<32u> == 4u);
static_assert(bytes<40u> == 5u);
static_assert(bytes<48u> == 6u);
static_assert(bytes<56u> == 7u);
static_assert(bytes<64u> == 8u);
static_assert(bytes<128u> == 16u);
static_assert(bytes<256u> == 32u);
static_assert(is_same_type<decltype(bytes<8u>), const size_t>);

static_assert(is_std_array<std::array<uint8_t, 0>>);
static_assert(is_std_array<std::array<base, 0>>);
static_assert(!is_std_array<uint8_t>);
static_assert(is_same_type<decltype(is_std_array<std::array<uint8_t, 0>>), const bool>);

static_assert(array_count<std_array<uint8_t, 42>> == 42);
static_assert(array_count<std_array<std_array<uint32_t, 42>, 24>> == 24);

static_assert(std_array<uint8_t, 42>{}.size() == 42);
static_assert(std_array<std_array<uint32_t, 42>, 24>{}.size() == 24);
static_assert(std_array<std_array<std_array<uint32_t, 42>, 24>, 8>{}.size() == 8);

static_assert(size_of<uint32_t>() == sizeof(uint32_t));
static_assert(size_of<std_array<uint32_t, 42>>() == sizeof(uint32_t) * 42);
static_assert(size_of<std_array<std_array<uint32_t, 42>, 24>>() == sizeof(uint32_t) * 42 * 24);
static_assert(size_of<std_array<std_array<std_array<uint32_t, 42>, 24>, 8>>() == sizeof(uint32_t) * 42 * 24 * 8);

// std_array fails on "negative subscript"
////static_assert(size_of<std_array<uint8_t, max_size_t>>() == sizeof(uint8_t) * max_size_t);
////static_assert(size_of<std_array<uint16_t, max_size_t>>() == sizeof(uint16_t) * max_size_t);

// multiply overlow
////static_assert(size_of<std_array<uint16_t, to_half(max_size_t)>>() == sizeof(uint16_t) * to_half(max_size_t));

// TODO: This is not currently restricted (is_std_array || !is_std_array).
////static_assert(size_of<uintx>() == 24);