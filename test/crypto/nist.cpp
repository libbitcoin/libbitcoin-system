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
#include "ecdh.hpp"
#include "ecdsa.hpp"

// datatracker.ietf.org/doc/html/rfc6979 (A.2.5, A.2.6)

BOOST_AUTO_TEST_SUITE(nist_tests)

static data_chunk sha384(const data_chunk& data)
{
    return to_chunk(accumulator<sha512_384>::hash(data));
}

// p-256

constexpr auto p256_secret = base16_array("c9afa9d845ba75166b5c215767b1d6934e50c3db36e89b127b8a622b120f6721");
constexpr auto p256_point = base16_array("0460fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb67903fe1008b8bc99a41ae9e95628bc64f2f1b20c2d7e9f5177a3c294d4462299");
constexpr auto p256_generator = base16_array("046b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c2964fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
constexpr auto p256_negated_generator = base16_array("046b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296b01cbd1c01e58065711814b583f061e9d431cca994cea1313449bf97c840ae0a");

BOOST_AUTO_TEST_CASE(nist__secp256r1_public_key__one__generator)
{
    constexpr auto secret = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::public_key(point, secret));
    BOOST_REQUIRE_EQUAL(point, p256_generator);
    BOOST_REQUIRE(secp256r1::is_valid(point));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_public_key__order_minus_one__negated_generator)
{
    constexpr auto secret = base16_array("ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632550");
    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::public_key(point, secret));
    BOOST_REQUIRE_EQUAL(point, p256_negated_generator);
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_public_key__zero_or_order__false)
{
    constexpr secp256r1::secret_t zero{};
    constexpr auto order = base16_array("ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
    secp256r1::point_t point{};
    BOOST_REQUIRE(!secp256r1::public_key(point, zero));
    BOOST_REQUIRE(!secp256r1::public_key(point, order));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_public_key__rfc6979__expected)
{
    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::public_key(point, p256_secret));
    BOOST_REQUIRE_EQUAL(point, p256_point);
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_sign__rfc6979_sample__expected)
{
    const auto digest = sha256_hash(to_chunk("sample"));
    const auto expected = base16_array("efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8");

    secp256r1::signature_t signature{};
    BOOST_REQUIRE(secp256r1::sign(signature, p256_secret, digest));
    BOOST_REQUIRE_EQUAL(signature, expected);
    BOOST_REQUIRE(secp256r1::verify(signature, p256_point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_sign__rfc6979_test__expected)
{
    const auto digest = sha256_hash(to_chunk("test"));
    const auto expected = base16_array("f1abb023518351cd71d881567b1ea663ed3efcf6c5132b354f28d3b0b7d38367019f4113742a2b14bd25926b49c649155f267e60d3814b4c0cc84250e46f0083");

    secp256r1::signature_t signature{};
    BOOST_REQUIRE(secp256r1::sign(signature, p256_secret, digest));
    BOOST_REQUIRE_EQUAL(signature, expected);
    BOOST_REQUIRE(secp256r1::verify(signature, p256_point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_verify__rfc6979_sha384_sample__true)
{
    const auto signature = base16_array("0eafea039b20e9b42309fb1d89e213057cbf973dc0cfc8f129edddc800ef77194861f0491e6998b9455193e34e7b0d284ddd7149a74b95b9261f13abde940954");
    BOOST_REQUIRE(secp256r1::verify(signature, p256_point, sha384(to_chunk("sample"))));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_verify__changed_digest__false)
{
    const auto signature = base16_array("efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8");
    BOOST_REQUIRE(!secp256r1::verify(signature, p256_point, sha256_hash(to_chunk("samples"))));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_verify__invalid_point__false)
{
    const auto digest = sha256_hash(to_chunk("sample"));
    const auto signature = base16_array("efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8");
    auto point = p256_point;
    point.back() ^= 0x01;
    BOOST_REQUIRE(!secp256r1::is_valid(point));
    BOOST_REQUIRE(!secp256r1::verify(signature, point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_is_valid__compressed_prefix__false)
{
    auto point = p256_point;
    point.front() = 0x02;
    BOOST_REQUIRE(!secp256r1::is_valid(point));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_sign__zero_secret__false)
{
    constexpr secp256r1::secret_t zero{};
    secp256r1::signature_t signature{};
    BOOST_REQUIRE(!secp256r1::sign(signature, zero, sha256_hash(to_chunk("sample"))));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_generate__sign_verify__true)
{
    const auto digest = sha256_hash(to_chunk("sample"));
    const auto secret = secp256r1::generate();
    secp256r1::point_t point{};
    secp256r1::signature_t signature{};
    BOOST_REQUIRE(secp256r1::public_key(point, secret));
    BOOST_REQUIRE(secp256r1::sign(signature, secret, digest));
    BOOST_REQUIRE(secp256r1::verify(signature, point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_compress_decompress__rfc6979__round_trip)
{
    secp256r1::compressed_t compressed{};
    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::compress(compressed, p256_point));
    BOOST_REQUIRE_EQUAL(compressed, base16_array("0360fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb6"));

    BOOST_REQUIRE(secp256r1::decompress(point, compressed));
    BOOST_REQUIRE_EQUAL(point, p256_point);
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_compress_decompress__generator__round_trip)
{
    secp256r1::compressed_t compressed{};
    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::compress(compressed, p256_generator));
    BOOST_REQUIRE_EQUAL(compressed, base16_array("036b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296"));

    BOOST_REQUIRE(secp256r1::decompress(point, compressed));
    BOOST_REQUIRE_EQUAL(point, p256_generator);
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_decompress__invalid__false)
{
    constexpr auto not_on_curve = base16_array("020000000000000000000000000000000000000000000000000000000000000001");
    constexpr auto bad_prefix = base16_array("0460fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb6");
    constexpr auto not_field = base16_array("02ffffffff00000001000000000000000000000000ffffffffffffffffffffffff");
    secp256r1::point_t point{};
    BOOST_REQUIRE(!secp256r1::decompress(point, not_on_curve));
    BOOST_REQUIRE(!secp256r1::decompress(point, bad_prefix));
    BOOST_REQUIRE(!secp256r1::decompress(point, not_field));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_compress__invalid__false)
{
    auto point = p256_point;
    point.front() = 0x05;
    secp256r1::compressed_t compressed{};
    BOOST_REQUIRE(!secp256r1::compress(compressed, point));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_decode__wycheproof_valid__verified_canonical)
{
    for (const auto& vector: secp256r1_sha256_valid_vectors)
    {
        secp256r1::signature_t signature{};
        BOOST_REQUIRE_MESSAGE(secp256r1::decode(signature, vector.signature), vector.id);
        BOOST_REQUIRE_MESSAGE(secp256r1::encode(signature) == vector.signature, vector.id);
        BOOST_REQUIRE_MESSAGE(secp256r1::verify(signature, vector.key, sha256_hash(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_decode__wycheproof_invalid__not_verified)
{
    for (const auto& vector: secp256r1_sha256_invalid_vectors)
    {
        secp256r1::signature_t signature{};
        secp256r1::decode(signature, vector.signature);
        BOOST_REQUIRE_MESSAGE(!secp256r1::verify(signature, vector.key, sha256_hash(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_agree__wycheproof_valid__expected)
{
    for (const auto& vector: secp256r1_ecdh_valid_vectors)
    {
        secp256r1::shared_t shared{};
        BOOST_REQUIRE_MESSAGE(secp256r1::agree(shared, vector.secret, vector.point), vector.id);
        BOOST_REQUIRE_MESSAGE(shared == vector.shared, vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_agree__wycheproof_invalid__false)
{
    for (const auto& vector: secp256r1_ecdh_invalid_vectors)
    {
        secp256r1::shared_t shared{};
        BOOST_REQUIRE_MESSAGE(!secp256r1::agree(shared, vector.secret, vector.point), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_agree__zero_secret__false)
{
    constexpr secp256r1::secret_t zero{};
    secp256r1::shared_t shared{};
    BOOST_REQUIRE(!secp256r1::agree(shared, zero, p256_generator));
}

BOOST_AUTO_TEST_CASE(nist__secp256r1_agree__generated_pair__same_shared)
{
    const auto secret1 = secp256r1::generate();
    const auto secret2 = secp256r1::generate();
    secp256r1::point_t point1{};
    secp256r1::point_t point2{};
    BOOST_REQUIRE(secp256r1::public_key(point1, secret1));
    BOOST_REQUIRE(secp256r1::public_key(point2, secret2));

    secp256r1::shared_t shared1{};
    secp256r1::shared_t shared2{};
    BOOST_REQUIRE(secp256r1::agree(shared1, secret1, point2));
    BOOST_REQUIRE(secp256r1::agree(shared2, secret2, point1));
    BOOST_REQUIRE_EQUAL(shared1, shared2);
}

// p-384

constexpr auto p384_secret = base16_array("6b9d3dad2e1b8c1c05b19875b6659f4de23c3b667bf297ba9aa47740787137d896d5724e4c70a825f872c9ea60d2edf5");
constexpr auto p384_point = base16_array("04ec3a4e415b4e19a4568618029f427fa5da9a8bc4ae92e02e06aae5286b300c64def8f0ea9055866064a254515480bc138015d9b72d7d57244ea8ef9ac0c621896708a59367f9dfb9f54ca84b3f1c9db1288b231c3ae0d4fe7344fd2533264720");
constexpr auto p384_generator = base16_array("04aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab73617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f");

BOOST_AUTO_TEST_CASE(nist__secp384r1_public_key__one__generator)
{
    constexpr auto secret = base16_array("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
    secp384r1::point_t point{};
    BOOST_REQUIRE(secp384r1::public_key(point, secret));
    BOOST_REQUIRE_EQUAL(point, p384_generator);
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_public_key__rfc6979__expected)
{
    secp384r1::point_t point{};
    BOOST_REQUIRE(secp384r1::public_key(point, p384_secret));
    BOOST_REQUIRE_EQUAL(point, p384_point);
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_sign__rfc6979_sample__expected)
{
    const auto digest = sha384(to_chunk("sample"));
    const auto expected = base16_array("94edbb92a5ecb8aad4736e56c691916b3f88140666ce9fa73d64c4ea95ad133c81a648152e44acf96e36dd1e80fabe4699ef4aeb15f178cea1fe40db2603138f130e740a19624526203b6351d0a3a94fa329c145786e679e7b82c71a38628ac8");

    secp384r1::signature_t signature{};
    BOOST_REQUIRE(secp384r1::sign(signature, p384_secret, digest));
    BOOST_REQUIRE_EQUAL(signature, expected);
    BOOST_REQUIRE(secp384r1::verify(signature, p384_point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_sign__rfc6979_test__expected)
{
    const auto digest = sha384(to_chunk("test"));
    const auto expected = base16_array("8203b63d3c853e8d77227fb377bcf7b7b772e97892a80f36ab775d509d7a5feb0542a7f0812998da8f1dd3ca3cf023dbddd0760448d42d8a43af45af836fce4de8be06b485e9b61b827c2f13173923e06a739f040649a667bf3b828246baa5a5");

    secp384r1::signature_t signature{};
    BOOST_REQUIRE(secp384r1::sign(signature, p384_secret, digest));
    BOOST_REQUIRE_EQUAL(signature, expected);
    BOOST_REQUIRE(secp384r1::verify(signature, p384_point, digest));
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_compress_decompress__rfc6979__round_trip)
{
    secp384r1::compressed_t compressed{};
    secp384r1::point_t point{};
    BOOST_REQUIRE(secp384r1::compress(compressed, p384_point));
    BOOST_REQUIRE(secp384r1::decompress(point, compressed));
    BOOST_REQUIRE_EQUAL(point, p384_point);
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_decode__wycheproof_sha384_valid__verified_canonical)
{
    for (const auto& vector: secp384r1_sha384_valid_vectors)
    {
        secp384r1::signature_t signature{};
        BOOST_REQUIRE_MESSAGE(secp384r1::decode(signature, vector.signature), vector.id);
        BOOST_REQUIRE_MESSAGE(secp384r1::encode(signature) == vector.signature, vector.id);
        BOOST_REQUIRE_MESSAGE(secp384r1::verify(signature, vector.key, sha384(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_decode__wycheproof_sha384_invalid__not_verified)
{
    for (const auto& vector: secp384r1_sha384_invalid_vectors)
    {
        secp384r1::signature_t signature{};
        secp384r1::decode(signature, vector.signature);
        BOOST_REQUIRE_MESSAGE(!secp384r1::verify(signature, vector.key, sha384(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_decode__wycheproof_sha256_valid__verified_canonical)
{
    for (const auto& vector: secp384r1_sha256_valid_vectors)
    {
        secp384r1::signature_t signature{};
        BOOST_REQUIRE_MESSAGE(secp384r1::decode(signature, vector.signature), vector.id);
        BOOST_REQUIRE_MESSAGE(secp384r1::encode(signature) == vector.signature, vector.id);
        BOOST_REQUIRE_MESSAGE(secp384r1::verify(signature, vector.key, sha256_hash(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_decode__wycheproof_sha256_invalid__not_verified)
{
    for (const auto& vector: secp384r1_sha256_invalid_vectors)
    {
        secp384r1::signature_t signature{};
        secp384r1::decode(signature, vector.signature);
        BOOST_REQUIRE_MESSAGE(!secp384r1::verify(signature, vector.key, sha256_hash(vector.message)), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_agree__wycheproof_valid__expected)
{
    for (const auto& vector: secp384r1_ecdh_valid_vectors)
    {
        secp384r1::shared_t shared{};
        BOOST_REQUIRE_MESSAGE(secp384r1::agree(shared, vector.secret, vector.point), vector.id);
        BOOST_REQUIRE_MESSAGE(shared == vector.shared, vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_agree__wycheproof_invalid__false)
{
    for (const auto& vector: secp384r1_ecdh_invalid_vectors)
    {
        secp384r1::shared_t shared{};
        BOOST_REQUIRE_MESSAGE(!secp384r1::agree(shared, vector.secret, vector.point), vector.id);
    }
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_agree__zero_secret__false)
{
    constexpr secp384r1::secret_t zero{};
    secp384r1::shared_t shared{};
    BOOST_REQUIRE(!secp384r1::agree(shared, zero, p384_generator));
}

BOOST_AUTO_TEST_CASE(nist__secp384r1_agree__generated_pair__same_shared)
{
    const auto secret1 = secp384r1::generate();
    const auto secret2 = secp384r1::generate();
    secp384r1::point_t point1{};
    secp384r1::point_t point2{};
    BOOST_REQUIRE(secp384r1::public_key(point1, secret1));
    BOOST_REQUIRE(secp384r1::public_key(point2, secret2));

    secp384r1::shared_t shared1{};
    secp384r1::shared_t shared2{};
    BOOST_REQUIRE(secp384r1::agree(shared1, secret1, point2));
    BOOST_REQUIRE(secp384r1::agree(shared2, secret2, point1));
    BOOST_REQUIRE_EQUAL(shared1, shared2);
}

BOOST_AUTO_TEST_SUITE_END()
