/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(secp256k1_tests)

// addition

BOOST_AUTO_TEST_CASE(secp256k1__ec_add__positive__expected)
{
    const auto sum = base16_array(
        "0404040000000000000000000000000000000000000000000000000000000000");

    ec_secret secret_one{ { 1, 2, 3 } };
    ec_compressed public1;
    BOOST_REQUIRE(secret_to_public(public1, secret_one));

    const ec_secret secret_two{ { 3, 2, 1 } };
    BOOST_REQUIRE(ec_add(secret_one, secret_two));
    BOOST_REQUIRE_EQUAL(secret_one, sum);

    ec_compressed public2;
    BOOST_REQUIRE(secret_to_public(public2, secret_one));
    BOOST_REQUIRE(ec_add(public1, secret_two));
    BOOST_REQUIRE_EQUAL(public1, public2);
}

BOOST_AUTO_TEST_CASE(secp256k1__ec_add__negative__expected)
{
    // = n - 1
    auto secret_one = base16_array(
        "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
    ec_secret secret_two{ { 0 } };
    secret_two[31] = 1;
    ec_compressed public1;
    BOOST_REQUIRE(secret_to_public(public1, secret_one));
    BOOST_REQUIRE(!ec_add(secret_one, secret_two));
    BOOST_REQUIRE(!ec_add(public1, secret_two));
}

BOOST_AUTO_TEST_CASE(secp256k1__ec_sum__expected)
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
    BOOST_REQUIRE(ec_sum(out, points));
    BOOST_REQUIRE_EQUAL(out, generator_point_times_4);
}

// multiplication

BOOST_AUTO_TEST_CASE(secp256k1__ec_multiply__expected)
{
    ec_secret secret_one{ {0} };
    ec_secret secret_two{ {0} };
    secret_one[31] = 11;
    secret_two[31] = 22;
    ec_compressed public1;
    BOOST_REQUIRE(secret_to_public(public1, secret_one));
    BOOST_REQUIRE(ec_multiply(secret_one, secret_two));
    BOOST_REQUIRE_EQUAL(secret_one[31], 242u);
    BOOST_REQUIRE(ec_multiply(public1, secret_two));

    ec_compressed public2;
    BOOST_REQUIRE(secret_to_public(public2, secret_one));
    BOOST_REQUIRE_EQUAL(public1, public2);
}

// negation

BOOST_AUTO_TEST_SUITE_END()
