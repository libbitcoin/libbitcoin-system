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

BOOST_AUTO_TEST_SUITE(authority_tests)

using namespace config;
using namespace boost::program_options;

// tools.ietf.org/html/rfc4291#section-2.2
#define BC_AUTHORITY_IPV4_ADDRESS "1.2.240.1"
#define BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "::"
#define BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "2001:db8::2"

////// tools.ietf.org/html/rfc4291#section-2.5.5.2
////constexpr messages::ip_address test_mapped_ip_address =
////{
////    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
////    0x00, 0x00, 0xff, 0xff, 0x01, 0x02, 0xf0, 0x01
////};
////// tools.ietf.org/html/rfc4291#section-2.5.5.1
////constexpr messages::ip_address test_ipv6_address =
////{
////    0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
////    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
////};

// construct

BOOST_AUTO_TEST_CASE(authority__construct__bogus_ip__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("bogus"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__bogus_port__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[::]:bogus"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__invalid_ipv4__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("999.999.999.999"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__invalid_ipv6__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[:::]"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__invalid_port__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[::]:12345678901"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__zero_port__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[::]:0"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__mapped_address__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[::1.2.240.1]"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__leading_zero_port__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("42.42.42.42:0"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__zero_cidr__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("[::]/0"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__leading_zero_cidr__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("42.42.42.42/01"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__alpha_cidr__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("42.42.42.42/1ab"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__high_v4_cidr__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("42.42.42.42/33"), invalid_option_value);
}

BOOST_AUTO_TEST_CASE(authority__construct__high_v6_cidr__throws_invalid_option)
{
    BOOST_REQUIRE_THROW(authority host("42:42::42:42/65"), invalid_option_value);
}

// ip/port

BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__default__zero)
{
    const authority host{};
    BOOST_REQUIRE_EQUAL(host.port(), 0u);
    BOOST_REQUIRE(host.ip().is_unspecified());
    BOOST_REQUIRE_EQUAL(host.cidr(), 0u);
}

////BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__copy__expected)
////{
////    constexpr uint16_t expected_port = 42;
////    const authority other(from_address(test_ipv6_address), expected_port);
////    const authority host(other);
////    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
////    BOOST_REQUIRE_EQUAL(host.ip(), from_address(test_ipv6_address));
////    BOOST_REQUIRE_EQUAL(host.cidr(), 0u);
////}
////
////BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__move__expected)
////{
////    constexpr uint16_t expected_port = 42;
////    constexpr uint16_t expected_cidr = 123;
////    authority other(from_address(test_ipv6_address), expected_port, 123);
////    const authority host(std::move(other));
////    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
////    BOOST_REQUIRE_EQUAL(host.ip(), from_address(test_ipv6_address));
////    BOOST_REQUIRE_EQUAL(host.cidr(), expected_cidr);
////}

BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__ipv4_authority__expected)
{
    constexpr uint16_t expected_port = 42;
    constexpr uint16_t expected_cidr = 32;
    std::stringstream stream{};
    stream << BC_AUTHORITY_IPV4_ADDRESS ":" << expected_port << "/" << expected_cidr;
    const authority host(stream.str());
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
    BOOST_REQUIRE_EQUAL(host.ip(), from_host(BC_AUTHORITY_IPV4_ADDRESS));
    BOOST_REQUIRE_EQUAL(host.cidr(), expected_cidr);
}

BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__ipv6_authority__expected)
{
    constexpr uint16_t expected_port = 42;
    constexpr uint16_t expected_cidr = 24;
    std::stringstream stream{};
    stream << "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]:" << expected_port << "/" << expected_cidr;
    const authority host(stream.str());
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
    BOOST_REQUIRE_EQUAL(host.ip(), from_host("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]"));
    BOOST_REQUIRE_EQUAL(host.cidr(), expected_cidr);
}

BOOST_AUTO_TEST_CASE(authority__ip_port_cidr__boost_address__expected)
{
    constexpr uint16_t expected_port = 42;
    const auto address = asio::address::from_string(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
    const authority host(address, expected_port);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
    BOOST_REQUIRE_EQUAL(host.ip(), address);
    BOOST_REQUIRE_EQUAL(host.cidr(), 0u);
}

BOOST_AUTO_TEST_CASE(authority__port__boost_endpoint__expected)
{
    constexpr uint16_t expected_port = 42;
    const auto address = asio::address::from_string(BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
    const asio::endpoint tcp_endpoint(address, expected_port);
    const authority host(tcp_endpoint);
    BOOST_REQUIRE_EQUAL(host.port(), expected_port);
    BOOST_REQUIRE_EQUAL(host.ip(), address);
    BOOST_REQUIRE_EQUAL(host.cidr(), 0u);
}

// to_host

BOOST_AUTO_TEST_CASE(authority__to_host__default__ipv6_unspecified)
{
    const authority host{};
    BOOST_REQUIRE_EQUAL(host.to_host(), BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS);
}

////BOOST_AUTO_TEST_CASE(authority__to_host__ipv4_mapped_ip_address__ipv4)
////{
////    // A mapped ip address serializes as IPv4.
////    const authority host(from_address(test_mapped_ip_address), 0);
////    BOOST_REQUIRE_EQUAL(host.to_host(), BC_AUTHORITY_IPV4_ADDRESS);
////}
////
////BOOST_AUTO_TEST_CASE(authority__to_host__ipv6_address__ipv6_compressed)
////{
////    // An ipv6 address serializes using compression.
////    const authority host(from_address(test_ipv6_address), 0);
////    BOOST_REQUIRE_EQUAL(host.to_host(), BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS);
////}

// to_literal

BOOST_AUTO_TEST_CASE(authority__to_literal__default__unspecified)
{
    const authority host;
    BOOST_REQUIRE_EQUAL(host.to_literal(), "[" BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "]");
}

BOOST_AUTO_TEST_CASE(authority__to_literal__unspecified__unspecified)
{
    const auto line = "[" BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "]";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_literal(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_literal__ipv4__expected)
{
    const auto line = BC_AUTHORITY_IPV4_ADDRESS;
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_literal(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_literal__ipv4_port__expected)
{
    const authority host(BC_AUTHORITY_IPV4_ADDRESS ":42");
    BOOST_REQUIRE_EQUAL(host.to_literal(), BC_AUTHORITY_IPV4_ADDRESS);
}

BOOST_AUTO_TEST_CASE(authority__to_literal__ipv6__expected)
{
    const auto line = "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_literal(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_literal__ipv6_port__expected)
{
    const auto line = "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]:42";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_literal(), "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
}

// to_string

BOOST_AUTO_TEST_CASE(authority__to_string__default__unspecified)
{
    const authority host;
    BOOST_REQUIRE_EQUAL(host.to_string(), "[" BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "]");
}

BOOST_AUTO_TEST_CASE(authority__to_string__unspecified__unspecified)
{
    const auto line = "[" BC_AUTHORITY_IPV6_UNSPECIFIED_ADDRESS "]";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_string(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_string__ipv4__expected)
{
    const auto line = BC_AUTHORITY_IPV4_ADDRESS;
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_string(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_string__ipv4_port__expected)
{
    const auto line = BC_AUTHORITY_IPV4_ADDRESS ":42";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_string(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_string__ipv6__expected)
{
    const auto line = "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_string(), line);
}

BOOST_AUTO_TEST_CASE(authority__to_string__ipv6_port__expected)
{
    const auto line = "[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]:42";
    const authority host(line);
    BOOST_REQUIRE_EQUAL(host.to_string(), line);
}

// equality

BOOST_AUTO_TEST_CASE(authority__equality__default_default__true)
{
    const authority host1{};
    const authority host2{};
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_ipv4__true)
{
    const authority host1(BC_AUTHORITY_IPV4_ADDRESS);
    const authority host2(BC_AUTHORITY_IPV4_ADDRESS);
    BOOST_REQUIRE(host1 == host2);
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_ipv6__false)
{
    const authority host1(BC_AUTHORITY_IPV4_ADDRESS);
    const authority host2("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
    BOOST_REQUIRE(!(host1 == host2));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv6_ipv6__true)
{
    const authority host1("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
    const authority host2("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
    BOOST_REQUIRE(host1 == host2);
}

// equality _ipv4

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_same_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42") == authority("42.42.42.42"));
    BOOST_REQUIRE(authority("42.42.42.42:80") == authority("42.42.42.42:80"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_same_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42/1") == authority("42.42.42.42/1"));
    BOOST_REQUIRE(authority("42.42.42.42/8") == authority("42.42.42.42/8"));
    BOOST_REQUIRE(authority("42.42.42.42/24") == authority("42.42.42.42/24"));
    BOOST_REQUIRE(authority("42.42.42.42:80/32") == authority("42.42.42.42:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_distinct_default_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:80") == authority("42.42.42.42"));
    BOOST_REQUIRE(authority("42.42.42.42") == authority("42.42.42.42:80"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv4_distinct_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:88") != authority("42.42.42.42:99"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_distinct_default_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:80/8") == authority("42.42.42.42/8"));
    BOOST_REQUIRE(authority("42.42.42.42/24") == authority("42.42.42.42:80/24"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv4_distinct_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:81/32") != authority("42.42.42.42:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv4_distinct_port_distinct_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:80/8") != authority("42.42.42.42/7"));
    BOOST_REQUIRE(authority("42.42.42.42/24") != authority("42.42.42.42:80/12"));
    BOOST_REQUIRE(authority("42.42.42.42:81/25") != authority("42.42.42.42:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv4_same_port_single_cidr__true)
{
    BOOST_REQUIRE(authority("42.42.42.42") == authority("42.42.42.42/24"));
    BOOST_REQUIRE(authority("42.42.42.42/24") == authority("42.42.42.42"));
}

BOOST_AUTO_TEST_CASE(authority__equality__contained_by_right_ipv4__true)
{
    BOOST_REQUIRE(authority("42.42.42.42:80") == authority("42.0.0.0:80/8"));
    BOOST_REQUIRE(authority("42.42.42.42:8333") == authority("42.42.0.0:8333/16"));
    BOOST_REQUIRE(authority("42.42.42.42:42") == authority("42.42.42.0/24"));
    BOOST_REQUIRE(authority("42.42.42.42") == authority("42.42.42.42/32"));
}

BOOST_AUTO_TEST_CASE(authority__equality__contained_by_left_ipv4__true)
{
    BOOST_REQUIRE(authority("42.0.0.0:80/8") == authority("42.42.42.42:80"));
    BOOST_REQUIRE(authority("42.42.0.0:8333/16") == authority("42.42.42.42:8333"));
    BOOST_REQUIRE(authority("42.42.42.0/24") == authority("42.42.42.42:42"));
    BOOST_REQUIRE(authority("42.42.42.42/32") == authority("42.42.42.42"));
}

// equality ipv6

BOOST_AUTO_TEST_CASE(authority__equality__ipv6_same_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]") == authority("[abcd:abcd::abcd:abcd]"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80") == authority("[abcd:abcd::abcd:abcd]:80"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv6_same_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/1") == authority("[abcd:abcd::abcd:abcd]/1"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/8") == authority("[abcd:abcd::abcd:abcd]/8"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/24") == authority("[abcd:abcd::abcd:abcd]/24"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80/32") == authority("[abcd:abcd::abcd:abcd]:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_distinct_default_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80") == authority("[abcd:abcd::abcd:abcd]"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]") == authority("[abcd:abcd::abcd:abcd]:80"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_distinct_port_no_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:88") != authority("[abcd:abcd::abcd:abcd]:99"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv6_distinct_default_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80/8") == authority("[abcd:abcd::abcd:abcd]/8"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/24") == authority("[abcd:abcd::abcd:abcd]:80/24"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_distinct_port_same_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:81/32") != authority("[abcd:abcd::abcd:abcd]:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_distinct_default_port_distinct_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80/8") != authority("[abcd:abcd::abcd:abcd]/7"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/24") != authority("[abcd:abcd::abcd:abcd]:80/12"));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_distinct_port_distinct_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:81/25") != authority("[abcd:abcd::abcd:abcd]:80/32"));
}

BOOST_AUTO_TEST_CASE(authority__equality__ipv6_same_port_single_cidr__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]") == authority("[abcd:abcd::abcd:abcd]/24"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/24") == authority("[abcd:abcd::abcd:abcd]"));
}

BOOST_AUTO_TEST_CASE(authority__equality__contained_by_right_ipv6__true)
{
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:80") == authority("[abcd::]:80/16"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:8333") == authority("[abcd:abcd::]:8333/32"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]:42") == authority("[abcd:abcd::abcd:0]/48"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]") == authority("[abcd:abcd::abcd:abcd]/64"));
}

BOOST_AUTO_TEST_CASE(authority__equality__contained_by_left_ipv6__true)
{
    BOOST_REQUIRE(authority("[abcd::]:80/16") == authority("[abcd:abcd::abcd:abcd]:80"));
    BOOST_REQUIRE(authority("[abcd:abcd::]:8333/32") == authority("[abcd:abcd::abcd:abcd]:8333"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:0]/48") == authority("[abcd:abcd::abcd:abcd]:42"));
    BOOST_REQUIRE(authority("[abcd:abcd::abcd:abcd]/64") == authority("[abcd:abcd::abcd:abcd]"));
}

BOOST_AUTO_TEST_CASE(authority__equality__contains__expected)
{
    ////blacklist = 209.222.252.0/24
    ////blacklist = 162.218.65.0/24
    ////blacklist = 91.198.115.0/24
    ////Inbound channel start[162.218.65.145:33859] success
    ////Inbound channel start[209.222.252.190:56614] success
    ////Inbound channel start[91.198.115.114:16942] success
    std_vector<authority> authorities{};
    authorities.emplace_back("209.222.252.0/24");
    authorities.emplace_back("162.218.65.0/24");
    authorities.emplace_back("91.198.115.0/24");
    BOOST_REQUIRE(system::contains(authorities, authority{ "162.218.65.145:33859" }));
    BOOST_REQUIRE(system::contains(authorities, authority{ "209.222.252.190:56614" }));
    BOOST_REQUIRE(system::contains(authorities, authority{ "91.198.115.114:16942" }));
}

// inequality

BOOST_AUTO_TEST_CASE(authority__inequality__default_default__false)
{
    const authority host1{};
    const authority host2{};
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_CASE(authority__inequality__ipv6_ipv6__false)
{
    const authority host1("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
    const authority host2("[" BC_AUTHORITY_IPV6_COMPRESSED_ADDRESS "]");
    BOOST_REQUIRE(!(host1 != host2));
}

BOOST_AUTO_TEST_SUITE_END()
