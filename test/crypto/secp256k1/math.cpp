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
#include "../../test.hpp"

BC_AUTO_TEST_SUITE(secp256k1_tests)

// addition

BC_AUTO_TEST_CASE(secp256k1__ec_add__positive__expected)
{
    const auto sum = base16_array(
        "0404040000000000000000000000000000000000000000000000000000000000");

    ec_secret secret_one{ { 1, 2, 3 } };
    ec_compressed public1;
    BC_REQUIRE(secret_to_public(public1, secret_one));

    const ec_secret secret_two{ { 3, 2, 1 } };
    BC_REQUIRE(ec_add(secret_one, secret_two));
    BC_REQUIRE_EQUAL(secret_one, sum);

    ec_compressed public2;
    BC_REQUIRE(secret_to_public(public2, secret_one));
    BC_REQUIRE(ec_add(public1, secret_two));
    BC_REQUIRE_EQUAL(public1, public2);
}

BC_AUTO_TEST_CASE(secp256k1__ec_add__negative__expected)
{
    // = n - 1
    auto secret_one = base16_array(
        "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
    ec_secret secret_two{ { 0 } };
    secret_two[31] = 1;
    ec_compressed public1;
    BC_REQUIRE(secret_to_public(public1, secret_one));
    BC_REQUIRE(!ec_add(secret_one, secret_two));
    BC_REQUIRE(!ec_add(public1, secret_two));
}

BC_AUTO_TEST_CASE(secp256k1__ec_sum__expected)
{
    const ec_compressed generator_point_times_4 = base16_array(
        "02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13");

    const ec_compresseds points
    {
        ec_compressed_generator,
        ec_compressed_generator,
        ec_compressed_generator,
        ec_compressed_generator
    };

    ec_compressed out;
    BC_REQUIRE(ec_sum(out, points));
    BC_REQUIRE_EQUAL(out, generator_point_times_4);
}

// multiplication

BC_AUTO_TEST_CASE(secp256k1__ec_multiply__expected)
{
    ec_secret secret_one{ {0} };
    ec_secret secret_two{ {0} };
    secret_one[31] = 11;
    secret_two[31] = 22;
    ec_compressed public1;
    BC_REQUIRE(secret_to_public(public1, secret_one));
    BC_REQUIRE(ec_multiply(secret_one, secret_two));
    BC_REQUIRE_EQUAL(secret_one[31], 242u);
    BC_REQUIRE(ec_multiply(public1, secret_two));

    ec_compressed public2;
    BC_REQUIRE(secret_to_public(public2, secret_one));
    BC_REQUIRE_EQUAL(public1, public2);
}

// negation

// uncompressed and point-to-point overloads

BC_AUTO_TEST_CASE(secp256k1__ec_add__uncompressed_point_secret__expected)
{
    const ec_secret secret{ { 1, 2, 3 } };
    const ec_secret tweak{ { 3, 2, 1 } };

    ec_compressed compressed;
    BC_REQUIRE(secret_to_public(compressed, secret));
    ec_uncompressed uncompressed;
    BC_REQUIRE(secret_to_public(uncompressed, secret));

    BC_REQUIRE(ec_add(compressed, tweak));
    BC_REQUIRE(ec_add(uncompressed, tweak));

    ec_compressed expected;
    BC_REQUIRE(compress(expected, uncompressed));
    BC_REQUIRE_EQUAL(expected, compressed);
}

BC_AUTO_TEST_CASE(secp256k1__ec_add__compressed_points__matches_sum)
{
    const ec_secret one{ { 1, 2, 3 } };
    const ec_secret two{ { 3, 2, 1 } };

    ec_compressed left;
    ec_compressed right;
    BC_REQUIRE(secret_to_public(left, one));
    BC_REQUIRE(secret_to_public(right, two));

    ec_compressed expected;
    BC_REQUIRE(ec_sum(expected, { left, right }));
    BC_REQUIRE(ec_add(left, right));
    BC_REQUIRE_EQUAL(left, expected);
}

BC_AUTO_TEST_CASE(secp256k1__ec_add__compressed_and_uncompressed_points__matches_sum)
{
    const ec_secret one{ { 1, 2, 3 } };
    const ec_secret two{ { 3, 2, 1 } };

    ec_compressed left;
    ec_compressed right;
    ec_uncompressed right_uncompressed;
    BC_REQUIRE(secret_to_public(left, one));
    BC_REQUIRE(secret_to_public(right, two));
    BC_REQUIRE(secret_to_public(right_uncompressed, two));

    ec_compressed expected;
    BC_REQUIRE(ec_sum(expected, { left, right }));
    BC_REQUIRE(ec_add(left, right_uncompressed));
    BC_REQUIRE_EQUAL(left, expected);
}

BC_AUTO_TEST_CASE(secp256k1__ec_add__uncompressed_points__expected)
{
    const ec_secret one{ { 1, 2, 3 } };
    const ec_secret two{ { 3, 2, 1 } };

    ec_secret sum{ one };
    BC_REQUIRE(ec_add(sum, two));

    ec_uncompressed expected;
    BC_REQUIRE(secret_to_public(expected, sum));

    ec_uncompressed left;
    ec_uncompressed right;
    BC_REQUIRE(secret_to_public(left, one));
    BC_REQUIRE(secret_to_public(right, two));
    BC_REQUIRE(ec_add(left, right));
    BC_REQUIRE_EQUAL(left, expected);
}

BC_AUTO_TEST_CASE(secp256k1__ec_sum__empty__false)
{
    ec_compressed out;
    BC_REQUIRE(!ec_sum(out, {}));
}

BC_AUTO_TEST_CASE(secp256k1__ec_sum__invalid_point__false)
{
    ec_compressed out;
    BC_REQUIRE(!ec_sum(out, { ec_compressed{} }));
}

BC_AUTO_TEST_CASE(secp256k1__ec_sum__negated_point__false)
{
    auto negated = ec_compressed_generator;
    BC_REQUIRE(ec_negate(negated));

    ec_compressed out;
    BC_REQUIRE(!ec_sum(out, { ec_compressed_generator, negated }));
}

BC_AUTO_TEST_CASE(secp256k1__ec_multiply__zero__false)
{
    auto point = ec_compressed_generator;
    BC_REQUIRE(!ec_multiply(point, ec_secret{}));
}

BC_AUTO_TEST_CASE(secp256k1__ec_multiply__uncompressed_point__matches_compressed)
{
    const ec_secret secret{ { 1, 2, 3 } };
    const ec_secret factor{ { 3, 2, 1 } };

    ec_compressed compressed;
    ec_uncompressed uncompressed;
    BC_REQUIRE(secret_to_public(compressed, secret));
    BC_REQUIRE(secret_to_public(uncompressed, secret));

    BC_REQUIRE(ec_multiply(compressed, factor));
    BC_REQUIRE(ec_multiply(uncompressed, factor));

    ec_compressed expected;
    BC_REQUIRE(compress(expected, uncompressed));
    BC_REQUIRE_EQUAL(expected, compressed);
}

BC_AUTO_TEST_CASE(secp256k1__ec_negate__secret_and_points__round_trips)
{
    const ec_secret secret{ { 1, 2, 3 } };

    ec_secret negated{ secret };
    BC_REQUIRE(ec_negate(negated));
    BC_REQUIRE(ec_negate(negated));
    BC_REQUIRE_EQUAL(negated, secret);

    ec_compressed compressed;
    BC_REQUIRE(secret_to_public(compressed, secret));
    const auto expected_compressed = compressed;
    BC_REQUIRE(ec_negate(compressed));
    BC_REQUIRE(ec_negate(compressed));
    BC_REQUIRE_EQUAL(compressed, expected_compressed);

    ec_uncompressed uncompressed;
    BC_REQUIRE(secret_to_public(uncompressed, secret));
    const auto expected_uncompressed = uncompressed;
    BC_REQUIRE(ec_negate(uncompressed));
    BC_REQUIRE(ec_negate(uncompressed));
    BC_REQUIRE_EQUAL(uncompressed, expected_uncompressed);
}

BC_AUTO_TEST_SUITE_END()
