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

static_assert(minimum<int8_t>() == min_int8);
static_assert(minimum<int16_t>() == min_int16);
static_assert(minimum<int32_t>() == min_int32);
static_assert(minimum<int64_t>() == min_int64);
////static_assert(minimum<float>() == 0);
////static_assert(minimum<bool>() == 0);

static_assert(maximum<uint8_t>() == max_uint8);
static_assert(maximum<uint16_t>() == max_uint16);
static_assert(maximum<uint32_t>() == max_uint32);
static_assert(maximum<uint64_t>() == max_uint64);
////static_assert(maximum<bool>() == 0);
////static_assert(maximum<float>() == 0);

constexpr auto min_int24 = -0x800000l;
constexpr auto min_int40 = -0x8000000000ll;
constexpr auto min_int48 = -0x800000000000ll;
constexpr auto min_int56 = -0x80000000000000ll;

constexpr auto max_int24 = sub1(0x800000l);
constexpr auto max_int40 = sub1(0x8000000000ll);
constexpr auto max_int48 = sub1(0x800000000000ll);
constexpr auto max_int56 = sub1(0x80000000000000ll);

static_assert(minimum<1>() == min_int8);
static_assert(minimum<2>() == min_int16);
static_assert(minimum<3>() == min_int24);
static_assert(minimum<4>() == min_int32);
static_assert(minimum<5>() == min_int40);
static_assert(minimum<6>() == min_int48);
static_assert(minimum<7>() == min_int56);
static_assert(minimum<8>() == min_int64);

static_assert(maximum<1>() == max_int8);
static_assert(maximum<2>() == max_int16);
static_assert(maximum<3>() == max_int24);
static_assert(maximum<4>() == max_int32);
static_assert(maximum<5>() == max_int40);
static_assert(maximum<6>() == max_int48);
static_assert(maximum<7>() == max_int56);
static_assert(maximum<8>() == max_int64);

static_assert(bitcoin_min<1>() == add1(min_int8));
static_assert(bitcoin_min<2>() == add1(min_int16));
static_assert(bitcoin_min<3>() == add1(min_int24));
static_assert(bitcoin_min<4>() == add1(min_int32));
static_assert(bitcoin_min<5>() == add1(min_int40));
static_assert(bitcoin_min<6>() == add1(min_int48));
static_assert(bitcoin_min<7>() == add1(min_int56));
static_assert(bitcoin_min<8>() == add1(min_int64));

static_assert(bitcoin_max<1>() == max_int8);
static_assert(bitcoin_max<2>() == max_int16);
static_assert(bitcoin_max<3>() == max_int24);
static_assert(bitcoin_max<4>() == max_int32);
static_assert(bitcoin_max<5>() == max_int40);
static_assert(bitcoin_max<6>() == max_int48);
static_assert(bitcoin_max<7>() == max_int56);
static_assert(bitcoin_max<8>() == max_int64);

static_assert(bitcoin_min<4>() == add1(minimum<4>()));
static_assert(bitcoin_min<5>() == add1(minimum<5>()));
