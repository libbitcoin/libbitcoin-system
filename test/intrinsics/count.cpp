/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BOOST_AUTO_TEST_SUITE(intrinsics_count_tests)

// count_left_zeros

static_assert(count_left_zeros(0_u8) == 8);
static_assert(count_left_zeros(1_u8) == 7);
static_assert(count_left_zeros(0x80_u8) == 0);
static_assert(count_left_zeros(0xff_u8) == 0);
static_assert(count_left_zeros(0_u16) == 16);
static_assert(count_left_zeros(1_u16) == 15);
static_assert(count_left_zeros(0x0100_u16) == 7);
static_assert(count_left_zeros(0x8000_u16) == 0);
static_assert(count_left_zeros(0_u32) == 32);
static_assert(count_left_zeros(1_u32) == 31);
static_assert(count_left_zeros(0x00010000_u32) == 15);
static_assert(count_left_zeros(0x80000000_u32) == 0);
static_assert(count_left_zeros(0_u64) == 64);
static_assert(count_left_zeros(1_u64) == 63);
static_assert(count_left_zeros(0x0000000100000000_u64) == 31);
static_assert(count_left_zeros(0x8000000000000000_u64) == 0);
static_assert(count_left_zeros(max_uint64) == 0);

BOOST_AUTO_TEST_CASE(intrinsics__count_left_zeros__values__expected)
{
    BOOST_REQUIRE_EQUAL(count_left_zeros(0_u8), 8);
    BOOST_REQUIRE_EQUAL(count_left_zeros(1_u8), 7);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x80_u8), 0);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0_u16), 16);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x0100_u16), 7);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x8000_u16), 0);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0_u32), 32);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x00010000_u32), 15);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x80000000_u32), 0);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0_u64), 64);
    BOOST_REQUIRE_EQUAL(count_left_zeros(1_u64), 63);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x0000000100000000_u64), 31);
    BOOST_REQUIRE_EQUAL(count_left_zeros(0x8000000000000000_u64), 0);
    BOOST_REQUIRE_EQUAL(count_left_zeros(max_uint64), 0);
}

// count_right_zeros

static_assert(count_right_zeros(0_u8) == 8);
static_assert(count_right_zeros(1_u8) == 0);
static_assert(count_right_zeros(0x80_u8) == 7);
static_assert(count_right_zeros(0xff_u8) == 0);
static_assert(count_right_zeros(0_u16) == 16);
static_assert(count_right_zeros(1_u16) == 0);
static_assert(count_right_zeros(0x0100_u16) == 8);
static_assert(count_right_zeros(0x8000_u16) == 15);
static_assert(count_right_zeros(0_u32) == 32);
static_assert(count_right_zeros(1_u32) == 0);
static_assert(count_right_zeros(0x00010000_u32) == 16);
static_assert(count_right_zeros(0x80000000_u32) == 31);
static_assert(count_right_zeros(0_u64) == 64);
static_assert(count_right_zeros(1_u64) == 0);
static_assert(count_right_zeros(0x0000000100000000_u64) == 32);
static_assert(count_right_zeros(0x8000000000000000_u64) == 63);
static_assert(count_right_zeros(max_uint64) == 0);

BOOST_AUTO_TEST_CASE(intrinsics__count_right_zeros__values__expected)
{
    BOOST_REQUIRE_EQUAL(count_right_zeros(0_u8), 8);
    BOOST_REQUIRE_EQUAL(count_right_zeros(1_u8), 0);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x80_u8), 7);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0_u16), 16);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x0100_u16), 8);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x8000_u16), 15);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0_u32), 32);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x00010000_u32), 16);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x80000000_u32), 31);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0_u64), 64);
    BOOST_REQUIRE_EQUAL(count_right_zeros(1_u64), 0);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x0000000100000000_u64), 32);
    BOOST_REQUIRE_EQUAL(count_right_zeros(0x8000000000000000_u64), 63);
    BOOST_REQUIRE_EQUAL(count_right_zeros(max_uint64), 0);
}

BOOST_AUTO_TEST_SUITE_END()
