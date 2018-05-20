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
#include <random>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "ring_signature_vectors.hpp"

using namespace bc;

data_chunk new_seed()
{
    data_chunk seed(10);
    pseudo_random_fill(seed);
    return seed;
}

ec_secret new_key()
{
    const auto seed = new_seed();
    const wallet::hd_private key(seed);
    return key.secret();
}

template <typename SecretsIterator>
point_list extract_public_keys(SecretsIterator start, SecretsIterator end)
{
    point_list ring;
    for (auto it = start; it != end; ++it)
    {
        ec_compressed public_key;
        bool rc = secret_to_public(public_key, *it);
        BITCOIN_ASSERT(rc);
        ring.push_back(public_key);
    }
    return ring;
}

bool is_secret_in_ring(const ec_secret& secret, const point_list& ring)
{
    ec_compressed public_key;
    bool rc = secret_to_public(public_key, secret);
    BITCOIN_ASSERT(rc);
    for (const auto& ring_key: ring)
    {
        if (ring_key == public_key)
            return true;
    }
    return false;
}

BOOST_AUTO_TEST_SUITE(ring_signature_tests)

BOOST_AUTO_TEST_CASE(ring_signature__basic_test)
{
    for (const auto& test: ring_signature_test_vectors)
    {
        ring_signature signature;
        signature.s = test.s;

        BOOST_REQUIRE(sign(
            signature, test.secrets, test.public_rings, test.message, test.k));

        BOOST_REQUIRE(std::equal(
            signature.e.begin(), signature.e.end(), test.e.begin()));

        BOOST_REQUIRE(verify(test.public_rings, test.message, signature));
    }
}

/*
BOOST_AUTO_TEST_CASE(ring_signature__basic_test111)
{
    // Generate new random secret keys
    secret_list secrets;
    for (size_t i = 0; i < 8; ++i)
        secrets.push_back(new_key());

    // Create two rings from the first 4 and last 4 keys
    auto ring0 = extract_public_keys(
        secrets.begin(), secrets.begin() + 4);
    auto ring1 = extract_public_keys(
        secrets.begin() + 4, secrets.end());
    BOOST_REQUIRE_EQUAL(ring0.size(), 4);
    BOOST_REQUIRE_EQUAL(ring1.size(), 4);
    const key_rings rings{ring0, ring1};

    // Randomly choose two random secret keys which exist in both rings
    // So one key from first 4, and one from last 4
    secret_list secret_keys;
    secret_keys.push_back(secrets[pseudo_random(0, 3)]);
    secret_keys.push_back(secrets[pseudo_random(4, 7)]);
    BOOST_REQUIRE_EQUAL(secret_keys.size(), 2);
    BOOST_REQUIRE(is_secret_in_ring(secret_keys[0], ring0));
    BOOST_REQUIRE(is_secret_in_ring(secret_keys[1], ring1));

    std::string message = "hello world!";
    const auto message_data = to_chunk(message);

    ring_signature signature;
    const auto seed = new_seed();
    BOOST_REQUIRE(sign(signature, secret_keys, rings, message_data, seed));

    BOOST_REQUIRE(verify(rings, message_data, signature));

    BOOST_REQUIRE_EQUAL(signature.s.size(), rings.size());
    for (size_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];
        BOOST_REQUIRE_EQUAL(signature.s[i].size(), ring.size());
        for (size_t j = 0; j < ring.size(); ++j)
        {
            ring_signature signature_copy = signature;
            if (pseudo_random(0, 1))
                BOOST_REQUIRE(ec_negate(signature_copy.s[i][j]));
            else
            {
                ec_secret one;
                std::fill(one.begin(), one.end(), 0);
                one.back() = 1;
                BOOST_REQUIRE(ec_add(signature_copy.s[i][j], one));
            }
            BOOST_REQUIRE(signature.s[i][j] != signature_copy.s[i][j]);
            BOOST_REQUIRE(!verify(rings, message_data, signature_copy));
        }
    }
}
*/

BOOST_AUTO_TEST_SUITE_END()

