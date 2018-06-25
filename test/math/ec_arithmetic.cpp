/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(ec_arithmetic_tests)

// Scenario 1
#define SECRET1A "b9d22ea0ed7e3d53cc9e1b3d436bb9260fc3211634c821f6bb3cb3a73ee7a0b5"
#define SECRET1B "f9416cd856d81a00366ad7234a7b5e5dbda61812e56effcb4cde0695bc092380"
#define RESULT_ADD_SCALAR "b3139b79445657540308f2608de7178512ba5c426aee818648485bb02aba82f4"
#define RESULT_MUL_SCALAR "aba800424dd618d36a42bf6d6d3156a38a74298b9e4519bbba34cf0d7dce4d4f"

// Negative scalar
#define NEGATIVE_ONE "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"

// Scenario 2
#define SECRET2 "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define COMPRESSED2 "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"

// Scenario 3
#define POINTX "0245dbb7e2cd3a5de19fde8d556fd567a036f9c377ecf69a9202aa4affce41c623"
#define POINTY "02cfc43e064c50cfd1896766ef70e7da82b16e8cfebd8d5dec618212d0db1e6d12"
#define RESULT3 "03332bf6821c7c0e1080efc131d2b745760a8245c0b91a05f13308ff8600d30525"

BOOST_AUTO_TEST_CASE(ec_arithmetic__scalar__zero_fail)
{
    const auto result = ec_scalar::zero * ec_point::G;
    BOOST_REQUIRE(!result);
    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__scalar__addition)
{
    const ec_scalar x = base16_literal(SECRET1A);
    const ec_scalar y = base16_literal(SECRET1B);

    const auto result = x + y;
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(encode_base16(result.secret()), RESULT_ADD_SCALAR);
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__scalar__multiplication)
{
    const ec_scalar x = base16_literal(SECRET1A);
    const ec_scalar y = base16_literal(SECRET1B);

    const auto result = x * y;
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(encode_base16(result.secret()), RESULT_MUL_SCALAR);
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__scalar__negative_addition)
{
    const ec_scalar one = 1;
    const ec_scalar two = 2;
    const auto negative_one = -one;

    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);
    BOOST_REQUIRE(negative_one);
    BOOST_REQUIRE_EQUAL(encode_base16(negative_one.secret()), NEGATIVE_ONE);

    const auto result = negative_one + two;
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(result, one);
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__point__multiply)
{
    const ec_scalar key = base16_literal(SECRET2);
    const auto result = key * ec_point::G;
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(encode_base16(result.point()), COMPRESSED2);
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__point__addition)
{
    const ec_point x = base16_literal(POINTX);
    const ec_point y = base16_literal(POINTY);
    const auto result = x + y;
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(encode_base16(result.point()), RESULT3);
}

BOOST_AUTO_TEST_SUITE_END()

