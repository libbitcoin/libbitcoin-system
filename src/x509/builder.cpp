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
#include <bitcoin/system/x509/builder.hpp>

#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/x509/der.hpp>
#include <bitcoin/system/x509/oids.hpp>
#include <bitcoin/system/x509/pem.hpp>

// rfc5280 (profile)

namespace libbitcoin {
namespace system {
namespace x509 {

using namespace der;

constexpr uint64_t version3 = 2;
constexpr uint8_t digital_signature_bit = 0x80;
constexpr uint8_t digital_signature_unused = 7;
constexpr std::string_view certificate_label{ "CERTIFICATE" };

static void write_extension(writer& out, const_byte_span identifier,
    bool critical, const writer& value) NOEXCEPT
{
    writer extension{};
    extension.write_oid(identifier);
    if (critical)
        extension.write_boolean(true);

    extension.write_octet_string(value.data());
    out.write_nested(sequence_tag, extension);
}

// Name ::= SEQUENCE OF RelativeDistinguishedName (common name only).
static writer write_name(const std::string& common_name) NOEXCEPT
{
    writer name{};
    if (common_name.empty())
        return name;

    writer attribute{};
    attribute.write_oid(oid::common_name);
    attribute.write_string(utf8_string_tag, common_name);

    writer relative{};
    relative.write_nested(sequence_tag, attribute);
    name.write_nested(set_tag, relative);
    return name;
}

static writer write_extensions(const subject& subject) NOEXCEPT
{
    writer extensions{};

    writer constraints{};
    constraints.write_nested(sequence_tag, writer{});
    write_extension(extensions, oid::basic_constraints, true, constraints);

    writer usage{};
    const data_array<one> bits{ digital_signature_bit };
    usage.write_bit_string(bits, digital_signature_unused);
    write_extension(extensions, oid::key_usage, true, usage);

    writer purposes{};
    purposes.write_oid(oid::server_authentication);
    purposes.write_oid(oid::client_authentication);

    writer extended{};
    extended.write_nested(sequence_tag, purposes);
    write_extension(extensions, oid::extended_key_usage, false, extended);

    writer names{};
    for (const auto& dns: subject.dns_names)
        names.write(implicit_tag(2), to_chunk(dns));

    for (const auto& ip: subject.ip_addresses)
        names.write(implicit_tag(7), ip);

    // Alternative names are critical when the subject name is empty.
    if (!names.data().empty())
    {
        writer alternatives{};
        alternatives.write_nested(sequence_tag, names);
        const auto critical = subject.common_name.empty();
        const auto& identifier = oid::subject_alternative_name;
        write_extension(extensions, identifier, critical, alternatives);
    }

    writer sequence{};
    sequence.write_nested(sequence_tag, extensions);
    return sequence;
}

bool build_self_signed(data_chunk& out, const secret& key,
    const subject& subject) NOEXCEPT
{
    secp256r1::point_t point{};
    if ((subject.not_before > subject.not_after) ||
        !secp256r1::public_key(point, key))
        return false;

    writer algorithm{};
    algorithm.write_oid(oid::ecdsa_sha256);

    writer version{};
    version.write_unsigned(version3);

    writer validity{};
    validity.write_time(subject.not_before);
    validity.write_time(subject.not_after);

    writer key_algorithm{};
    key_algorithm.write_oid(oid::ec_public_key);
    key_algorithm.write_oid(oid::secp256r1);

    writer key_information{};
    key_information.write_nested(sequence_tag, key_algorithm);
    key_information.write_bit_string(point);

    const auto name = write_name(subject.common_name);

    writer tbs{};
    tbs.write_nested(explicit_tag(0), version);
    tbs.write_integer(subject.serial);
    tbs.write_nested(sequence_tag, algorithm);
    tbs.write_nested(sequence_tag, name);
    tbs.write_nested(sequence_tag, validity);
    tbs.write_nested(sequence_tag, name);
    tbs.write_nested(sequence_tag, key_information);
    tbs.write_nested(explicit_tag(3), write_extensions(subject));

    writer signed_data{};
    signed_data.write_nested(sequence_tag, tbs);

    secp256r1::signature_t signature{};
    const auto digest = sha256_hash(signed_data.data());
    const auto signed_out = secp256r1::sign(signature, key, digest);

    writer body{};
    body.write_nested(sequence_tag, tbs);
    body.write_nested(sequence_tag, algorithm);
    body.write_bit_string(secp256r1::encode(signature));

    writer certificate{};
    certificate.write_nested(sequence_tag, body);
    out = certificate.data();
    return signed_out;
}

std::string encode_certificate(const_byte_span der) NOEXCEPT
{
    return encode_pem(std::string{ certificate_label }, der);
}

} // namespace x509
} // namespace system
} // namespace libbitcoin
