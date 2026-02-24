/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <sstream>

BOOST_AUTO_TEST_SUITE(version_tests)

using namespace bc::system::config;
using namespace boost::program_options;

BOOST_AUTO_TEST_CASE(version__construct__default__zero_segments)
{
    const version instance;
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 0u);
    BOOST_REQUIRE_EQUAL(segments[1], 0u);
    BOOST_REQUIRE_EQUAL(segments[2], 0u);
    BOOST_REQUIRE_EQUAL(segments[3], 0u);
    BOOST_REQUIRE(instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__construct__non_default__expected_segments)
{
    const version instance("1.2.3.4");
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 1u);
    BOOST_REQUIRE_EQUAL(segments[1], 2u);
    BOOST_REQUIRE_EQUAL(segments[2], 3u);
    BOOST_REQUIRE_EQUAL(segments[3], 4u);
    BOOST_REQUIRE(!instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__construct__string_invalid_too_few_segments__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(version("1"), istream_exception);
}

BOOST_AUTO_TEST_CASE(version__construct__string_invalid_too_many_segments__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(version("1.2.3.4.5"), istream_exception);
}

BOOST_AUTO_TEST_CASE(version__construct__string_invalid_non_numeric__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(version("1.a"), istream_exception);
}

BOOST_AUTO_TEST_CASE(version__construct__string_invalid_negative__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(version("1.-2"), istream_exception);
}

BOOST_AUTO_TEST_CASE(version__construct__string_invalid_out_of_range__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(version("1.4294967296"), istream_exception);
}

BOOST_AUTO_TEST_CASE(version__construct__segments_two__padded_zeros)
{
    const version instance(1u, 2u);
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 1u);
    BOOST_REQUIRE_EQUAL(segments[1], 2u);
    BOOST_REQUIRE_EQUAL(segments[2], 0u);
    BOOST_REQUIRE_EQUAL(segments[3], 0u);
}

BOOST_AUTO_TEST_CASE(version__construct__segments_three__padded_zero)
{
    const version instance(1u, 2u, 3u);
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 1u);
    BOOST_REQUIRE_EQUAL(segments[1], 2u);
    BOOST_REQUIRE_EQUAL(segments[2], 3u);
    BOOST_REQUIRE_EQUAL(segments[3], 0u);
}

BOOST_AUTO_TEST_CASE(version__construct__segments_four__expected)
{
    const version instance(1u, 2u, 3u, 4u);
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 1u);
    BOOST_REQUIRE_EQUAL(segments[1], 2u);
    BOOST_REQUIRE_EQUAL(segments[2], 3u);
    BOOST_REQUIRE_EQUAL(segments[3], 4u);
}

BOOST_AUTO_TEST_CASE(version__is_default__default__true)
{
    const version instance;
    BOOST_REQUIRE(instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__is_default__non_default__false)
{
    const version instance(1u, 0u);
    BOOST_REQUIRE(!instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__is_default__zero_segments__true)
{
    const version instance(0u, 0u, 0u, 0u);
    BOOST_REQUIRE(instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__is_default__partial_zeros__false)
{
    const version instance(0u, 0u, 1u, 0u);
    BOOST_REQUIRE(!instance.is_default());
}

BOOST_AUTO_TEST_CASE(version__to_string__two_segments__expected)
{
    const version instance(1u, 2u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "1.2");
}

BOOST_AUTO_TEST_CASE(version__to_string__three_segments__expected_no_trailing_zero)
{
    const version instance(1u, 2u, 3u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "1.2.3");
}

BOOST_AUTO_TEST_CASE(version__to_string__four_segments_trailing_zero__omits_trailing_zero)
{
    const version instance(1u, 2u, 3u, 0u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "1.2.3");
}

BOOST_AUTO_TEST_CASE(version__to_string__default__minimum_two_segments)
{
    const version instance;
    BOOST_REQUIRE_EQUAL(instance.to_string(), "0.0");
}

BOOST_AUTO_TEST_CASE(version__operator_shift_right__valid__deserializes)
{
    version instance;
    std::istringstream input("1.2.3");
    input >> instance;
    const auto& segments = instance.segments();
    BOOST_REQUIRE_EQUAL(segments[0], 1u);
    BOOST_REQUIRE_EQUAL(segments[1], 2u);
    BOOST_REQUIRE_EQUAL(segments[2], 3u);
    BOOST_REQUIRE_EQUAL(segments[3], 0u);
}

BOOST_AUTO_TEST_CASE(version__operator_shift_right__invalid__throws_istream_exception)
{
    version instance;
    std::istringstream input("1.a");
    BOOST_REQUIRE_THROW(input >> instance, istream_exception);
}

BOOST_AUTO_TEST_CASE(version__operator_shift_left__valid__serializes)
{
    const version instance(1u, 2u, 3u);
    std::ostringstream output;
    output << instance;
    BOOST_REQUIRE_EQUAL(output.str(), "1.2.3");
}

BOOST_AUTO_TEST_CASE(version__operator_equal__same__true)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 4u);
    BOOST_REQUIRE(a == b);
}

BOOST_AUTO_TEST_CASE(version__operator_equal__different__false)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 5u);
    BOOST_REQUIRE(!(a == b));
}

BOOST_AUTO_TEST_CASE(version__operator_not_equal__same__false)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 4u);
    BOOST_REQUIRE(!(a != b));
}

BOOST_AUTO_TEST_CASE(version__operator_not_equal__different__true)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 5u);
    BOOST_REQUIRE(a != b);
}

BOOST_AUTO_TEST_CASE(version__operator_less__prefix_shorter__true)
{
    const version a(1u, 2u, 3u);
    const version b(1u, 2u, 3u, 1u);
    BOOST_REQUIRE(a < b);
}

BOOST_AUTO_TEST_CASE(version__operator_less__prefix_longer__false)
{
    const version a(1u, 2u, 3u, 1u);
    const version b(1u, 2u, 3u);
    BOOST_REQUIRE(!(a < b));
}

BOOST_AUTO_TEST_CASE(version__operator_less__differing_component__expected)
{
    const version a(1u, 2u, 4u);
    const version b(1u, 3u, 0u);
    BOOST_REQUIRE(a < b);
}

BOOST_AUTO_TEST_CASE(version__operator_less_equal__equal__true)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 4u);
    BOOST_REQUIRE(a <= b);
}

BOOST_AUTO_TEST_CASE(version__operator_less_equal__less__true)
{
    const version a(1u, 2u, 3u);
    const version b(1u, 2u, 3u, 1u);
    BOOST_REQUIRE(a <= b);
}

BOOST_AUTO_TEST_CASE(version__operator_less_equal__greater__false)
{
    const version a(1u, 2u, 3u, 1u);
    const version b(1u, 2u, 3u);
    BOOST_REQUIRE(!(a <= b));
}

BOOST_AUTO_TEST_CASE(version__operator_greater__prefix_longer__true)
{
    const version a(1u, 2u, 3u, 1u);
    const version b(1u, 2u, 3u);
    BOOST_REQUIRE(a > b);
}

BOOST_AUTO_TEST_CASE(version__operator_greater__prefix_shorter__false)
{
    const version a(1u, 2u, 3u);
    const version b(1u, 2u, 3u, 1u);
    BOOST_REQUIRE(!(a > b));
}

BOOST_AUTO_TEST_CASE(version__operator_greater_equal__equal__true)
{
    const version a(1u, 2u, 3u, 4u);
    const version b(1u, 2u, 3u, 4u);
    BOOST_REQUIRE(a >= b);
}

BOOST_AUTO_TEST_CASE(version__operator_greater_equal__greater__true)
{
    const version a(1u, 2u, 3u, 1u);
    const version b(1u, 2u, 3u);
    BOOST_REQUIRE(a >= b);
}

BOOST_AUTO_TEST_CASE(version__operator_greater_equal__less__false)
{
    const version a(1u, 2u, 3u);
    const version b(1u, 2u, 3u, 1u);
    BOOST_REQUIRE(!(a >= b));
}

BOOST_AUTO_TEST_SUITE_END()
