/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(limit_tests)

static const size_t minimum = 0;
static const size_t maximum = max_size_t;
static const size_t half = maximum / 2;

// ceiling_add
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__ceiling_add__size_t_minimum_plus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__size_t_maximum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__size_t_half_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half, maximum), maximum);
}

// floor_subtract
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__floor_subtract__size_t_minimum_minus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__size_t_maximum_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__size_t_maximum_minus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__size_t_minimum_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__size_t_half_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half, maximum), minimum);
}

static const uint32_t min_uint32 = 0;
static const uint32_t half_uint32 = max_uint32 / 2;

// ceiling_add32
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__ceiling_add__min_uint32_plus_minimum__min_uint32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(min_uint32, min_uint32), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__max_uint32_plus_max_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(max_uint32, max_uint32), max_uint32);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__min_uint32_plus_max_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(min_uint32, max_uint32), max_uint32);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__max_uint32_plus_min_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(max_uint32, min_uint32), max_uint32);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__half_uint32_plus_max_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half_uint32, max_uint32), max_uint32);
}

// floor_subtract32
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__floor_subtract__min_uint32_minus_min_uint32__min_uint32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(min_uint32, min_uint32), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__max_uint32_minus_max_uint32__min_uint32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(max_uint32, max_uint32), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__max_uint32_minus_min_uint32__max_uint32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(max_uint32, min_uint32), max_uint32);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__min_uint32_minus_max_uint32__min_uint32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(min_uint32, max_uint32), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__half_uint32_minus_max_uint32__min_uint32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half_uint32, max_uint32), min_uint32);
}

static const uint64_t min_uint64 = 0;
static const uint64_t half_uint64 = max_uint64 / 2;

// ceiling_add64
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__ceiling_add__min_uint64_plus_min_uint64__min_uint64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(min_uint64, min_uint64), min_uint64);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__max_uint64_plus_max_uint64__max_uint64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(max_uint64, max_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__min_uint64_plus_max_uint64__max_uint64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(min_uint64, max_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__max_uint64_plus_min_uint64__max_uint64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(max_uint64, min_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__ceiling_add__half_uint64_plus_max_uint64__max_uint64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half_uint64, max_uint64), max_uint64);
}

// floor_subtract64
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__floor_subtract__min_uint64_minus_min_uint64__min_uint64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(min_uint64, min_uint64), min_uint64);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__max_uint64_minus_max_uint64__min_uint64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(max_uint64, max_uint64), min_uint64);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__max_uint64_minus_min_uint64__max_uint64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(max_uint64, min_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__min_uint64_minus_max_uint64__min_uint64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(min_uint64, max_uint64), min_uint64);
}

BOOST_AUTO_TEST_CASE(limits__floor_subtract__half_uint64_minus_max_uint64__min_uint64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half_uint64, max_uint64), min_uint64);
}

// safe_add
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__safe_add__size_t_minimum_plus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__safe_add__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(maximum, maximum), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__safe_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__safe_add__size_t_half_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(half, maximum), std::overflow_error);
}

// safe_subtract
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__safe_subtract__size_t_minimum_minus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_subtract(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__safe_subtract__size_t_maximum_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_subtract(maximum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__safe_subtract__size_t_maximum_minus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_subtract(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__safe_subtract__size_t_minimum_minus_maximum__throws_underflow)
{
    BOOST_REQUIRE_THROW(safe_subtract(minimum, maximum), std::underflow_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_subtract__size_t_half_minus_maximum__throws_underflow)
{
    BOOST_REQUIRE_THROW(safe_subtract(half, maximum), std::underflow_error);
}

// safe_increment
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__safe_increment__size_t_minimum__expected)
{
    auto value = minimum;
    static const auto expected = minimum + 1u;
    BOOST_REQUIRE_EQUAL(safe_increment(value), expected);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(limits__safe_increment__size_t_half__expected)
{
    auto value = half;
    static const auto expected = half + 1u;
    BOOST_REQUIRE_EQUAL(safe_increment(value), expected);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(limits__safe_increment__size_t_maximum__throws_overflow)
{
    auto value = maximum;
    BOOST_REQUIRE_THROW(safe_increment(value), std::overflow_error);
}

// safe_decrement
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__safe_decrement__size_t_maximum__expected)
{
    auto value = maximum;
    static const auto expected = maximum - 1u;
    BOOST_REQUIRE_EQUAL(safe_decrement(value), expected);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(limits__safe_decrement__size_t_half__expected)
{
    auto value = half;
    static const auto expected = half - 1u;
    BOOST_REQUIRE_EQUAL(safe_decrement(value), expected);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(limits__safe_decrement__size_t_minimum__throws_underflow)
{
    auto value = minimum;
    BOOST_REQUIRE_THROW(safe_decrement(value), std::underflow_error);
}

// safe_assign
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__safe_assign__size_t_minimum_to_size_t__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_assign<size_t>(minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__size_t_maximum_to_size_t_maximum)
{
    BOOST_REQUIRE_EQUAL(safe_assign<size_t>(maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__min_uint64_to_uint32__min_uint32)
{
    BOOST_REQUIRE_EQUAL(safe_assign<uint32_t>(min_uint64), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__max_uint32_to_uint64__max_uint32)
{
    BOOST_REQUIRE_EQUAL(safe_assign<uint64_t>(min_uint32), min_uint32);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__max_uint64_to_uint32__throws_range)
{
    BOOST_REQUIRE_THROW(safe_assign<uint32_t>(max_uint64), std::range_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__min_int64_to_uint32__min_uint32)
{
    BOOST_REQUIRE_THROW(safe_assign<uint32_t>(min_int64), std::range_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__max_int64_to_uint32__throws_range)
{
    BOOST_REQUIRE_THROW(safe_assign<uint32_t>(max_int64), std::range_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__min_uint64_to_int32__min_int32)
{
    BOOST_REQUIRE_THROW(safe_assign<int32_t>(min_uint64), std::range_error);
}

BOOST_AUTO_TEST_CASE(limits__safe_assign__max_uint64_to_int32__throws_range)
{
    BOOST_REQUIRE_THROW(safe_assign<int32_t>(max_uint64), std::range_error);
}

// range_constrain
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limits__range_constrain__over__max)
{
    const size_t expected = 10;
    const auto result = range_constrain(size_t(42), size_t(1), expected);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(limits__range_constrain__under__min)
{
    const size_t expected = 50;
    const auto result = range_constrain(size_t(42), expected, size_t(100));
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(limits__range_constrain__internal__unchanged)
{
    const size_t expected = 42;
    const auto result = range_constrain(expected, size_t(10), size_t(100));
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
