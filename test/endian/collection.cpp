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

BOOST_AUTO_TEST_CASE(from_big_endian__parallel__always__expected)
{
    uint32_t to[4];
    const uint8_t from[16] =
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x00
    };

    from_big_endian<4>(to, from);
    BOOST_REQUIRE_EQUAL(to[0], 0x01020304u);
    BOOST_REQUIRE_EQUAL(to[1], 0x05060708u);
    BOOST_REQUIRE_EQUAL(to[2], 0x090a0b0cu);
    BOOST_REQUIRE_EQUAL(to[3], 0x0d0e0f00u);
}

BOOST_AUTO_TEST_CASE(from_little_endian__parallel__always__expected)
{
    uint32_t to[4];
    const uint8_t from[16] =
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x00
    };

    from_little_endian<4>(to, from);
    BOOST_REQUIRE_EQUAL(to[0], 0x04030201u);
    BOOST_REQUIRE_EQUAL(to[1], 0x08070605u);
    BOOST_REQUIRE_EQUAL(to[2], 0x0c0b0a09u);
    BOOST_REQUIRE_EQUAL(to[3], 0x000f0e0du);
}

BOOST_AUTO_TEST_CASE(to_big_endian__parallel__always__expected)
{
    uint8_t to[16];
    const uint32_t from[4] =
    {
        0x01020304,
        0x05060708,
        0x090a0b0c,
        0x0d0e0f00
    };

    to_big_endian<4>(to, from);

    BOOST_REQUIRE_EQUAL(to[0], 0x01u);
    BOOST_REQUIRE_EQUAL(to[1], 0x02u);
    BOOST_REQUIRE_EQUAL(to[2], 0x03u);
    BOOST_REQUIRE_EQUAL(to[3], 0x04u);

    BOOST_REQUIRE_EQUAL(to[4], 0x05u);
    BOOST_REQUIRE_EQUAL(to[5], 0x06u);
    BOOST_REQUIRE_EQUAL(to[6], 0x07u);
    BOOST_REQUIRE_EQUAL(to[7], 0x08u);

    BOOST_REQUIRE_EQUAL(to[8], 0x09u);
    BOOST_REQUIRE_EQUAL(to[9], 0x0au);
    BOOST_REQUIRE_EQUAL(to[10], 0x0bu);
    BOOST_REQUIRE_EQUAL(to[11], 0x0cu);

    BOOST_REQUIRE_EQUAL(to[12], 0x0du);
    BOOST_REQUIRE_EQUAL(to[13], 0x0eu);
    BOOST_REQUIRE_EQUAL(to[14], 0x0fu);
    BOOST_REQUIRE_EQUAL(to[15], 0x00u);
}

BOOST_AUTO_TEST_CASE(to_little_endian__parallel__always__expected)
{
    uint8_t to[16];
    const uint32_t from[4] =
    {
        0x01020304,
        0x05060708,
        0x090a0b0c,
        0x0d0e0f00
    };

    to_little_endian<4>(to, from);

    BOOST_REQUIRE_EQUAL(to[0], 0x04u);
    BOOST_REQUIRE_EQUAL(to[1], 0x03u);
    BOOST_REQUIRE_EQUAL(to[2], 0x02u);
    BOOST_REQUIRE_EQUAL(to[3], 0x01u);

    BOOST_REQUIRE_EQUAL(to[4], 0x08u);
    BOOST_REQUIRE_EQUAL(to[5], 0x07u);
    BOOST_REQUIRE_EQUAL(to[6], 0x06u);
    BOOST_REQUIRE_EQUAL(to[7], 0x05u);

    BOOST_REQUIRE_EQUAL(to[8], 0x0cu);
    BOOST_REQUIRE_EQUAL(to[9], 0x0bu);
    BOOST_REQUIRE_EQUAL(to[10], 0x0au);
    BOOST_REQUIRE_EQUAL(to[11], 0x09u);

    BOOST_REQUIRE_EQUAL(to[12], 0x00u);
    BOOST_REQUIRE_EQUAL(to[13], 0x0fu);
    BOOST_REQUIRE_EQUAL(to[14], 0x0eu);
    BOOST_REQUIRE_EQUAL(to[15], 0x0du);
}

BOOST_AUTO_TEST_SUITE_END()
