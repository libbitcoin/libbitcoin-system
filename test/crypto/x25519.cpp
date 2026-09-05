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

BOOST_AUTO_TEST_SUITE(x25519_tests)

// rfc7748 section 6.1 (Diffie-Hellman).
const auto alice_secret = base16_array("77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
const auto alice_public = base16_array("8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a");
const auto bob_secret = base16_array("5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
const auto bob_public = base16_array("de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");
const auto shared_secret = base16_array("4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742");

BOOST_AUTO_TEST_CASE(x25519__multiply__alice_base__public_key)
{
    x25519::key out{};
    BOOST_REQUIRE(x25519::multiply(out, alice_secret));
    BOOST_REQUIRE_EQUAL(out, alice_public);
}

BOOST_AUTO_TEST_CASE(x25519__multiply__bob_base__public_key)
{
    x25519::key out{};
    BOOST_REQUIRE(x25519::multiply(out, bob_secret));
    BOOST_REQUIRE_EQUAL(out, bob_public);
}

BOOST_AUTO_TEST_CASE(x25519__multiply__alice_bob__shared_secret)
{
    x25519::key alice{};
    x25519::key bob{};
    BOOST_REQUIRE(x25519::multiply(alice, alice_secret, bob_public));
    BOOST_REQUIRE(x25519::multiply(bob, bob_secret, alice_public));
    BOOST_REQUIRE_EQUAL(alice, shared_secret);
    BOOST_REQUIRE_EQUAL(bob, shared_secret);
}

// rfc7748 section 5.2 (first vector).
BOOST_AUTO_TEST_CASE(x25519__multiply__rfc7748_vector_1__expected)
{
    const auto scalar = base16_array("a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4");
    const auto point = base16_array("e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c");
    const auto expected = base16_array("c3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552");

    x25519::key out{};
    BOOST_REQUIRE(x25519::multiply(out, scalar, point));
    BOOST_REQUIRE_EQUAL(out, expected);
}

// rfc7748 section 5.2 (second vector, point with high bit set).
BOOST_AUTO_TEST_CASE(x25519__multiply__rfc7748_vector_2__expected)
{
    const auto scalar = base16_array("4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d");
    const auto point = base16_array("e5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493");
    const auto expected = base16_array("95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957");

    x25519::key out{};
    BOOST_REQUIRE(x25519::multiply(out, scalar, point));
    BOOST_REQUIRE_EQUAL(out, expected);
}

// rfc7748 section 5.2 (one iteration from the base point).
BOOST_AUTO_TEST_CASE(x25519__multiply__rfc7748_iteration_1__expected)
{
    const x25519::key scalar{ 9 };
    const auto expected = base16_array("422c8e7a6227d7bca1350b3e2bb7279f7897b87bb6854b783c60e80311ae3079");

    x25519::key out{};
    BOOST_REQUIRE(x25519::multiply(out, scalar, scalar));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(x25519__multiply__zero_point__false)
{
    const x25519::key point{};
    x25519::key out{};
    BOOST_REQUIRE(!x25519::multiply(out, alice_secret, point));
    BOOST_REQUIRE_EQUAL(out, point);
}

BOOST_AUTO_TEST_CASE(x25519__generate__twice__distinct_consistent_keys)
{
    x25519::key secret1{};
    x25519::key public1{};
    x25519::key secret2{};
    x25519::key public2{};
    x25519::generate(secret1, public1);
    x25519::generate(secret2, public2);
    BOOST_REQUIRE_NE(secret1, secret2);
    BOOST_REQUIRE_NE(public1, public2);

    x25519::key derived{};
    BOOST_REQUIRE(x25519::multiply(derived, secret1));
    BOOST_REQUIRE_EQUAL(derived, public1);
}

BOOST_AUTO_TEST_SUITE_END()
