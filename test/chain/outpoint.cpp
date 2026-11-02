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

BC_AUTO_TEST_SUITE(outpoint_tests)

namespace json = system::json;
using namespace system::chain;

static const auto outpoint_hash = base16_hash(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");

static const auto outpoint_data = base16_chunk(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f000000150000000000000042");

static const outpoint expected_outpoint{ outpoint_data };

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BC_AUTO_TEST_CASE(outpoint__constructor__default__invalid)
{
    const outpoint instance;
    BC_REQUIRE(!instance.is_valid());
}

BC_AUTO_TEST_CASE(outpoint__constructor__move__valid)
{
    auto copy = expected_outpoint;
    const outpoint instance{ std::move(copy) };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_outpoint);
}

BC_AUTO_TEST_CASE(outpoint__constructor__copy__valid)
{
    const outpoint instance{ expected_outpoint };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_outpoint);
}

BC_AUTO_TEST_CASE(outpoint__constructor__move_parameters__valid)
{
    constexpr uint32_t value{ 42 };
    constexpr uint32_t index{ 1234 };
    outpoint instance{ { outpoint_hash, index }, value };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.point().hash(), outpoint_hash);
    BC_REQUIRE_EQUAL(instance.point().index(), index);
    BC_REQUIRE_EQUAL(instance.value(), value);
}

BC_AUTO_TEST_CASE(outpoint__constructor__copy_parameters__valid)
{
    constexpr uint32_t value{ 42 };
    constexpr uint32_t index{ 1234 };
    point copy{ outpoint_hash, index };
    outpoint instance{ std::move(copy), value };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.point().hash(), outpoint_hash);
    BC_REQUIRE_EQUAL(instance.point().index(), index);
    BC_REQUIRE_EQUAL(instance.value(), value);
}

// operators
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(outpoint__assign__copy__expected)
{
    const auto& alpha = expected_outpoint;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(outpoint__assign__move__expected)
{
    const auto& alpha = expected_outpoint;
    const auto beta = alpha;
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(outpoint__equality__same__true)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta(alpha);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(outpoint__equality__different__false)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta;
    BC_REQUIRE(!(alpha == beta));
}

BC_AUTO_TEST_CASE(outpoint__inequality__same__false)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta(alpha);
    BC_REQUIRE(!(alpha != beta));
}

BC_AUTO_TEST_CASE(outpoint__inequality__different__true)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta;
    BC_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(outpoint__to_data__data__expected)
{
    const auto size = expected_outpoint.to_data().size();
    BC_REQUIRE_EQUAL(size, expected_outpoint.serialized_size());
}

BC_AUTO_TEST_CASE(outpoint__to_data__stream__expected)
{
    // Write outpoint to stream.
    std::stringstream iostream;
    expected_outpoint.to_data(iostream);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const outpoint copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_outpoint);
}

BC_AUTO_TEST_CASE(outpoint__to_data__writer__expected)
{
    // Write outpoint to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_outpoint.to_data(out);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const outpoint copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_outpoint);
}

// properties
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(outpoint__is_null__not_null__false)
{
    BC_REQUIRE(!expected_outpoint.is_null());
}

BC_AUTO_TEST_CASE(outpoint__is_null__default_null__true)
{
    BC_REQUIRE(outpoint{}.is_null());
}

BC_AUTO_TEST_CASE(outpoint__serialized_size__always__expected)
{
    static_assert(outpoint::serialized_size() == point::serialized_size() + sizeof(uint64_t));
    BC_REQUIRE_EQUAL(outpoint::serialized_size(), point::serialized_size() + sizeof(uint64_t));
}

BC_AUTO_TEST_CASE(outpoint__construct__copy_point__expected)
{
    const point value{ one_hash, 42 };
    const outpoint instance{ value, 99 };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.point() == value);
    BC_REQUIRE_EQUAL(instance.value(), 99u);
}

BC_AUTO_TEST_CASE(outpoint__construct__fast_stream__round_trips)
{
    const outpoint expected{ point{ one_hash, 42 }, 99 };
    const auto data = expected.to_data();
    stream::in::fast source{ data };
    const outpoint instance{ source };
    BC_REQUIRE(instance == expected);
    BC_REQUIRE_EQUAL(instance.value(), 99u);
}

BC_AUTO_TEST_CASE(outpoint__lesser__lesser_point__true)
{
    const outpoint instance1{ point{ one_hash, 1 }, 99 };
    const outpoint instance2{ point{ one_hash, 2 }, 0 };
    BC_REQUIRE(instance1 < instance2);
    BC_REQUIRE(!(instance2 < instance1));
}

BC_AUTO_TEST_SUITE_END()
