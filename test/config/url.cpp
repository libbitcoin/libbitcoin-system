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
#include <sstream>

BC_AUTO_TEST_SUITE(url_tests)

using namespace config;
// construct

BC_AUTO_TEST_CASE(url__construct__empty__throws_invalid_option)
{
    BC_REQUIRE_THROW(url instance(""), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__no_host__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__port_only__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance(":42"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__question_mark__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://foo.bar?foobar:42"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__forward_slash__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://foo.bar/foobar:42"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__backslash__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://foo.bar\\foobar:42"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__host_colon__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://foo.bar:foobar:42"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__ipv6_non_literal__throws_invalid_option_value)
{
    BC_REQUIRE_THROW(url instance("tcp://a::bc:def::123:45:6"), istream_exception);
}

BC_AUTO_TEST_CASE(url__construct__default__localhost)
{
    url instance{};
    BC_REQUIRE_EQUAL(instance.scheme(), "");
    BC_REQUIRE_EQUAL(instance.host(), "localhost");
    BC_REQUIRE_EQUAL(instance.port(), 0u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "localhost");
}

BC_AUTO_TEST_CASE(url__construct__host__expected_values)
{
    url instance("foo");
    BC_REQUIRE_EQUAL(instance.scheme(), "");
    BC_REQUIRE_EQUAL(instance.host(), "foo");
    BC_REQUIRE_EQUAL(instance.port(), 0u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "foo");
}

BC_AUTO_TEST_CASE(url__construct__host_port__expected_values)
{
    url instance("foo.bar:42");
    BC_REQUIRE_EQUAL(instance.scheme(), "");
    BC_REQUIRE_EQUAL(instance.host(), "foo.bar");
    BC_REQUIRE_EQUAL(instance.port(), 42u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "foo.bar:42");
}

BC_AUTO_TEST_CASE(url__construct__scheme_host__expected_values)
{
    url instance("tcp://foo.bar");
    BC_REQUIRE_EQUAL(instance.scheme(), "tcp");
    BC_REQUIRE_EQUAL(instance.host(), "foo.bar");
    BC_REQUIRE_EQUAL(instance.port(), 0u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "tcp://foo.bar");
}

BC_AUTO_TEST_CASE(url__construct__scheme_host_port__expected_values)
{
    url instance("tcp://foo.bar:42");
    BC_REQUIRE_EQUAL(instance.scheme(), "tcp");
    BC_REQUIRE_EQUAL(instance.host(), "foo.bar");
    BC_REQUIRE_EQUAL(instance.port(), 42u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "tcp://foo.bar:42");
}

BC_AUTO_TEST_CASE(url__construct__scheme_ipv4_port__expected_values)
{
    url instance("tcp://127.0.0.1:42");
    BC_REQUIRE_EQUAL(instance.scheme(), "tcp");
    BC_REQUIRE_EQUAL(instance.host(), "127.0.0.1");
    BC_REQUIRE_EQUAL(instance.port(), 42u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "tcp://127.0.0.1:42");
}

BC_AUTO_TEST_CASE(url__construct__scheme_ipv6_port__expected_values)
{
    url instance("tcp://[a::bc:def::123:45:6]:42");
    BC_REQUIRE_EQUAL(instance.scheme(), "tcp");
    BC_REQUIRE_EQUAL(instance.host(), "[a::bc:def::123:45:6]");
    BC_REQUIRE_EQUAL(instance.port(), 42u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "tcp://[a::bc:def::123:45:6]:42");
}

// to_local

BC_AUTO_TEST_CASE(url__to_local__scheme_host_port__expected_values)
{
    url original("tcp://*:12345");
    const auto instance = original.to_local();
    BC_REQUIRE_EQUAL(instance.scheme(), "tcp");
    BC_REQUIRE_EQUAL(instance.host(), "localhost");
    BC_REQUIRE_EQUAL(instance.port(), 12345u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "tcp://localhost:12345");
    BC_REQUIRE_EQUAL(original.to_uri(), "tcp://*:12345");
}

BC_AUTO_TEST_CASE(url__to_local__host_port__expected_values)
{
    url original("*:12345");
    const auto instance = original.to_local();
    BC_REQUIRE_EQUAL(instance.scheme(), "");
    BC_REQUIRE_EQUAL(instance.host(), "localhost");
    BC_REQUIRE_EQUAL(instance.port(), 12345u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "localhost:12345");
    BC_REQUIRE_EQUAL(original.to_uri(), "*:12345");
}

// equality

BC_AUTO_TEST_CASE(url__equality__default_default__true)
{
    const url instance1{};
    const url instance2{};
    BC_REQUIRE(instance1 == instance2);
}

BC_AUTO_TEST_CASE(url__equality__distinct__false)
{
    const url instance1{};
    const url instance2("tcp://*:12345");
    BC_REQUIRE(!(instance1 == instance2));
}

BC_AUTO_TEST_CASE(url__equality__distinct_port__false)
{
    const url instance1("tcp://foo.bar:12345");
    const url instance2("tcp://foo.bar:1234");
    BC_REQUIRE(!(instance1 == instance2));
}

BC_AUTO_TEST_CASE(url__equality__same__true)
{
    const url instance1("tcp://*:12345");
    const url instance2("tcp://*:12345");
    BC_REQUIRE(instance1 == instance2);
}

// inequality

BC_AUTO_TEST_CASE(url__inequality__distinct__true)
{
    const url instance1{};
    const url instance2("tcp://*:12345");
    BC_REQUIRE(instance1 != instance2);
}

BC_AUTO_TEST_CASE(url__inequality__distinct_port__true)
{
    const url instance1("tcp://foo.bar:12345");
    const url instance2("tcp://foo.bar:1234");
    BC_REQUIRE(instance1 != instance2);
}

BC_AUTO_TEST_CASE(url__inequality__same__false)
{
    const url instance1("tcp://*:12345");
    const url instance2("tcp://*:12345");
    BC_REQUIRE(!(instance1 != instance2));
}

// equality

BC_AUTO_TEST_CASE(url__equality1__default_default__true)
{
    const url instance1{};
    const url instance2{};
    BC_REQUIRE(instance1 == instance2);
}

BC_AUTO_TEST_CASE(url__equality1__distinct_port__false)
{
    const url instance1("tcp://42.42.42.42:12345");
    const url instance2("tcp://42.42.42.42:1234");
    BC_REQUIRE(!(instance1 == instance2));
}

BC_AUTO_TEST_CASE(url__equality1__same__true)
{
    const url instance1("tcp://127.0.0.1:12345");
    const url instance2("tcp://127.0.0.1:12345");
    BC_REQUIRE(instance1 == instance2);
}

BC_AUTO_TEST_CASE(url__equality1__same_v6__true)
{
    const url instance1("tcp://[abcd:abcd::abcd:abcd]:12345");
    const url instance2("tcp://[abcd:abcd::abcd:abcd]:12345");
    BC_REQUIRE(instance1 == instance2);
}

// inequality

BC_AUTO_TEST_CASE(url__inequality1__distinct_port__true)
{
    const url instance1("tcp://42.42.42.42:12345");
    const url instance2("tcp://42.42.42.42:1234");
    BC_REQUIRE(instance1 != instance2);
}

BC_AUTO_TEST_CASE(url__inequality1__same__false)
{
    const url instance1("tcp://127.0.0.1:12345");
    const url instance2("tcp://127.0.0.1:12345");
    BC_REQUIRE(!(instance1 != instance2));
}

BC_AUTO_TEST_CASE(url__inequality1__same_v6__false)
{
    const url instance1("tcp://[abcd:abcd::abcd:abcd]:12345");
    const url instance2("tcp://[abcd:abcd::abcd:abcd]:12345");
    BC_REQUIRE(!(instance1 != instance2));
}

BC_AUTO_TEST_CASE(url__construct__host_port__expected)
{
    const config::url instance("libbitcoin.org", 42);
    BC_REQUIRE_EQUAL(instance.host(), "libbitcoin.org");
    BC_REQUIRE_EQUAL(instance.port(), 42u);
    BC_REQUIRE_EQUAL(instance.to_uri(), "libbitcoin.org:42");
}

BC_AUTO_TEST_CASE(url__construct__host_zero_port__no_port_in_authority)
{
    const config::url instance("libbitcoin.org", 0);
    BC_REQUIRE_EQUAL(instance.to_uri(), "libbitcoin.org");
}

BC_AUTO_TEST_CASE(url__construct__authority__expected)
{
    const config::authority authority("42.42.42.42:42");
    const config::url instance(authority);
    BC_REQUIRE_EQUAL(instance.port(), 42u);
}

BC_AUTO_TEST_CASE(url__construct__ip_endpoint__expected)
{
    const config::authority authority("42.42.42.42:42");
    const config::url instance(authority.to_endpoint());
    BC_REQUIRE_EQUAL(instance.port(), 42u);
}

BC_AUTO_TEST_CASE(url__construct__ip_address__expected)
{
    const config::authority authority("42.42.42.42:42");
    const config::url instance(authority.ip(), 42);
    BC_REQUIRE_EQUAL(instance.port(), 42u);
}

BC_AUTO_TEST_SUITE_END()
