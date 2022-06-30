/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation == either version 3 of the License == or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not == see <http://www.gnu.org/licenses/>.
 */
#include "../test.hpp"
#include <cstddef>
#include <cstdint>

// limit

static_assert(limit< uint8_t>(min_uint64) == min_uint8);
static_assert(limit< int16_t>(min_uint64) == min_uint64);
static_assert(limit<uint16_t>(min_uint64) == min_uint16);
static_assert(limit< int32_t>(min_uint64) == min_uint64);
static_assert(limit<uint32_t>(min_uint64) == min_uint32);
static_assert(limit< int64_t>(min_uint64) == min_uint64);
static_assert(limit<uint64_t>(min_uint64) == min_uint64);
static_assert(limit< uint8_t>(min_int64)  == min_uint8);
static_assert(limit< int16_t>(min_int64)  == min_int16);
static_assert(limit<uint16_t>(min_int64)  == min_uint16);
static_assert(limit< int32_t>(min_int64)  == min_int32);
static_assert(limit<uint32_t>(min_int64)  == min_uint32);
static_assert(limit< int64_t>(min_int64)  == min_int64);
static_assert(limit<uint64_t>(min_int64)  == min_uint64);
static_assert(limit< uint8_t>(max_int64)  == max_uint8);
static_assert(limit< int16_t>(max_int64)  == max_int16);
static_assert(limit<uint16_t>(max_int64)  == max_uint16);
static_assert(limit< int32_t>(max_int64)  == max_int32);
static_assert(limit<uint32_t>(max_int64)  == max_uint32);
static_assert(limit< int64_t>(max_int64)  == max_int64);
static_assert(limit<uint64_t>(max_int64)  == max_int64);
static_assert(limit< uint8_t>(max_uint64) == max_uint8);
static_assert(limit< int16_t>(max_uint64) == max_int16);
static_assert(limit<uint16_t>(max_uint64) == max_uint16);
static_assert(limit< int32_t>(max_uint64) == max_int32);
static_assert(limit<uint32_t>(max_uint64) == max_uint32);
static_assert(limit< int64_t>(max_uint64) == max_int64);
static_assert(limit<uint64_t>(max_uint64) == max_uint64);
static_assert(std::is_same<decltype(limit<uint8_t>(0)), uint8_t>::value);

static_assert(limit(size_t(0), size_t(0)) == size_t(0));
static_assert(limit(size_t(25), size_t(60)) == size_t(25));
static_assert(limit(size_t(75), size_t(60)) == size_t(60));
static_assert(std::is_same<decltype(limit<uint8_t>(0, 0)), uint8_t>::value);

static_assert(limit(size_t(0),   size_t(0), size_t(0))  == size_t(0));
static_assert(limit(size_t(25), size_t(50), size_t(60)) == size_t(50));
static_assert(limit(size_t(50), size_t(50), size_t(60)) == size_t(50));
static_assert(limit(size_t(55), size_t(50), size_t(60)) == size_t(55));
static_assert(limit(size_t(60), size_t(50), size_t(60)) == size_t(60));
static_assert(limit(size_t(75), size_t(50), size_t(60)) == size_t(60));
static_assert(limit( 0, size_t(0) ,  size_t(0)) == size_t(0));
static_assert(limit(25, size_t(50), size_t(60)) == size_t(50));
static_assert(limit(50, size_t(50), size_t(60)) == size_t(50));
static_assert(limit(55, size_t(50), size_t(60)) == size_t(55));
static_assert(limit(60, size_t(50), size_t(60)) == size_t(60));
static_assert(limit(75, size_t(50), size_t(60)) == size_t(60));
static_assert(limit(size_t(0),   0,  0) ==  0);
static_assert(limit(size_t(25), 50, 60) == 50);
static_assert(limit(size_t(50), 50, 60) == 50);
static_assert(limit(size_t(55), 50, 60) == 55);
static_assert(limit(size_t(60), 50, 60) == 60);
static_assert(limit(size_t(75), 50, 60) == 60);
static_assert(limit( 0,  0,  0) ==  0);
static_assert(limit(25, 50, 60) == 50);
static_assert(limit(50, 50, 60) == 50);
static_assert(limit(55, 50, 60) == 55);
static_assert(limit(60, 50, 60) == 60);
static_assert(limit(75, 50, 60) == 60);
static_assert(limit(-25, -60, -50) == -50);
static_assert(limit(-50, -60, -50) == -50);
static_assert(limit(-55, -60, -50) == -55);
static_assert(limit(-60, -60, -50) == -60);
static_assert(limit(-75, -60, -50) == -60);
static_assert(limit(-25, -60, 50) == -25);
static_assert(limit(-50, -60, 50) == -50);
static_assert(limit(-55, -60, 50) == -55);
static_assert(limit(-60, -60, 50) == -60);
static_assert(limit(-75, -60, 50) == -60);
static_assert(std::is_same<decltype(is_limited<uint8_t>(0, 0, 0)), bool>::value);

// is_limited

static_assert(!is_limited< uint8_t>(min_uint64));
static_assert(!is_limited< int16_t>(min_uint64));
static_assert(!is_limited<uint16_t>(min_uint64));
static_assert(!is_limited< int32_t>(min_uint64));
static_assert(!is_limited<uint32_t>(min_uint64));
static_assert(!is_limited< int64_t>(min_uint64));
static_assert(!is_limited<uint64_t>(min_uint64));
static_assert(is_limited< uint8_t>(min_int64));
static_assert(is_limited< int16_t>(min_int64));
static_assert(is_limited<uint16_t>(min_int64));
static_assert(is_limited< int32_t>(min_int64));
static_assert(is_limited<uint32_t>(min_int64));
static_assert(!is_limited< int64_t>(min_int64));
static_assert(is_limited<uint64_t>(min_int64));
static_assert(is_limited< uint8_t>(max_int64));
static_assert(is_limited< int16_t>(max_int64));
static_assert(is_limited<uint16_t>(max_int64));
static_assert(is_limited< int32_t>(max_int64));
static_assert(is_limited<uint32_t>(max_int64));
static_assert(!is_limited< int64_t>(max_int64));
static_assert(!is_limited<uint64_t>(max_int64));
static_assert(is_limited< uint8_t>(max_uint64));
static_assert(is_limited< int16_t>(max_uint64));
static_assert(is_limited<uint16_t>(max_uint64));
static_assert(is_limited< int32_t>(max_uint64));
static_assert(is_limited<uint32_t>(max_uint64));
static_assert(is_limited< int64_t>(max_uint64));
static_assert(!is_limited<uint64_t>(max_uint64));
static_assert(std::is_same<decltype(is_limited<uint8_t>(0)), bool>::value);

static_assert(!is_limited(size_t(0), size_t(0)));
static_assert(!is_limited(size_t(25), size_t(60)));
static_assert(is_limited(size_t(75), size_t(60)));
static_assert(std::is_same<decltype(is_limited<uint8_t>(0, 0)), bool>::value);

static_assert(!is_limited(size_t(0),  size_t(0),  size_t(0)));
static_assert( is_limited(size_t(25), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(50), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(55), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(60), size_t(50), size_t(60)));
static_assert( is_limited(size_t(75), size_t(50), size_t(60)));
static_assert(!is_limited(0,  size_t(0),  size_t(0)));
static_assert( is_limited(25, size_t(50), size_t(60)));
static_assert(!is_limited(50, size_t(50), size_t(60)));
static_assert(!is_limited(55, size_t(50), size_t(60)));
static_assert(!is_limited(60, size_t(50), size_t(60)));
static_assert( is_limited(75, size_t(50), size_t(60)));
static_assert(!is_limited(size_t(0),   0,  0));
static_assert( is_limited(size_t(25), 50, 60));
static_assert(!is_limited(size_t(50), 50, 60));
static_assert(!is_limited(size_t(55), 50, 60));
static_assert(!is_limited(size_t(60), 50, 60));
static_assert( is_limited(size_t(75), 50, 60));
static_assert(!is_limited(0,   0,  0));
static_assert( is_limited(25, 50, 60));
static_assert(!is_limited(50, 50, 60));
static_assert(!is_limited(55, 50, 60));
static_assert(!is_limited(60, 50, 60));
static_assert( is_limited(75, 50, 60));
static_assert( is_limited(-25, -60, -50));
static_assert(!is_limited(-50, -60, -50));
static_assert(!is_limited(-55, -60, -50));
static_assert(!is_limited(-60, -60, -50));
static_assert( is_limited(-75, -60, -50));
static_assert(!is_limited(-25, -60, 50));
static_assert(!is_limited(-50, -60, 50));
static_assert(!is_limited(-55, -60, 50));
static_assert(!is_limited(-60, -60, 50));
static_assert( is_limited(-75, -60, 50));
static_assert(std::is_same<decltype(is_limited<uint8_t>(0, 0, 0)), bool>::value);

// minimum<T>

static_assert(minimum<int8_t >() == min_int8);
static_assert(minimum<int16_t>() == min_int16);
static_assert(minimum<int32_t>() == min_int32);
static_assert(minimum<int64_t>() == min_int64);
static_assert(minimum<uint8_t >() == min_uint8);
static_assert(minimum<uint16_t>() == min_uint16);
static_assert(minimum<uint32_t>() == min_uint32);
static_assert(minimum<uint64_t>() == min_uint64);
static_assert(std::is_same<decltype(maximum<int64_t>()), int64_t>::value);

// maximum<T>

static_assert(maximum<int8_t >() == max_int8);
static_assert(maximum<int16_t>() == max_int16);
static_assert(maximum<int32_t>() == max_int32);
static_assert(maximum<int64_t>() == max_int64);
static_assert(maximum<uint8_t >() == max_uint8);
static_assert(maximum<uint16_t>() == max_uint16);
static_assert(maximum<uint32_t>() == max_uint32);
static_assert(maximum<uint64_t>() == max_uint64);
static_assert(std::is_same<decltype(maximum<int16_t>()), int16_t>::value);

// absolute_minimum<T>

static_assert(absolute_minimum<int8_t >() == power2(sub1(width<int8_t>())));
static_assert(absolute_minimum<int16_t>() == power2(sub1(width<int16_t>())));
static_assert(absolute_minimum<int32_t>() == power2(sub1(width<int32_t>())));
static_assert(absolute_minimum<int64_t>() == power2(sub1(width<int64_t>())));
static_assert(absolute_minimum<int8_t >() == add1(absolute(add1(min_int8))));
static_assert(absolute_minimum<int16_t>() == add1(absolute(add1(min_int16))));
static_assert(absolute_minimum<int32_t>() == add1(absolute(add1(min_int32))));
static_assert(absolute_minimum<int64_t>() == add1(absolute(add1(min_int64))));
static_assert(absolute_minimum<uint8_t>()  == min_uint8);
static_assert(absolute_minimum<uint16_t>() == min_uint16);
static_assert(absolute_minimum<uint32_t>() == min_uint32);
static_assert(absolute_minimum<uint64_t>() == min_uint64);
static_assert(std::is_same<decltype(maximum<int64_t>()), int64_t>::value);

// unsigned_maximum<T>

static_assert(unsigned_maximum<int8_t >() == sub1(to_half(power2(width<int8_t>()))));
static_assert(unsigned_maximum<int16_t>() == sub1(to_half(power2(width<int16_t>()))));
static_assert(unsigned_maximum<int32_t>() == sub1(to_half(power2(width<int32_t>()))));
static_assert(unsigned_maximum<int64_t>() == sub1(to_half(power2<uint256_t>(width<int64_t>()))));
static_assert(unsigned_maximum<int8_t >() == to_unsigned(max_int8));
static_assert(unsigned_maximum<int16_t>() == to_unsigned(max_int16));
static_assert(unsigned_maximum<int32_t>() == to_unsigned(max_int32));
static_assert(unsigned_maximum<int64_t>() == to_unsigned(max_int64));
static_assert(unsigned_maximum<uint8_t >() == max_uint8);
static_assert(unsigned_maximum<uint16_t>() == max_uint16);
static_assert(unsigned_maximum<uint32_t>() == max_uint32);
static_assert(unsigned_maximum<uint64_t>() == max_uint64);
static_assert(std::is_same<decltype(maximum<int16_t>()), int16_t>::value);

// min/max bytes expecations
constexpr auto max_int24 = sub1(0x00000000800000_i32);
constexpr auto max_int40 = sub1(0x00008000000000_i64);
constexpr auto max_int48 = sub1(0x00800000000000_i64);
constexpr auto max_int56 = sub1(0x80000000000000_i64);
constexpr auto min_int24 = negate(0x00000000800000_i32);
constexpr auto min_int40 = negate(0x00008000000000_i64);
constexpr auto min_int48 = negate(0x00800000000000_i64);
constexpr auto min_int56 = negate(0x80000000000000_i64);

// minimum<Bytes>

static_assert(minimum<1>() == min_int8);
static_assert(minimum<2>() == min_int16);
static_assert(minimum<3>() == min_int24);
static_assert(minimum<4>() == min_int32);
static_assert(minimum<5>() == min_int40);
static_assert(minimum<6>() == min_int48);
static_assert(minimum<7>() == min_int56);
static_assert(minimum<8>() == min_int64);
static_assert(std::is_same<decltype(minimum<2>()), int16_t>::value);

// maximum<Bytes>

static_assert(maximum<1>() == max_int8);
static_assert(maximum<2>() == max_int16);
static_assert(maximum<3>() == max_int24);
static_assert(maximum<4>() == max_int32);
static_assert(maximum<5>() == max_int40);
static_assert(maximum<6>() == max_int48);
static_assert(maximum<7>() == max_int56);
static_assert(maximum<8>() == max_int64);
static_assert(std::is_same<decltype(maximum<1>()), int8_t>::value);

// bitcoin_min<Bytes>

static_assert(bitcoin_min<1>() == add1(min_int8));
static_assert(bitcoin_min<2>() == add1(min_int16));
static_assert(bitcoin_min<3>() == add1(min_int24));
static_assert(bitcoin_min<4>() == add1(min_int32));
static_assert(bitcoin_min<5>() == add1(min_int40));
static_assert(bitcoin_min<6>() == add1(min_int48));
static_assert(bitcoin_min<7>() == add1(min_int56));
static_assert(bitcoin_min<8>() == add1(min_int64));
static_assert(std::is_same<decltype(bitcoin_min<8>()), int64_t>::value);

// bitcoin_max<Bytes>

static_assert(bitcoin_max<1>() == max_int8);
static_assert(bitcoin_max<2>() == max_int16);
static_assert(bitcoin_max<3>() == max_int24);
static_assert(bitcoin_max<4>() == max_int32);
static_assert(bitcoin_max<5>() == max_int40);
static_assert(bitcoin_max<6>() == max_int48);
static_assert(bitcoin_max<7>() == max_int56);
static_assert(bitcoin_max<8>() == max_int64);
static_assert(std::is_same<decltype(bitcoin_max<4>()), int32_t>::value);

/// Bitcoin serialization imposes the following domain constraint on integers.
/// The domains are constrained by one less negative value than C++ integrals.

/// 1 byte :[-2^07+1...2^07-1]
/// 2 bytes:[-2^15+1...2^15-1]
/// 3 bytes:[-2^23+1...2^23-1]
/// 4 bytes:[-2^31+1...2^31-1]
/// 5 bytes:[-2^39+1...2^39-1]
/// 6 bytes:[-2^47+1...2^47-1]
/// 7 bytes:[-2^55+1...2^55-1]
/// 8 bytes:[-2^63+1...2^63-1]

static_assert(bitcoin_min<4>() == add1(minimum<4>()));
static_assert(bitcoin_min<5>() == add1(minimum<5>()));
static_assert(ones_complement(max_uint8) == min_uint8);
static_assert(ones_complement(max_uint16) == min_uint16);
static_assert(ones_complement(max_uint32) == min_uint32);
static_assert(ones_complement(max_uint64) == min_uint64);
static_assert(twos_complement(max_uint8) == add1(min_uint8));
static_assert(twos_complement(max_uint16) == add1(min_uint16));
static_assert(twos_complement(max_uint32) == add1(min_uint32));
static_assert(twos_complement(max_uint64) == add1(min_uint64));

////// This demonstrates the loss of one value in the domain.
////static_assert(!is_negated(0b11111111_u8));
////static_assert(!is_negated(0b11111110_u8));
////static_assert(!is_negated(0b11111100_u8));
////static_assert(!is_negated(0b11111000_u8));
////static_assert(!is_negated(0b11110000_u8));
////static_assert(!is_negated(0b11100000_u8));
////static_assert(!is_negated(0b11000000_u8));
////static_assert(is_negated(0b10000000_u8)); // <==
////static_assert(!is_negated(0b01111111_u8));
////static_assert(!is_negated(0b00111111_u8));
////static_assert(!is_negated(0b00011111_u8));
////static_assert(!is_negated(0b00001111_u8));
////static_assert(!is_negated(0b00000111_u8));
////static_assert(!is_negated(0b00000011_u8));
////static_assert(!is_negated(0b00000010_u8));
////static_assert(!is_negated(0b00000001_u8));
////static_assert(!is_negated(0b00000000_u8));
////
////// Leading byte and leading bit (signed).
////static_assert(is_negated(0x80_i8));
////static_assert(is_negated(0x8000_i16));
////static_assert(is_negated(0x80000000_i32));
////static_assert(is_negated(0x8000000000000000_i64));
////
////// Leading byte and leading bit (unsigned).
////static_assert(is_negated(0x81_u8));
////static_assert(is_negated(0x8001_u16));
////static_assert(is_negated(0x80000001_u32));
////static_assert(is_negated(0x8000000000000001_u64));
////
////// Leading bit but not leading byte (unsigned).
////static_assert(!is_negated(0x00820002_u32));
////static_assert(!is_negated(0x00008303_u32));
////static_assert(!is_negated(0x00000084_u32));
////static_assert(!is_negated(0x0000810000000001_u64));
////static_assert(!is_negated(0x0000008200000002_u64));
////static_assert(!is_negated(0x0000000083000003_u64));
////static_assert(!is_negated(0x0000000000840004_u64));
////static_assert(!is_negated(0x0000000000008505_u64));
////static_assert(!is_negated(0x0000000000000086_u64));
////
////// However stack number compresses out all leading zero bytes, and then adds 
////// a sign byte. Yet when converting to stack number to a int32_t, it guards
////// against previous operation overflow by rejecting any 4 byte little-endian
////// stack chunk size. This causes a 4 byte negated integer to be discarded,
////// because its sign byte is a 5th stack chunk byte. But, the only value that
////// can have an absolute value with a high bit is the integral minimum. So that
////// value is considered an overflow as a matter of consensus. This could have
////// been easily avoided, by converting the necessary 5 bytes and confirming the
////// range, but alas.