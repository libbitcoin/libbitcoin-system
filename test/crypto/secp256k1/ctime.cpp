/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#if __has_include(<valgrind/memcheck.h>)
    #include <valgrind/memcheck.h>
    #define SECRET(value) VALGRIND_MAKE_MEM_UNDEFINED(&(value), sizeof(value))
    #define PUBLIC(value) VALGRIND_MAKE_MEM_DEFINED(&(value), sizeof(value))
#else
    #define SECRET(value)
    #define PUBLIC(value)
#endif

BOOST_AUTO_TEST_SUITE(secp256k1_ctime_tests)

// Under valgrind a secret is undefined, so a branch or memory address that
// depends on it is reported as an error, and each output is defined before it
// is compared to the same computation on the defined secret.

const ec_secret secret1 = base16_array("8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
const ec_secret secret2 = base16_array("33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce");
const hash_digest message = base16_hash("f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0");
const ec_ellswift key_a = base16_array("ec0adff257bbfe500c188c80b4fdd640f6b45a482bbc15fc7cef5931deff0aa186f6eb9bba7b85dc4dcc28b28722de1e3d9108b985e2967045668f66098e475b");
const ec_ellswift key_b = base16_array("a4a94dfce69b4a2a0a099313d10f9f7e7d649d60501c9e1d274c300e0d89aafaffffffffffffffffffffffffffffffffffffffffffffffffffffffff8faf88d5");

BOOST_AUTO_TEST_CASE(secp256k1_ctime__secret_to_public__undefined_secret__expected)
{
    ec_compressed expected{};
    BOOST_REQUIRE(secret_to_public(expected, secret1));

    auto secret = secret1;
    SECRET(secret);
    ec_compressed point{};
    auto result = secret_to_public(point, secret);
    PUBLIC(result);
    PUBLIC(point);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(point, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ec_add_secret__undefined_secrets__expected)
{
    auto expected = secret1;
    BOOST_REQUIRE(ec_add(expected, secret2));

    auto left = secret1;
    auto right = secret2;
    SECRET(left);
    SECRET(right);
    auto result = ec_add(left, right);
    PUBLIC(result);
    PUBLIC(left);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(left, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ec_add_point__undefined_secret__expected)
{
    ec_compressed expected{};
    BOOST_REQUIRE(secret_to_public(expected, secret1));
    BOOST_REQUIRE(ec_add(expected, secret2));

    auto tweak = secret2;
    SECRET(tweak);
    ec_compressed point{};
    BOOST_REQUIRE(secret_to_public(point, secret1));
    auto result = ec_add(point, tweak);
    PUBLIC(result);
    PUBLIC(point);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(point, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ec_multiply_secret__undefined_secrets__expected)
{
    auto expected = secret1;
    BOOST_REQUIRE(ec_multiply(expected, secret2));

    auto left = secret1;
    auto right = secret2;
    SECRET(left);
    SECRET(right);
    auto result = ec_multiply(left, right);
    PUBLIC(result);
    PUBLIC(left);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(left, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ec_multiply_point__undefined_secret__expected)
{
    ec_compressed expected{};
    BOOST_REQUIRE(secret_to_public(expected, secret1));
    BOOST_REQUIRE(ec_multiply(expected, secret2));

    auto tweak = secret2;
    SECRET(tweak);
    ec_compressed point{};
    BOOST_REQUIRE(secret_to_public(point, secret1));
    auto result = ec_multiply(point, tweak);
    PUBLIC(result);
    PUBLIC(point);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(point, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ecdsa_sign__undefined_secret__expected)
{
    ec_signature expected{};
    BOOST_REQUIRE(ecdsa::sign(expected, secret2, message));

    auto secret = secret2;
    SECRET(secret);
    ec_signature signature{};
    auto result = ecdsa::sign(signature, secret, message);
    PUBLIC(result);
    PUBLIC(signature);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(signature, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ecdsa_sign_recoverable__undefined_secret__expected)
{
    recoverable_signature expected{};
    BOOST_REQUIRE(ecdsa::sign_recoverable(expected, secret2, message));

    auto secret = secret2;
    SECRET(secret);
    recoverable_signature signature{};
    auto result = ecdsa::sign_recoverable(signature, secret, message);
    PUBLIC(result);
    PUBLIC(signature);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(signature.signature, expected.signature);
    BOOST_REQUIRE_EQUAL(signature.recovery_id, expected.recovery_id);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__schnorr_sign__undefined_secret__expected)
{
    ec_signature expected{};
    BOOST_REQUIRE(schnorr::sign(expected, secret2, message, message));

    auto secret = secret2;
    SECRET(secret);
    ec_signature signature{};
    auto result = schnorr::sign(signature, secret, message, message);
    PUBLIC(result);
    PUBLIC(signature);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(signature, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ellswift_create__undefined_secret__expected)
{
    ec_ellswift expected{};
    BOOST_REQUIRE(ellswift::create(expected, secret1, message));

    auto secret = secret1;
    SECRET(secret);
    ec_ellswift key{};
    auto result = ellswift::create(key, secret, message);
    PUBLIC(result);
    PUBLIC(key);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(key, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1_ctime__ellswift_exchange__undefined_secret__expected)
{
    hash_digest expected{};
    BOOST_REQUIRE(ellswift::exchange(expected, secret1, key_a, key_b, false));

    auto secret = secret1;
    SECRET(secret);
    hash_digest shared{};
    auto result = ellswift::exchange(shared, secret, key_a, key_b, false);
    PUBLIC(result);
    PUBLIC(shared);
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(shared, expected);
}

BOOST_AUTO_TEST_SUITE_END()
