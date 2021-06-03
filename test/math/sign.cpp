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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(sign_tests)

// absolute signed

BOOST_AUTO_TEST_CASE(sign__absolute_signed__zero__zero)
{
    BOOST_REQUIRE_EQUAL(absolute(0), 0);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__one__one)
{
    BOOST_REQUIRE_EQUAL(absolute(1), 1);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__value__value)
{
    BOOST_REQUIRE_EQUAL(absolute(42), 42);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__negative_one__one)
{
    BOOST_REQUIRE_EQUAL(absolute(-1), 1);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__negative_value__value)
{
    BOOST_REQUIRE_EQUAL(absolute(-42), 42);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__min_int32_plus_one__max_int32)
{
    BOOST_REQUIRE_EQUAL(absolute(min_int32 + 1), max_int32);
}

BOOST_AUTO_TEST_CASE(sign__absolute_signed__max_int32__max_int32)
{
    BOOST_REQUIRE_EQUAL(absolute(max_int32), max_int32);
}

// absolute unsigned

BOOST_AUTO_TEST_CASE(sign__absolute_unsigned__zero__zero)
{
    BOOST_REQUIRE_EQUAL(absolute(0u), 0u);
}

BOOST_AUTO_TEST_CASE(sign__absolute_unsigned__one__one)
{
    BOOST_REQUIRE_EQUAL(absolute(1u), 1u);
}

BOOST_AUTO_TEST_CASE(sign__absolute_unsigned__value__value)
{
    BOOST_REQUIRE_EQUAL(absolute(42u), 42u);
}

BOOST_AUTO_TEST_CASE(sign__absolute_unsigned__max_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(absolute(max_uint32), max_uint32);
}

// is_negative signed

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__zero__false)
{
    BOOST_REQUIRE(!is_negative(0u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__one__false)
{
    BOOST_REQUIRE(!is_negative(1u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__positive_value__false)
{
    BOOST_REQUIRE(!is_negative(42u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__negative_one__true)
{
    BOOST_REQUIRE(is_negative(-1));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__negative_value__true)
{
    BOOST_REQUIRE(is_negative(-42));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__min_int32__true)
{
    BOOST_REQUIRE(is_negative(min_int32));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_signed__max_int32__false)
{
    BOOST_REQUIRE(!is_negative(max_int32));
}

// is_negative unsigned

BOOST_AUTO_TEST_CASE(sign__is_negative_unsigned__zero__false)
{
    BOOST_REQUIRE(!is_negative(0u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_unsigned__one__false)
{
    BOOST_REQUIRE(!is_negative(1u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_unsigned__positive_value__false)
{
    BOOST_REQUIRE(!is_negative(42u));
}

BOOST_AUTO_TEST_CASE(sign__is_negative_unsigned__max_uint32__false)
{
    BOOST_REQUIRE(!is_negative(max_uint32));
}

// is_odd

BOOST_AUTO_TEST_CASE(sign__is_odd__zero__false)
{
    BOOST_REQUIRE(!is_odd(0u));
}

BOOST_AUTO_TEST_CASE(sign__is_odd__odd__true)
{
    BOOST_REQUIRE(is_odd(1));
    BOOST_REQUIRE(is_odd(3u));
    BOOST_REQUIRE(is_odd(5));
    BOOST_REQUIRE(is_odd(7u));
    BOOST_REQUIRE(is_odd(9));
    BOOST_REQUIRE(is_odd(max_uint32));
}

BOOST_AUTO_TEST_CASE(sign__is_odd__even__false)
{
    BOOST_REQUIRE(!is_odd(2));
    BOOST_REQUIRE(!is_odd(4u));
    BOOST_REQUIRE(!is_odd(6));
    BOOST_REQUIRE(!is_odd(8u));
    BOOST_REQUIRE(!is_odd(10));
    BOOST_REQUIRE(!is_odd(max_uint32 - 1u));
}

BOOST_AUTO_TEST_SUITE_END()
