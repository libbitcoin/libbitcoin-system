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

BOOST_AUTO_TEST_SUITE(ip_address_tests)

using namespace ip;

// construct

BOOST_AUTO_TEST_CASE(address__construct__default__unspecified_v4)
{
    const address instance{};
    BOOST_REQUIRE(instance.is_v4());
    BOOST_REQUIRE(!instance.is_v6());
    BOOST_REQUIRE(instance.is_unspecified());
    BOOST_REQUIRE_EQUAL(instance.to_v4(), ipv4{});
}

BOOST_AUTO_TEST_CASE(address__construct__ipv4__v4)
{
    const address instance{ ipv4::loopback() };
    BOOST_REQUIRE(instance.is_v4());
    BOOST_REQUIRE_EQUAL(instance.to_v4(), ipv4::loopback());
    BOOST_REQUIRE_EQUAL(instance.to_v6(), ipv6{});
}

BOOST_AUTO_TEST_CASE(address__construct__ipv6__v6)
{
    const address instance{ ipv6::loopback() };
    BOOST_REQUIRE(instance.is_v6());
    BOOST_REQUIRE_EQUAL(instance.to_v6(), ipv6::loopback());
    BOOST_REQUIRE_EQUAL(instance.to_v4(), ipv4{});
}

// from_string

BOOST_AUTO_TEST_CASE(address__from_string__ipv4__v4)
{
    address out{};
    BOOST_REQUIRE(address::from_string(out, "127.0.0.1"));
    BOOST_REQUIRE(out.is_v4());
    BOOST_REQUIRE_EQUAL(out.to_v4(), ipv4::loopback());
}

BOOST_AUTO_TEST_CASE(address__from_string__ipv6__v6)
{
    address out{};
    BOOST_REQUIRE(address::from_string(out, "::1"));
    BOOST_REQUIRE(out.is_v6());
    BOOST_REQUIRE_EQUAL(out.to_v6(), ipv6::loopback());
}

BOOST_AUTO_TEST_CASE(address__from_string__mapped__v6)
{
    address out{};
    BOOST_REQUIRE(address::from_string(out, "::ffff:127.0.0.1"));
    BOOST_REQUIRE(out.is_v6());
    BOOST_REQUIRE_EQUAL(out.to_v6(), ipv6::mapped(ipv4::loopback()));
}

BOOST_AUTO_TEST_CASE(address__from_string__invalid__false_unchanged)
{
    address out{ ipv6::loopback() };
    BOOST_REQUIRE(!address::from_string(out, ""));
    BOOST_REQUIRE(!address::from_string(out, "1.2.3"));
    BOOST_REQUIRE(!address::from_string(out, "1::2::3"));
    BOOST_REQUIRE(!address::from_string(out, "[::1]"));
    BOOST_REQUIRE(!address::from_string(out, "127.0.0.1:42"));
    BOOST_REQUIRE_EQUAL(out, address{ ipv6::loopback() });
}

// to_string

BOOST_AUTO_TEST_CASE(address__to_string__ipv4__dotted)
{
    BOOST_REQUIRE_EQUAL(address{ ipv4::loopback() }.to_string(), "127.0.0.1");
}

BOOST_AUTO_TEST_CASE(address__to_string__ipv6__canonical)
{
    BOOST_REQUIRE_EQUAL(address{ ipv6::loopback() }.to_string(), "::1");
}

// is_loopback

BOOST_AUTO_TEST_CASE(address__is_loopback__loopbacks__true)
{
    BOOST_REQUIRE(address{ ipv4::loopback() }.is_loopback());
    BOOST_REQUIRE(address{ ipv6::loopback() }.is_loopback());
}

BOOST_AUTO_TEST_CASE(address__is_loopback__unspecified__false)
{
    BOOST_REQUIRE(!address{ ipv4{} }.is_loopback());
    BOOST_REQUIRE(!address{ ipv6{} }.is_loopback());
}

// is_unspecified

BOOST_AUTO_TEST_CASE(address__is_unspecified__unspecified__true)
{
    BOOST_REQUIRE(address{ ipv4{} }.is_unspecified());
    BOOST_REQUIRE(address{ ipv6{} }.is_unspecified());
}

BOOST_AUTO_TEST_CASE(address__is_unspecified__loopbacks__false)
{
    BOOST_REQUIRE(!address{ ipv4::loopback() }.is_unspecified());
    BOOST_REQUIRE(!address{ ipv6::loopback() }.is_unspecified());
}

// operators

BOOST_AUTO_TEST_CASE(address__equality__same_bytes_different_family__false)
{
    const address v4{ ipv4{} };
    const address v6{ ipv6{} };
    BOOST_REQUIRE(v4 != v6);
    BOOST_REQUIRE(v4 == address{});
}

BOOST_AUTO_TEST_CASE(address__ordering__v4_before_v6__expected)
{
    const address v4{ ipv4::loopback() };
    const address v6{ ipv6{} };
    BOOST_REQUIRE(v4 < v6);
    BOOST_REQUIRE(address{ ipv4{} } < v4);
}

BOOST_AUTO_TEST_CASE(address__ostream__ipv6__canonical)
{
    std::stringstream stream{};
    stream << address{ ipv6::loopback() };
    BOOST_REQUIRE_EQUAL(stream.str(), "::1");
}

BOOST_AUTO_TEST_SUITE_END()
