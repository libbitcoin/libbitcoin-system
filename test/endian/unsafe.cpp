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

BOOST_AUTO_TEST_SUITE(endian_unsafe_tests)

constexpr uint32_t value32 = 0x01020304;
const data_chunk data_forward{ 0x01, 0x02, 0x03, 0x04 };
const data_chunk data_reverse{ 0x04, 0x03, 0x02, 0x01 };

BOOST_AUTO_TEST_CASE(endian__unsafe_from_big_endian__always__expected)
{
    BOOST_REQUIRE_EQUAL(unsafe_from_big_endian<uint32_t>(data_forward.data()), value32);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_little_endian__always__expected)
{
    BOOST_REQUIRE_EQUAL(unsafe_from_little_endian<uint32_t>(data_reverse.data()), value32);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_variable__one_byte__expected_advance_one)
{
    const data_chunk data{ 0xfc };
    const auto* at = data.data();
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0xfcu);
    BOOST_REQUIRE_EQUAL(std::distance(data.data(), at), 1);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_variable__two_bytes__expected_advance_three)
{
    const data_chunk data{ 0xfd, 0x02, 0x01 };
    const auto* at = data.data();
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0x0102u);
    BOOST_REQUIRE_EQUAL(std::distance(data.data(), at), 3);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_variable__four_bytes__expected_advance_five)
{
    const data_chunk data{ 0xfe, 0x04, 0x03, 0x02, 0x01 };
    const auto* at = data.data();
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0x01020304u);
    BOOST_REQUIRE_EQUAL(std::distance(data.data(), at), 5);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_variable__eight_bytes__expected_advance_nine)
{
    const data_chunk data{ 0xff, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    const auto* at = data.data();
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0x0102030405060708u);
    BOOST_REQUIRE_EQUAL(std::distance(data.data(), at), 9);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_from_variable__sequence__expected)
{
    const data_chunk data{ 0x00, 0xfd, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x80 };
    const auto* at = data.data();
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0x00u);
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0xffffu);
    BOOST_REQUIRE_EQUAL(unsafe_from_variable(at), 0x80000000u);
    BOOST_REQUIRE_EQUAL(std::distance(data.data(), at), 9);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_to_big_endian__always__expected)
{
    data_chunk buffer(sizeof(uint32_t));
    unsafe_to_big_endian(buffer.data(), value32);
    BOOST_REQUIRE_EQUAL(buffer, data_forward);
}

BOOST_AUTO_TEST_CASE(endian__unsafe_to_little_endian__always__expected)
{
    data_chunk buffer(sizeof(uint32_t));
    unsafe_to_little_endian(buffer.data(), value32);
    BOOST_REQUIRE_EQUAL(buffer, data_reverse);
}

BOOST_AUTO_TEST_SUITE_END()
