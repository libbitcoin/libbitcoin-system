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

BOOST_AUTO_TEST_SUITE(ip_endpoint_tests)

using namespace ip;

BOOST_AUTO_TEST_CASE(endpoint__construct__default__unspecified_v4_zero_port)
{
    const ip::endpoint instance{};
    BOOST_REQUIRE_EQUAL(instance.address(), ip::address{});
    BOOST_REQUIRE_EQUAL(instance.port(), 0u);
}

BOOST_AUTO_TEST_CASE(endpoint__construct__address_port__expected)
{
    const ip::address address{ ipv6::loopback() };
    const ip::endpoint instance{ address, 42 };
    BOOST_REQUIRE_EQUAL(instance.address(), address);
    BOOST_REQUIRE_EQUAL(instance.port(), 42u);
}

BOOST_AUTO_TEST_CASE(endpoint__equality__address_and_port__expected)
{
    const ip::address address{ ipv4::loopback() };
    const ip::endpoint instance{ address, 42 };
    BOOST_REQUIRE(instance == ip::endpoint(address, 42));
    BOOST_REQUIRE(instance != ip::endpoint(address, 43));
    BOOST_REQUIRE(instance != ip::endpoint(ipv6::loopback(), 42));
}

BOOST_AUTO_TEST_CASE(endpoint__ordering__address_then_port__expected)
{
    const ip::address address{ ipv4::loopback() };
    BOOST_REQUIRE(ip::endpoint(address, 42) < ip::endpoint(address, 43));
    BOOST_REQUIRE(ip::endpoint(address, 43) < ip::endpoint(ipv6{}, 0));
}

BOOST_AUTO_TEST_SUITE_END()
