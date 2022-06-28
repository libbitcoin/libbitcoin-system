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

static_assert(limit< uint8_t>(min_uint64) == 0u);
static_assert(limit< int16_t>(min_uint64) == 0);
static_assert(limit<uint16_t>(min_uint64) == 0u);
static_assert(limit< int32_t>(min_uint64) == 0);
static_assert(limit<uint32_t>(min_uint64) == 0u);
static_assert(limit< int64_t>(min_uint64) == 0);
static_assert(limit<uint64_t>(min_uint64) == 0u);

static_assert(limit< uint8_t>(min_int64) == 0u);
static_assert(limit< int16_t>(min_int64) == min_int16);
static_assert(limit<uint16_t>(min_int64) == 0u);
static_assert(limit< int32_t>(min_int64) == min_int32);
static_assert(limit<uint32_t>(min_int64) == 0u);
static_assert(limit< int64_t>(min_int64) == min_int64);
static_assert(limit<uint64_t>(min_int64) == 0u);

static_assert(limit< uint8_t>(max_int64) == max_uint8);
static_assert(limit< int16_t>(max_int64) == max_int16);
static_assert(limit<uint16_t>(max_int64) == max_uint16);
static_assert(limit< int32_t>(max_int64) == max_int32);
static_assert(limit<uint32_t>(max_int64) == max_uint32);
static_assert(limit< int64_t>(max_int64) == max_int64);
static_assert(limit<uint64_t>(max_int64) == max_int64);

static_assert(limit< uint8_t>(max_uint64) == max_uint8);
static_assert(limit< int16_t>(max_uint64) == max_int16);
static_assert(limit<uint16_t>(max_uint64) == max_uint16);
static_assert(limit< int32_t>(max_uint64) == max_int32);
static_assert(limit<uint32_t>(max_uint64) == max_uint32);
static_assert(limit< int64_t>(max_uint64) == max_int64);
static_assert(limit<uint64_t>(max_uint64) == max_uint64);

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

static_assert(std::is_same<decltype(limit<uint8_t>(0)), uint8_t>::value);

static_assert(limit(size_t(0), size_t(0)) == size_t(0));
static_assert(limit(size_t(25), size_t(60)) == size_t(25));
static_assert(limit(size_t(75), size_t(60)) == size_t(60));

static_assert(!is_limited(size_t(0), size_t(0)));
static_assert(!is_limited(size_t(25), size_t(60)));
static_assert(is_limited(size_t(75), size_t(60)));

static_assert(std::is_same<decltype(limit<uint8_t>(0, 0)), uint8_t>::value);

static_assert(limit(size_t(0),   size_t(0), size_t(0))  == size_t(0),  "");
static_assert(limit(size_t(25), size_t(50), size_t(60)) == size_t(50));
static_assert(limit(size_t(50), size_t(50), size_t(60)) == size_t(50));
static_assert(limit(size_t(55), size_t(50), size_t(60)) == size_t(55));
static_assert(limit(size_t(60), size_t(50), size_t(60)) == size_t(60));
static_assert(limit(size_t(75), size_t(50), size_t(60)) == size_t(60));

static_assert(limit( 0, size_t(0) ,  size_t(0)) == size_t(0),  "");
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

static_assert(!is_limited(size_t(0),  size_t(0),  size_t(0)),  "");
static_assert( is_limited(size_t(25), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(50), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(55), size_t(50), size_t(60)));
static_assert(!is_limited(size_t(60), size_t(50), size_t(60)));
static_assert( is_limited(size_t(75), size_t(50), size_t(60)));

static_assert(!is_limited(0,  size_t(0),  size_t(0)),  "");
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

static_assert(std::is_same<decltype(limit<uint8_t>(0, 0, 0)), uint8_t>::value);

static_assert(maximum<uint8_t>() == max_uint8);
static_assert(maximum<uint16_t>() == max_uint16);
static_assert(maximum<uint32_t>() == max_uint32);
static_assert(maximum<uint64_t>() == max_uint64);
////static_assert(maximum<bool>() == 0);
////static_assert(maximum<float>() == 0);
static_assert(std::is_same<decltype(maximum<int16_t>()), int16_t>::value);

static_assert(minimum<int8_t>() == min_int8);
static_assert(minimum<int16_t>() == min_int16);
static_assert(minimum<int32_t>() == min_int32);
static_assert(minimum<int64_t>() == min_int64);
////static_assert(minimum<float>() == 0);
////static_assert(minimum<bool>() == 0);
static_assert(std::is_same<decltype(maximum<int64_t>()), int64_t>::value);

constexpr auto max_int24 = sub1(0x800000l);
constexpr auto max_int40 = sub1(0x8000000000ll);
constexpr auto max_int48 = sub1(0x800000000000ll);
constexpr auto max_int56 = sub1(0x80000000000000ll);

constexpr auto min_int24 = -0x800000l;
constexpr auto min_int40 = -0x8000000000ll;
constexpr auto min_int48 = -0x800000000000ll;
constexpr auto min_int56 = -0x80000000000000ll;

static_assert(maximum<1>() == max_int8);
static_assert(maximum<2>() == max_int16);
static_assert(maximum<3>() == max_int24);
static_assert(maximum<4>() == max_int32);
static_assert(maximum<5>() == max_int40);
static_assert(maximum<6>() == max_int48);
static_assert(maximum<7>() == max_int56);
static_assert(maximum<8>() == max_int64);
static_assert(std::is_same<decltype(maximum<1>()), int8_t>::value);

static_assert(minimum<1>() == min_int8);
static_assert(minimum<2>() == min_int16);
static_assert(minimum<3>() == min_int24);
static_assert(minimum<4>() == min_int32);
static_assert(minimum<5>() == min_int40);
static_assert(minimum<6>() == min_int48);
static_assert(minimum<7>() == min_int56);
static_assert(minimum<8>() == min_int64);
static_assert(std::is_same<decltype(minimum<2>()), int16_t>::value);

static_assert(bitcoin_max<1>() == max_int8);
static_assert(bitcoin_max<2>() == max_int16);
static_assert(bitcoin_max<3>() == max_int24);
static_assert(bitcoin_max<4>() == max_int32);
static_assert(bitcoin_max<5>() == max_int40);
static_assert(bitcoin_max<6>() == max_int48);
static_assert(bitcoin_max<7>() == max_int56);
static_assert(bitcoin_max<8>() == max_int64);
static_assert(std::is_same<decltype(bitcoin_max<4>()), int32_t>::value);

static_assert(bitcoin_min<1>() == add1(min_int8));
static_assert(bitcoin_min<2>() == add1(min_int16));
static_assert(bitcoin_min<3>() == add1(min_int24));
static_assert(bitcoin_min<4>() == add1(min_int32));
static_assert(bitcoin_min<5>() == add1(min_int40));
static_assert(bitcoin_min<6>() == add1(min_int48));
static_assert(bitcoin_min<7>() == add1(min_int56));
static_assert(bitcoin_min<8>() == add1(min_int64));
static_assert(std::is_same<decltype(bitcoin_min<8>()), int64_t>::value);

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

// This demonstrates the loss of one value in the domain.
static_assert(!is_negated(absolute(int8_t(0b11111111))));
static_assert(!is_negated(absolute(int8_t(0b11111110))));
static_assert(!is_negated(absolute(int8_t(0b11111100))));
static_assert(!is_negated(absolute(int8_t(0b11111000))));
static_assert(!is_negated(absolute(int8_t(0b11110000))));
static_assert(!is_negated(absolute(int8_t(0b11100000))));
static_assert(!is_negated(absolute(int8_t(0b1100000))));
static_assert( is_negated(absolute(int8_t(0b10000000)))); // <== min_int8 (unpromoted)
static_assert(!is_negated(absolute(int8_t(0b01111111))));
static_assert(!is_negated(absolute(int8_t(0b00111111))));
static_assert(!is_negated(absolute(int8_t(0b00011111))));
static_assert(!is_negated(absolute(int8_t(0b00001111))));
static_assert(!is_negated(absolute(int8_t(0b00000111))));
static_assert(!is_negated(absolute(int8_t(0b00000011))));
static_assert(!is_negated(absolute(int8_t(0b00000010))));
static_assert(!is_negated(absolute(int8_t(0b00000001))));
static_assert(!is_negated(absolute(int8_t(0b00000000))));

// For any integral, this is the integral signed integer domain minimum value.
static_assert(is_negated(absolute(0x80_i8)));
static_assert(is_negated(absolute(0x8000_i16)));
static_assert(is_negated(absolute(0x80000000_i32)));
////static_assert(is_negated(absolute(0x8000000000000000_i64))); // absolute undefined

// Note that the leading bit of any leading byte would satisfy the above assertions.
static_assert(is_negated(absolute(0x00800000_i32)));

// However stack number compresses out all leading zero bytes, and then adds 
// a sign byte. Yet when converting to stack number to a int32_t, it guards
// against previous operation overflow by rejecting any 4 byte little-endian
// stack chunk size. This causes a 4 byte negated integer to be discarded,
// because its sign byte is a 5th stack chunk byte. But, the only value that
// can have an absolute value with a high bit is the integral minimum. So that
// value is considered an overflow as a matter of consensus. This could have
// been easily avoided, by converting the necessary 5 bytes and confirming the
// range, but alas.