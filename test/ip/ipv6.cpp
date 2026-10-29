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

BOOST_AUTO_TEST_SUITE(ipv6_tests)

using namespace ip;

struct ipv6_vector
{
    std::string text;
    std::string canonical;
};

// rfc4291 2.2 and rfc5952 4 examples, with canonical text (rfc5952).
const std_vector<ipv6_vector> valid_ipv6
{
    { "2001:DB8:0:0:8:800:200C:417A", "2001:db8::8:800:200c:417a" },
    { "2001:DB8::8:800:200C:417A", "2001:db8::8:800:200c:417a" },
    { "FF01:0:0:0:0:0:0:101", "ff01::101" },
    { "FF01::101", "ff01::101" },
    { "0:0:0:0:0:0:0:1", "::1" },
    { "::1", "::1" },
    { "0:0:0:0:0:0:0:0", "::" },
    { "::", "::" },
    { "0:0:0:0:0:0:13.1.68.3", "::13.1.68.3" },
    { "::13.1.68.3", "::13.1.68.3" },
    { "0:0:0:0:0:FFFF:129.144.52.38", "::ffff:129.144.52.38" },
    { "::FFFF:129.144.52.38", "::ffff:129.144.52.38" },
    { "2001:db8:0:0:0:0:2:1", "2001:db8::2:1" },
    { "2001:db8:0:1:1:1:1:1", "2001:db8:0:1:1:1:1:1" },
    { "2001:0:0:1:0:0:0:1", "2001:0:0:1::1" },
    { "2001:db8:0:0:1:0:0:1", "2001:db8::1:0:0:1" },
    { "2001:0db8::0001", "2001:db8::1" },
    { "2001:DB8::ABCD", "2001:db8::abcd" },
    { "1::", "1::" },
    { "1:2:3:4:5:6:7::", "1:2:3:4:5:6:7:0" },
    { "::2:3:4:5:6:7:8", "0:2:3:4:5:6:7:8" },
    { "1:2:3:4:5:6:1.2.3.4", "1:2:3:4:5:6:102:304" },
    { "::1:0:0:0", "::1:0:0:0" },
    { "0:0:0:0:0:0:0.0.1.0", "::100" },
    { "4242::4242", "4242::4242" },
    { "42:42::42:42", "42:42::42:42" }
};

const std_vector<std::string> invalid_ipv6
{
    "",
    ":",
    ":::",
    "1:::2",
    "1::2::3",
    "::1::",
    ":1::2",
    "1::2:",
    "1:2:3:4:5:6:7",
    "1:2:3:4:5:6:7:8:9",
    "1:2:3:4:5:6:7:8::",
    "::1:2:3:4:5:6:7:8",
    "12345::",
    "g::",
    "::1.2.3",
    "::1.2.3.256",
    "::01.2.3.4",
    "1.2.3.4::",
    "1:2:3:4:5:6:7:1.2.3.4",
    "::1.2.3.4:1",
    "[::1]",
    "::1%1",
    "1.2.3.4"
};

// from_string

BOOST_AUTO_TEST_CASE(ipv6__from_string__valid__canonical_round_trip)
{
    for (const auto& vector: valid_ipv6)
    {
        ipv6 out{};
        BOOST_REQUIRE_MESSAGE(ipv6::from_string(out, vector.text), vector.text);
        BOOST_REQUIRE_EQUAL(out.to_string(), vector.canonical);

        ipv6 canonical{};
        BOOST_REQUIRE(ipv6::from_string(canonical, vector.canonical));
        BOOST_REQUIRE_EQUAL(canonical, out);
    }
}

BOOST_AUTO_TEST_CASE(ipv6__from_string__invalid__false_unchanged)
{
    for (const auto& text: invalid_ipv6)
    {
        ipv6 out{ ipv6::loopback() };
        BOOST_REQUIRE_MESSAGE(!ipv6::from_string(out, text), text);
        BOOST_REQUIRE_EQUAL(out, ipv6::loopback());
    }
}

BOOST_AUTO_TEST_CASE(ipv6__from_string__full__expected_bytes)
{
    ipv6 out{};
    const ipv6::bytes_type expected{ 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0x08, 0x08, 0x00, 0x20, 0x0c, 0x41, 0x7a };
    BOOST_REQUIRE(ipv6::from_string(out, "2001:DB8:0:0:8:800:200C:417A"));
    BOOST_REQUIRE_EQUAL(out.to_bytes(), expected);
}

BOOST_AUTO_TEST_CASE(ipv6__from_string__embedded__expected_bytes)
{
    ipv6 out{};
    const ipv6::bytes_type expected{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 129, 144, 52, 38 };
    BOOST_REQUIRE(ipv6::from_string(out, "::ffff:129.144.52.38"));
    BOOST_REQUIRE_EQUAL(out.to_bytes(), expected);
}

// statics

BOOST_AUTO_TEST_CASE(ipv6__any__always__unspecified)
{
    BOOST_REQUIRE(ipv6::any().is_unspecified());
    BOOST_REQUIRE_EQUAL(ipv6::any(), ipv6{});
}

BOOST_AUTO_TEST_CASE(ipv6__loopback__always__expected)
{
    const ipv6::bytes_type expected{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    BOOST_REQUIRE_EQUAL(ipv6::loopback().to_bytes(), expected);
}

BOOST_AUTO_TEST_CASE(ipv6__mapped__ipv4__expected)
{
    const ipv6::bytes_type expected{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 1, 2, 3, 4 };
    const ipv4 ip{ { 1, 2, 3, 4 } };
    const auto mapped = ipv6::mapped(ip);
    BOOST_REQUIRE_EQUAL(mapped.to_bytes(), expected);
    BOOST_REQUIRE(mapped.is_v4_mapped());
    BOOST_REQUIRE_EQUAL(mapped.to_v4(), ip);
}

// is_v4_mapped

BOOST_AUTO_TEST_CASE(ipv6__is_v4_mapped__mapped__true)
{
    const ipv6 ip{ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0, 0, 0, 0 } };
    BOOST_REQUIRE(ip.is_v4_mapped());
}

BOOST_AUTO_TEST_CASE(ipv6__is_v4_mapped__other__false)
{
    const ipv6 compatible{ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4 } };
    const ipv6 other{ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0xff, 0xff, 1, 2, 3, 4 } };
    BOOST_REQUIRE(!ipv6{}.is_v4_mapped());
    BOOST_REQUIRE(!compatible.is_v4_mapped());
    BOOST_REQUIRE(!other.is_v4_mapped());
}

// to_v4

BOOST_AUTO_TEST_CASE(ipv6__to_v4__any__low_bytes)
{
    const ipv6 ip{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } };
    const ipv4 expected{ { 13, 14, 15, 16 } };
    BOOST_REQUIRE_EQUAL(ip.to_v4(), expected);
}

// is_loopback

BOOST_AUTO_TEST_CASE(ipv6__is_loopback__loopback__true)
{
    BOOST_REQUIRE(ipv6::loopback().is_loopback());
}

BOOST_AUTO_TEST_CASE(ipv6__is_loopback__other__false)
{
    BOOST_REQUIRE(!ipv6{}.is_loopback());
    BOOST_REQUIRE(!ipv6::mapped(ipv4::loopback()).is_loopback());
}

// is_unspecified

BOOST_AUTO_TEST_CASE(ipv6__is_unspecified__default__true)
{
    BOOST_REQUIRE(ipv6{}.is_unspecified());
}

BOOST_AUTO_TEST_CASE(ipv6__is_unspecified__nonzero__false)
{
    BOOST_REQUIRE(!ipv6::loopback().is_unspecified());
}

// operators

BOOST_AUTO_TEST_CASE(ipv6__ordering__by_bytes__expected)
{
    BOOST_REQUIRE(ipv6{} < ipv6::loopback());
    BOOST_REQUIRE(ipv6::loopback() == ipv6::loopback());
    BOOST_REQUIRE(ipv6::loopback() != ipv6{});
}

BOOST_AUTO_TEST_CASE(ipv6__ostream__loopback__expected)
{
    std::stringstream stream{};
    stream << ipv6::loopback();
    BOOST_REQUIRE_EQUAL(stream.str(), "::1");
}

BOOST_AUTO_TEST_SUITE_END()
