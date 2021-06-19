/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"
#include <cstddef>
#include <cstdint>

 // TODO: rename to addition.cpp

constexpr size_t minimum = 0;
constexpr size_t maximum = max_size_t;
constexpr size_t half = to_half(maximum);

// ceilinged_add
static_assert(ceilinged_add(minimum, minimum) == minimum, "");
static_assert(ceilinged_add(minimum, maximum) == maximum, "");
static_assert(ceilinged_add(maximum, minimum) == maximum, "");
static_assert(ceilinged_add(maximum, maximum) == maximum, "");
static_assert(ceilinged_add(maximum, half) == maximum, "");
static_assert(ceilinged_add(half, maximum) == maximum, "");
static_assert(ceilinged_add(half, half) == maximum - lo_bit(maximum), "");

// floor_subtract
static_assert(floored_subtract(minimum, minimum) == minimum, "");
static_assert(floored_subtract(maximum, minimum) == maximum, "");
static_assert(floored_subtract(minimum, maximum) == minimum, "");
static_assert(floored_subtract(maximum, maximum) == minimum, "");
static_assert(floored_subtract(maximum, half) == half + lo_bit(maximum), "");
static_assert(floored_subtract(half, maximum) == minimum, "");
static_assert(floored_subtract(half, half) == minimum, "");

BOOST_AUTO_TEST_SUITE(addition_tests)

constexpr uint64_t min_uint64 = 0;
constexpr uint32_t min_uint32 = 0;
constexpr int32_t zer_int32 = 0;

constexpr uint32_t pos_uint32 = 42;
constexpr int32_t pos_int32 = 42;
constexpr int32_t neg_int32 = -42;

BOOST_AUTO_TEST_CASE(limits__add__all__expected)
{
    BOOST_REQUIRE_EQUAL(add<int64_t>(min_uint32, min_uint32), (int64_t)min_uint32 + (int64_t)min_uint32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(min_uint32, max_uint32), (int32_t)min_uint32 + (int32_t)max_uint32);
    BOOST_REQUIRE_EQUAL(add<int16_t>(max_uint32, min_uint32), (int16_t)max_uint32 + (int16_t)min_uint32);
    BOOST_REQUIRE_EQUAL(add<int8_t>(max_uint32, max_uint32), (int8_t)max_uint32 + (int8_t)max_uint32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(pos_uint32, pos_uint32), (int64_t)pos_uint32 + (int64_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(pos_uint32, max_uint32), (int32_t)pos_uint32 + (int32_t)max_uint32);
    BOOST_REQUIRE_EQUAL(add<int16_t>(max_uint32, pos_uint32), (int16_t)max_uint32 + (int16_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(pos_uint32, min_uint32), (int64_t)pos_uint32 + (int64_t)min_uint32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(min_uint32, pos_uint32), (int32_t)min_uint32 + (int32_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(zer_int32, zer_int32), (int64_t)zer_int32 + (int64_t)zer_int32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(zer_int32, max_int32), (int32_t)zer_int32 + (int32_t)max_int32);
    BOOST_REQUIRE_EQUAL(add<int16_t>(max_int32, zer_int32), (int16_t)max_int32 + (int16_t)zer_int32);
    BOOST_REQUIRE_EQUAL(add<int8_t>(max_int32, max_int32), (int8_t)max_int32 + (int8_t)max_int32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(neg_int32, neg_int32), (int64_t)neg_int32 + (int64_t)neg_int32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(neg_int32, max_int32), (int32_t)neg_int32 + (int32_t)max_int32);
    BOOST_REQUIRE_EQUAL(add<int16_t>(max_int32, neg_int32), (int16_t)max_int32 + (int16_t)neg_int32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(neg_int32, min_int32), (int64_t)neg_int32 + (int64_t)min_int32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(min_int32, neg_int32), (int32_t)min_int32 + (int32_t)neg_int32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(pos_int32, neg_int32), (int64_t)pos_int32 + (int64_t)neg_int32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(neg_int32, pos_int32), (int32_t)neg_int32 + (int32_t)pos_int32);
    BOOST_REQUIRE_EQUAL(add<int16_t>(pos_int32, pos_int32), (int16_t)pos_int32 + (int16_t)pos_int32);
    BOOST_REQUIRE_EQUAL(add<int64_t>(pos_int32, min_int32), (int64_t)pos_int32 + (int64_t)min_int32);
    BOOST_REQUIRE_EQUAL(add<int32_t>(min_int32, pos_int32), (int32_t)min_int32 + (int32_t)pos_int32);
}

BOOST_AUTO_TEST_CASE(limits__subtract__all__expected)
{
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(min_uint32, min_uint32), (int64_t)min_uint32 - (int64_t)min_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(min_uint32, max_uint32), (int32_t)min_uint32 - (int32_t)max_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int16_t>(max_uint32, min_uint32), (int16_t)max_uint32 - (int16_t)min_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int8_t>(max_uint32, max_uint32), (int8_t)max_uint32 - (int8_t)max_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(pos_uint32, pos_uint32), (int64_t)pos_uint32 - (int64_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(pos_uint32, max_uint32), (int32_t)pos_uint32 - (int32_t)max_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int16_t>(max_uint32, pos_uint32), (int16_t)max_uint32 - (int16_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(pos_uint32, min_uint32), (int64_t)pos_uint32 - (int64_t)min_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(min_uint32, pos_uint32), (int32_t)min_uint32 - (int32_t)pos_uint32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(zer_int32, zer_int32), (int64_t)zer_int32 - (int64_t)zer_int32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(zer_int32, max_int32), (int32_t)zer_int32 - (int32_t)max_int32);
    BOOST_REQUIRE_EQUAL(subtract<int16_t>(max_int32, zer_int32), (int16_t)max_int32 - (int16_t)zer_int32);
    BOOST_REQUIRE_EQUAL(subtract<int8_t>(max_int32, max_int32), (int8_t)max_int32 - (int8_t)max_int32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(neg_int32, neg_int32), (int64_t)neg_int32 - (int64_t)neg_int32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(neg_int32, max_int32), (int32_t)neg_int32 - (int32_t)max_int32);
    BOOST_REQUIRE_EQUAL(subtract<int16_t>(max_int32, neg_int32), (int16_t)max_int32 - (int16_t)neg_int32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(neg_int32, min_int32), (int64_t)neg_int32 - (int64_t)min_int32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(min_int32, neg_int32), (int32_t)min_int32 - (int32_t)neg_int32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(pos_int32, neg_int32), (int64_t)pos_int32 - (int64_t)neg_int32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(neg_int32, pos_int32), (int32_t)neg_int32 - (int32_t)pos_int32);
    BOOST_REQUIRE_EQUAL(subtract<int16_t>(pos_int32, pos_int32), (int16_t)pos_int32 - (int16_t)pos_int32);
    BOOST_REQUIRE_EQUAL(subtract<int64_t>(pos_int32, min_int32), (int64_t)pos_int32 - (int64_t)min_int32);
    BOOST_REQUIRE_EQUAL(subtract<int32_t>(min_int32, pos_int32), (int32_t)min_int32 - (int32_t)pos_int32);
}

// TODO: rename to limit.cpp

constexpr uint64_t min_uint64 = 0;

BOOST_AUTO_TEST_CASE(limits__limit1__all__expected)
{
    BOOST_REQUIRE_EQUAL(limit<uint8_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<int16_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<int32_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<int64_t>(min_uint64), 0);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(min_uint64), 0);

    BOOST_REQUIRE_EQUAL(limit<uint8_t>(min_int64), 0);
    BOOST_REQUIRE_EQUAL(limit<int16_t>(min_int64), min_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(min_int64), 0);
    BOOST_REQUIRE_EQUAL(limit<int32_t>(min_int64), min_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(min_int64), 0);
    BOOST_REQUIRE_EQUAL(limit<int64_t>(min_int64), min_int64);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(min_int64), 0);

    BOOST_REQUIRE_EQUAL(limit<uint8_t>(max_int64), max_uint8);
    BOOST_REQUIRE_EQUAL(limit<int16_t>(max_int64), max_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(max_int64), max_uint16);
    BOOST_REQUIRE_EQUAL(limit<int32_t>(max_int64), max_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(max_int64), max_uint32);
    BOOST_REQUIRE_EQUAL(limit<int64_t>(max_int64), max_int64);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(max_int64), max_int64);

    BOOST_REQUIRE_EQUAL(limit<uint8_t>(max_uint64), max_uint8);
    BOOST_REQUIRE_EQUAL(limit<int16_t>(max_uint64), max_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(max_uint64), max_uint16);
    BOOST_REQUIRE_EQUAL(limit<int32_t>(max_uint64), max_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(max_uint64), max_uint32);
    BOOST_REQUIRE_EQUAL(limit<int64_t>(max_uint64), max_int16);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(max_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__limit3__all__expected)
{
    BOOST_REQUIRE_EQUAL(limit(size_t(0), size_t(0), size_t(0)), size_t(0));
    BOOST_REQUIRE_EQUAL(limit(size_t(25), size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(size_t(50), size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(size_t(55), size_t(50), size_t(60)), size_t(55));
    BOOST_REQUIRE_EQUAL(limit(size_t(60), size_t(50), size_t(60)), size_t(60));
    BOOST_REQUIRE_EQUAL(limit(size_t(75), size_t(50), size_t(60)), size_t(60));

    BOOST_REQUIRE_EQUAL(limit(0, size_t(0), size_t(0)), size_t(0));
    BOOST_REQUIRE_EQUAL(limit(25, size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(50, size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(55, size_t(50), size_t(60)), size_t(55));
    BOOST_REQUIRE_EQUAL(limit(60, size_t(50), size_t(60)), size_t(60));
    BOOST_REQUIRE_EQUAL(limit(75, size_t(50), size_t(60)), size_t(60));

    BOOST_REQUIRE_EQUAL(limit(size_t(0), 0, 0), 0);
    BOOST_REQUIRE_EQUAL(limit(size_t(25), 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(size_t(50), 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(size_t(55), 50, 60), 55);
    BOOST_REQUIRE_EQUAL(limit(size_t(60), 50, 60), 60);
    BOOST_REQUIRE_EQUAL(limit(size_t(75), 50, 60), 60);

    BOOST_REQUIRE_EQUAL(limit(0, 0, 0), 0);
    BOOST_REQUIRE_EQUAL(limit(25, 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(50, 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(55, 50, 60), 55);
    BOOST_REQUIRE_EQUAL(limit(60, 50, 60), 60);
    BOOST_REQUIRE_EQUAL(limit(75, 50, 60), 60);

    BOOST_REQUIRE_EQUAL(limit(-25, -60, -50), -50);
    BOOST_REQUIRE_EQUAL(limit(-50, -60, -50), -50);
    BOOST_REQUIRE_EQUAL(limit(-55, -60, -50), -55);
    BOOST_REQUIRE_EQUAL(limit(-60, -60, -50), -60);
    BOOST_REQUIRE_EQUAL(limit(-75, -60, -50), -60);

    BOOST_REQUIRE_EQUAL(limit(-25, -60, 50), -25);
    BOOST_REQUIRE_EQUAL(limit(-50, -60, 50), -50);
    BOOST_REQUIRE_EQUAL(limit(-55, -60, 50), -55);
    BOOST_REQUIRE_EQUAL(limit(-60, -60, 50), -60);
    BOOST_REQUIRE_EQUAL(limit(-75, -60, 50), -60);
}

BOOST_AUTO_TEST_SUITE_END()
