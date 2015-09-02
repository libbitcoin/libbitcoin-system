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

#ifndef ENABLE_TESTNET

BOOST_AUTO_TEST_SUITE(bip38_tests)

BOOST_AUTO_TEST_SUITE(bip38__utilities)

BOOST_AUTO_TEST_CASE(bip38__last_byte__two_bytes__expected)
{
    const uint8_t expected = 24;
    const data_chunk buffer
    {
        42, expected
    };

    const auto last = bip38::last_byte(buffer);
    BOOST_REQUIRE_EQUAL(last, expected);
}

BOOST_AUTO_TEST_CASE(bip38__normal__ascii__expected)
{
    const std::string expected("ascii");
    const auto normalized = bip38::normal(expected);
    BOOST_REQUIRE_EQUAL(normalized[0], expected[0]);
    BOOST_REQUIRE_EQUAL(normalized[1], expected[1]);
    BOOST_REQUIRE_EQUAL(normalized[2], expected[2]);
    BOOST_REQUIRE_EQUAL(normalized[3], expected[3]);
}

BOOST_AUTO_TEST_CASE(bip38__split__data_chunk__expected)
{
    const uint8_t expected_lower = 42;
    const uint8_t expected_upper = 24;
    data_chunk from
    {
        expected_lower, expected_upper
    };

    data_chunk lower_half;
    data_chunk upper_half;
    bip38::split(from, lower_half, upper_half, from.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], expected_lower);
    BOOST_REQUIRE_EQUAL(upper_half[0], expected_upper);
}

BOOST_AUTO_TEST_CASE(bip38__split__long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    long_hash from
    { 
        {
            l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        } 
    };

    data_chunk lower_half;
    data_chunk upper_half;
    bip38::split(from, lower_half, upper_half, from.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], l);
    BOOST_REQUIRE_EQUAL(upper_half[0], u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bip38__lock_secret)

static void test_lock_secret(const bip38_vector& vector)
{
    data_chunk unlocked;
    BOOST_REQUIRE(decode_base16(unlocked, vector.unlocked));
    BOOST_REQUIRE_EQUAL(unlocked.size(), ec_secret_size);

    ec_secret private_key;
    std::copy(unlocked.begin(), unlocked.end(), private_key.begin());

    const auto& locked = bip38::lock_secret(private_key, vector.passphrase, vector.compressed);
    BOOST_REQUIRE_EQUAL(encode_base58(locked), vector.locked);
}

BOOST_AUTO_TEST_CASE(bip38__lock__vector_0_not_multiplied_uncompressed__expected)
{
    test_lock_secret(bip38_test_vector[0]);
}

BOOST_AUTO_TEST_CASE(bip38__lock__vector_1_not_multiplied_uncompressed__expected)
{
    test_lock_secret(bip38_test_vector[1]);
}

BOOST_AUTO_TEST_CASE(bip38__lock__vector_2_not_multiplied__expected)
{
    test_lock_secret(bip38_test_vector[2]);
}

BOOST_AUTO_TEST_CASE(bip38__lock__vector_3_not_multiplied__expected)
{
    test_lock_secret(bip38_test_vector[3]);
}

// #3 from: github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
BOOST_AUTO_TEST_CASE(bip38__lock__vector_10_not_multiplied_uncompressed_unicode___expected)
{
    data_chunk not_normalized;
    BOOST_REQUIRE(decode_base16(not_normalized, "cf92cc8100f0909080f09f92a9"));
    std::string passphrase(not_normalized.begin(), not_normalized.end());

    // associated address: "16ktGzmfrurhbhi6JGqsMWf7TyqK9HNAeF"
    data_chunk unlocked;
    BOOST_REQUIRE(decode_base16(unlocked, "64eeab5f9be2a01a8365a579511eb3373c87c40da6d2a25f05bda68fe077b66e"));
    BOOST_REQUIRE_EQUAL(unlocked.size(), ec_secret_size);
    ec_secret private_key;
    std::copy(unlocked.begin(), unlocked.end(), private_key.begin());

    const auto compressed = false;
    const auto& locked = bip38::lock_secret(private_key, passphrase, compressed);
    BOOST_REQUIRE_EQUAL(encode_base58(locked), "6PRW5o9FLp4gJDDVqJQKJFTpMvdsSGJxMYHtHaQBF3ooa8mwD69bapcDQn");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bip38__unlock_secret)

static void test_unlock_secret(const bip38_vector& vector)
{
    data_chunk locked;
    decode_base58(locked, vector.locked);

    bip38::encrypted_private_key private_key;
    BOOST_REQUIRE_EQUAL(vector.locked.size(), bip38::encrypted_key_encoded_size);
    BOOST_REQUIRE_EQUAL(locked.size(), bip38::encrypted_key_decoded_size);

    std::copy(locked.begin(), locked.end(), private_key.begin());
    const auto& unlocked = bip38::unlock_secret(private_key, vector.passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(unlocked), vector.unlocked);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_0_not_multiplied_uncompressed__expected)
{
    test_unlock_secret(bip38_test_vector[0]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_1_not_multiplied_uncompressed__expected)
{
    test_unlock_secret(bip38_test_vector[1]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_2_not_multiplied__expected)
{
    test_unlock_secret(bip38_test_vector[2]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_3_not_multiplied__expected)
{
    test_unlock_secret(bip38_test_vector[3]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_4_uncompressed__expected)
{
    test_unlock_secret(bip38_test_vector[4]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_5_uncompressed__expected)
{
    test_unlock_secret(bip38_test_vector[5]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_6_uncompressed_confirmation__expected)
{
    test_unlock_secret(bip38_test_vector[6]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_7_uncompressed_confirmation__expected)
{
    test_unlock_secret(bip38_test_vector[7]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_8_uncompressed_confirmation_seeded__expected)
{
    test_unlock_secret(bip38_test_vector[8]);
}

BOOST_AUTO_TEST_CASE(bip38__unlock_secret__vector_9_uncompressed_confirmation_seeded__expected)
{
    test_unlock_secret(bip38_test_vector[9]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(bip38__lock_intermediate)

static void test_lock_intermediate(const bip38_vector& vector)
{
    data_chunk buffer;
    decode_base58(buffer, vector.intermediate);
    BOOST_REQUIRE_EQUAL(buffer.size(), bip38::intermediate_decoded_size);
    bip38::intermediate intermediate;
    std::copy(buffer.begin(), buffer.end(), intermediate.begin());

    buffer.clear();
    decode_base16(buffer, vector.random_seed_data);
    BOOST_REQUIRE_EQUAL(buffer.size(), bip38::seed_size);
    bip38::seed seed;
    std::copy(buffer.begin(), buffer.end(), seed.begin());

    bip38::confirmation_code confirmation_code;
    const auto locked = bip38::lock_intermediate(intermediate, seed, confirmation_code, vector.compressed);
    BOOST_REQUIRE_EQUAL(encode_base58(locked), vector.locked);
    BOOST_REQUIRE_EQUAL(encode_base58(confirmation_code), vector.confirmation_code);

    wallet::payment_address address;
    BOOST_REQUIRE(bip38::lock_verify(confirmation_code, vector.passphrase, address));
    BOOST_REQUIRE_EQUAL(address.to_string(), vector.address);
}

BOOST_AUTO_TEST_CASE(bip38__lock_intermediate__vector_8_uncompressed__expected)
{
    // Currently failing
    test_lock_intermediate(bip38_test_vector[8]);
}

BOOST_AUTO_TEST_CASE(bip38__lock_intermediate__vector_9_uncompressed__expected)
{
    // Currently failing
    test_lock_intermediate(bip38_test_vector[9]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

#endif // !ENABLE_TESTNET
