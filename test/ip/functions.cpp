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

BC_AUTO_TEST_SUITE(ip_functions_tests)

static ip::address make_address(const std::string_view& text)
{
    ip::address out{};
    BC_REQUIRE(ip::address::from_string(out, text));
    return out;
}

// is_member

BC_AUTO_TEST_CASE(functions__is_member__defaults_zero__false)
{
    BC_REQUIRE(!is_member(ip::address{}, ip::address{}, 0));
}

BC_AUTO_TEST_CASE(functions__is_member__defaults_nonzero__false)
{
    BC_REQUIRE(!is_member(ip::address{}, ip::address{}, 1));
}

BC_AUTO_TEST_CASE(functions__is_member__ipv4_defaults_nonzero__false)
{
    BC_REQUIRE(!is_member(ip::ipv4{}, ip::ipv4{}, 24));
}

BC_AUTO_TEST_CASE(functions__is_member__ipv6_defaults_nonzero__true)
{
    BC_REQUIRE(is_member(ip::ipv6{}, ip::ipv6{}, 56));
}

BC_AUTO_TEST_CASE(functions__is_member__zero_cidr__expected)
{
    // zero CIDR valid for ipv4, invalid for ipv6
    BC_REQUIRE(is_member(make_address("42.42.42.42"), make_address("99.99.99.99"), 0));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("9999:9999:9999:9999:9999:9999:9999:9999"), 0));
}

BC_AUTO_TEST_CASE(functions__is_member__ipv4_member__true)
{
    BC_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.99.99.99"), 8));
    BC_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.99.99"), 16));
    BC_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.42.99"), 24));
    BC_REQUIRE(is_member(make_address("42.42.42.42"), make_address("42.42.42.42"), 32));
}

BC_AUTO_TEST_CASE(functions__is_member__not_ipv4_member__false)
{
    BC_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("99.99.99.99"), 8));
    BC_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.99.99.99"), 16));
    BC_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.42.99.99"), 24));
    BC_REQUIRE(!is_member(make_address("42.42.42.42"), make_address("42.42.42.99"), 32));
}

BC_AUTO_TEST_CASE(functions__is_member__ipv6_non_member__true)
{
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("ab99:9999:9999:9999:9999:9999:9999:9999"), 8));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:9999:9999:9999:9999:9999:9999:9999"), 16));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:ab99:9999:9999:9999:9999:9999:9999"), 24));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:9999:9999:9999:9999:9999:9999"), 32));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:ab99:9999:9999:9999:9999:9999"), 40));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:9999:9999:9999:9999:9999"), 48));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:ab99:9999:9999:9999:9999"), 56));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:9999:9999:9999:9999"), 64));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:ab99:9999:9999:9999"), 72));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:9999:9999:9999"), 80));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:ab99:9999:9999"), 88));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:9999:9999"), 96));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:ab99:9999"), 104));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:9999"), 112));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:ab99"), 120));
    BC_REQUIRE(is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), 128));
}

BC_AUTO_TEST_CASE(functions__is_member__ipv6_non_member__false)
{
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("ab99:9999:9999:9999:9999:9999:9999:9999"), 16));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:9999:9999:9999:9999:9999:9999:9999"), 24));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:ab99:9999:9999:9999:9999:9999:9999"), 32));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:9999:9999:9999:9999:9999:9999"), 40));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:ab99:9999:9999:9999:9999:9999"), 48));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:9999:9999:9999:9999:9999"), 56));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:ab99:9999:9999:9999:9999"), 64));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:9999:9999:9999:9999"), 72));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:ab99:9999:9999:9999"), 80));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:9999:9999:9999"), 88));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:ab99:9999:9999"), 96));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:9999:9999"), 104));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:ab99:9999"), 112));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:9999"), 120));
    BC_REQUIRE(!is_member(make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd"), make_address("abcd:abcd:abcd:abcd:abcd:abcd:abcd:ab99"), 128));
}

// denormalize

BC_AUTO_TEST_CASE(functions__denormalize__defaults__unchanged)
{
    BC_REQUIRE(denormalize({ ip::ipv4{} }).is_v4());
    BC_REQUIRE(denormalize({ ip::ipv6{} }).is_v6());
}

BC_AUTO_TEST_CASE(functions__denormalize__mapped__unmapped)
{
    constexpr ip::ipv6::bytes_type mapped
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 1, 2, 3, 4
    };

    const ip::address ip{ ip::ipv6{ mapped } };
    BC_REQUIRE(denormalize(ip).is_v4());
}

BC_AUTO_TEST_CASE(functions__denormalize__unmapped__unchanged)
{
    constexpr ip::ipv6::bytes_type unmapped
    {
        0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xab, 0xcd, 1, 2, 3, 4
    };

    const ip::address ip{ ip::ipv6{ unmapped } };
    BC_REQUIRE(denormalize(ip) == ip);
}

// to_host

BC_AUTO_TEST_CASE(functions__to_host__default__unspecified_v4)
{
    // Default address is ipv4.
    const auto host = to_host(ip::address{});
    BC_REQUIRE_EQUAL(host, "0.0.0.0");
}

BC_AUTO_TEST_CASE(functions__to_host__default_v4__unspecified_v4)
{
    const auto host = to_host(ip::ipv4{});
    BC_REQUIRE_EQUAL(host, "0.0.0.0");
}

BC_AUTO_TEST_CASE(functions__to_host__default_v6__unspecified_v6)
{
    const auto host = to_host(ip::ipv6{});
    BC_REQUIRE_EQUAL(host, "::");
}

BC_AUTO_TEST_CASE(functions__to_host__v4__expected_v4)
{
    const auto host = to_host(make_address("42.42.42.42"));
    BC_REQUIRE_EQUAL(host, "42.42.42.42");
}

BC_AUTO_TEST_CASE(functions__to_host__v6__expected_v6)
{
    const auto host = to_host(make_address("42:42::42:42"));
    BC_REQUIRE_EQUAL(host, "42:42::42:42");
}

// to_literal

BC_AUTO_TEST_CASE(functions__to_literal__default__unspecified_v4)
{
    // Default address is ipv4.
    const auto host = to_literal(ip::address{});
    BC_REQUIRE_EQUAL(host, "0.0.0.0");
}

BC_AUTO_TEST_CASE(functions__to_literal__default_v4__unspecified_v4)
{
    const auto host = to_literal(ip::ipv4{});
    BC_REQUIRE_EQUAL(host, "0.0.0.0");
}

BC_AUTO_TEST_CASE(functions__to_literal__default_v6__unspecified_literal_v6)
{
    const auto host = to_literal(ip::ipv6{});
    BC_REQUIRE_EQUAL(host, "[::]");
}

BC_AUTO_TEST_CASE(functions__to_literal__v4__expected_v4)
{
    const auto host = to_literal(make_address("42.42.42.42"));
    BC_REQUIRE_EQUAL(host, "42.42.42.42");
}

BC_AUTO_TEST_CASE(functions__to_literal__v6__expected_literal_v6)
{
    const auto host = to_literal(make_address("42:42::42:42"));
    BC_REQUIRE_EQUAL(host, "[42:42::42:42]");
}

BC_AUTO_TEST_SUITE_END()
