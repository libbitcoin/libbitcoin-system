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
#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE(private_key_tests)

using namespace x509;
using namespace x509::der;
using reader = x509::der::reader;
using writer = x509::der::writer;

const std::string password{ "libbitcoin" };

static data_chunk chunk(const_byte_span bytes)
{
    return { bytes.begin(), bytes.end() };
}

static data_chunk first_block(const std::string& text)
{
    pems blocks{};
    decode_pem(blocks, text);
    return blocks.front().data;
}

static data_chunk wrapped(const writer& content)
{
    writer out{};
    out.write_nested(sequence_tag, content);
    return out.data();
}

// EncryptedPrivateKeyInfo fields.
struct pbes2
{
    data_chunk scheme;
    data_chunk function;
    data_chunk salt;
    uint64_t iterations;
    uint64_t key_length;
    data_chunk prf;
    data_chunk cipher;
    data_chunk iv;
    data_chunk encrypted;
};

// Split an openssl EncryptedPrivateKeyInfo (no key length, null prf params).
static pbes2 split(const data_chunk& der)
{
    reader outer{ der };
    auto info = outer.read_nested(sequence_tag);
    auto algorithm = info.read_nested(sequence_tag);
    const auto scheme = algorithm.read_oid();
    auto parameters = algorithm.read_nested(sequence_tag);
    auto derivation = parameters.read_nested(sequence_tag);
    const auto function = derivation.read_oid();
    auto settings = derivation.read_nested(sequence_tag);
    const auto salt = settings.read_octet_string();
    const auto iterations = settings.read_unsigned();
    auto prf = settings.read_nested(sequence_tag);
    const auto hmac = prf.read_oid();
    auto encryption = parameters.read_nested(sequence_tag);
    const auto cipher = encryption.read_oid();
    const auto iv = encryption.read_octet_string();
    const auto encrypted = info.read_octet_string();
    return { chunk(scheme), chunk(function), chunk(salt), iterations, 0, chunk(hmac), chunk(cipher), chunk(iv), chunk(encrypted) };
}

// Join the fields, with key length only if nonzero and prf only if nonempty.
static data_chunk join(const pbes2& fields)
{
    writer prf{};
    prf.write_oid(fields.prf);
    prf.write_null();

    writer settings{};
    settings.write_octet_string(fields.salt);
    settings.write_unsigned(fields.iterations);
    if (!is_zero(fields.key_length))
        settings.write_unsigned(fields.key_length);

    if (!fields.prf.empty())
        settings.write_nested(sequence_tag, prf);

    writer derivation{};
    derivation.write_oid(fields.function);
    derivation.write_nested(sequence_tag, settings);

    writer encryption{};
    encryption.write_oid(fields.cipher);
    encryption.write_octet_string(fields.iv);

    writer parameters{};
    parameters.write_nested(sequence_tag, derivation);
    parameters.write_nested(sequence_tag, encryption);

    writer algorithm{};
    algorithm.write_oid(fields.scheme);
    algorithm.write_nested(sequence_tag, parameters);

    writer info{};
    info.write_nested(sequence_tag, algorithm);
    info.write_octet_string(fields.encrypted);
    return wrapped(info);
}

// SEC1 ECPrivateKey with optional (nonempty) parameters and public key.
static data_chunk sec1(uint64_t version, const_byte_span key, const_byte_span curve, const_byte_span point)
{
    writer parameters{};
    parameters.write_oid(curve);

    writer public_key{};
    public_key.write_bit_string(point);

    writer body{};
    body.write_unsigned(version);
    body.write_octet_string(key);
    if (!curve.empty())
        body.write_nested(explicit_tag(0), parameters);

    if (!point.empty())
        body.write_nested(explicit_tag(1), public_key);

    return wrapped(body);
}

// PKCS8 PrivateKeyInfo with the key octets and optional attributes.
static data_chunk pkcs8(uint64_t version, const_byte_span curve, const_byte_span key, bool attributes)
{
    writer algorithm{};
    algorithm.write_oid(oid::ec_public_key);
    algorithm.write_oid(curve);

    writer body{};
    body.write_unsigned(version);
    body.write_nested(sequence_tag, algorithm);
    body.write_octet_string(key);
    if (attributes)
        body.write_nested(explicit_tag(0), writer{});

    return wrapped(body);
}

static secp256r1::point_t fixture_point()
{
    secp256r1::point_t point{};
    secp256r1::public_key(point, fixture::key_secret);
    return point;
}

// decode_private_key (openssl fixtures)

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__sec1__expected)
{
    secret out{};
    BOOST_REQUIRE(decode_private_key(out, fixture::key_pem));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__pkcs8__expected)
{
    secret out{};
    BOOST_REQUIRE(decode_private_key(out, fixture::key_pkcs8_pem));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__pbes2_aes256__expected)
{
    secret out{};
    BOOST_REQUIRE(decode_private_key(out, fixture::key_aes256_pem, password));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__pbes2_aes128__expected)
{
    secret out{};
    BOOST_REQUIRE(decode_private_key(out, fixture::key_aes128_pem, password));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__certificate_first__skipped)
{
    secret out{};
    BOOST_REQUIRE(decode_private_key(out, fixture::self_pem + fixture::key_pem));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__no_key__false)
{
    secret out{};
    BOOST_REQUIRE(!decode_private_key(out, fixture::self_pem));
}

BOOST_AUTO_TEST_CASE(private_key__decode_private_key__malformed_pem__false)
{
    secret out{};
    BOOST_REQUIRE(!decode_private_key(out, "-----BEGIN PRIVATE KEY-----\n"));
}

// decode_sec1

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__minimal__expected)
{
    const auto der = sec1(1, fixture::key_secret, {}, {});
    secret out{};
    BOOST_REQUIRE(decode_sec1(out, der));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__version_two__false)
{
    const auto der = sec1(2, fixture::key_secret, {}, {});
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__short_key__false)
{
    const auto der = sec1(1, base16_chunk("01"), {}, {});
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__secp384r1_parameters__false)
{
    const auto der = sec1(1, fixture::key_secret, oid::secp384r1, {});
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__zero_key__false)
{
    const auto der = sec1(1, secret{}, {}, {});
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__mismatched_public_key__false)
{
    auto point = fixture_point();
    point.back() ^= 0x01;
    const auto der = sec1(1, fixture::key_secret, oid::secp256r1, point);
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__malformed_public_key__false)
{
    writer wrapper{};
    wrapper.write_null();

    writer body{};
    body.write_unsigned(1);
    body.write_octet_string(fixture::key_secret);
    body.write_nested(explicit_tag(1), wrapper);

    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, wrapped(body)));
}

BOOST_AUTO_TEST_CASE(private_key__decode_sec1__trailing_data__false)
{
    const auto der = splice(sec1(1, fixture::key_secret, {}, {}), base16_chunk("0500"));
    secret out{};
    BOOST_REQUIRE(!decode_sec1(out, der));
}

// decode_pkcs8

BOOST_AUTO_TEST_CASE(private_key__decode_pkcs8__attributes__expected)
{
    const auto key = sec1(1, fixture::key_secret, {}, {});
    const auto der = pkcs8(0, oid::secp256r1, key, true);
    secret out{};
    BOOST_REQUIRE(decode_pkcs8(out, der));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_pkcs8__version_one__false)
{
    const auto key = sec1(1, fixture::key_secret, {}, {});
    const auto der = pkcs8(1, oid::secp256r1, key, false);
    secret out{};
    BOOST_REQUIRE(!decode_pkcs8(out, der));
}

BOOST_AUTO_TEST_CASE(private_key__decode_pkcs8__secp384r1__false)
{
    const auto key = sec1(1, fixture::key_secret, {}, {});
    const auto der = pkcs8(0, oid::secp384r1, key, false);
    secret out{};
    BOOST_REQUIRE(!decode_pkcs8(out, der));
}

// decode_encrypted_pkcs8

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__joined__expected)
{
    const auto fields = split(first_block(fixture::key_aes256_pem));
    secret out{};
    BOOST_REQUIRE(decode_encrypted_pkcs8(out, join(fields), password));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__matching_key_length__expected)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.key_length = 32;
    secret out{};
    BOOST_REQUIRE(decode_encrypted_pkcs8(out, join(fields), password));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__mismatched_key_length__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.key_length = 16;
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__default_prf__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.prf.clear();
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__unsupported_prf__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.prf = to_chunk(oid::ecdsa_sha256);
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__unsupported_scheme__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.scheme = to_chunk(oid::pbkdf2);
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__unsupported_cipher__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.cipher = to_chunk(oid::hmac_sha256);
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__short_iv__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.iv.pop_back();
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__partial_block__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.encrypted.pop_back();
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__zero_iterations__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.iterations = 0;
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__excessive_iterations__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.iterations = add1<uint64_t>(max_uint32);
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__zero_padding__false)
{
    // The last pad byte (0x06) is changed through the preceding cipher block.
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.encrypted.at(fields.encrypted.size() - 17u) ^= 0x06;
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__inconsistent_padding__false)
{
    auto fields = split(first_block(fixture::key_aes256_pem));
    fields.encrypted.at(fields.encrypted.size() - 17u) ^= 0x04;
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, join(fields), password));
}

BOOST_AUTO_TEST_CASE(private_key__decode_encrypted_pkcs8__wrong_password__false)
{
    secret out{};
    BOOST_REQUIRE(!decode_encrypted_pkcs8(out, first_block(fixture::key_aes128_pem), "wrong"));
}

// encode

BOOST_AUTO_TEST_CASE(private_key__encode_sec1__fixture__round_trip)
{
    const auto der = encode_sec1(fixture::key_secret);
    BOOST_REQUIRE_EQUAL(der, sec1(1, fixture::key_secret, oid::secp256r1, fixture_point()));

    secret out{};
    BOOST_REQUIRE(decode_sec1(out, der));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__encode_sec1__zero__empty)
{
    BOOST_REQUIRE(encode_sec1(secret{}).empty());
}

BOOST_AUTO_TEST_CASE(private_key__encode_pkcs8__fixture__round_trip)
{
    const auto der = encode_pkcs8(fixture::key_secret);
    secret out{};
    BOOST_REQUIRE(decode_pkcs8(out, der));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__encode_pkcs8__zero__empty)
{
    BOOST_REQUIRE(encode_pkcs8(secret{}).empty());
}

BOOST_AUTO_TEST_CASE(private_key__encode_private_key__fixture__round_trip)
{
    const auto text = encode_private_key(fixture::key_secret);
    BOOST_REQUIRE(text.starts_with("-----BEGIN PRIVATE KEY-----\n"));

    secret out{};
    BOOST_REQUIRE(decode_private_key(out, text));
    BOOST_REQUIRE_EQUAL(out, fixture::key_secret);
}

BOOST_AUTO_TEST_CASE(private_key__encode_private_key__zero__empty)
{
    BOOST_REQUIRE(encode_private_key(secret{}).empty());
}

BOOST_AUTO_TEST_SUITE_END()
