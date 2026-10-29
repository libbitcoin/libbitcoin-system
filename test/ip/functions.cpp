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

using namespace ip;

BOOST_AUTO_TEST_SUITE(ip_functions_tests)

static ip::address make_address(const std::string_view& text)
{
    ip::address out{};
    BOOST_REQUIRE(ip::address::from_string(out, text));
    return out;
}

// is_member

BOOST_AUTO_TEST_CASE(functions__is_member__defaults_zero__false)
{
    BOOST_REQUIRE(!is_member(ip::address{}, ip::address{}, 0));
}

BOOST_AUTO_TEST_CASE(functions__is_member__defaults_nonzero__false)
{
    BOOST_REQUIRE(!is_member(ip::address{}, ip::address{}, 1));
}

BOOST_AUTO_TEST_CASE(functions__is_member__ipv4_defaults_nonzero__false)
{
    BOOST_REQUIRE(!is_member(ip::ipv4{}, ip::ipv4{}, 24));
}

BOOST_AUTO_TEST_CASE(functions__is_member__ipv6_defaults_nonzero__true)
{
    BOOST_REQUIRE(is_member(ip::ipv6{}, ip::ipv6{}, 56));
}

BOOST_AUTO_TEST_CASE(functions__is_member__zero_cidr__expected)
{
    // zero CIDR valid for ipv4, invalid for ipv6
    BOOST_REQUIRE(is_member(make_address("42.42.42.42"), make_address("99.99.99.99"), 0));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("9999:9999:9999:9999:9999:9999:9999:9999"), 0));
}

BOOST_AUTO_TEST_CASE(functions__is_member__ipv4_member__true)
{
    BOOST_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.99.99.99"), 8));
    BOOST_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.99.99"), 16));
    BOOST_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.42.99"), 24));
    BOOST_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.42.42"), 32));
}

BOOST_AUTO_TEST_CASE(functions__is_member__not_ipv4_member__false)
{
    BOOST_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("99.99.99.99"), 8));
    BOOST_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.99.99.99"), 16));
    BOOST_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.42.99.99"), 24));
    BOOST_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.42.42.99"), 32));
}

BOOST_AUTO_TEST_CASE(functions__is_member__ipv6_non_member__true)
{
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("ab99:9999:9999:9999:9999:9999:9999:9999"), 8));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:9999:9999:9999:9999:9999:9999:9999"), 16));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:ab99:9999:9999:9999:9999:9999:9999"), 24));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:9999:9999:9999:9999:9999:9999"), 32));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:ab99:9999:9999:9999:9999:9999"), 40));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:9999:9999:9999:9999:9999"), 48));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:ab99:9999:9999:9999:9999"), 56));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:9999:9999:9999:9999"), 64));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:ab99:9999:9999:9999"), 72));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:9999:9999:9999"), 80));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:ab99:9999:9999"), 88));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:9999:9999"), 96));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:ab99:9999"), 104));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:9999"), 112));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:ab99"), 120));
    BOOST_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), 128));
}

BOOST_AUTO_TEST_CASE(functions__is_member__ipv6_non_member__false)
{
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("ab99:9999:9999:9999:9999:9999:9999:9999"), 16));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:9999:9999:9999:9999:9999:9999:9999"), 24));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:ab99:9999:9999:9999:9999:9999:9999"), 32));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:9999:9999:9999:9999:9999:9999"), 40));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:ab99:9999:9999:9999:9999:9999"), 48));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:9999:9999:9999:9999:9999"), 56));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:ab99:9999:9999:9999:9999"), 64));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:9999:9999:9999:9999"), 72));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:ab99:9999:9999:9999"), 80));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:9999:9999:9999"), 88));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:ab99:9999:9999"), 96));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:9999:9999"), 104));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:ab99:9999"), 112));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:9999"), 120));
    BOOST_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:ab99"), 128));
}

// denormalize

BOOST_AUTO_TEST_CASE(functions__denormalize__defaults__unchanged)
{
    BOOST_REQUIRE(denormalize({ ip::ipv4{} }).is_v4());
    BOOST_REQUIRE(denormalize({ ip::ipv6{} }).is_v6());
}

BOOST_AUTO_TEST_CASE(functions__denormalize__mapped__unmapped)
{
    constexpr ip::ipv6::bytes_type mapped
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 1, 2, 3, 4
    };

    const ip::address ip{ ip::ipv6{ mapped } };
    BOOST_REQUIRE(denormalize(ip).is_v4());
}

BOOST_AUTO_TEST_CASE(functions__denormalize__unmapped__unchanged)
{
    constexpr ip::ipv6::bytes_type unmapped
    {
        0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xab, 0xcd, 1, 2, 3, 4
    };

    const ip::address ip{ ip::ipv6{ unmapped } };
    BOOST_REQUIRE(denormalize(ip) == ip);
}

// to_host

BOOST_AUTO_TEST_CASE(functions__to_host__default__unspecified_v4)
{
    // Default address is ipv4.
    const auto host = to_host(ip::address{});
    BOOST_REQUIRE_EQUAL(host, "0.0.0.0");
}

BOOST_AUTO_TEST_CASE(functions__to_host__default_v4__unspecified_v4)
{
    const auto host = to_host(ip::ipv4{});
    BOOST_REQUIRE_EQUAL(host, "0.0.0.0");
}

BOOST_AUTO_TEST_CASE(functions__to_host__default_v6__unspecified_v6)
{
    const auto host = to_host(ip::ipv6{});
    BOOST_REQUIRE_EQUAL(host, "::");
}

BOOST_AUTO_TEST_CASE(functions__to_host__v4__expected_v4)
{
    const auto host = to_host(make_address("42.42.42.42"));
    BOOST_REQUIRE_EQUAL(host, "42.42.42.42");
}

BOOST_AUTO_TEST_CASE(functions__to_host__v6__expected_v6)
{
    const auto host = to_host(make_address("42:42::42:42"));
    BOOST_REQUIRE_EQUAL(host, "42:42::42:42");
}

// to_literal

BOOST_AUTO_TEST_CASE(functions__to_literal__default__unspecified_v4)
{
    // Default address is ipv4.
    const auto host = to_literal(ip::address{});
    BOOST_REQUIRE_EQUAL(host, "0.0.0.0");
}

BOOST_AUTO_TEST_CASE(functions__to_literal__default_v4__unspecified_v4)
{
    const auto host = to_literal(ip::ipv4{});
    BOOST_REQUIRE_EQUAL(host, "0.0.0.0");
}

BOOST_AUTO_TEST_CASE(functions__to_literal__default_v6__unspecified_literal_v6)
{
    const auto host = to_literal(ip::ipv6{});
    BOOST_REQUIRE_EQUAL(host, "[::]");
}

BOOST_AUTO_TEST_CASE(functions__to_literal__v4__expected_v4)
{
    const auto host = to_literal(make_address("42.42.42.42"));
    BOOST_REQUIRE_EQUAL(host, "42.42.42.42");
}

BOOST_AUTO_TEST_CASE(functions__to_literal__v6__expected_literal_v6)
{
    const auto host = to_literal(make_address("42:42::42:42"));
    BOOST_REQUIRE_EQUAL(host, "[42:42::42:42]");
}

BOOST_AUTO_TEST_SUITE_END()
