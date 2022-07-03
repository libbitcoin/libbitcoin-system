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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(endian_tests)

// single byte (specialized)

BOOST_AUTO_TEST_CASE(from_big_endian__single_byte__chunk__expected)
{
    const data_chunk data{ 0x42, 0xff };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint8_t>(data), data[0]);
}

BOOST_AUTO_TEST_CASE(from_little_endian__single_byte__chunk___expected)
{
    const data_chunk data{ 0x42, 0xff };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint8_t>(data), data[0]);
}

BOOST_AUTO_TEST_CASE(to_big_endian__single_byte__array__expected)
{
    const data_array<1> data{ { 0x42 } };
    BOOST_REQUIRE_EQUAL(to_big_endian(data[0]), data);
}

BOOST_AUTO_TEST_CASE(to_little_endian__single_byte__array__expected)
{
    const data_array<1> data{ { 0x42 } };
    BOOST_REQUIRE_EQUAL(to_little_endian(data[0]), data);
}

// negatives

BOOST_AUTO_TEST_CASE(to_big_endian__negative__full__expected)
{
    const data_array<mini_hash_size> full_mini_hash{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_big_endian<mini_hash_size>(-1), full_mini_hash);
}

BOOST_AUTO_TEST_CASE(to_big_endian__negative__partial__expected)
{
    const data_array<mini_hash_size> half_mini_big{ { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 } };
    BOOST_REQUIRE_EQUAL(to_big_endian<mini_hash_size>(mask_right(-1, 24)), half_mini_big);
}

BOOST_AUTO_TEST_CASE(to_little_endian__negative__full__expected)
{
    const data_array<mini_hash_size> full_mini_hash{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_little_endian<mini_hash_size>(-1), full_mini_hash);
}

BOOST_AUTO_TEST_CASE(to_little_endian__negative__partial__expected)
{
    const data_array<mini_hash_size> half_mini_little{ { 0x00, 0x00, 0x00, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_little_endian<mini_hash_size>(mask_right(-1, 24)), half_mini_little);
}

// chunk

const auto chunk8 = base16_chunk("0102030405060708");

BOOST_AUTO_TEST_CASE(endian__to_endian_chunk_size__uint64_t__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian_size(from_little_endian<uint64_t>(chunk8)), reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(to_little_endian_size(from_big_endian<uint64_t>(chunk8)), reverse_copy(chunk8));
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian_size__excess__expected_capacity)
{
    constexpr auto excess = 42u;
    const auto chunk = to_big_endian_size(from_little_endian<uint64_t>(chunk8), excess);
    BOOST_REQUIRE_EQUAL(chunk, reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(chunk.size() + excess, chunk.capacity());
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian_size__excess__expected_capacity)
{
    constexpr auto excess = 42u;
    const auto chunk = to_little_endian_size(from_big_endian<uint64_t>(chunk8), excess);
    BOOST_REQUIRE_EQUAL(chunk, reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(chunk.size() + excess, chunk.capacity());
}

// array

const auto array8 = base16_array("0102030405060708");

BOOST_AUTO_TEST_CASE(endian__to_endian_array_size__uint64_t__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian_size<8>(from_little_endian<uint64_t>(array8)), reverse_copy(array8));
    BOOST_REQUIRE_EQUAL(to_little_endian_size<8>(from_big_endian<uint64_t>(array8)), reverse_copy(array8));
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

const data_array<4> array_forward{ { 0x01, 0x02, 0x03, 0x04 } };
const data_array<4> array_reverse{ { 0x04, 0x03, 0x02, 0x01 } };

// matched data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_big_endian<uint32_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_little_endian<uint32_t>(array_reverse), value32);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_array__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian(value32), array_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_array__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian(value32), array_reverse);
}

// short data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(array_reverse), value32);
}

// long data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(array_forward), value32 >> 16u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(array_reverse), value32 & 0x0000ffffu);
}

BOOST_AUTO_TEST_SUITE_END()
