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

BOOST_AUTO_TEST_SUITE(outpoint_tests)

namespace json = boost::json;
using namespace system::chain;

static const auto outpoint_hash = base16_hash(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");

static const auto outpoint_data = base16_chunk(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f000000150000000000000042");

static const outpoint expected_outpoint{ outpoint_data };

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(output__constructor__default__invalid)
{
    const outpoint instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(output__constructor__move__valid)
{
    auto copy = expected_outpoint;
    const outpoint instance{ std::move(copy) };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_outpoint);
}

BOOST_AUTO_TEST_CASE(output__constructor__copy__valid)
{
    const outpoint instance{ expected_outpoint };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_outpoint);
}

BOOST_AUTO_TEST_CASE(output__constructor__move_parameters__valid)
{
    constexpr uint32_t value{ 42 };
    constexpr uint32_t index{ 1234 };
    outpoint instance{ { outpoint_hash, index }, value };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.point().hash(), outpoint_hash);
    BOOST_REQUIRE_EQUAL(instance.point().index(), index);
    BOOST_REQUIRE_EQUAL(instance.value(), value);
}

BOOST_AUTO_TEST_CASE(output__constructor__copy_parameters__valid)
{
    constexpr uint32_t value{ 42 };
    constexpr uint32_t index{ 1234 };
    point copy{ outpoint_hash, index };
    outpoint instance{ std::move(copy), value };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.point().hash(), outpoint_hash);
    BOOST_REQUIRE_EQUAL(instance.point().index(), index);
    BOOST_REQUIRE_EQUAL(instance.value(), value);
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(outpoint__assign__copy__expected)
{
    const auto& alpha = expected_outpoint;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(outpoint__assign__move__expected)
{
    const auto& alpha = expected_outpoint;
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(outpoint__equality__same__true)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(outpoint__equality__different__false)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(outpoint__inequality__same__false)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(outpoint__inequality__different__true)
{
    const auto& alpha = expected_outpoint;
    const outpoint beta;
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(outpoint__to_data__data__expected)
{
    const auto size = expected_outpoint.to_data().size();
    BOOST_REQUIRE_EQUAL(size, expected_outpoint.serialized_size());
}

BOOST_AUTO_TEST_CASE(outpoint__to_data__stream__expected)
{
    // Write outpoint to stream.
    std::stringstream iostream;
    expected_outpoint.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const outpoint copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_outpoint);
}

BOOST_AUTO_TEST_CASE(outpoint__to_data__writer__expected)
{
    // Write outpoint to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_outpoint.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const outpoint copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_outpoint);
}

// properties
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(outpoint__is_null__not_null__false)
{
    BOOST_REQUIRE(!expected_outpoint.is_null());
}

BOOST_AUTO_TEST_CASE(outpoint__is_null__default_null__true)
{
    BOOST_REQUIRE(outpoint{}.is_null());
}

BOOST_AUTO_TEST_CASE(outpoint__serialized_size__always__expected)
{
    static_assert(outpoint::serialized_size() == point::serialized_size() + sizeof(uint64_t));
    BOOST_REQUIRE_EQUAL(outpoint::serialized_size(), point::serialized_size() + sizeof(uint64_t));
}

// json
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(outpoint__json__conversions__expected)
{
    const std::string text
    {
        "{"
            "\"point\":"
            "{"
                "\"hash\":\"0000000000000000000000000000000000000000000000000000000000000001\","
                "\"index\":42"
            "},"
            "\"value\":24"
        "}"
    };

    const chain::outpoint instance
    {
        chain::point{ one_hash, 42 },
        24
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::outpoint>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
