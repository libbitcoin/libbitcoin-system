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

BC_AUTO_TEST_SUITE(point_tests)

namespace json = system::json;
using namespace system::chain;

static const auto point_hash = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
static const auto point_data = base16_chunk("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f00000015");
static const point expected_point(point_data);

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BC_AUTO_TEST_CASE(point__constructor__default__invalid)
{
    const point instance;
    BC_REQUIRE(!instance.is_valid());
}

BC_AUTO_TEST_CASE(point__constructor__move__valid)
{
    auto copy = expected_point;
    const point instance(std::move(copy));
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_point);
}

BC_AUTO_TEST_CASE(point__constructor__copy__valid)
{
    const point instance(expected_point);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_point);
}

BC_AUTO_TEST_CASE(point__constructor__move_parameters__valid)
{
    const uint32_t index = 1234;
    auto copy = point_hash;
    point instance(std::move(copy), index);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.hash(), point_hash);
    BC_REQUIRE_EQUAL(instance.index(), index);
}

BC_AUTO_TEST_CASE(point__constructor__copy_parameters__valid)
{
    const uint32_t index = 1234;
    point instance(point_hash, index);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.hash(), point_hash);
    BC_REQUIRE_EQUAL(instance.index(), index);
}

// operators
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(point__assign__copy__expected)
{
    const auto& alpha = expected_point;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(point__assign__move__expected)
{
    const auto& alpha = expected_point;
    const auto beta = alpha;
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(point__equality__same__true)
{
    const auto& alpha = expected_point;
    const point beta(alpha);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(point__equality__different__false)
{
    const auto& alpha = expected_point;
    const point beta;
    BC_REQUIRE(!(alpha == beta));
}

BC_AUTO_TEST_CASE(point__inequality__same__false)
{
    const auto& alpha = expected_point;
    const point beta(alpha);
    BC_REQUIRE(!(alpha != beta));
}

BC_AUTO_TEST_CASE(point__inequality__different__true)
{
    const auto& alpha = expected_point;
    const point beta;
    BC_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(point__to_data__data__expected)
{
    const auto size = expected_point.to_data().size();
    BC_REQUIRE_EQUAL(size, expected_point.serialized_size());
}

BC_AUTO_TEST_CASE(point__to_data__stream__expected)
{
    // Write point to stream.
    std::stringstream iostream;
    expected_point.to_data(iostream);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const point copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_point);
}

BC_AUTO_TEST_CASE(point__to_data__writer__expected)
{
    // Write point to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_point.to_data(out);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const point copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_point);
}

// properties
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(point__is_null__not_null__false)
{
    BC_REQUIRE(!expected_point.is_null());
}

BC_AUTO_TEST_CASE(point__is_null__default_null__true)
{
    BC_REQUIRE(point{}.is_null());
}

BC_AUTO_TEST_CASE(point__serialized_size__always__expected)
{
    static_assert(point::serialized_size() == hash_size + sizeof(uint32_t));
    BC_REQUIRE_EQUAL(point::serialized_size(), hash_size + sizeof(uint32_t));
}

BC_AUTO_TEST_CASE(point__construct__fast_stream__round_trips)
{
    const point expected{ one_hash, 42 };
    const auto data = expected.to_data();
    stream::in::fast source{ data };
    const point instance{ source };
    BC_REQUIRE(instance == expected);
}

// Constant reference optimizers.

BC_AUTO_TEST_CASE(point__cref_lesser__lesser_index__true)
{
    const point instance1{ one_hash, 1 };
    const point instance2{ one_hash, 2 };
    const point_cref left{ instance1 };
    const point_cref right{ instance2 };
    BC_REQUIRE(left < right);
    BC_REQUIRE(!(right < left));
}

BC_AUTO_TEST_CASE(point__cref_inequality__different__true)
{
    const point instance1{ one_hash, 1 };
    const point instance2{ one_hash, 2 };
    const point_cref left{ instance1 };
    const point_cref right{ instance2 };
    BC_REQUIRE(left != right);
    BC_REQUIRE(!(left != left));
}

BC_AUTO_TEST_SUITE_END()
