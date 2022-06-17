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
#include <cstddef>
#include <cstdint>

BOOST_AUTO_TEST_SUITE(limits_tests)

constexpr size_t minimum = 0;
constexpr size_t maximum = max_size_t;
constexpr size_t half = maximum / 2;

// ????
////constexpr int32_t left = static_cast<int32_t>(min_int64);
////constexpr int32_t right = narrow_cast<int32_t>(min_int64);
////static_assert(left == 0, "");
////static_assert(left == right, "");
////static_assert(right == min_int64, "");

// safe_multiply

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(maximum, maximum), overflow_exception);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(maximum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_half_times_2__maximum_minus_1)
{
    // The maximum of an unsigned integer is always odd, so half is always rounded down.
    // Therefore 2 * half is always one less than the unsigned integer maximum.
    BOOST_REQUIRE_EQUAL(safe_multiply(half, size_t(2)), sub1(maximum));
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_2_times_half_plus_1__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(size_t(2), add1(half)), overflow_exception);
}

// safe_add

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(maximum, maximum), overflow_exception);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_half_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(half, maximum), overflow_exception);
}

BOOST_AUTO_TEST_SUITE_END()
