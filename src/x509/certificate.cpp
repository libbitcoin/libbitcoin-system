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
#include <bitcoin/system/x509/certificate.hpp>

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/x509/der.hpp>
#include <bitcoin/system/x509/oids.hpp>
#include <bitcoin/system/x509/pem.hpp>

// rfc5280 (profile), rfc5480 (keys), rfc5758 (signatures)

namespace libbitcoin {
namespace system {
namespace x509 {

using namespace der;

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

constexpr uint64_t version3 = 2;
constexpr uint8_t uncompressed = 0x04;
constexpr size_t key_usage_bits = 9;
constexpr size_t ipv4_size = 4;
constexpr size_t ipv6_size = 16;
constexpr std::string_view certificate_label{ "CERTIFICATE" };

static bool is_equal(const_byte_span left, const_byte_span right) NOEXCEPT
{
    return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

static data_chunk to_data(const_byte_span bytes) NOEXCEPT
{
    return { bytes.begin(), bytes.end() };
}

// AlgorithmIdentifier of an ECDSA signature (parameters absent).
static bool parse_algorithm(signature_algorithm& out,
    const_byte_span encoding) NOEXCEPT
{
    reader outer{ encoding };
    auto algorithm = outer.read_nested(sequence_tag);
    const auto identifier = algorithm.read_oid();
    if (!outer.is_complete() || !algorithm.is_complete())
        return false;

    if (is_equal(identifier, oid::ecdsa_sha256))
    {
        out = signature_algorithm::ecdsa_sha256;
        return true;
    }

    if (is_equal(identifier, oid::ecdsa_sha384))
    {
        out = signature_algorithm::ecdsa_sha384;
        return true;
    }

    return false;
}

// SubjectPublicKeyInfo of a named curve and uncompressed point.
static bool parse_key(certificate& out, reader& source) NOEXCEPT
{
    auto information = source.read_nested(sequence_tag);
    auto algorithm = information.read_nested(sequence_tag);
    const auto key_type = algorithm.read_oid();
    const auto named_curve = algorithm.read_oid();
    const auto point = information.read_bit_string();
    if (!information.is_complete() || !algorithm.is_complete() ||
        !is_equal(key_type, oid::ec_public_key) || point.empty() ||
        (point.front() != uncompressed))
        return false;

    if (is_equal(named_curve, oid::secp256r1))
    {
        secp256r1::point_t key{};
        if (point.size() != key.size())
            return false;

        std::copy(point.begin(), point.end(), key.begin());
        out.curve = curve::secp256r1;
        out.public_key = to_data(point);
        return secp256r1::is_valid(key);
    }

    if (is_equal(named_curve, oid::secp384r1))
    {
        secp384r1::point_t key{};
        if (point.size() != key.size())
            return false;

        std::copy(point.begin(), point.end(), key.begin());
        out.curve = curve::secp384r1;
        out.public_key = to_data(point);
        return secp384r1::is_valid(key);
    }

    return false;
}

// BasicConstraints ::= SEQUENCE { cA BOOLEAN DEFAULT FALSE,
//     pathLenConstraint INTEGER OPTIONAL }
static bool parse_basic_constraints(certificate& out,
    const_byte_span value) NOEXCEPT
{
    reader outer{ value };
    auto constraints = outer.read_nested(sequence_tag);
    if (constraints.peek() == boolean_tag)
        out.authority = constraints.read_boolean();

    if (constraints.peek() == integer_tag)
    {
        const auto length = constraints.read_unsigned();
        out.path_length = limit<size_t>(length);
    }

    return outer.is_complete() && constraints.is_complete();
}

// KeyUsage ::= BIT STRING (named bits, zero is the high bit of byte zero).
static bool parse_key_usage(certificate& out, const_byte_span value) NOEXCEPT
{
    reader outer{ value };
    uint8_t unused{};
    const auto bits = outer.read_bit_string(unused);
    if (!outer.is_complete())
        return false;

    uint16_t usage{};
    const auto count = std::min(bits.size() * byte_bits, key_usage_bits);
    for (size_t bit{}; bit < count; ++bit)
    {
        const auto byte = bits[bit / byte_bits];
        const auto shift = sub1(byte_bits) - (bit % byte_bits);
        const auto flag = to_int<uint16_t>(get_right(byte, shift));
        usage = bit_or(usage, shift_left(flag, bit));
    }

    out.key_usage = usage;
    return true;
}

// ExtKeyUsageSyntax ::= SEQUENCE SIZE (1..MAX) OF KeyPurposeId
static bool parse_extended_key_usage(certificate& out,
    const_byte_span value) NOEXCEPT
{
    reader outer{ value };
    auto purposes = outer.read_nested(sequence_tag);
    out.server_authentication = false;
    out.client_authentication = false;

    auto empty = true;
    while (purposes && !purposes.is_complete())
    {
        const auto purpose = purposes.read_oid();
        const auto any = is_equal(purpose, oid::any_extended_key_usage);
        const auto server = is_equal(purpose, oid::server_authentication);
        const auto client = is_equal(purpose, oid::client_authentication);
        auto& server_usage = out.server_authentication;
        auto& client_usage = out.client_authentication;
        server_usage = server_usage || any || server;
        client_usage = client_usage || any || client;
        empty = false;
    }

    return !empty && outer.is_complete() && purposes.is_complete();
}

// GeneralNames ::= SEQUENCE SIZE (1..MAX) OF GeneralName, of which only
// dNSName [2] and iPAddress [7] are retained.
static bool parse_alternative_names(certificate& out,
    const_byte_span value) NOEXCEPT
{
    reader outer{ value };
    auto names = outer.read_nested(sequence_tag);

    auto empty = true;
    while (names && !names.is_complete())
    {
        const auto tag = names.peek();
        empty = false;

        if (tag == implicit_tag(2))
        {
            const auto name = names.read(tag);
            out.dns_names.emplace_back(name.begin(), name.end());
            continue;
        }

        if (tag == implicit_tag(7))
        {
            const auto address = names.read(tag);
            if ((address.size() != ipv4_size) && (address.size() != ipv6_size))
                return false;

            out.ip_addresses.push_back(to_data(address));
            continue;
        }

        names.skip();
    }

    return !empty && outer.is_complete() && names.is_complete();
}

// Extension ::= SEQUENCE { extnID OBJECT IDENTIFIER,
//     critical BOOLEAN DEFAULT FALSE, extnValue OCTET STRING }
static bool parse_extensions(certificate& out, reader& source) NOEXCEPT
{
    auto wrapper = source.read_nested(explicit_tag(3));
    auto extensions = wrapper.read_nested(sequence_tag);
    auto constraints = false, usage = false, extended = false, names = false;

    while (extensions && !extensions.is_complete())
    {
        auto extension = extensions.read_nested(sequence_tag);
        const auto identifier = extension.read_oid();

        auto critical = false;
        if (extension.peek() == boolean_tag)
            critical = extension.read_boolean();

        const auto value = extension.read_octet_string();
        if (!extension.is_complete())
            return false;

        const auto parse_once = [&](bool& seen, auto parser) NOEXCEPT
        {
            const auto first = !seen;
            seen = true;
            return first && parser(out, value);
        };

        if (is_equal(identifier, oid::basic_constraints))
        {
            if (!parse_once(constraints, parse_basic_constraints))
                return false;
        }
        else if (is_equal(identifier, oid::key_usage))
        {
            if (!parse_once(usage, parse_key_usage))
                return false;
        }
        else if (is_equal(identifier, oid::extended_key_usage))
        {
            if (!parse_once(extended, parse_extended_key_usage))
                return false;
        }
        else if (is_equal(identifier, oid::subject_alternative_name))
        {
            if (!parse_once(names, parse_alternative_names))
                return false;
        }
        else if (critical)
        {
            return false;
        }
    }

    return wrapper.is_complete() && extensions.is_complete();
}

// Certificate ::= SEQUENCE { tbsCertificate TBSCertificate,
//     signatureAlgorithm AlgorithmIdentifier, signatureValue BIT STRING }
bool parse(certificate& out, const_byte_span der) NOEXCEPT
{
    certificate value{};
    reader outer{ der };
    auto body = outer.read_nested(sequence_tag);
    const auto signed_data = body.read_encoding(sequence_tag);
    const auto outer_algorithm = body.read_encoding(sequence_tag);
    const auto signature = body.read_bit_string();
    if (!outer.is_complete() || !body.is_complete())
        return false;

    reader wrapper{ signed_data };
    auto tbs = wrapper.read_nested(sequence_tag);
    auto version = tbs.read_nested(explicit_tag(0));
    const auto number = version.read_unsigned();
    const auto serial = tbs.read_integer();
    const auto inner_algorithm = tbs.read_encoding(sequence_tag);
    const auto issuer = tbs.read_encoding(sequence_tag);
    auto validity = tbs.read_nested(sequence_tag);
    value.not_before = validity.read_time();
    value.not_after = validity.read_time();
    const auto subject = tbs.read_encoding(sequence_tag);

    if (!version.is_complete() || !validity.is_complete() ||
        (number != version3) || !parse_key(value, tbs) ||
        !is_equal(inner_algorithm, outer_algorithm) ||
        !parse_algorithm(value.algorithm, outer_algorithm))
        return false;

    // Unique identifiers are ignored.
    if (tbs.peek() == implicit_tag(1))
        tbs.skip();

    if (tbs.peek() == implicit_tag(2))
        tbs.skip();

    if ((tbs.peek() == explicit_tag(3)) && !parse_extensions(value, tbs))
        return false;

    if (!wrapper.is_complete() || !tbs.is_complete())
        return false;

    value.encoding = to_data(der);
    value.signed_data = to_data(signed_data);
    value.serial = to_data(serial);
    value.issuer = to_data(issuer);
    value.subject = to_data(subject);
    value.signature = to_data(signature);
    out = std::move(value);
    return true;
}

bool parse(certificates& out, const std::string& text) NOEXCEPT
{
    pems blocks{};
    if (!decode_pem(blocks, text))
        return false;

    certificates chain{};
    for (const auto& block: blocks)
    {
        if (block.label != certificate_label)
            continue;

        certificate value{};
        if (!parse(value, block.data))
            return false;

        chain.push_back(std::move(value));
    }

    if (chain.empty())
        return false;

    out = std::move(chain);
    return true;
}

// verification
// ----------------------------------------------------------------------------

template <typename Curve>
static bool verify_signature(const data_chunk& key, const data_chunk& der,
    const_byte_span digest) NOEXCEPT
{
    typename Curve::point_t point{};
    typename Curve::signature_t signature{};
    if (key.size() != point.size() || !Curve::decode(signature, der))
        return false;

    std::copy(key.begin(), key.end(), point.begin());
    return Curve::verify(signature, point, digest);
}

bool is_signed_by(const certificate& subject,
    const certificate& issuer) NOEXCEPT
{
    const auto sha384 = (subject.algorithm ==
        signature_algorithm::ecdsa_sha384);
    const auto digest = sha384 ?
        to_chunk(accumulator<sha512_384>::hash(subject.signed_data)) :
        to_chunk(sha256_hash(subject.signed_data));

    const auto& key = issuer.public_key;
    const auto& signature = subject.signature;
    if (issuer.curve == curve::secp384r1)
        return verify_signature<secp384r1>(key, signature, digest);

    return verify_signature<secp256r1>(key, signature, digest);
}

BC_POP_WARNING()

} // namespace x509
} // namespace system
} // namespace libbitcoin
