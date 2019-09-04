/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#define POSITIVE_THREE "0000000000000000000000000000000000000000000000000000000000000003"
#define POSITIVE_FORTY_TWO "000000000000000000000000000000000000000000000000000000000000002a"
#define NEGATIVE_ONE "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140"

#define SECRET2 "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define COMPRESSED2 "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"

// construct

BOOST_AUTO_TEST_CASE(ec_scalar__construct__default__invalid)
{
    const ec_scalar value;
    BOOST_REQUIRE(!value);
}

BOOST_AUTO_TEST_CASE(ec_scalar__construct__secret__initialized)
{
    const ec_scalar value(base16_literal(POSITIVE_FORTY_TWO));
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), POSITIVE_FORTY_TWO);
}

BOOST_AUTO_TEST_CASE(ec_scalar__construct__copy__initialized)
{
    const ec_scalar x(42);
    const ec_scalar y(x);
    BOOST_REQUIRE(x);
    BOOST_REQUIRE(y);
    BOOST_REQUIRE_EQUAL(encode_base16(x.secret()), POSITIVE_FORTY_TWO);
    BOOST_REQUIRE_EQUAL(encode_base16(y.secret()), POSITIVE_FORTY_TWO);
}

BOOST_AUTO_TEST_CASE(ec_scalar__construct__move__uninitialized_transferred)
{
    ec_scalar x(42);
    const auto pointer = x.secret().data();
    const ec_scalar y(std::move(x));
    BOOST_REQUIRE(!x);
    BOOST_REQUIRE(y);
    BOOST_REQUIRE(y.secret().data() == pointer);
    BOOST_REQUIRE_EQUAL(encode_base16(y.secret()), POSITIVE_FORTY_TWO);
}

BOOST_AUTO_TEST_CASE(ec_scalar__construct__uint64__initialized)
{
    const ec_scalar value(42);
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), POSITIVE_FORTY_TWO);
}

// equality

BOOST_AUTO_TEST_CASE(ec_scalar__equality__same_valid__true)
{
    const ec_scalar x(42);
    const ec_scalar y(42);
    BOOST_REQUIRE(x);
    BOOST_REQUIRE(y);
    BOOST_REQUIRE(x == y);
    BOOST_REQUIRE(y == x);
}

BOOST_AUTO_TEST_CASE(ec_scalar__equality__both_invalid__false)
{
    const ec_scalar x;
    const ec_scalar y;
    BOOST_REQUIRE(!x);
    BOOST_REQUIRE(!y);

    // Any invalid results in inequality.
    BOOST_REQUIRE(x != y);
    BOOST_REQUIRE(y != x);
}

BOOST_AUTO_TEST_CASE(ec_scalar__equality__one_invalid__false)
{
    const ec_scalar x(42);
    const ec_scalar y;
    BOOST_REQUIRE(x);
    BOOST_REQUIRE(!y);

    // Any invalid results in inequality.
    BOOST_REQUIRE(!(x == y));
    BOOST_REQUIRE(!(y == x));
}

// inequality

BOOST_AUTO_TEST_CASE(ec_scalar__inequality__same_valid__false)
{
    const ec_scalar x(42);
    const ec_scalar y(42);
    BOOST_REQUIRE(x);
    BOOST_REQUIRE(y);
    BOOST_REQUIRE(!(x != y));
    BOOST_REQUIRE(!(y != x));
}

BOOST_AUTO_TEST_CASE(ec_scalar__inequality__both_invalid__false)
{
    const ec_scalar x;
    const ec_scalar y;
    BOOST_REQUIRE(!x);
    BOOST_REQUIRE(!y);

    // Any invalid results in inequality.
    BOOST_REQUIRE(x != y);
    BOOST_REQUIRE(y != x);
}

BOOST_AUTO_TEST_CASE(ec_scalar__inequality__one_invalid__true)
{
    const ec_scalar x(42);
    const ec_scalar y;
    BOOST_REQUIRE(x);
    BOOST_REQUIRE(!y);

    // Any invalid results in inequality.
    BOOST_REQUIRE(x != y);
    BOOST_REQUIRE(y != x);
}

// negation

BOOST_AUTO_TEST_CASE(ec_scalar__negation__of_invalid__invalid)
{
    ec_scalar value;
    BOOST_REQUIRE(!value);

    -value;
    BOOST_REQUIRE(!value);
}

BOOST_AUTO_TEST_CASE(ec_scalar__negation__of_positive__negative)
{
    ec_scalar value{ base16_literal(POSITIVE_ONE) };
    BOOST_REQUIRE(value);

    -value;
    BOOST_REQUIRE(value);
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), NEGATIVE_ONE);
}

BOOST_AUTO_TEST_CASE(ec_scalar__negation__of_negative__positive)
{
    ec_scalar value{ base16_literal(NEGATIVE_ONE) };
    BOOST_REQUIRE(value);

    -value;
    BOOST_REQUIRE(value);
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), POSITIVE_ONE);
}

// sum

BOOST_AUTO_TEST_CASE(ec_scalar__sum__of_invalid__invalid)
{
    const ec_scalar x;
    const ec_scalar y;
    const ec_scalar z(42);

    // Sum of any invalid is invalid.
    BOOST_REQUIRE(!(x + y));
    BOOST_REQUIRE(!(y + x));
    BOOST_REQUIRE(!(x + z));
    BOOST_REQUIRE(!(z + x));
}

BOOST_AUTO_TEST_CASE(ec_scalar__sum_assignment__of_invalid__invalid)
{
    ec_scalar x;
    ec_scalar y;
    ec_scalar z(42);

    // Difference of any invalid is invalid.
    BOOST_REQUIRE(!((x += y)));
    BOOST_REQUIRE(!((y += x)));
    BOOST_REQUIRE(!((x += z)));
    BOOST_REQUIRE(!((z += x)));
}

BOOST_AUTO_TEST_CASE(ec_scalar__sum__of_secrets__expected)
{
    const ec_scalar x{ base16_literal(SECRET1A) };
    const ec_scalar y{ base16_literal(SECRET1B) };
    const auto sum = x + y;
    BOOST_REQUIRE(sum);
    BOOST_REQUIRE_EQUAL(encode_base16(sum.secret()), SUM1);
}

BOOST_AUTO_TEST_CASE(ec_scalar__sum__of_uint64__expected)
{
    const ec_scalar x(42);
    const ec_scalar y(24);
    const auto sum = x + y;
    BOOST_REQUIRE(sum);

    const ec_scalar expected(42 + 24);
    BOOST_REQUIRE_EQUAL(sum, expected);
}

BOOST_AUTO_TEST_CASE(ec_scalar__sum__of_negated_positive_and_positive__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const ec_scalar two{ base16_literal(POSITIVE_TWO) };
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);

    auto value = one;
    BOOST_REQUIRE(value);

    const auto sum = -value + two;
    BOOST_REQUIRE(sum);
    BOOST_REQUIRE_EQUAL(encode_base16(sum.secret()), encode_base16(one.secret()));
}

BOOST_AUTO_TEST_CASE(ec_scalar__sum_assignment__positives__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const ec_scalar two{ base16_literal(POSITIVE_TWO) };
    const ec_scalar three{ base16_literal(POSITIVE_THREE) };
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);

    auto value = one;
    BOOST_REQUIRE(value);

    value += two;
    BOOST_REQUIRE(value);
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), encode_base16(three.secret()));
}

// difference

BOOST_AUTO_TEST_CASE(ec_scalar__difference__of_invalid__invalid)
{
    const ec_scalar x;
    const ec_scalar y;
    const ec_scalar z(42);

    // Difference of any invalid is invalid.
    BOOST_REQUIRE(!(x - y));
    BOOST_REQUIRE(!(y - x));
    BOOST_REQUIRE(!(x - z));
    BOOST_REQUIRE(!(z - x));
}

BOOST_AUTO_TEST_CASE(ec_scalar__difference_assignment__of_invalid__invalid)
{
    ec_scalar x;
    ec_scalar y;
    ec_scalar z(42);

    // Difference of any invalid is invalid.
    BOOST_REQUIRE(!((x -= y)));
    BOOST_REQUIRE(!((y -= x)));
    BOOST_REQUIRE(!((x -= z)));
    BOOST_REQUIRE(!((z -= x)));
}

BOOST_AUTO_TEST_CASE(ec_scalar__difference__positives__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const ec_scalar two{ base16_literal(POSITIVE_TWO) };
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);

    const auto difference = two - one;
    BOOST_REQUIRE(difference);
    BOOST_REQUIRE_EQUAL(encode_base16(difference.secret()), encode_base16(one.secret()));
}

BOOST_AUTO_TEST_CASE(ec_scalar__difference_assignment__positives__expected)
{
    const ec_scalar one{ base16_literal(POSITIVE_ONE) };
    const ec_scalar two{ base16_literal(POSITIVE_TWO) };
    BOOST_REQUIRE(one);
    BOOST_REQUIRE(two);

    auto value = two;
    BOOST_REQUIRE(value);

    value -= one;
    BOOST_REQUIRE(value);
    BOOST_REQUIRE_EQUAL(encode_base16(value.secret()), encode_base16(one.secret()));
}

// product

BOOST_AUTO_TEST_CASE(ec_scalar__product__of_invalid__invalid)
{
    const ec_scalar x;
    const ec_scalar y;
    const ec_scalar z(42);

    // Product of any invalid is invalid.
    BOOST_REQUIRE(!(x * y));
    BOOST_REQUIRE(!(y * x));
    BOOST_REQUIRE(!(x * z));
    BOOST_REQUIRE(!(z * x));
}

BOOST_AUTO_TEST_CASE(ec_scalar__product_assignment__of_invalid__invalid)
{
    ec_scalar x;
    ec_scalar y;
    ec_scalar z(42);

    // Product of any invalid is invalid.
    BOOST_REQUIRE(!((x *= y)));
    BOOST_REQUIRE(!((y *= x)));
    BOOST_REQUIRE(!((x *= z)));
    BOOST_REQUIRE(!((z *= x)));
}

BOOST_AUTO_TEST_CASE(ec_scalar__product__from_secrets__expected)
{
    const ec_scalar x{ base16_literal(SECRET1A) };
    const ec_scalar y{ base16_literal(SECRET1B) };
    const auto product = x * y;
    BOOST_REQUIRE(product);
    BOOST_REQUIRE_EQUAL(encode_base16(product.secret()), PRODUCT1);
}

BOOST_AUTO_TEST_CASE(ec_scalar__product__point_by_g__expected)
{
    const ec_scalar key = base16_literal(SECRET2);
    const auto product = key * ec_point::G;
    BOOST_REQUIRE(product);
    BOOST_REQUIRE_EQUAL(encode_base16(product.point()), COMPRESSED2);
}

BOOST_AUTO_TEST_CASE(ec_scalar__product__zero_by_g__fail)
{
    const auto product = ec_scalar::zero * ec_point::G;
    BOOST_REQUIRE(!product);
}

BOOST_AUTO_TEST_CASE(ec_scalar__product_assignment__from_secrets__expected)
{
    ec_scalar x{ base16_literal(SECRET1A) };
    const ec_scalar y{ base16_literal(SECRET1B) };
    x *= y;
    BOOST_REQUIRE(x);
    BOOST_REQUIRE_EQUAL(encode_base16(x.secret()), PRODUCT1);
}

BOOST_AUTO_TEST_SUITE_END()
