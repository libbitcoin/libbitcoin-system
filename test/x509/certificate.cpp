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

BOOST_AUTO_TEST_SUITE(certificate_tests)

using namespace x509;
using namespace x509::der;
using reader = x509::der::reader;
using writer = x509::der::writer;

const auto no_flag = data_chunk{};
const auto critical_flag = base16_chunk("0101ff");
const auto noncritical_flag = base16_chunk("010100");

static data_chunk chunk(const_byte_span bytes)
{
    return { bytes.begin(), bytes.end() };
}

static data_chunk wrap(uint8_t tag, const data_chunk& content)
{
    writer out{};
    out.write(tag, content);
    return out.data();
}

static data_chunk first_block(const std::string& text)
{
    pems blocks{};
    decode_pem(blocks, text);
    return blocks.front().data;
}

static certificate parsed(const std::string& text)
{
    certificate out{};
    parse(out, first_block(text));
    return out;
}

// Certificate fields as encodings (signature as bit string content).
struct parts
{
    data_chunk version;
    data_chunk serial;
    data_chunk algorithm;
    data_chunk issuer;
    data_chunk validity;
    data_chunk subject;
    data_chunk key;
    data_chunk extensions;
    data_chunk outer_algorithm;
    data_chunk signature;
};

static parts split(const std::string& text)
{
    const auto der = first_block(text);
    reader outer{ der };
    auto body = outer.read_nested(sequence_tag);
    auto tbs = body.read_nested(sequence_tag);
    parts out{};
    out.version = chunk(tbs.read_encoding(explicit_tag(0)));
    out.serial = chunk(tbs.read_encoding(integer_tag));
    out.algorithm = chunk(tbs.read_encoding(sequence_tag));
    out.issuer = chunk(tbs.read_encoding(sequence_tag));
    out.validity = chunk(tbs.read_encoding(sequence_tag));
    out.subject = chunk(tbs.read_encoding(sequence_tag));
    out.key = chunk(tbs.read_encoding(sequence_tag));
    out.extensions = chunk(tbs.read_encoding(explicit_tag(3)));
    out.outer_algorithm = chunk(body.read_encoding(sequence_tag));
    out.signature = chunk(body.read_bit_string());
    return out;
}

// Join the fields, with extra encodings between the key and extensions.
static data_chunk join(const parts& fields, const data_chunk& extra={})
{
    const auto tbs = build_chunk({ fields.version, fields.serial, fields.algorithm, fields.issuer, fields.validity, fields.subject, fields.key, extra, fields.extensions });

    writer body{};
    body.write(sequence_tag, tbs);
    const auto signature = wrap(bit_string_tag, splice(base16_chunk("00"), fields.signature));
    return wrap(sequence_tag, build_chunk({ body.data(), fields.outer_algorithm, signature }));
}

static data_chunk extension(const_byte_span identifier, const data_chunk& flag, const data_chunk& value)
{
    return wrap(sequence_tag, build_chunk({ wrap(oid_tag, chunk(identifier)), flag, wrap(octet_string_tag, value) }));
}

static data_chunk extensions(const data_loaf& list)
{
    return wrap(explicit_tag(3), wrap(sequence_tag, build_chunk(list)));
}

static data_chunk algorithm(const_byte_span identifier)
{
    return wrap(sequence_tag, wrap(oid_tag, chunk(identifier)));
}

static data_chunk key(const_byte_span type, const_byte_span curve, const data_chunk& point)
{
    const auto identifiers = wrap(sequence_tag, splice(wrap(oid_tag, chunk(type)), wrap(oid_tag, chunk(curve))));
    return wrap(sequence_tag, splice(identifiers, wrap(bit_string_tag, splice(base16_chunk("00"), point))));
}

static bool parses(const data_chunk& der)
{
    certificate out{};
    return parse(out, der);
}

// parse (openssl fixtures)

BOOST_AUTO_TEST_CASE(certificate__parse__server__expected)
{
    const auto der = first_block(fixture::server_pem);
    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE_EQUAL(out.encoding, der);
    BOOST_REQUIRE_EQUAL(out.serial, base16_chunk("03"));
    BOOST_REQUIRE_EQUAL(out.not_before, 1735689600u);
    BOOST_REQUIRE_EQUAL(out.not_after, 2051222400u);
    BOOST_REQUIRE(out.curve == curve::secp256r1);
    BOOST_REQUIRE_EQUAL(out.public_key.size(), 65u);
    BOOST_REQUIRE(out.algorithm == signature_algorithm::ecdsa_sha256);
    BOOST_REQUIRE(!out.authority);
    BOOST_REQUIRE_EQUAL(out.path_length, max_size_t);
    BOOST_REQUIRE_EQUAL(out.key_usage, digital_signature);
    BOOST_REQUIRE(out.server_authentication);
    BOOST_REQUIRE(!out.client_authentication);
    BOOST_REQUIRE_EQUAL(out.dns_names.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.dns_names.front(), "localhost");
    BOOST_REQUIRE_EQUAL(out.ip_addresses.size(), 2u);
    BOOST_REQUIRE_EQUAL(out.ip_addresses.front(), base16_chunk("7f000001"));
    BOOST_REQUIRE_EQUAL(out.ip_addresses.back(), base16_chunk("00000000000000000000000000000001"));
}

BOOST_AUTO_TEST_CASE(certificate__parse__root__secp384r1_authority)
{
    const auto out = parsed(fixture::root_pem);
    BOOST_REQUIRE(out.curve == curve::secp384r1);
    BOOST_REQUIRE_EQUAL(out.public_key.size(), 97u);
    BOOST_REQUIRE(out.algorithm == signature_algorithm::ecdsa_sha384);
    BOOST_REQUIRE(out.authority);
    BOOST_REQUIRE_EQUAL(out.path_length, max_size_t);
    BOOST_REQUIRE_EQUAL(out.key_usage, 0x0060);
    BOOST_REQUIRE_EQUAL(out.issuer, out.subject);
}

BOOST_AUTO_TEST_CASE(certificate__parse__self__defaults)
{
    const auto out = parsed(fixture::self_pem);
    BOOST_REQUIRE(out.authority);
    BOOST_REQUIRE_EQUAL(out.key_usage, max_uint16);
    BOOST_REQUIRE(out.server_authentication);
    BOOST_REQUIRE(out.client_authentication);
    BOOST_REQUIRE(out.dns_names.empty());
    BOOST_REQUIRE(out.ip_addresses.empty());
}

BOOST_AUTO_TEST_CASE(certificate__parse__constrained__path_length_zero)
{
    const auto out = parsed(fixture::constrained_pem);
    BOOST_REQUIRE(out.authority);
    BOOST_REQUIRE_EQUAL(out.path_length, 0u);
}

BOOST_AUTO_TEST_CASE(certificate__parse__unknown_critical_extension__false)
{
    BOOST_REQUIRE(!parses(first_block(fixture::unknown_pem)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__joined__round_trip)
{
    const auto der = first_block(fixture::server_pem);
    BOOST_REQUIRE_EQUAL(join(split(fixture::server_pem)), der);
}

BOOST_AUTO_TEST_CASE(certificate__parse__trailing_data__false)
{
    BOOST_REQUIRE(!parses(splice(first_block(fixture::server_pem), base16_chunk("0500"))));
}

BOOST_AUTO_TEST_CASE(certificate__parse__tbs_trailing_data__false)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = splice(fields.extensions, base16_chunk("0500"));
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__version_one__false)
{
    auto fields = split(fixture::server_pem);
    fields.version = wrap(explicit_tag(0), base16_chunk("020100"));
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__unique_identifiers__skipped)
{
    const auto fields = split(fixture::server_pem);
    const auto identifiers = base16_chunk("8102000182020002");
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields, identifiers)));
    BOOST_REQUIRE_EQUAL(out.dns_names.size(), 1u);
}

BOOST_AUTO_TEST_CASE(certificate__parse__no_extensions__defaults)
{
    auto fields = split(fixture::server_pem);
    fields.extensions.clear();
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE(!out.authority);
    BOOST_REQUIRE_EQUAL(out.key_usage, max_uint16);
}

// algorithms

BOOST_AUTO_TEST_CASE(certificate__parse__mismatched_algorithms__false)
{
    auto fields = split(fixture::server_pem);
    fields.algorithm = algorithm(oid::ecdsa_sha384);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__unsupported_algorithm__false)
{
    auto fields = split(fixture::server_pem);
    fields.algorithm = algorithm(oid::pbes2);
    fields.outer_algorithm = fields.algorithm;
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__algorithm_parameters__false)
{
    auto fields = split(fixture::server_pem);
    fields.algorithm = wrap(sequence_tag, splice(wrap(oid_tag, chunk(oid::ecdsa_sha256)), base16_chunk("0500")));
    fields.outer_algorithm = fields.algorithm;
    BOOST_REQUIRE(!parses(join(fields)));
}

// keys

BOOST_AUTO_TEST_CASE(certificate__parse__secp384r1_key_secp256r1_size__false)
{
    auto fields = split(fixture::server_pem);
    const auto point = parsed(fixture::server_pem).public_key;
    fields.key = key(oid::ec_public_key, oid::secp384r1, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__secp256r1_key_secp384r1_size__false)
{
    auto fields = split(fixture::server_pem);
    const auto point = parsed(fixture::root_pem).public_key;
    fields.key = key(oid::ec_public_key, oid::secp256r1, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__secp384r1_key__expected)
{
    auto fields = split(fixture::server_pem);
    const auto point = parsed(fixture::root_pem).public_key;
    fields.key = key(oid::ec_public_key, oid::secp384r1, point);
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE(out.curve == curve::secp384r1);
    BOOST_REQUIRE_EQUAL(out.public_key, point);
}

BOOST_AUTO_TEST_CASE(certificate__parse__point_off_curve__false)
{
    auto fields = split(fixture::server_pem);
    auto point = parsed(fixture::server_pem).public_key;
    point.back() ^= 0x01;
    fields.key = key(oid::ec_public_key, oid::secp256r1, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__compressed_point__false)
{
    auto fields = split(fixture::server_pem);
    auto point = parsed(fixture::server_pem).public_key;
    point.front() = 0x02;
    fields.key = key(oid::ec_public_key, oid::secp256r1, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__unsupported_key_type__false)
{
    auto fields = split(fixture::server_pem);
    const auto point = parsed(fixture::server_pem).public_key;
    fields.key = key(oid::pbes2, oid::secp256r1, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__unsupported_curve__false)
{
    auto fields = split(fixture::server_pem);
    const auto point = parsed(fixture::server_pem).public_key;
    fields.key = key(oid::ec_public_key, oid::pbes2, point);
    BOOST_REQUIRE(!parses(join(fields)));
}

// extensions

BOOST_AUTO_TEST_CASE(certificate__parse__unknown_noncritical_extension__ignored)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::pbes2, noncritical_flag, base16_chunk("0500")) });
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE_EQUAL(out.key_usage, max_uint16);
}

BOOST_AUTO_TEST_CASE(certificate__parse__malformed_extension__false)
{
    auto fields = split(fixture::server_pem);
    const auto entry = wrap(sequence_tag, wrap(oid_tag, chunk(oid::key_usage)));
    fields.extensions = extensions({ entry });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__duplicate_extension__false)
{
    auto fields = split(fixture::server_pem);
    const auto constraints = extension(oid::basic_constraints, critical_flag, base16_chunk("3000"));
    fields.extensions = extensions({ constraints, constraints });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__basic_constraints_path_length__expected)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::basic_constraints, critical_flag, base16_chunk("30060101ff020102")) });
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE(out.authority);
    BOOST_REQUIRE_EQUAL(out.path_length, 2u);
}

BOOST_AUTO_TEST_CASE(certificate__parse__malformed_basic_constraints__false)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::basic_constraints, no_flag, base16_chunk("0500")) });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__long_key_usage__nine_bits)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::key_usage, critical_flag, base16_chunk("030300ffff")) });
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE_EQUAL(out.key_usage, 0x01ff);
}

BOOST_AUTO_TEST_CASE(certificate__parse__malformed_key_usage__false)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::key_usage, critical_flag, base16_chunk("0500")) });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__any_extended_key_usage__both)
{
    auto fields = split(fixture::server_pem);
    const auto purposes = wrap(sequence_tag, wrap(oid_tag, chunk(oid::any_extended_key_usage)));
    fields.extensions = extensions({ extension(oid::extended_key_usage, no_flag, purposes) });
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE(out.server_authentication);
    BOOST_REQUIRE(out.client_authentication);
}

BOOST_AUTO_TEST_CASE(certificate__parse__empty_extended_key_usage__false)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::extended_key_usage, no_flag, base16_chunk("3000")) });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__other_alternative_name__skipped)
{
    auto fields = split(fixture::server_pem);
    const auto names = wrap(sequence_tag, splice(wrap(implicit_tag(1), to_chunk("a@b")), wrap(implicit_tag(2), to_chunk("bs"))));
    fields.extensions = extensions({ extension(oid::subject_alternative_name, critical_flag, names) });
    certificate out{};
    BOOST_REQUIRE(parse(out, join(fields)));
    BOOST_REQUIRE_EQUAL(out.dns_names.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.dns_names.front(), "bs");
}

BOOST_AUTO_TEST_CASE(certificate__parse__invalid_ip_address__false)
{
    auto fields = split(fixture::server_pem);
    const auto names = wrap(sequence_tag, wrap(implicit_tag(7), base16_chunk("0102030405")));
    fields.extensions = extensions({ extension(oid::subject_alternative_name, no_flag, names) });
    BOOST_REQUIRE(!parses(join(fields)));
}

BOOST_AUTO_TEST_CASE(certificate__parse__empty_alternative_names__false)
{
    auto fields = split(fixture::server_pem);
    fields.extensions = extensions({ extension(oid::subject_alternative_name, no_flag, base16_chunk("3000")) });
    BOOST_REQUIRE(!parses(join(fields)));
}

// parse text

BOOST_AUTO_TEST_CASE(certificate__parse_text__chain__two)
{
    certificates out{};
    BOOST_REQUIRE(parse(out, fixture::server_pem + fixture::key_pem + fixture::intermediate_pem));
    BOOST_REQUIRE_EQUAL(out.size(), 2u);
    BOOST_REQUIRE_EQUAL(out.front().encoding, first_block(fixture::server_pem));
    BOOST_REQUIRE_EQUAL(out.front().issuer, out.back().subject);
}

BOOST_AUTO_TEST_CASE(certificate__parse_text__no_certificate__false)
{
    certificates out{};
    BOOST_REQUIRE(!parse(out, fixture::key_pem));
}

BOOST_AUTO_TEST_CASE(certificate__parse_text__malformed_pem__false)
{
    certificates out{};
    BOOST_REQUIRE(!parse(out, "-----BEGIN CERTIFICATE-----\n"));
}

BOOST_AUTO_TEST_CASE(certificate__parse_text__invalid_certificate__false)
{
    certificates out{};
    BOOST_REQUIRE(!parse(out, fixture::unknown_pem));
}

// is_signed_by

BOOST_AUTO_TEST_CASE(certificate__is_signed_by__server_by_intermediate__true)
{
    BOOST_REQUIRE(is_signed_by(parsed(fixture::server_pem), parsed(fixture::intermediate_pem)));
}

BOOST_AUTO_TEST_CASE(certificate__is_signed_by__intermediate_by_secp384r1_root__true)
{
    BOOST_REQUIRE(is_signed_by(parsed(fixture::intermediate_pem), parsed(fixture::root_pem)));
}

BOOST_AUTO_TEST_CASE(certificate__is_signed_by__server_by_root__false)
{
    BOOST_REQUIRE(!is_signed_by(parsed(fixture::server_pem), parsed(fixture::root_pem)));
}

BOOST_AUTO_TEST_CASE(certificate__is_signed_by__invalid_issuer_key__false)
{
    auto issuer = parsed(fixture::intermediate_pem);
    issuer.public_key.pop_back();
    BOOST_REQUIRE(!is_signed_by(parsed(fixture::server_pem), issuer));
}

BOOST_AUTO_TEST_SUITE_END()
