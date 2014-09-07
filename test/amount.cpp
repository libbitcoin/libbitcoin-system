/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(parse_amount_tests)

BOOST_AUTO_TEST_CASE(parse_amount_test)
{
    const uint64_t invalid_amount = MAX_UINT64;
    BOOST_REQUIRE(parse_amount("4.432") == 443200000);
    BOOST_REQUIRE(parse_amount("4.432.") == invalid_amount);
    BOOST_REQUIRE(parse_amount("4")  == 400000000);
    BOOST_REQUIRE(parse_amount("4.") == 400000000);
    BOOST_REQUIRE(parse_amount(".4") == 40000000);
    BOOST_REQUIRE(parse_amount(".")  == 0);
    BOOST_REQUIRE(parse_amount("0.00000004")  == 4);
    BOOST_REQUIRE(parse_amount("0.000000044") == 4);
    BOOST_REQUIRE(parse_amount("0.000000045") == 5);
    BOOST_REQUIRE(parse_amount("0.000000049") == 5);
    BOOST_REQUIRE(parse_amount("4.432112395") == 443211240);
    BOOST_REQUIRE(parse_amount("21000000") == 2100000000000000);
    BOOST_REQUIRE(parse_amount("1234.9", 0) == 1235);
    BOOST_REQUIRE(parse_amount("64.25", 5) == 6425000);
}

BOOST_AUTO_TEST_CASE(parse_amount_overflow_test)
{
    const uint64_t invalid_amount = MAX_UINT64;
    BOOST_REQUIRE(parse_amount("9999999999999999999", 0) ==
        9999999999999999999U);
    BOOST_REQUIRE(parse_amount("18446744073709551614", 0) ==
        18446744073709551614U);
    BOOST_REQUIRE(parse_amount("18446744073709551615", 0) == invalid_amount);
    BOOST_REQUIRE(parse_amount("18446744073709551616", 0) == invalid_amount);
    BOOST_REQUIRE(parse_amount("99999999999999999999", 0) == invalid_amount);
}

BOOST_AUTO_TEST_CASE(format_amount_test)
{
    BOOST_REQUIRE(format_amount(123, 0) == "123");
    BOOST_REQUIRE(format_amount(123, 2) == "1.23");
    BOOST_REQUIRE(format_amount(123, 4) == "0.0123");
}

BOOST_AUTO_TEST_SUITE_END()
