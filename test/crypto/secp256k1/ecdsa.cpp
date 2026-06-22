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

// These vectors use hash encoding for sighash values.

// scenario 1
const ec_secret secret1 = base16_array(
    "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
////const ec_compressed compressed1 = base16_array(
////    "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731");
////const ec_uncompressed uncompressed1 = base16_array(
////    "0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc73273"
////    "18c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9");

// scenario 2
const ec_compressed compressed2 = base16_array(
    "03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b");
const hash_digest sighash2 = base16_hash(
    "ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f");
const der_signature der_signature2 = base16_chunk(
    "3045022100bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cd"
    "a33a62437802206b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f6b8bb67d");

// scenario 3
// signature3 is the private format low-s ECDSA signature associated with der_signature3.
const ec_secret secret3 = base16_array(
    "33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce");
const hash_digest sighash3 = base16_hash(
    "f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0");
const der_signature der_signature3 = base16_chunk(
    "3044022057d0b4fb0df5cefa245e76ba9b099bb63aa46340cb152a927c1cb3f8"
    "befe324802203750eae5727db3e6cc4275062971552e467f7cfd7269fec2626588b6c6d58e92");
const ec_signature signature3 = base16_array(
    "4832febef8b31c7c922a15cb4063a43ab69b099bba765e24facef50dfbb4d057"
    "928ed5c6b6886562c2fe6972fd7c7f462e557129067542cce6b37d72e5ea5037");

BOOST_AUTO_TEST_CASE(secp256k1__sign__positive__expected)
{
    using namespace system::ecdsa;
    ec_signature signature;
    BOOST_REQUIRE(sign(signature, secret3, sighash3));
    BOOST_REQUIRE_EQUAL(signature, signature3);
}

BOOST_AUTO_TEST_CASE(secp256k1__encode_signature__positive__expected)
{
    using namespace system::ecdsa;
    der_signature signature;
    BOOST_REQUIRE(encode_signature(signature, signature3));
    BOOST_REQUIRE_EQUAL(signature, der_signature3);
}

BOOST_AUTO_TEST_CASE(secp256k1__sign__round_trip_positive__expected)
{
    using namespace system::ecdsa;
    ec_compressed point;
    BOOST_REQUIRE(secret_to_public(point, secret1));

    const auto hash = bitcoin_hash(to_chunk("data"));

    ec_signature signature;
    BOOST_REQUIRE(sign(signature, secret1, hash));
    BOOST_REQUIRE(verify_signature(point, hash, signature));
}

BOOST_AUTO_TEST_CASE(secp256k1__sign__round_trip_negative__expected)
{
    using namespace system::ecdsa;
    ec_compressed point;
    BOOST_REQUIRE(secret_to_public(point, secret1));

    auto hash = bitcoin_hash(to_chunk("data"));

    ec_signature signature;
    BOOST_REQUIRE(sign(signature, secret1, hash));

    // Invalidate the positive test.
    hash[0] = 0;

    BOOST_REQUIRE(!verify_signature(to_chunk(point), hash, signature));
}

BOOST_AUTO_TEST_CASE(secp256k1__verify_signature__positive__expected)
{
    using namespace system::ecdsa;
    ec_signature signature;
    BOOST_REQUIRE(decode_signature(signature, der_signature2, false));
    BOOST_REQUIRE(verify_signature(compressed2, sighash2, signature));
}

BOOST_AUTO_TEST_CASE(secp256k1__verify_signature__negative__expected)
{
    using namespace system::ecdsa;
    ec_signature signature;
    BOOST_REQUIRE(decode_signature(signature, der_signature2, false));

    // Invalidate the positive test.
    signature[10] = 110;
    BOOST_REQUIRE(!verify_signature(compressed2, sighash2, signature));
}

BOOST_AUTO_TEST_CASE(secp256k1__verify_signature__block_704789__expected)
{
    using namespace system::ecdsa;
    constexpr bool bip66 = true;

    const hash_digest sighash = base16_array(
        "504d68beac187dd0b259ddd6ed6d5d6348150b9b23ee6dfdb43e87f74dd3c547");
    const ec_compressed compressed = base16_array(
        "039cfcfe4a5d0efad27382e5d2b478eb398a8b691a66e01c878b600b5042b33166");
    const der_signature der_signature = base16_chunk(
        "3044022043f141440d4cd28cae8903cb59ce706763fff70d746d13e1713dd620c7c734"
        "b4022008dac3365610ad1a3f78fd989ea60d81c6b2e4c8ecae3f68b4326fc3dbe1f401");
    const ec_signature expected_signature = base16_array(
        "b434c7c720d63d71e1136d740df7ff636770ce59cb0389ae8cd24c0d4441f143"
        "01f4e1dbc36f32b4683faeecc8e4b2c6810da69e98fd783f1aad105636c3da08");

    ec_signature signature1{};
    BOOST_CHECK(decode_signature(signature1, der_signature, bip66));
    BOOST_CHECK_EQUAL(signature1, expected_signature);

    ec_signature signature2{};
    BOOST_CHECK(decode_signature(signature2, der_signature, !bip66));
    BOOST_CHECK_EQUAL(signature2, expected_signature);

    BOOST_CHECK(verify_signature(compressed, sighash, expected_signature));
}

BOOST_AUTO_TEST_SUITE_END()
