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

BOOST_AUTO_TEST_SUITE(ipv4_tests)

using namespace ip;

struct ipv4_vector
{
    std::string text;
    ipv4::bytes_type bytes;
};

const std_vector<ipv4_vector> valid_ipv4
{
    { "0.0.0.0", { 0, 0, 0, 0 } },
    { "127.0.0.1", { 127, 0, 0, 1 } },
    { "1.2.240.1", { 1, 2, 240, 1 } },
    { "192.0.2.1", { 192, 0, 2, 1 } },
    { "255.255.255.255", { 255, 255, 255, 255 } }
};

const std_vector<std::string> invalid_ipv4
{
    "",
    ".",
    "1.2.3",
    "1.2.3.4.5",
    "1..2.3",
    "1.2.3.4.",
    ".1.2.3.4",
    "256.0.0.0",
    "1.2.3.256",
    "999.999.999.999",
    "01.2.3.4",
    "1.2.3.04",
    "00.0.0.0",
    "1.2.3.4:80",
    "1.2.3.4/24",
    " 1.2.3.4",
    "1.2.3.4 ",
    "+1.2.3.4",
    "-1.2.3.4",
    "a.b.c.d",
    "0x1.2.3.4",
    "::1"
};

// from_string

BOOST_AUTO_TEST_CASE(ipv4__from_string__valid__expected_bytes)
{
    for (const auto& vector: valid_ipv4)
    {
        ipv4 out{};
        BOOST_REQUIRE_MESSAGE(ipv4::from_string(out, vector.text), vector.text);
        BOOST_REQUIRE_EQUAL(out.to_bytes(), vector.bytes);
    }
}

BOOST_AUTO_TEST_CASE(ipv4__from_string__invalid__false_unchanged)
{
    for (const auto& text: invalid_ipv4)
    {
        ipv4 out{ ipv4::loopback() };
        BOOST_REQUIRE_MESSAGE(!ipv4::from_string(out, text), text);
        BOOST_REQUIRE_EQUAL(out, ipv4::loopback());
    }
}

// to_string

BOOST_AUTO_TEST_CASE(ipv4__to_string__valid__round_trip)
{
    for (const auto& vector: valid_ipv4)
    {
        BOOST_REQUIRE_EQUAL(ipv4{ vector.bytes }.to_string(), vector.text);
    }
}

BOOST_AUTO_TEST_CASE(ipv4__to_string__default__unspecified)
{
    BOOST_REQUIRE_EQUAL(ipv4{}.to_string(), "0.0.0.0");
}

// statics

BOOST_AUTO_TEST_CASE(ipv4__any__always__unspecified)
{
    BOOST_REQUIRE(ipv4::any().is_unspecified());
    BOOST_REQUIRE_EQUAL(ipv4::any(), ipv4{});
}

BOOST_AUTO_TEST_CASE(ipv4__loopback__always__expected)
{
    const ipv4::bytes_type expected{ 127, 0, 0, 1 };
    BOOST_REQUIRE_EQUAL(ipv4::loopback().to_bytes(), expected);
}

// is_loopback

BOOST_AUTO_TEST_CASE(ipv4__is_loopback__loopback_network__true)
{
    const ipv4 network{ { 127, 0, 0, 0 } };
    const ipv4 broadcast{ { 127, 255, 255, 255 } };
    BOOST_REQUIRE(ipv4::loopback().is_loopback());
    BOOST_REQUIRE(network.is_loopback());
    BOOST_REQUIRE(broadcast.is_loopback());
}

BOOST_AUTO_TEST_CASE(ipv4__is_loopback__other__false)
{
    const ipv4 above{ { 128, 0, 0, 1 } };
    const ipv4 below{ { 126, 255, 255, 255 } };
    BOOST_REQUIRE(!ipv4{}.is_loopback());
    BOOST_REQUIRE(!above.is_loopback());
    BOOST_REQUIRE(!below.is_loopback());
}

// is_unspecified

BOOST_AUTO_TEST_CASE(ipv4__is_unspecified__default__true)
{
    BOOST_REQUIRE(ipv4{}.is_unspecified());
}

BOOST_AUTO_TEST_CASE(ipv4__is_unspecified__nonzero__false)
{
    const ipv4 low{ { 0, 0, 0, 1 } };
    const ipv4 high{ { 1, 0, 0, 0 } };
    BOOST_REQUIRE(!low.is_unspecified());
    BOOST_REQUIRE(!high.is_unspecified());
}

// operators

BOOST_AUTO_TEST_CASE(ipv4__ordering__by_octets__expected)
{
    const ipv4 first{ { 1, 2, 3, 4 } };
    const ipv4 second{ { 1, 2, 3, 5 } };
    const ipv4 third{ { 1, 2, 4, 0 } };
    const ipv4 fourth{ { 2, 0, 0, 0 } };
    BOOST_REQUIRE(first == ipv4(first.to_bytes()));
    BOOST_REQUIRE(first != second);
    BOOST_REQUIRE(second < third);
    BOOST_REQUIRE(fourth > third);
}

BOOST_AUTO_TEST_CASE(ipv4__ostream__loopback__expected)
{
    std::stringstream stream{};
    stream << ipv4::loopback();
    BOOST_REQUIRE_EQUAL(stream.str(), "127.0.0.1");
}

BOOST_AUTO_TEST_SUITE_END()
