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
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(limit_tests)

static const size_t minimum = 0;
static const size_t maximum = max_size_t;
static const size_t half = maximum / 2;

// ceiling_add
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum_plus_minumum__minimum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_half_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half, maximum), maximum);
}

// floor_subtract
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum_minus_minumum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum_minus_minimum___maximum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum_minus_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_half_minus_maximum___minimum)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half, maximum), minimum);
}

static const uint32_t minimum32 = 0;
static const uint32_t maximum32 = max_uint32;
static const uint32_t half32 = maximum32 / 2;

// ceiling_add32
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum32_plus_minumum__minimum32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum32, minimum32), minimum32);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum32_plus_maximum32__maximum32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum32, maximum32), maximum32);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum32_plus_maximum32__maximum32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum32, maximum32), maximum32);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum32_plus_minimum32__maximum32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum32, minimum32), maximum32);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_half32_plus_maximum32__maximum32)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half32, maximum32), maximum32);
}

// floor_subtract32
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum32_minus_minumum__minimum32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum32, minimum32), minimum32);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum32_minus_maximum32__minimum32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum32, maximum32), minimum32);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum32_minus_minimum32___maximum32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum32, minimum32), maximum32);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum32_minus_maximum32__minimum32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum32, maximum32), minimum32);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_half32_minus_maximum32___minimum32)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half32, maximum32), minimum32);
}

static const uint64_t minimum64 = 0;
static const uint64_t maximum64 = max_uint64;
static const uint64_t half64 = maximum64 / 2;

// ceiling_add64
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum64_plus_minumum__minimum64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum64, minimum64), minimum64);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum64_plus_maximum64__maximum64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum64, maximum64), maximum64);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_minimum64_plus_maximum64__maximum64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(minimum64, maximum64), maximum64);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_maximum64_plus_minimum64__maximum64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(maximum64, minimum64), maximum64);
}

BOOST_AUTO_TEST_CASE(limit__ceiling_add__size_t_half64_plus_maximum64__maximum64)
{
    BOOST_REQUIRE_EQUAL(ceiling_add(half64, maximum64), maximum64);
}

// floor_subtract64
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum64_minus_minumum__minimum64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum64, minimum64), minimum64);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum64_minus_maximum64__minimum64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum64, maximum64), minimum64);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_maximum64_minus_minimum64___maximum64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(maximum64, minimum64), maximum64);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_minimum64_minus_maximum64__minimum64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(minimum64, maximum64), minimum64);
}

BOOST_AUTO_TEST_CASE(limit__floor_subtract__size_t_half64_minus_maximum64___minimum64)
{
    BOOST_REQUIRE_EQUAL(floor_subtract(half64, maximum64), minimum64);
}

BOOST_AUTO_TEST_SUITE_END()
