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
    BOOST_REQUIRE_THROW(endpoint url(""), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__no_host__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__port_only__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host(":42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__question_mark__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://foo.bar?foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__forward_slash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://foo.bar/foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__backslash__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://foo.bar\\foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host_colon__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://foo.bar:foobar:42"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__ipv6_non_literal__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(endpoint host("tcp://a::bc:def::123:45:6"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__default__localhost)
{
    endpoint host{};
    BOOST_REQUIRE_EQUAL(host.scheme(), "");
    BOOST_REQUIRE_EQUAL(host.host(), "localhost");
    BOOST_REQUIRE_EQUAL(host.port(), 0u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "localhost");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host__expected_values)
{
    endpoint host("foo");
    BOOST_REQUIRE_EQUAL(host.scheme(), "");
    BOOST_REQUIRE_EQUAL(host.host(), "foo");
    BOOST_REQUIRE_EQUAL(host.port(), 0u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "foo");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__host_port__expected_values)
{
    endpoint endpoint("foo.bar:42");
    BOOST_REQUIRE_EQUAL(endpoint.scheme(), "");
    BOOST_REQUIRE_EQUAL(endpoint.host(), "foo.bar");
    BOOST_REQUIRE_EQUAL(endpoint.port(), 42u);
    BOOST_REQUIRE_EQUAL(endpoint.to_uri(), "foo.bar:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__scheme_host__expected_values)
{
    endpoint host("tcp://foo.bar");
    BOOST_REQUIRE_EQUAL(host.scheme(), "tcp");
    BOOST_REQUIRE_EQUAL(host.host(), "foo.bar");
    BOOST_REQUIRE_EQUAL(host.port(), 0u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "tcp://foo.bar");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__scheme_host_port__expected_values)
{
    endpoint host("tcp://foo.bar:42");
    BOOST_REQUIRE_EQUAL(host.scheme(), "tcp");
    BOOST_REQUIRE_EQUAL(host.host(), "foo.bar");
    BOOST_REQUIRE_EQUAL(host.port(), 42u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "tcp://foo.bar:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__scheme_ipv4_port__expected_values)
{
    endpoint host("tcp://127.0.0.1:42");
    BOOST_REQUIRE_EQUAL(host.scheme(), "tcp");
    BOOST_REQUIRE_EQUAL(host.host(), "127.0.0.1");
    BOOST_REQUIRE_EQUAL(host.port(), 42u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "tcp://127.0.0.1:42");
}

BOOST_AUTO_TEST_CASE(endpoint__construct__scheme_ipv6_port__expected_values)
{
    endpoint host("tcp://[a::bc:def::123:45:6]:42");
    BOOST_REQUIRE_EQUAL(host.scheme(), "tcp");
    BOOST_REQUIRE_EQUAL(host.host(), "[a::bc:def::123:45:6]");
    BOOST_REQUIRE_EQUAL(host.port(), 42u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "tcp://[a::bc:def::123:45:6]:42");
}

// to_local

BOOST_AUTO_TEST_CASE(endpoint__to_local__scheme_host_port__expected_values)
{
    endpoint original("tcp://*:12345");
    const auto host = original.to_local();
    BOOST_REQUIRE_EQUAL(host.scheme(), "tcp");
    BOOST_REQUIRE_EQUAL(host.host(), "localhost");
    BOOST_REQUIRE_EQUAL(host.port(), 12345u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "tcp://localhost:12345");
    BOOST_REQUIRE_EQUAL(original.to_uri(), "tcp://*:12345");
}

BOOST_AUTO_TEST_CASE(endpoint__to_local__host_port__expected_values)
{
    endpoint original("*:12345");
    const auto host = original.to_local();
    BOOST_REQUIRE_EQUAL(host.scheme(), "");
    BOOST_REQUIRE_EQUAL(host.host(), "localhost");
    BOOST_REQUIRE_EQUAL(host.port(), 12345u);
    BOOST_REQUIRE_EQUAL(host.to_uri(), "localhost:12345");
    BOOST_REQUIRE_EQUAL(original.to_uri(), "*:12345");
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
    const endpoint host2("tcp://*:12345");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality__distinct_port__false)
{
    const endpoint host1("tcp://foo.bar:12345");
    const endpoint host2("tcp://foo.bar:1234");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality__same__true)
{
    const endpoint host1("tcp://*:12345");
    const endpoint host2("tcp://*:12345");
    BOOST_REQUIRE(host1 == host2);
}

// inequality

BOOST_AUTO_TEST_CASE(endpoint__inequality__distinct__true)
{
    const endpoint host1{};
    const endpoint host2("tcp://*:12345");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality__distinct_port__true)
{
    const endpoint host1("tcp://foo.bar:12345");
    const endpoint host2("tcp://foo.bar:1234");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality__same__false)
{
    const endpoint host1("tcp://*:12345");
    const endpoint host2("tcp://*:12345");
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
    const endpoint host1("tcp://42.42.42.42:12345");
    const endpoint host2("tcp://42.42.42.42:1234");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(endpoint__equality1__same__true)
{
    const endpoint host1("tcp://127.0.0.1:12345");
    const endpoint host2("tcp://127.0.0.1:12345");
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(endpoint__equality1__same_v6__true)
{
    const endpoint host1("tcp://[abcd:abcd::abcd:abcd]:12345");
    const endpoint host2("tcp://[abcd:abcd::abcd:abcd]:12345");
    BOOST_REQUIRE(host1 == host2);
}

// inequality

BOOST_AUTO_TEST_CASE(endpoint__inequality1__distinct_port__true)
{
    const endpoint host1("tcp://42.42.42.42:12345");
    const endpoint host2("tcp://42.42.42.42:1234");
    BOOST_REQUIRE(host1 != host2);
}

BOOST_AUTO_TEST_CASE(endpoint__inequality1__same__false)
{
    const endpoint host1("tcp://127.0.0.1:12345");
    const endpoint host2("tcp://127.0.0.1:12345");
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_CASE(endpoint__inequality1__same_v6__false)
{
    const endpoint host1("tcp://[abcd:abcd::abcd:abcd]:12345");
    const endpoint host2("tcp://[abcd:abcd::abcd:abcd]:12345");
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_SUITE_END()
