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

BOOST_AUTO_TEST_SUITE(byteswap_tests)

// Beware of type promotion (use explicit types).

// byteswap (constexpr)
static_assert(byteswap(0x01_i8) == 0x01);
static_assert(byteswap(0x0102_i16) == 0x0201);
static_assert(byteswap(0x01020304_i32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_i64) == 0x0807060504030201);
static_assert(byteswap(0x01_u8) == 0x01);
static_assert(byteswap(0x0102_u16) == 0x0201);
static_assert(byteswap(0x01020304_u32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_u64) == 0x0807060504030201);

BOOST_AUTO_TEST_CASE(byteswap__signed__not_constant_evaluated_signed__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_i8), 0x01_i8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_i16), 0x0201_i16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_i32), 0x04030201_i32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_i64), 0x0807060504030201_i64);
}

BOOST_AUTO_TEST_CASE(byteswap__unsigned___not_constant_evaluated_unsigned__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_u8), 0x01_u8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_u16), 0x0201_u16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_u32), 0x04030201_u32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_u64), 0x0807060504030201_u64);
}

BOOST_AUTO_TEST_SUITE_END()
