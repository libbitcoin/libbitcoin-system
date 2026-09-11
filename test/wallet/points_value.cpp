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

BOOST_AUTO_TEST_SUITE(points_value_tests)

using namespace bc::system::wallet;

BOOST_AUTO_TEST_CASE(points_value__value__default__zero)
{
    static const points_value instance{};
    BOOST_REQUIRE_EQUAL(instance.value(), 0u);
}

BOOST_AUTO_TEST_CASE(points_value__value__single_point_value__expected)
{
    static const auto expected = 42u;
    static const points_value instance{ { { { null_hash, 0u }, expected } } };
    BOOST_REQUIRE_EQUAL(instance.value(), expected);
}

BOOST_AUTO_TEST_CASE(points_value__value__multiple_point_values__expected)
{
    static const auto expected = 10u + 30u + 42u;
    static const points_value instance
    {
        {
            { { null_hash, 0u }, 10u },
            { { null_hash, 1u }, 30u },
            { { null_hash, 0u }, 42u },
        }
    };
    BOOST_REQUIRE_EQUAL(instance.value(), expected);
}

BOOST_AUTO_TEST_CASE(points_value__value__multiple_point_values_remove_last__expected)
{
    static const auto expected = 10u + 30u;
    points_value instance
    {
        {
            { { null_hash, 0u }, 10u },
            { { null_hash, 1u }, 30u },
            { { null_hash, 0u }, 42u },
        }
    };
    instance.points.pop_back();
    BOOST_REQUIRE_EQUAL(instance.value(), expected);
}

BOOST_AUTO_TEST_CASE(points_value__value__multiple_point_values_clear__zero)
{
    points_value instance
    {
        {
            { { null_hash, 0u }, 10u },
            { { null_hash, 1u }, 30u },
            { { null_hash, 0u }, 42u },
        }
    };
    instance.points.clear();
    BOOST_REQUIRE_EQUAL(instance.value(), 0u);
}

// select (greedy)
// ----------------------------------------------------------------------------

static points_value unspent_points() NOEXCEPT
{
    return points_value
    {
        {
            { { null_hash, 10u }, 10u },
            { { null_hash, 30u }, 30u },
            { { null_hash, 42u }, 42u },
            { { null_hash, 50u }, 50u }
        }
    };
}

BOOST_AUTO_TEST_CASE(points_value__select__insufficient_total__empty)
{
    points_value out{};
    points_value::select(out, unspent_points(), 1000u);
    BOOST_REQUIRE(out.points.empty());
}

BOOST_AUTO_TEST_CASE(points_value__select__single_sufficient_point__that_point)
{
    const points_value unspent{ { { { null_hash, 42u }, 42u } } };
    points_value out{};
    points_value::select(out, unspent, 40u);
    BOOST_REQUIRE_EQUAL(out.points.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.points.front().value(), 42u);
}

// The smallest individually sufficient point is preferred.
BOOST_AUTO_TEST_CASE(points_value__select__greedy_sufficient_point__smallest_sufficient)
{
    points_value out{};
    points_value::select(out, unspent_points(), 40u);
    BOOST_REQUIRE_EQUAL(out.points.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.points.front().value(), 42u);
}

// With no individually sufficient point the largest are accumulated.
BOOST_AUTO_TEST_CASE(points_value__select__greedy_no_sufficient_point__descending_set)
{
    points_value out{};
    points_value::select(out, unspent_points(), 80u);
    BOOST_REQUIRE_EQUAL(out.points.size(), 2u);
    BOOST_REQUIRE_EQUAL(out.points[0].value(), 50u);
    BOOST_REQUIRE_EQUAL(out.points[1].value(), 42u);
}

BOOST_AUTO_TEST_CASE(points_value__select__greedy_exhausts_points__all_points)
{
    points_value out{};
    points_value::select(out, unspent_points(), 132u);
    BOOST_REQUIRE_EQUAL(out.points.size(), 4u);
    BOOST_REQUIRE_EQUAL(out.value(), 132u);
}

// select (individual)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(points_value__select__individual_mixed__sufficient_ascending)
{
    points_value out{};
    points_value::select(out, unspent_points(), 30u, points_value::selection::individual);
    BOOST_REQUIRE_EQUAL(out.points.size(), 3u);
    BOOST_REQUIRE_EQUAL(out.points[0].value(), 30u);
    BOOST_REQUIRE_EQUAL(out.points[1].value(), 42u);
    BOOST_REQUIRE_EQUAL(out.points[2].value(), 50u);
}

BOOST_AUTO_TEST_CASE(points_value__select__individual_none_sufficient__empty)
{
    points_value out{};
    points_value::select(out, unspent_points(), 1000u, points_value::selection::individual);
    BOOST_REQUIRE(out.points.empty());
}

// The default option is greedy, which returns one sufficient point.
BOOST_AUTO_TEST_CASE(points_value__select__default_option__greedy)
{
    points_value greedy{}, defaulted{};
    points_value::select(greedy, unspent_points(), 40u, points_value::selection::greedy);
    points_value::select(defaulted, unspent_points(), 40u);
    BOOST_REQUIRE_EQUAL(defaulted.points.size(), greedy.points.size());
    BOOST_REQUIRE_EQUAL(defaulted.value(), greedy.value());
}

BOOST_AUTO_TEST_SUITE_END()
