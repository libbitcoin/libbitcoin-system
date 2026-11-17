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
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/x509/der.hpp>
#include <bitcoin/system/x509/oids.hpp>
#include <bitcoin/system/x509/pem.hpp>

// rfc5280 (profile), rfc5480 (keys), rfc5758 (signatures), rfc4055 (rsa)

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

// RSA algorithm parameters are null or absent (rfc4055 5).
static void read_null_parameters(reader& algorithm) NOEXCEPT
{
    if (algorithm.peek() == null_tag)
        algorithm.read_null();
}

// AlgorithmIdentifier of an ECDSA (parameters absent) or RSA signature.
static bool parse_algorithm(signature_algorithm& out,
    const_byte_span encoding) NOEXCEPT
{
    reader outer{ encoding };
    auto algorithm = outer.read_nested(sequence_tag);
    const auto identifier = algorithm.read_oid();

    if (is_equal(identifier, oid::ecdsa_sha256))
    {
        out = signature_algorithm::ecdsa_sha256;
    }
    else if (is_equal(identifier, oid::ecdsa_sha384))
    {
        out = signature_algorithm::ecdsa_sha384;
    }
    else if (is_equal(identifier, oid::rsa_sha256))
    {
        out = signature_algorithm::rsa_sha256;
        read_null_parameters(algorithm);
    }
    else if (is_equal(identifier, oid::rsa_sha384))
    {
        out = signature_algorithm::rsa_sha384;
        read_null_parameters(algorithm);
    }
    else if (is_equal(identifier, oid::rsa_sha512))
    {
        out = signature_algorithm::rsa_sha512;
        read_null_parameters(algorithm);
    }
    else
    {
        return false;
    }

    return outer.is_complete() && algorithm.is_complete();
}

// SubjectPublicKeyInfo of an RSAPublicKey.
static bool parse_rsa_key(certificate& out, reader& information,
    reader& algorithm) NOEXCEPT
{
    read_null_parameters(algorithm);
    const auto key = information.read_bit_string();
    if (!information.is_complete() || !algorithm.is_complete())
        return false;

    rsa::public_key value{};
    out.key_type = key_type::rsa;
    out.public_key = to_data(key);
    return rsa::decode(value, key);
}

// SubjectPublicKeyInfo of a named curve and uncompressed point, or RSA.
static bool parse_key(certificate& out, reader& source) NOEXCEPT
{
    auto information = source.read_nested(sequence_tag);
    auto algorithm = information.read_nested(sequence_tag);
    const auto type = algorithm.read_oid();
    if (is_equal(type, oid::rsa_encryption))
        return parse_rsa_key(out, information, algorithm);

    const auto named_curve = algorithm.read_oid();
    const auto point = information.read_bit_string();
    if (!information.is_complete() || !algorithm.is_complete() ||
        !is_equal(type, oid::ec_public_key) || point.empty() ||
        (point.front() != uncompressed))
        return false;

    out.key_type = key_type::ec;

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

// DigestInfo encodings preceding each digest (rfc8017 9.2, note 1).
constexpr auto sha256_info = base16_array(
    "3031300d060960864801650304020105000420");
constexpr auto sha384_info = base16_array(
    "3041300d060960864801650304020205000430");
constexpr auto sha512_info = base16_array(
    "3051300d060960864801650304020305000440");

template <typename Curve>
static bool verify_signature(const data_chunk& key, const data_chunk& der,
    const_byte_span digest) NOEXCEPT
{
    typename Curve::point_t point{};
    typename Curve::signature_t signature{};
    if (key.size() != point.size() || !Curve::decode(signature, der))
        return false;

    std::copy(key.cbegin(), key.cend(), point.begin());
    return Curve::verify(signature, point, digest);
}

static bool verify_ecdsa(const certificate& subject,
    const certificate& issuer, const_byte_span digest) NOEXCEPT
{
    const auto& key = issuer.public_key;
    const auto& signature = subject.signature;
    if (issuer.key_type != key_type::ec)
        return false;

    if (issuer.curve == curve::secp384r1)
        return verify_signature<secp384r1>(key, signature, digest);

    return verify_signature<secp256r1>(key, signature, digest);
}

template <size_t Size, size_t Digest>
static bool verify_rsa(const certificate& subject, const certificate& issuer,
    const data_array<Size>& info, const data_array<Digest>& digest) NOEXCEPT
{
    rsa::public_key key{};
    return (issuer.key_type == key_type::rsa) &&
        rsa::decode(key, issuer.public_key) &&
        rsa::verify_pkcs1(key, subject.signature, splice(info, digest));
}

bool is_signed_by(const certificate& subject,
    const certificate& issuer) NOEXCEPT
{
    const auto& data = subject.signed_data;
    switch (subject.algorithm)
    {
        case signature_algorithm::ecdsa_sha256:
        {
            return verify_ecdsa(subject, issuer, sha256_hash(data));
        }
        case signature_algorithm::ecdsa_sha384:
        {
            const auto digest = accumulator<sha512_384>::hash(data);
            return verify_ecdsa(subject, issuer, digest);
        }
        case signature_algorithm::rsa_sha256:
        {
            return verify_rsa(subject, issuer, sha256_info,
                sha256_hash(data));
        }
        case signature_algorithm::rsa_sha384:
        {
            return verify_rsa(subject, issuer, sha384_info,
                accumulator<sha512_384>::hash(data));
        }
        case signature_algorithm::rsa_sha512:
        {
            return verify_rsa(subject, issuer, sha512_info,
                sha512_hash(data));
        }
        default:
        {
            return false;
        }
    }
}

BC_POP_WARNING()

} // namespace x509
} // namespace system
} // namespace libbitcoin
