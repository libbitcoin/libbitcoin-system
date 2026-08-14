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

BOOST_AUTO_TEST_SUITE(secp256k1_tests)

// bips.dev/324 packet encoding test vector 1.
const ec_secret vector1_secret = base16_array(
    "61062ea5071d800bbfd59e2e8b53d47d194b095ae5a4df04936b49772ef0d4d7");
const ec_ellswift vector1_key_a = base16_array(
    "ec0adff257bbfe500c188c80b4fdd640f6b45a482bbc15fc7cef5931deff0aa1"
    "86f6eb9bba7b85dc4dcc28b28722de1e3d9108b985e2967045668f66098e475b");
const ec_ellswift vector1_key_b = base16_array(
    "a4a94dfce69b4a2a0a099313d10f9f7e7d649d60501c9e1d274c300e0d89aafa"
    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffff8faf88d5");
const hash_digest vector1_shared = base16_array(
    "c6992a117f5edbea70c3f511d32d26b9798be4b81a62eaee1a5acaa8459a3592");

// bips.dev/324 packet encoding test vector 2.
const ec_secret vector2_secret = base16_array(
    "6f312890ec83bbb26798abaadd574684a53e74ccef7953b790fcc29409080246");
const ec_ellswift vector2_key_a = base16_array(
    "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"
    "0000000000000000000000000000000000000000000000000000000000000000");
const ec_ellswift vector2_key_b = base16_array(
    "a8785af31c029efc82fa9fc677d7118031358d7c6a25b5779a9b900e5ccd94aa"
    "c97eb36a3c5dbcdb2ca5843cc4c2fe0aaa46d10eb3d233a81c3dde476da00eef");
const hash_digest vector2_shared = base16_array(
    "a6f79eb08243b6f65dbe42bfe4a6cf3f131d6963fa5d06c770a18f7b9c489b78");

// bitcoin/bips bip-0324/ellswift_decode_test_vectors.csv (rows 1-3).

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_decode__null_encoding__expected_x)
{
    const ec_ellswift key{};
    const auto expected_x = base16_array(
        "edd1fd3e327ce90cc7a3542614289aee9682003e9cf7dcc9cf2ca9743be5aa0c");

    ec_compressed point{};
    BOOST_REQUIRE(ellswift::decode(point, key));

    const auto x = slice<one, ec_compressed_size>(point);
    BOOST_REQUIRE_EQUAL(x, expected_x);
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_decode__vectors__expected_x)
{
    const ec_ellswift key1 = base16_array(
        "0000000000000000000000000000000000000000000000000000000000000000"
        "01d3475bf7655b0fb2d852921035b2ef607f49069b97454e6795251062741771");
    const auto expected1 = base16_array(
        "b5da00b73cd6560520e7c364086e7cd23a34bf60d0e707be9fc34d4cd5fdfa2c");

    const ec_ellswift key2 = base16_array(
        "0000000000000000000000000000000000000000000000000000000000000000"
        "82277c4a71f9d22e66ece523f8fa08741a7c0912c66a69ce68514bfd3515b49f");
    const auto expected2 = base16_array(
        "f482f2e241753ad0fb89150d8491dc1e34ff0b8acfbb442cfe999e2e5e6fd1d2");

    ec_compressed point{};
    BOOST_REQUIRE(ellswift::decode(point, key1));

    const auto x1 = slice<one, ec_compressed_size>(point);
    BOOST_REQUIRE_EQUAL(x1, expected1);
    BOOST_REQUIRE(ellswift::decode(point, key2));

    const auto x2 = slice<one, ec_compressed_size>(point);
    BOOST_REQUIRE_EQUAL(x2, expected2);
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_create__decode__expected_point)
{
    const ec_secret secret = base16_array(
        "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");

    ec_compressed expected{};
    BOOST_REQUIRE(secret_to_public(expected, secret));

    ec_ellswift key{};
    ec_compressed point{};
    BOOST_REQUIRE(ellswift::create(key, secret, {}));
    BOOST_REQUIRE(ellswift::decode(point, key));
    BOOST_REQUIRE_EQUAL(point, expected);
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_create__zero_secret__false)
{
    ec_ellswift key{};
    BOOST_REQUIRE(!ellswift::create(key, {}, {}));
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_exchange__initiating__expected)
{
    hash_digest secret{};
    BOOST_REQUIRE(ellswift::exchange(secret, vector1_secret, vector1_key_a, vector1_key_b, false));
    BOOST_REQUIRE_EQUAL(secret, vector1_shared);
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_exchange__responding__expected)
{
    hash_digest secret{};
    BOOST_REQUIRE(ellswift::exchange(secret, vector2_secret, vector2_key_a, vector2_key_b, true));
    BOOST_REQUIRE_EQUAL(secret, vector2_shared);
}

BOOST_AUTO_TEST_CASE(secp256k1__ellswift_exchange__both_parties__same_secret)
{
    const ec_secret secret_a = base16_array(
        "0000000000000000000000000000000000000000000000000000000000000001");
    const ec_secret secret_b = base16_array(
        "0000000000000000000000000000000000000000000000000000000000000002");

    ec_ellswift key_a{};
    ec_ellswift key_b{};
    BOOST_REQUIRE(ellswift::create(key_a, secret_a, {}));
    BOOST_REQUIRE(ellswift::create(key_b, secret_b, {}));

    hash_digest shared_a{};
    hash_digest shared_b{};
    BOOST_REQUIRE(ellswift::exchange(shared_a, secret_a, key_a, key_b, false));
    BOOST_REQUIRE(ellswift::exchange(shared_b, secret_b, key_a, key_b, true));
    BOOST_REQUIRE_EQUAL(shared_a, shared_b);
}

BOOST_AUTO_TEST_SUITE_END()
