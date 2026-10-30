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

using namespace config;
BOOST_AUTO_TEST_SUITE(utilities_tests)

// from_host

BOOST_AUTO_TEST_CASE(utilities__from_host__empty__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host({}), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v4_port__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("127.0.0.1:42"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v4_cidr__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("127.0.0.1/24"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v6_port__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("[42:42::42:42]:42"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v6_cidr__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("[42:42::42:42]/24"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__unbracketed_default_v6__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("::"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__unbracketed_v6__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("4242::4242"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__mapped_v6__throws_invalid_option_value)
{
    BOOST_REQUIRE_THROW(from_host("[::ffff:127.0.0.1]"), istream_exception);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__unspecified_v4__expected_v4)
{
    BOOST_REQUIRE_EQUAL(from_host("0.0.0.0"), ip::ipv4{});
}

BOOST_AUTO_TEST_CASE(utilities__from_host__unspecified_v6__expected_v6)
{
    const ip::ipv6 expected{};
    BOOST_REQUIRE_EQUAL(from_host("[::]"), ip::ipv6{});
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v4__expected_v4)
{
    const ip::ipv4 expected{ ip::ipv4::bytes_type{ 42, 42, 42, 42 } };
    BOOST_REQUIRE_EQUAL(from_host("42.42.42.42"), expected);
}

BOOST_AUTO_TEST_CASE(utilities__from_host__v6__expected_v6)
{
    const ip::ipv6 expected
    {
        ip::ipv6::bytes_type
        {
            0x42, 0x42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x42, 0x42
        }
    };

    BOOST_REQUIRE_EQUAL(from_host("[4242::4242]"), expected);
}

////BOOST_AUTO_TEST_CASE(utilities__from_host__compatible__expected_v4)
////{
////    const ip::address expected{ ip::ipv4{ ip::ipv4::bytes_type{ 127, 0, 1, 1 } } };
////    BOOST_REQUIRE_EQUAL(make_address("[::127.0.0.1]:8333"), expected);
////}
////
////BOOST_AUTO_TEST_CASE(utilities__from_host__mapped__expected_v4)
////{
////    const ip::address expected{ ip::ipv4{ ip::ipv4::bytes_type{ 127, 0, 1, 1 } } };
////    BOOST_REQUIRE_EQUAL(make_address("[::ffff.127.0.0.1]:8333"), expected);
////}

// parse_authority

BOOST_AUTO_TEST_CASE(utilities__parse_authority__empty__false)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, ""));
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v4__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "127.0.0.1"));
    BOOST_REQUIRE(ip.is_v4());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 0u);
    BOOST_REQUIRE_EQUAL(cidr, 0u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v4_port__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "127.0.0.1:65535"));
    BOOST_REQUIRE(ip.is_v4());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 65535u);
    BOOST_REQUIRE_EQUAL(cidr, 0u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v4_cidr__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "127.0.0.1/24"));
    BOOST_REQUIRE(ip.is_v4());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 0u);
    BOOST_REQUIRE_EQUAL(cidr, 24u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v4_port_cidr__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "127.0.0.1:42/24"));
    BOOST_REQUIRE(ip.is_v4());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 42u);
    BOOST_REQUIRE_EQUAL(cidr, 24u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v4_invalids__false)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "[127.0.0.1]"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "127.0.0.1:65536"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "127.0.0.1/0"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "127.0.0.1/33"));
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v6__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "[::1]"));
    BOOST_REQUIRE(ip.is_v6());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 0u);
    BOOST_REQUIRE_EQUAL(cidr, 0u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v6_port__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "[::1]:65535"));
    BOOST_REQUIRE(ip.is_v6());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 65535u);
    BOOST_REQUIRE_EQUAL(cidr, 0u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v6_cidr__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "[::1]/64"));
    BOOST_REQUIRE(ip.is_v6());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 0u);
    BOOST_REQUIRE_EQUAL(cidr, 64u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v6_port_cidr__expected)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(parse_authority(ip, port, cidr, "[::1]:42/64"));
    BOOST_REQUIRE(ip.is_v6());
    BOOST_REQUIRE(ip.is_loopback());
    BOOST_REQUIRE_EQUAL(port, 42u);
    BOOST_REQUIRE_EQUAL(cidr, 64u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_authority__v6_invalids__false)
{
    ip::address ip{};
    uint16_t port{};
    uint8_t cidr{};
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "::"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "::1"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "4242::4242"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "[::1]:65536"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "[::1]/0"));
    BOOST_REQUIRE(!parse_authority(ip, port, cidr, "[::1]/129"));
}

// parse_url

BOOST_AUTO_TEST_CASE(utilities__parse_url__full__true_expected)
{
    std::string scheme{};
    std::string host{};
    uint16_t port{};
    BOOST_REQUIRE(parse_url(scheme, host, port, "tcp://foo.bar:42"));
    BOOST_REQUIRE_EQUAL(scheme, "tcp");
    BOOST_REQUIRE_EQUAL(host, "foo.bar");
    BOOST_REQUIRE_EQUAL(port, 42u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_url__host_only__true_expected)
{
    std::string scheme{};
    std::string host{};
    uint16_t port{};
    BOOST_REQUIRE(parse_url(scheme, host, port, "foo.bar"));
    BOOST_REQUIRE(scheme.empty());
    BOOST_REQUIRE_EQUAL(host, "foo.bar");
    BOOST_REQUIRE_EQUAL(port, 0u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_url__host_port__true_expected)
{
    std::string scheme{};
    std::string host{};
    uint16_t port{};
    BOOST_REQUIRE(parse_url(scheme, host, port, "foo.bar:65535"));
    BOOST_REQUIRE(scheme.empty());
    BOOST_REQUIRE_EQUAL(host, "foo.bar");
    BOOST_REQUIRE_EQUAL(port, 65535u);
}

BOOST_AUTO_TEST_CASE(utilities__parse_url__invalids__false_expected)
{
    std::string scheme{};
    std::string host{};
    uint16_t port{};
    BOOST_REQUIRE(!parse_url(scheme, host, port, "tcp://foo.bar:65536"));
    BOOST_REQUIRE(!parse_url(scheme, host, port, "foobar://foo.bar:42"));
    BOOST_REQUIRE(!parse_url(scheme, host, port, "tcp://:42"));
    BOOST_REQUIRE(!parse_url(scheme, host, port, ":42"));
    BOOST_REQUIRE(!parse_url(scheme, host, port, ""));
}

BOOST_AUTO_TEST_CASE(utilities__is_member__mixed_families__false)
{
    const auto ip4 = from_host("42.42.42.42");
    const auto ip6 = from_host("[4242::4242]");
    BOOST_REQUIRE(!is_member(ip4, ip6, 8));
    BOOST_REQUIRE(!is_member(ip6, ip4, 8));
}

BOOST_AUTO_TEST_SUITE_END()
