/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <sstream>
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::config;
using namespace boost::asio;

BOOST_AUTO_TEST_SUITE(authority_tests)

// tools.ietf.org/html/rfc4291#section-2.2
#define BC_AUTHORITY_IPV4_ADDRESS "1.2.240.1"
#define BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "::"
#define BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "2001:db8::2"
#define BC_AUTHORITY_IPV6_COMPATIBLE_ADDRESS "::0102:f001"
#define BC_AUTHORITY_IPV6_ALTERNATIVE_COMPATIBLE_ADDRESS "::1.2.240.1"

// tools.ietf.org/html/rfc4291#section-2.5.2
BC_CONSTEXPR ip_address_type unspecified_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// tools.ietf.org/html/rfc4291#section-2.5.5.2
BC_CONSTEXPR ip_address_type mapped_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x01, 0x02, 0xf0, 0x01
};

// tools.ietf.org/html/rfc4291#section-2.5.5.1
BC_CONSTEXPR ip_address_type compatible_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0xf0, 0x01
};

BC_CONSTEXPR ip_address_type ipv6_address =
{
    0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
};

static bool ip_equal(const ip_address_type& left,
    const ip_address_type& right)
{
    return std::equal(left.begin(), left.end(), right.begin());
}

static bool net_equal(const network_address_type& left,
    const network_address_type& right)
{
    return ip_equal(left.ip, right.ip) && (left.port == right.port);
}

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(authority__port)

BOOST_AUTO_TEST_CASE(authority__port__default__zero)
{
    const authority host;
    BOOST_REQUIRE_EQUAL(host.port(), 0u);
}

BOOST_AUTO_TEST_CASE(authority__port__copy__expected)
{
    const uint16_t expected_port = 42;
    const authority other(ipv6_address, expected_port);
    const authority host(other);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__ipv4_authority__expected)
{
    const uint16_t expected_port = 42;
    std::stringstream address;
    address << BC_AUTHORITY_IPV4_ADDRESS ":" << expected_port;
    const authority host(address.str());
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__ipv6_authority__expected)
{
    const uint16_t expected_port = 42;
    std::stringstream address;
    address << "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]:" << expected_port;
    const authority host(address.str());
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__network_address__expected)
{
    const uint16_t expected_port = 42;
    const network_address_type address
    {
        0, 0, ipv6_address, expected_port
    };

    const authority host(address);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__ip_address__expected)
{
    const uint16_t expected_port = 42;
    const authority host(ipv6_address, expected_port);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__hostname__expected)
{
    const uint16_t expected_port = 42;
    const authority host(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS, expected_port);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__boost_address__expected)
{
    const uint16_t expected_port = 42;
    const auto address = ip::address::from_string(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
    const authority host(address, expected_port);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_CASE(authority__port__boost_endpoint__expected)
{
    const uint16_t expected_port = 42;
    const auto address = ip::address::from_string(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
    ip::tcp::endpoint tcp_endpoint(address, expected_port);
    const authority host(tcp_endpoint);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(authority__ip)

BOOST_AUTO_TEST_CASE(authority__ip__default__unspecified)
{
    const authority host;
    BOOST_REQUIRE(ip_equal(host.ip(), unspecified_ip_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__copy__expected)
{
    const auto& expected_ip = ipv6_address;
    const authority other(expected_ip, 42);
    const authority host(other);
    BOOST_REQUIRE(ip_equal(host.ip(), expected_ip));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv4_authority__expected)
{
    const authority host(BC_AUTHORITY_IPV4_ADDRESS ":42");
    BOOST_REQUIRE(ip_equal(host.ip(), mapped_ip_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv6_authority__expected)
{
    const authority host("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]:42");
    BOOST_REQUIRE(ip_equal(host.ip(), ipv6_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv6_compatible_authority__expected)
{
    // BC_AUTHORITY_IPV6_COMPATIBLE_ADDRESS A|B variants are equivalent.
    const authority host("[" BC_AUTHORITY_IPV6_COMPATIBLE_ADDRESS "]:42");
    BOOST_REQUIRE(ip_equal(host.ip(), compatible_ip_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv6_compatible_alternative_authority__expected)
{
    // BC_AUTHORITY_IPV6_COMPATIBLE_ADDRESS A|B variants are equivalent.
    const authority host("[" BC_AUTHORITY_IPV6_ALTERNATIVE_COMPATIBLE_ADDRESS "]:42");
    BOOST_REQUIRE(ip_equal(host.ip(), compatible_ip_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__network_address__expected)
{
    const auto& expected_ip = ipv6_address;
    const network_address_type address
    {
        0, 0, ipv6_address, 42
    };

    const authority host(address);
    BOOST_REQUIRE(ip_equal(host.ip(), ipv6_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ip_address__expected)
{
    const auto& expected_ip = ipv6_address;
    const authority host(expected_ip, 42);
    BOOST_REQUIRE(ip_equal(host.ip(), expected_ip));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv4_hostname__expected)
{
    const authority host(BC_AUTHORITY_IPV4_ADDRESS, 42);
    BOOST_REQUIRE(ip_equal(host.ip(), mapped_ip_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv6_host__expected)
{
    const authority host(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS, 42);
    BOOST_REQUIRE(ip_equal(host.ip(), ipv6_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__ipv6_hostname__expected)
{
    const authority host("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]", 42);
    BOOST_REQUIRE(ip_equal(host.ip(), ipv6_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__boost_address__expected)
{
    const auto address = ip::address::from_string(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
    const authority host(address, 42);
    BOOST_REQUIRE(ip_equal(host.ip(), ipv6_address));
}

BOOST_AUTO_TEST_CASE(authority__ip__boost_endpoint__expected)
{
    const auto address = ip::address::from_string(BC_AUTHORITY_IPV4_ADDRESS);
    ip::tcp::endpoint tcp_endpoint(address, 42);
    const authority host(tcp_endpoint);
    BOOST_REQUIRE(ip_equal(host.ip(), mapped_ip_address));
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(authority__to_hostname)

BOOST_AUTO_TEST_CASE(authority__to_hostname__default__ipv6_unspecified)
{
    const authority host;
    BOOST_REQUIRE_EQUAL(host.to_hostname(), "[" BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "]");
}

BOOST_AUTO_TEST_CASE(authority__to_hostname__ipv4_mapped_ip_address__ipv4)
{
    // A mapped ip address serializes as IPv4.
    const authority host(mapped_ip_address, 0);
    BOOST_REQUIRE_EQUAL(host.to_hostname(), BC_AUTHORITY_IPV4_ADDRESS);
}

BOOST_AUTO_TEST_CASE(authority__to_hostname__ipv4_compatible_ip_address__ipv6_alternative)
{
    // A compatible ip address serializes as alternative notation IPv6.
    const authority host(compatible_ip_address, 0);
    BOOST_REQUIRE_EQUAL(host.to_hostname(), "[" BC_AUTHORITY_IPV6_ALTERNATIVE_COMPATIBLE_ADDRESS "]");
}

BOOST_AUTO_TEST_CASE(authority__to_hostname__ipv6_address__ipv6_compressed)
{
    // An ipv6 address serializes using compression.
    const authority host(ipv6_address, 0);
    BOOST_REQUIRE_EQUAL(host.to_hostname(), "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(authority__to_network_address)

BOOST_AUTO_TEST_CASE(authority__to_network_address__default__ipv6_unspecified)
{
    const network_address_type expected_address
    {
        0, 0, unspecified_ip_address, 0,
    };

    const authority host;
    BOOST_REQUIRE(net_equal(host.to_network_address(), expected_address));
}

BOOST_AUTO_TEST_CASE(authority__to_network_address__ipv4_mapped_ip_address__ipv4)
{
    const network_address_type expected_address
    {
        0, 0, mapped_ip_address, 42,
    };

    const authority host(expected_address.ip, expected_address.port);
    BOOST_REQUIRE(net_equal(host.to_network_address(), expected_address));
}

BOOST_AUTO_TEST_CASE(authority__to_network_address__ipv4_compatible_ip_address__ipv6_alternative)
{
    const network_address_type expected_address
    {
        0, 0, compatible_ip_address, 42,
    };

    const authority host(expected_address.ip, expected_address.port);
    BOOST_REQUIRE(net_equal(host.to_network_address(), expected_address));
}

BOOST_AUTO_TEST_CASE(authority__to_network_address__ipv6_address__ipv6_compressed)
{
    const network_address_type expected_address
    {
        0, 0, ipv6_address, 42,
    };

    const authority host(expected_address.ip, expected_address.port);
    BOOST_REQUIRE(net_equal(host.to_network_address(), expected_address));
}

BOOST_AUTO_TEST_SUITE_END()

// TODO:
BOOST_AUTO_TEST_SUITE(authority__to_string)
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(authority__equality)
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(authority__input)
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(authority__output)
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
