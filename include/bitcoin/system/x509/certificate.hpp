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
#ifndef LIBBITCOIN_SYSTEM_X509_CERTIFICATE_HPP
#define LIBBITCOIN_SYSTEM_X509_CERTIFICATE_HPP

#include <string>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {

/// Public key curves.
enum class curve : uint8_t
{
    secp256r1,
    secp384r1
};

/// Signature algorithms.
enum class signature_algorithm : uint8_t
{
    ecdsa_sha256,
    ecdsa_sha384
};

/// Key usage flags (rfc5280 4.2.1.3), bit n of the named bit list.
constexpr uint16_t digital_signature = 0x0001;
constexpr uint16_t key_certificate_sign = 0x0020;

/// A parsed v3 certificate (rfc5280) with an ECDSA key and signature.
struct certificate
{
    /// Encodings.
    data_chunk encoding{};
    data_chunk signed_data{};
    data_chunk serial{};
    data_chunk issuer{};
    data_chunk subject{};

    /// Validity, as unix seconds.
    uint64_t not_before{};
    uint64_t not_after{};

    /// Subject public key (uncompressed sec1 point).
    x509::curve curve{};
    data_chunk public_key{};

    /// Issuer signature (der ECDSA-Sig-Value).
    signature_algorithm algorithm{};
    data_chunk signature{};

    /// Basic constraints (path length maximum if unconstrained).
    bool authority{};
    size_t path_length{ max_size_t };

    /// Key usage (all set if absent).
    uint16_t key_usage{ max_uint16 };

    /// Extended key usage (both set if absent or any).
    bool server_authentication{ true };
    bool client_authentication{ true };

    /// Subject alternative names.
    std::vector<std::string> dns_names{};
    std::vector<data_chunk> ip_addresses{};
};

using certificates = std::vector<certificate>;

/// Parse a der encoded certificate, false if malformed, not v3, of an
/// unsupported key or signature, or with an unsupported critical extension.
BC_API bool parse(certificate& out, const_byte_span der) NOEXCEPT;

/// Parse all "CERTIFICATE" blocks of the text (at least one).
BC_API bool parse(certificates& out, const std::string& text) NOEXCEPT;

/// True if the signature of the subject verifies by the issuer key.
BC_API bool is_signed_by(const certificate& subject,
    const certificate& issuer) NOEXCEPT;

} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
