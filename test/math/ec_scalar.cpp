/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(ec_scalar_tests)

#define SECRET1A "b9d22ea0ed7e3d53cc9e1b3d436bb9260fc3211634c821f6bb3cb3a73ee7a0b5"
#define SECRET1B "f9416cd856d81a00366ad7234a7b5e5dbda61812e56effcb4cde0695bc092380"
#define SUM1 "b3139b79445657540308f2608de7178512ba5c426aee818648485bb02aba82f4"
#define PRODUCT1 "aba800424dd618d36a42bf6d6d3156a38a74298b9e4519bbba34cf0d7dce4d4f"

#define POSITIVE_ONE "0000000000000000000000000000000000000000000000000000000000000001"
#define POSITIVE_TWO "0000000000000000000000000000000000000000000000000000000000000002"
#define NEGATIVE_ONE "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"

#define SECRET2 "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define COMPRESSED2 "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"

BOOST_AUTO_TEST_CASE(ec_scalar__zero__multiply_g__fail)
{
    const auto product = ec_scalar::zero * ec_point::G;
    BOOST_REQUIRE(!product);
}

BOOST_AUTO_TEST_CASE(ec_scalar__add__valid__expected)
{
    const ec_scalar x{ base16_literal(SECRET1A) };
    const ec_scalar y{ base16_literal(SECRET1B) };
    const auto sum = x + y;
    BOOST_REQUIRE(sum);
    BOOST_REQUIRE_EQUAL(encode_base16(sum.secret()), SUM1);
}

BOOST_AUTO_TEST_CASE(ec_scalar__multiply__valid__expected)
{
    const ec_scalar x{ base16_literal(SECRET1A) };
    const ec_scalar y{ base16_literal(SECRET1B) };
    const auto product = x * y;
    BOOST_REQUIRE(product);
    BOOST_REQUIRE_EQUAL(encode_base16(product.secret()), PRODUCT1);
}

BOOST_AUTO_TEST_CASE(ec_scalar__negate__positive__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const auto negation = -one;
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(negation);
    BOOST_REQUIRE_EQUAL(encode_base16(negation.secret()), NEGATIVE_ONE);
}

BOOST_AUTO_TEST_CASE(ec_scalar__add__negative__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const ec_scalar two{ base16_literal(POSITIVE_TWO) };
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);

    const auto sum = -one + two;
    BOOST_REQUIRE(sum);
    BOOST_REQUIRE_EQUAL(encode_base16(sum.secret()), encode_base16(one.secret()));
}

BOOST_AUTO_TEST_CASE(ec_arithmetic__point__multiply_g__expected)
{
    const ec_scalar key = base16_literal(SECRET2);
    const auto product = key * ec_point::G;
    BOOST_REQUIRE(product);
    BOOST_REQUIRE_EQUAL(encode_base16(product.point()), COMPRESSED2);
}

BOOST_AUTO_TEST_SUITE_END()
