/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(data_tests)

BOOST_AUTO_TEST_CASE(data__last_byte__two_bytes__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, expected
    };

    BOOST_REQUIRE_EQUAL(last_byte(source), expected);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_front__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        expected, 42, 42
    };

    BOOST_REQUIRE_EQUAL(slice(source, 0, 3)[0], expected);
}


BOOST_AUTO_TEST_CASE(data__slice__three_bytes_middle__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, expected, 42
    };

    BOOST_REQUIRE_EQUAL(slice(source, 1, 2)[0], expected);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_end__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, expected
    };

    BOOST_REQUIRE_EQUAL(slice(source, 2, 3)[0], expected);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_middle_empty__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, 42
    };

    BOOST_REQUIRE(slice(source, 2, 2).empty());
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_end_empty__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, 42
    };

    BOOST_REQUIRE(slice(source, 3, 3).empty());
}

BOOST_AUTO_TEST_CASE(data__split__data_chunk__expected)
{
    const uint8_t expected_lower = 42;
    const uint8_t expected_upper = 24;
    data_chunk source
    {
        expected_lower, expected_upper
    };

    data_chunk lower_half;
    data_chunk upper_half;
    split(source, lower_half, upper_half, source.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], expected_lower);
    BOOST_REQUIRE_EQUAL(upper_half[0], expected_upper);
}

BOOST_AUTO_TEST_CASE(data__split__long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    long_hash source
    { 
        {
            l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        } 
    };

    data_chunk lower_half;
    data_chunk upper_half;
    split(source, lower_half, upper_half, source.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], l);
    BOOST_REQUIRE_EQUAL(upper_half[0], u);
}

BOOST_AUTO_TEST_SUITE_END()
