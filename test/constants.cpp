/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "test.hpp"

BOOST_AUTO_TEST_SUITE(constants_tests)

BOOST_AUTO_TEST_CASE(constants__zero__always__zero_unsigned)
{
    BOOST_REQUIRE_EQUAL(zero, 0u);
}

BOOST_AUTO_TEST_CASE(constants__one__always__one_unsigned)
{
    BOOST_REQUIRE_EQUAL(one, 1u);
}

BOOST_AUTO_TEST_CASE(constants__two__always__two_unsigned)
{
    BOOST_REQUIRE_EQUAL(two, 2u);
}

BOOST_AUTO_TEST_CASE(constants__byte_bits__always__eight_unsigned)
{
    BOOST_REQUIRE_EQUAL(byte_bits, 8u);
}

BOOST_AUTO_TEST_CASE(constants__is_one__one__true)
{
    BOOST_REQUIRE(is_one(1));
}

BOOST_AUTO_TEST_CASE(constants__is_one__not_one__false)
{
    BOOST_REQUIRE(!is_one(0));
    BOOST_REQUIRE(!is_one(2));
    BOOST_REQUIRE(!is_one(-42));
    BOOST_REQUIRE(!is_one(0xff));
}

BOOST_AUTO_TEST_CASE(constants__is_zero__zero__true)
{
    BOOST_REQUIRE(is_zero(0));
}

BOOST_AUTO_TEST_CASE(constants__is_zero__not_zero__false)
{
    BOOST_REQUIRE(!is_zero(1));
    BOOST_REQUIRE(!is_zero(2));
    BOOST_REQUIRE(!is_zero(-42));
    BOOST_REQUIRE(!is_zero(0xff));
}

BOOST_AUTO_TEST_CASE(constants__is_even__odd__false)
{
    BOOST_REQUIRE(!is_even(1));
    BOOST_REQUIRE(!is_even(3u));
    BOOST_REQUIRE(!is_even(5));
    BOOST_REQUIRE(!is_even(7u));
    BOOST_REQUIRE(!is_even(9));
    BOOST_REQUIRE(!is_even(max_uint32));
}

BOOST_AUTO_TEST_CASE(constants__is_even__even__true)
{
    BOOST_REQUIRE(is_even(0u));
    BOOST_REQUIRE(is_even(2));
    BOOST_REQUIRE(is_even(4u));
    BOOST_REQUIRE(is_even(6));
    BOOST_REQUIRE(is_even(8u));
    BOOST_REQUIRE(is_even(10));
    BOOST_REQUIRE(is_even(max_uint32 - 1u));
}

BOOST_AUTO_TEST_CASE(constants__is_odd__odd__true)
{
    BOOST_REQUIRE(is_odd(1));
    BOOST_REQUIRE(is_odd(3u));
    BOOST_REQUIRE(is_odd(5));
    BOOST_REQUIRE(is_odd(7u));
    BOOST_REQUIRE(is_odd(9));
    BOOST_REQUIRE(is_odd(max_uint32));
}

BOOST_AUTO_TEST_CASE(constants__is_odd__even__false)
{
    BOOST_REQUIRE(!is_odd(0u));
    BOOST_REQUIRE(!is_odd(2));
    BOOST_REQUIRE(!is_odd(4u));
    BOOST_REQUIRE(!is_odd(6));
    BOOST_REQUIRE(!is_odd(8u));
    BOOST_REQUIRE(!is_odd(10));
    BOOST_REQUIRE(!is_odd(max_uint32 - 1u));
}

BOOST_AUTO_TEST_CASE(constants__to_bits__always__times_eight)
{
    BOOST_REQUIRE_EQUAL(to_bits(0), 0 * 8);
    BOOST_REQUIRE_EQUAL(to_bits(1u), 1 * 8);
    BOOST_REQUIRE_EQUAL(to_bits(-42), -42 * 8);
    BOOST_REQUIRE_EQUAL(to_bits(0xff), 0xff * 8);
}

BOOST_AUTO_TEST_CASE(constants__to_bytes__always__divided_by_eight)
{
    BOOST_REQUIRE_EQUAL(to_bytes(0), 0 / 8);
    BOOST_REQUIRE_EQUAL(to_bytes(1), 1 / 8);
    BOOST_REQUIRE_EQUAL(to_bytes(-42), -42 / 8);
    BOOST_REQUIRE_EQUAL(to_bytes(0xff), 0xff / 8);
}

BOOST_AUTO_TEST_CASE(constants__to_half__always__divided_by_two)
{
    BOOST_REQUIRE_EQUAL(to_half(0), 0 / 2);
    BOOST_REQUIRE_EQUAL(to_half(1), 1 / 2);
    BOOST_REQUIRE_EQUAL(to_half(-42), -42 / 2);
    BOOST_REQUIRE_EQUAL(to_half(0xff), 0xff / 2);
}

BOOST_AUTO_TEST_CASE(constants__is_byte_aligned__multiples_of_eight__true)
{
    BOOST_REQUIRE(is_byte_aligned(0));
    BOOST_REQUIRE(is_byte_aligned(8));
    BOOST_REQUIRE(is_byte_aligned(16));
    BOOST_REQUIRE(is_byte_aligned(128));
}

BOOST_AUTO_TEST_CASE(constants__is_byte_aligned__not_multiples_of_eight__false)
{
    BOOST_REQUIRE(!is_byte_aligned(1));
    BOOST_REQUIRE(!is_byte_aligned(2));
    BOOST_REQUIRE(!is_byte_aligned(-42));
    BOOST_REQUIRE(!is_byte_aligned(0xff));
}

BOOST_AUTO_TEST_SUITE_END()
