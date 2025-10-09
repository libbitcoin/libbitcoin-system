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
#include <sstream>

BOOST_AUTO_TEST_SUITE(endpoint_tests)

using namespace config;
using namespace boost::program_options;

// construct

BOOST_AUTO_TEST_CASE(endpoint__construct__empty__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(endpoint instance(""), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__question_mark__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint instance("foo.bar?foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__forward_slash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint instance("foo.bar/foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__backslash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint instance("foo.bar\\foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host_colon__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint instance("foo.bar:foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__ipv6_non_literal__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint instance("a::bc:def::123:45:6"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__default__localhost)
{
    endpoint instance{};
    BOOST_REQUIRE_EQUAL(instance.host(), "localhost");
    BOOST_REQUIRE_EQUAL(instance.port(), 0u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "localhost");
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "localhost");
    BOOST_REQUIRE_EQUAL(instance.to_string(42), "localhost:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(), "localhost");
    BOOST_REQUIRE_EQUAL(instance.to_lower(0), "localhost");
    BOOST_REQUIRE_EQUAL(instance.to_lower(42), "localhost:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host__expected_values)
{
    endpoint instance("FOO");
    BOOST_REQUIRE_EQUAL(instance.host(), "FOO");
    BOOST_REQUIRE_EQUAL(instance.port(), 0u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "FOO");
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "FOO");
    BOOST_REQUIRE_EQUAL(instance.to_string(42), "FOO:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(), "foo");
    BOOST_REQUIRE_EQUAL(instance.to_lower(0), "foo");
    BOOST_REQUIRE_EQUAL(instance.to_lower(42), "foo:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host_port__expected_values)
{
    endpoint instance("foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.host(), "foo.bar");
    BOOST_REQUIRE_EQUAL(instance.port(), 42u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(80), "foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(), "foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(0), "foo.bar:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(80), "foo.bar:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__ipv4_port__expected_values)
{
    endpoint instance("127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.host(), "127.0.0.1");
    BOOST_REQUIRE_EQUAL(instance.port(), 42u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(80), "127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(), "127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(0), "127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(80), "127.0.0.1:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__ipv6_port__expected_values)
{
    // Requires lower case hex.
    endpoint instance("[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.host(), "[a::bc:def::123:45:6]");
    BOOST_REQUIRE_EQUAL(instance.port(), 42u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.to_string(80), "[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(), "[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(0), "[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(instance.to_lower(80), "[a::bc:def::123:45:6]:42");
}

// to_local

BOOST_AUTO_TEST_CASE(endpoint__to_local__host_port__expected_values)
{
    endpoint original("*:12345");
    const auto instance = original.to_local();
    BOOST_REQUIRE_EQUAL(instance.host(), "localhost");
    BOOST_REQUIRE_EQUAL(instance.port(), 12345u);
    BOOST_REQUIRE_EQUAL(instance.to_string(), "localhost:12345");
    BOOST_REQUIRE_EQUAL(original.to_string(), "*:12345");
    BOOST_REQUIRE_EQUAL(original.to_string(0), "*:12345");
    BOOST_REQUIRE_EQUAL(original.to_string(80), "*:12345");
    BOOST_REQUIRE_EQUAL(original.to_lower(), "*:12345");
    BOOST_REQUIRE_EQUAL(original.to_lower(0), "*:12345");
    BOOST_REQUIRE_EQUAL(original.to_lower(80), "*:12345");
}

// equality

BOOST_AUTO_TEST_CASE(endpoint__equality__default_default__true)
{
    const endpoint host1{};
    const endpoint host2{};
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(endpoint__equality__distinct__false)
{
    const endpoint host1{};
    const endpoint host2("*:12345");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality__distinct_port__false)
{
    const endpoint host1("foo.bar:12345");
    const endpoint host2("foo.bar:1234");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality__same__true)
{
    const endpoint host1("*:12345");
    const endpoint host2("*:12345");
    BOOST_REQUIRE(host1 == host2);
}

// inequality

BOOST_AUTO_TEST_CASE(endpoint__inequality__distinct__true)
{
    const endpoint host1{};
    const endpoint host2("*:12345");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality__distinct_port__true)
{
    const endpoint host1("foo.bar:12345");
    const endpoint host2("foo.bar:1234");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality__same__false)
{
    const endpoint host1("*:12345");
    const endpoint host2("*:12345");
    BOOST_REQUIRE(!(host1 != host2));
}

// equality

BOOST_AUTO_TEST_CASE(endpoint__equality1__default_default__true)
{
    const endpoint host1{};
    const endpoint host2{};
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(endpoint__equality1__distinct_port__false)
{
    const endpoint host1("42.42.42.42:12345");
    const endpoint host2("42.42.42.42:1234");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality1__same__true)
{
    const endpoint host1("127.0.0.1:12345");
    const endpoint host2("127.0.0.1:12345");
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(endpoint__equality1__same_v6__true)
{
    const endpoint host1("[abcd:abcd::abcd:abcd]:12345");
    const endpoint host2("[abcd:abcd::abcd:abcd]:12345");
    BOOST_REQUIRE(host1 == host2);
}

// inequality

BOOST_AUTO_TEST_CASE(endpoint__inequality1__distinct_port__true)
{
    const endpoint host1("42.42.42.42:12345");
    const endpoint host2("42.42.42.42:1234");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality1__same__false)
{
    const endpoint host1("127.0.0.1:12345");
    const endpoint host2("127.0.0.1:12345");
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_CASE(endpoint__inequality1__same_v6__false)
{
    const endpoint host1("[abcd:abcd::abcd:abcd]:12345");
    const endpoint host2("[abcd:abcd::abcd:abcd]:12345");
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_SUITE_END()
