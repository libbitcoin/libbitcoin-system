/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(point_tests)

using namespace system::chain;

static const auto point_hash = base16_hash(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");

static const auto point_data = base16_chunk(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f00000015");

static const point expected_point(point_data);

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(output__constructor__default__invalid)
{
    const point instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(output__constructor__move__valid)
{
    auto copy = expected_point;
    const point instance(std::move(copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_point);
}

BOOST_AUTO_TEST_CASE(output__constructor__copy__valid)
{
    const point instance(expected_point);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_point);
}

BOOST_AUTO_TEST_CASE(output__constructor__move_parameters__valid)
{
    const uint32_t index = 1234;
    auto copy = point_hash;
    point instance(std::move(copy), index);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), point_hash);
    BOOST_REQUIRE_EQUAL(instance.index(), index);
}

BOOST_AUTO_TEST_CASE(output__constructor__copy_parameters__valid)
{
    const uint32_t index = 1234;
    point instance(point_hash, index);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), point_hash);
    BOOST_REQUIRE_EQUAL(instance.index(), index);
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(point__assign__copy__expected)
{
    const auto& alpha = expected_point;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(point__assign__move__expected)
{
    const auto& alpha = expected_point;
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(point__equality__same__true)
{
    const auto& alpha = expected_point;
    const point beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(point__equality__different__false)
{
    const auto& alpha = expected_point;
    const point beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(point__inequality__same__false)
{
    const auto& alpha = expected_point;
    const point beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(point__inequality__different__true)
{
    const auto& alpha = expected_point;
    const point beta;
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(point__to_data__data__expected)
{
    const auto size = expected_point.to_data().size();
    BOOST_REQUIRE_EQUAL(size, expected_point.serialized_size());
}

BOOST_AUTO_TEST_CASE(point__to_data__stream__expected)
{
    // Write point to stream.
    std::stringstream iostream;
    expected_point.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const point copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_point);
}

BOOST_AUTO_TEST_CASE(point__to_data__writer__expected)
{
    // Write point to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_point.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const point copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_point);
}

// properties
// ----------------------------------------------------------------------------

// is_null
// serialized_size

BOOST_AUTO_TEST_SUITE_END()
