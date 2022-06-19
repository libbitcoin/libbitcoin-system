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

 // std::is_constant_evaluated

 // Beware of type promotion (use explicit argument).
static_assert(byteswap(0x01u) == 0x01000000u);
static_assert(byteswap(0x0102u) == 0x02010000u);
static_assert(byteswap(0x01020304u) == 0x04030201u);
static_assert(byteswap(0x0102030405060708ull) == 0x0807060504030201ull);

static_assert(byteswap<uint8_t>(0x01u) == 0x01u);
static_assert(byteswap<uint16_t>(0x0102u) == 0x0201u);
static_assert(byteswap<uint32_t>(0x01020304u) == 0x04030201u);
static_assert(byteswap<uint64_t>(0x0102030405060708ull) == 0x0807060504030201ull);

static_assert(byteswap(0x01) == 0x01000000);
static_assert(byteswap(0x0102) == 0x02010000);
static_assert(byteswap(0x01020304) == 0x04030201);
static_assert(byteswap(0x0102030405060708ll) == 0x0807060504030201ll);

static_assert(byteswap<int8_t>(0x01) == 0x01);
static_assert(byteswap<int16_t>(0x0102) == 0x0201);
static_assert(byteswap<int32_t>(0x01020304) == 0x04030201);
static_assert(byteswap<int64_t>(0x0102030405060708ll) == 0x0807060504030201ll);

static_assert(byteswap(0xfe) == 0xfe000000);
static_assert(byteswap(0xfffe) == 0xfeff0000);
static_assert(byteswap(0xffffff00) == 0x00ffffff);
static_assert(byteswap(0xffffffffffffff00ll) == 0x00ffffffffffffffll);

static_assert(byteswap<int8_t>(narrow_cast<int8_t>(0xfe)) == narrow_cast<int8_t>(0xfe));
static_assert(byteswap<int16_t>(narrow_cast<int8_t>(0xfffe)) == narrow_cast<int16_t>(0xfeff));
static_assert(byteswap<int32_t>(0xffffff00) == 0x00ffffff);
static_assert(byteswap<int64_t>(0xffffffffffffff00ll) == 0x00ffffffffffffffll);

BOOST_AUTO_TEST_SUITE(bytes_tests)

// !std::is_constant_evaluated

BOOST_AUTO_TEST_CASE(byteswap__not_constant_evaluated__always__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01u), 0x01000000u);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102u), 0x02010000u);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304u), 0x04030201u);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708ull), 0x0807060504030201ull);

    BOOST_REQUIRE_EQUAL(byteswap<uint8_t>(0x01u), 0x01u);
    BOOST_REQUIRE_EQUAL(byteswap<uint16_t>(0x0102u), 0x0201u);
    BOOST_REQUIRE_EQUAL(byteswap<uint32_t>(0x01020304u), 0x04030201u);
    BOOST_REQUIRE_EQUAL(byteswap<uint64_t>(0x0102030405060708ull), 0x0807060504030201ull);

    BOOST_REQUIRE_EQUAL(byteswap<int8_t>(0x01), 0x01);
    BOOST_REQUIRE_EQUAL(byteswap<int16_t>(0x0102), 0x0201);
    BOOST_REQUIRE_EQUAL(byteswap<int32_t>(0x01020304), 0x04030201);
    BOOST_REQUIRE_EQUAL(byteswap<int64_t>(0x0102030405060708), 0x0807060504030201);

    BOOST_REQUIRE_EQUAL(byteswap(0xfe), to_signed(0xfe000000));
    BOOST_REQUIRE_EQUAL(byteswap(0xff00), 0x00ff0000);
    BOOST_REQUIRE_EQUAL(byteswap(0xffffff00), to_unsigned(0x00ffffff));
    BOOST_REQUIRE_EQUAL(byteswap(0xffffffffffffff00), to_unsigned(0x00ffffffffffffff));

    BOOST_REQUIRE_EQUAL(byteswap<int8_t>(narrow_cast<int8_t>(0xfe)), narrow_cast<int8_t>(0xfe));
    BOOST_REQUIRE_EQUAL(byteswap<int16_t>(narrow_cast<int8_t>(0xfffe)), narrow_cast<int16_t>(0xfeff));
    BOOST_REQUIRE_EQUAL(byteswap<int32_t>(0xffffff00l), 0x00ffffff);
    BOOST_REQUIRE_EQUAL(byteswap<int64_t>(0xffffffffffffff00ll), 0x00ffffffffffffff);
}
BOOST_AUTO_TEST_SUITE_END()