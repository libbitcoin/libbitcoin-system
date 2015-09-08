/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "bip38.hpp"

#include <algorithm>
#include <string>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::wallet;

// TODO: implement testnet tests.

#ifndef ENABLE_TESTNET

BOOST_AUTO_TEST_SUITE(bip38_tests)

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(bip38__create_token)

static void test_create_token(/* const bip38_vector& vector */)
{
    // TODO
}

// TODO: add test vectors for create_token.
BOOST_AUTO_TEST_CASE(bip38__create_token__always__throws)
{
    test_create_token(/* vector */);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(bip38__create_key_pair)

// TODO: create compressed vector(s).

static void test_create_key_pair(const bip38_vector& vector)
{
    bip38::token token;
    BOOST_REQUIRE(decode_base58(token, vector.token));

    bip38::seed seed;
    BOOST_REQUIRE(decode_base16(seed, vector.seed));

    bip38::public_key public_key;
    bip38::private_key private_key;
    BOOST_REQUIRE(bip38::create_key_pair(private_key, public_key, token, seed, vector.version, vector.compressed));
    BOOST_REQUIRE_EQUAL(encode_base58(public_key), vector.public_key);
    BOOST_REQUIRE_EQUAL(encode_base58(private_key), vector.private_key);
}

BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_8__expected)
{
    // Currently failing
    test_create_key_pair(bip38_test_vector8);
}

BOOST_AUTO_TEST_CASE(bip38__create_key_pair__vector_9__expected)
{
    // Currently failing
    test_create_key_pair(bip38_test_vector9);
}

BOOST_AUTO_TEST_SUITE_END()

#ifdef WITH_ICU

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(bip38__encrypt)

static void test_encrypt(const bip38_vector& vector)
{
    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, vector.ec_secret));

    bip38::private_key private_key;
    bip38::encrypt(private_key, secret, vector.passphrase, vector.version, vector.compressed);
    BOOST_REQUIRE_EQUAL(encode_base58(private_key), vector.private_key);
}

BOOST_AUTO_TEST_CASE(bip38__encrypt__vector_0__expected)
{
    test_encrypt(bip38_test_vector0);
}

BOOST_AUTO_TEST_CASE(bip38__encrypt__vector_1__expected)
{
    test_encrypt(bip38_test_vector1);
}

BOOST_AUTO_TEST_CASE(bip38__encrypt__vector_2_compressed__expected)
{
    test_encrypt(bip38_test_vector2);
}

BOOST_AUTO_TEST_CASE(bip38__encrypt__vector_3_compressed__expected)
{
    test_encrypt(bip38_test_vector3);
}

// #3 from: github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__encrypt__vector_unicode___expected)
{
    // This vector encodes as the passphrase as base16.
    bip38_vector vector;
    vector.passphrase = "cf92cc8100f0909080f09f92a9";
    vector.ec_secret = "64eeab5f9be2a01a8365a579511eb3373c87c40da6d2a25f05bda68fe077b66e";
    vector.private_key = "6PRW5o9FLp4gJDDVqJQKJFTpMvdsSGJxMYHtHaQBF3ooa8mwD69bapcDQn";
    ////vector.address = "16ktGzmfrurhbhi6JGqsMWf7TyqK9HNAeF";

    data_chunk not_normalized;
    BOOST_REQUIRE(decode_base16(not_normalized, vector.passphrase));
    std::string passphrase(not_normalized.begin(), not_normalized.end());

    ec_secret secret;
    BOOST_REQUIRE(decode_base16(secret, vector.ec_secret));

    bip38::private_key private_key;
    bip38::encrypt(private_key, secret, passphrase, false);
    BOOST_REQUIRE_EQUAL(encode_base58(private_key), vector.private_key);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(bip38__decrypt1)

// TODO: create compressed + multiplied vector(s).

static void test_decrypt1(const bip38_vector& vector)
{
    bip38::private_key private_key;
    BOOST_REQUIRE(decode_base58(private_key, vector.private_key));

    ec_secret secret;
    uint8_t version;
    bool compressed;
    BOOST_REQUIRE(bip38::decrypt(secret, version, compressed, private_key, vector.passphrase));
    BOOST_REQUIRE_EQUAL(encode_base16(secret), vector.ec_secret);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_0__expected)
{
    test_decrypt1(bip38_test_vector0);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_1__expected)
{
    test_decrypt1(bip38_test_vector1);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_2_compressed__expected)
{
    test_decrypt1(bip38_test_vector2);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_3_compressed__expected)
{
    test_decrypt1(bip38_test_vector3);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_4_multiplied__expected)
{
    test_decrypt1(bip38_test_vector4);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_5_multiplied__expected)
{
    test_decrypt1(bip38_test_vector5);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_6_multiplied__expected)
{
    test_decrypt1(bip38_test_vector6);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_7_multiplied__expected)
{
    test_decrypt1(bip38_test_vector7);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_8_multiplied__expected)
{
    test_decrypt1(bip38_test_vector8);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt1__vector_9_multiplied__expected)
{
    test_decrypt1(bip38_test_vector9);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(bip38__decrypt2)

// TODO: create compressed vector(s).

static void test_decrypt2(const bip38_vector& vector)
{
    bip38::public_key public_key;
    BOOST_REQUIRE(decode_base58(public_key, vector.public_key));

    ec_point point;
    uint8_t out_version;
    BOOST_REQUIRE(bip38::decrypt(point, out_version, public_key, vector.passphrase));
    BOOST_REQUIRE_EQUAL(out_version, vector.version);

    // This will vary by WITH_TESTNET. The vector must be for bitcoin address.
    const auto version = payment_address::pubkey_version;
    const auto expected = payment_address(version, point).to_string();
    BOOST_REQUIRE_EQUAL(expected, vector.address);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt2__vector_8__expected)
{
    // Currently failing
    test_decrypt2(bip38_test_vector8);
}

BOOST_AUTO_TEST_CASE(bip38__decrypt2__vector_9__expected)
{
    // Currently failing
    test_decrypt2(bip38_test_vector9);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

#endif // WITH_ICU

BOOST_AUTO_TEST_SUITE_END()

#endif // !ENABLE_TESTNET
