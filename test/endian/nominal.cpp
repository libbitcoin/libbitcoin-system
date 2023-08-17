/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(endian_nominal_tests)

constexpr data_array<9> bytes{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
static_assert(from_big_endian<uint8_t >(bytes) == 0x01_u8);
static_assert(from_big_endian<uint16_t>(bytes) == 0x0102_u16);
static_assert(from_big_endian<uint32_t>(bytes) == 0x01020304_u32);
static_assert(from_big_endian<uint64_t>(bytes) == 0x0102030405060708_u64);
static_assert(from_big_endian< int8_t >(bytes) == 0x01_i8);
static_assert(from_big_endian< int16_t>(bytes) == 0x0102_i16);
static_assert(from_big_endian< int32_t>(bytes) == 0x01020304_i32);
static_assert(from_big_endian< int64_t>(bytes) == 0x0102030405060708_i64);
static_assert(from_little_endian<uint8_t >(bytes) == 0x01_u8);
static_assert(from_little_endian<uint16_t>(bytes) == 0x0201_u16);
static_assert(from_little_endian<uint32_t>(bytes) == 0x04030201_u32);
static_assert(from_little_endian<uint64_t>(bytes) == 0x0807060504030201_u64);
static_assert(from_little_endian< int8_t >(bytes) == 0x01_i8);
static_assert(from_little_endian< int16_t>(bytes) == 0x0201_i16);
static_assert(from_little_endian< int32_t>(bytes) == 0x04030201_i32);
static_assert(from_little_endian< int64_t>(bytes) == 0x0807060504030201_i64);

const data_chunk chunk{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

BOOST_AUTO_TEST_CASE(endian__from_big_endian__single_byte_chunk__expected)
{
    BOOST_REQUIRE_EQUAL(from_big_endian<uint8_t >(chunk), 0x01_u8);
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(chunk), 0x0102_u16);
    BOOST_REQUIRE_EQUAL(from_big_endian<uint32_t>(chunk), 0x01020304_u32);
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(chunk), 0x0102030405060708_u64);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__single_byte_chunk___expected)
{
    BOOST_REQUIRE_EQUAL(from_big_endian<int8_t >(chunk), 0x01_i8);
    BOOST_REQUIRE_EQUAL(from_big_endian<int16_t>(chunk), 0x0102_i16);
    BOOST_REQUIRE_EQUAL(from_big_endian<int32_t>(chunk), 0x01020304_i32);
    BOOST_REQUIRE_EQUAL(from_big_endian<int64_t>(chunk), 0x0102030405060708_i64);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__single_byte_array__expected)
{
    BOOST_REQUIRE_EQUAL(from_little_endian<uint8_t >(chunk), 0x01_u8);
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(chunk), 0x0201_u16);
    BOOST_REQUIRE_EQUAL(from_little_endian<uint32_t>(chunk), 0x04030201_u32);
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(chunk), 0x0807060504030201_u64);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__single_byte_array__expected)
{
    BOOST_REQUIRE_EQUAL(from_little_endian<int8_t >(chunk), 0x01_i8);
    BOOST_REQUIRE_EQUAL(from_little_endian<int16_t>(chunk), 0x0201_i16);
    BOOST_REQUIRE_EQUAL(from_little_endian<int32_t>(chunk), 0x04030201_i32);
    BOOST_REQUIRE_EQUAL(from_little_endian<int64_t>(chunk), 0x0807060504030201_i64);
}

constexpr uint32_t value32 = 0x01020304;
const data_chunk data_forward{ 0x01, 0x02, 0x03, 0x04 };
const data_chunk data_reverse{ 0x04, 0x03, 0x02, 0x01 };

// short data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__chunk_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(data_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__chunk_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(data_reverse), value32);
}

// long data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__chunk_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(data_forward), value32 >> 16u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__chunk_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(data_reverse), value32 & 0x0000ffffu);
}

constexpr data_array<4> array_forward{ { 0x01, 0x02, 0x03, 0x04 } };
constexpr data_array<4> array_reverse{ { 0x04, 0x03, 0x02, 0x01 } };

// same data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer__expected)
{
    static_assert(from_big_endian<uint32_t>(array_forward) == value32);
    BOOST_REQUIRE_EQUAL(from_big_endian<uint32_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer__expected)
{
    static_assert(from_little_endian<uint32_t>(array_reverse) == value32);
    BOOST_REQUIRE_EQUAL(from_little_endian<uint32_t>(array_reverse), value32);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_array__expected)
{
    static_assert(to_big_endian(value32) == array_forward);
    BOOST_REQUIRE_EQUAL(to_big_endian(value32), array_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_array__expected)
{
    static_assert(to_little_endian(value32) == array_reverse);
    BOOST_REQUIRE_EQUAL(to_little_endian(value32), array_reverse);
}

// less data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    static_assert(from_big_endian<uint64_t>(array_forward) == value32);
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    static_assert(from_little_endian<uint64_t>(array_reverse) == value32);
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(array_reverse), value32);
}

// more data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    static_assert(from_big_endian<uint16_t>(array_forward) == (value32 >> 16u));
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(array_forward), value32 >> 16u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    static_assert(from_little_endian<uint16_t>(array_reverse) == (value32 & 0x0000ffffu));
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(array_reverse), value32 & 0x0000ffffu);
}

BOOST_AUTO_TEST_SUITE_END()
