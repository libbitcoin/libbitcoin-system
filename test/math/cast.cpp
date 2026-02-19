/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <cfloat>

// promote
// depromote

// narrow_cast
// sign_cast
// narrow_sign_cast
// wide_cast
//
// possible_narrow_cast
// possible_sign_cast
// possible_narrow_sign_cast
// possible_sign_narrow_cast
// possible_narrow_and_sign_cast
// possible_wide_cast
//
// pointer_cast
// possible_pointer_cast

static_assert(to_signed(-2) == -2);
static_assert(to_signed(-1) == -1);
static_assert(to_signed(0) == 0);
static_assert(to_signed(0u) == 0);
static_assert(to_signed(1) == 1);
static_assert(to_signed(1u) == 1);
static_assert(to_signed(2u) == 2);
static_assert(is_same_type<decltype(to_signed(-1)), signed>);
static_assert(is_same_type<decltype(to_signed(0)), signed>);
static_assert(is_same_type<decltype(to_signed(0u)), signed>);
static_assert(is_same_type<decltype(to_signed(1)), signed>);
static_assert(is_same_type<decltype(to_signed(1u)), signed>);

static_assert(to_unsigned(-2) == 0xfffffffe);
static_assert(to_unsigned(-1) == 0xffffffff);
static_assert(to_unsigned(0) == 0u);
static_assert(to_unsigned(0u) == 0u);
static_assert(to_unsigned(1) == 1u);
static_assert(to_unsigned(1u) == 1u);
static_assert(to_unsigned(2u) == 2u);
static_assert(is_same_type<decltype(to_unsigned(-1)), unsigned>);
static_assert(is_same_type<decltype(to_unsigned(0)), unsigned>);
static_assert(is_same_type<decltype(to_unsigned(0u)), unsigned>);
static_assert(is_same_type<decltype(to_unsigned(1)), unsigned>);
static_assert(is_same_type<decltype(to_unsigned(1u)), unsigned>);

static_assert(to_integer(1.0f) == 1_size);
static_assert(to_integer<int>(1.0f) == 1u);
static_assert(to_integer<unsigned>(2.0) == 2u);
static_assert(is_same_type<decltype(to_integer(1.0f)), size_t>);
static_assert(is_same_type<decltype(to_integer<int>(1.0f)), int>);
static_assert(is_same_type<decltype(to_integer<unsigned>(1.0)), unsigned>);

static_assert(to_floating(1u) == 1.0);
static_assert(to_floating<float>(1u) == 1.0f);
static_assert(to_floating<double>(2u) == 2.0);
static_assert(is_same_type<decltype(to_floating(1u)), double>);
static_assert(is_same_type<decltype(to_floating<float>(1u)), float>);
static_assert(is_same_type<decltype(to_floating<double>(2u)), double>);

// TODO: below can move to static_assert under c++23.

BOOST_AUTO_TEST_SUITE(cast_tests)

template <typename Type>
using limit = std::numeric_limits<Type>;

// to_integer2/3

BOOST_AUTO_TEST_CASE(cast__to_integer__non_finites__false)
{
    size_t value{};
    BOOST_REQUIRE(!to_integer(value, limit<float>::quiet_NaN()));
    BOOST_REQUIRE(!to_integer(value, limit<float>::infinity()));
    BOOST_REQUIRE(!to_integer(value, -limit<float>::infinity()));
    BOOST_REQUIRE(!to_integer(value, limit<float>::quiet_NaN(), false));
    BOOST_REQUIRE(!to_integer(value, limit<float>::infinity(), false));
    BOOST_REQUIRE(!to_integer(value, -limit<float>::infinity(), false));
}

BOOST_AUTO_TEST_CASE(cast__to_integer__finites__expected)
{
    size_t value{};
    BOOST_REQUIRE(to_integer(value, 1.0, true));
    BOOST_REQUIRE_EQUAL(value, 1u);
    BOOST_REQUIRE(to_integer(value, 2.0, false));
    BOOST_REQUIRE_EQUAL(value, 2u);
    BOOST_REQUIRE(to_integer(value, 3.1, false));
    BOOST_REQUIRE_EQUAL(value, 3u);
    BOOST_REQUIRE(!to_integer(value, 3.1, true));
}

// to_ceilinged_integer

BOOST_AUTO_TEST_CASE(cast__to_ceilinged_integer__non_finites__max)
{
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(limit<double>::quiet_NaN()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(limit<double>::infinity()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(-limit<double>::infinity()), max_size_t);
}

BOOST_AUTO_TEST_CASE(cast__to_ceilinged_integer__overflow_underflow__clamped)
{
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(limit<double>::max()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(-limit<double>::max()), min_size_t);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<int64_t>(limit<double>::max()), max_int64);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<int64_t>(-limit<double>::max()), min_int64);
}

BOOST_AUTO_TEST_CASE(cast__to_ceilinged_integer__finites__expected)
{
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(0.0), 0u);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(1.0), 1u);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(1.1), 2u);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(1.9), 2u);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(-0.1), 0u);  // Ceil to 0 for unsigned
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<size_t>(-1.1), 0u);  // Clamped to min (0)
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<int64_t>(-1.0), -1);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<int64_t>(-1.1), -1);
    BOOST_REQUIRE_EQUAL(to_ceilinged_integer<int64_t>(-1.9), -1);
}

// to_floored_integer

BOOST_AUTO_TEST_CASE(cast__to_floored_integer__non_finites__max)
{
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(limit<double>::quiet_NaN()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(limit<double>::infinity()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(-limit<double>::infinity()), max_size_t);
}

BOOST_AUTO_TEST_CASE(cast__to_floored_integer__overflow_underflow__clamped)
{
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(limit<double>::max()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(-limit<double>::max()), min_size_t);
    BOOST_REQUIRE_EQUAL(to_floored_integer<int64_t>(limit<double>::max()), max_int64);
    BOOST_REQUIRE_EQUAL(to_floored_integer<int64_t>(-limit<double>::max()), min_int64);
}

BOOST_AUTO_TEST_CASE(cast__to_floored_integer__finites__expected)
{
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(0.0), 0u);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(1.0), 1u);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(1.1), 1u);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(1.9), 1u);
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(-0.1), 0u);  // Floor to -1, but clamped to 0
    BOOST_REQUIRE_EQUAL(to_floored_integer<size_t>(-1.1), 0u);  // Clamped to min (0)
    BOOST_REQUIRE_EQUAL(to_floored_integer<int64_t>(-1.0), -1);
    BOOST_REQUIRE_EQUAL(to_floored_integer<int64_t>(-1.1), -2);
    BOOST_REQUIRE_EQUAL(to_floored_integer<int64_t>(-1.9), -2);
}

// to_rounded_integer

BOOST_AUTO_TEST_CASE(cast__to_rounded_integer__non_finites__max)
{
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(limit<double>::quiet_NaN()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(limit<double>::infinity()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(-limit<double>::infinity()), max_size_t);
}

BOOST_AUTO_TEST_CASE(cast__to_rounded_integer__overflow_underflow__clamped)
{
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(limit<double>::max()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(-limit<double>::max()), min_size_t);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<int64_t>(limit<double>::max()), max_int64);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<int64_t>(-limit<double>::max()), min_int64);
}

BOOST_AUTO_TEST_CASE(cast__to_rounded_integer__finites__expected)
{
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(0.0), 0u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(1.0), 1u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(1.1), 1u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(1.4), 1u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(1.5), 2u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(1.6), 2u);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(-0.1), 0u);  // Round to 0
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(-0.5), 0u);  // Round to -1, clamped to 0
    BOOST_REQUIRE_EQUAL(to_rounded_integer<size_t>(-1.1), 0u);  // Clamped to min (0)
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-1.0), -1);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-1.1), -1);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-1.4), -1);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-1.5), -2);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-1.6), -2);
    BOOST_REQUIRE_EQUAL(to_rounded_integer<signed_size_t>(-2.5), -3);  // Halfway away from min_size_t
}

// to_truncated_integer

BOOST_AUTO_TEST_CASE(cast__to_truncated_integer__non_finites__max)
{
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(limit<double>::quiet_NaN()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(limit<double>::infinity()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(-limit<double>::infinity()), max_size_t);
}

BOOST_AUTO_TEST_CASE(cast__to_truncated_integer__overflow_underflow__clamped)
{
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(limit<double>::max()), max_size_t);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(-limit<double>::max()), min_size_t);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<signed_size_t>(limit<double>::max()), max_signed_size_t);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<signed_size_t>(-limit<double>::max()), min_signed_size_t);
}

BOOST_AUTO_TEST_CASE(cast__to_truncated_integer__finites__expected)
{
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(0.0), 0u);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(1.0), 1u);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(1.1), 1u);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(1.9), 1u);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(-0.1), 0u);  // Trunc to 0
    BOOST_REQUIRE_EQUAL(to_truncated_integer<size_t>(-1.1), 0u);  // Clamped to min (0)
    BOOST_REQUIRE_EQUAL(to_truncated_integer<signed_size_t>(-1.0), -1);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<signed_size_t>(-1.1), -1);
    BOOST_REQUIRE_EQUAL(to_truncated_integer<signed_size_t>(-1.9), -1);
}

BOOST_AUTO_TEST_SUITE_END()
