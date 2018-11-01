/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_ELLIPTIC_CURVE_HPP
#define LIBBITCOIN_ELLIPTIC_CURVE_HPP

#include <cstddef>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

/// The sign byte value for an even (y-valued) key.
static BC_CONSTEXPR uint8_t ec_even_sign = 2;

/// Private key:
static BC_CONSTEXPR size_t ec_secret_size = 32;
typedef byte_array<ec_secret_size> ec_secret;

typedef std::vector<ec_secret> secret_list;

/// Compressed public key:
static BC_CONSTEXPR size_t ec_compressed_size = 33;
typedef byte_array<ec_compressed_size> ec_compressed;

typedef std::vector<ec_compressed> point_list;

/// Uncompressed public key:
static BC_CONSTEXPR size_t ec_uncompressed_size = 65;
typedef byte_array<ec_uncompressed_size> ec_uncompressed;

// Parsed ECDSA signature:
static BC_CONSTEXPR size_t ec_signature_size = 64;
typedef byte_array<ec_signature_size> ec_signature;

// DER encoded signature:
static BC_CONSTEXPR size_t max_der_signature_size = 72;
typedef data_chunk der_signature;

/// DER encoded signature with sighash byte for contract endorsement:
static BC_CONSTEXPR size_t min_endorsement_size = 9;
static BC_CONSTEXPR size_t max_endorsement_size = 73;
typedef data_chunk endorsement;

/// Recoverable ecdsa signature for message signing:
struct BC_API recoverable_signature
{
    ec_signature signature;
    uint8_t recovery_id;
};

static BC_CONSTEXPR ec_compressed null_compressed_point =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static BC_CONSTEXPR ec_uncompressed null_uncompressed_point =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Add and multiply EC values
// ----------------------------------------------------------------------------

/// Compute the sum a += G*b, where G is the curve's generator point.
BC_API bool ec_add(ec_compressed& point, const ec_secret& scalar);

/// Compute the sum a += G*b, where G is the curve's generator point.
BC_API bool ec_add(ec_uncompressed& point, const ec_secret& scalar);

/// Compute the sum a = (a + b) % n, where n is the curve order.
BC_API bool ec_add(ec_secret& left, const ec_secret& right);

/// Compute the product point *= secret.
BC_API bool ec_multiply(ec_compressed& point, const ec_secret& scalar);

/// Compute the product point *= secret.
BC_API bool ec_multiply(ec_uncompressed& point, const ec_secret& scalar);

/// Compute the product a = (a * b) % n, where n is the curve order.
BC_API bool ec_multiply(ec_secret& left, const ec_secret& right);

/// Negate a scalar.
BC_API bool ec_negate(ec_secret& scalar);

/// Invert a point (flip on Y axis).
BC_API bool ec_negate(ec_compressed& point);

/// Compute the addition of EC curve points.
BC_API bool ec_sum(ec_compressed& result, const point_list& values);

// Convert keys
// ----------------------------------------------------------------------------

/// Convert an uncompressed public point to compressed.
BC_API bool compress(ec_compressed& out, const ec_uncompressed& point);

/// Convert a compressed public point to decompressed.
BC_API bool decompress(ec_uncompressed& out, const ec_compressed& point);

/// Convert a secret to a compressed public point.
BC_API bool secret_to_public(ec_compressed& out, const ec_secret& secret);

/// Convert a secret parameter to an uncompressed public point.
BC_API bool secret_to_public(ec_uncompressed& out, const ec_secret& secret);

// Verify keys
// ----------------------------------------------------------------------------

/// Verify a secret.
BC_API bool verify(const ec_secret& secret);

/// Verify a point.
BC_API bool verify(const ec_compressed& point);

/// Verify a point.
BC_API bool verify(const ec_uncompressed& point);

// Detect public keys
// ----------------------------------------------------------------------------

/// Determine if the compressed public key is even (y-valued).
bool is_even_key(const ec_compressed& point);

/// Fast detection of compressed public key structure.
bool is_compressed_key(data_slice point);

/// Fast detection of uncompressed public key structure.
bool is_uncompressed_key(data_slice point);

/// Fast detection of compressed or uncompressed public key structure.
bool is_public_key(data_slice point);

/// Fast detection of endorsement structure (DER with signature hash type).
bool is_endorsement(const endorsement& endorsement);

// DER parse/encode
// ----------------------------------------------------------------------------

/// Parse an endorsement into signature hash type and DER signature.
BC_API bool parse_endorsement(uint8_t& sighash_type,
    der_signature& der_signature, const endorsement& endorsement);

/// Parse a DER encoded signature with optional strict DER enforcement.
/// Treat an empty DER signature as invalid, in accordance with BIP66.
BC_API bool parse_signature(ec_signature& out,
    const der_signature& der_signature, bool strict);

/// Encode an EC signature as DER (strict).
BC_API bool encode_signature(der_signature& out, const ec_signature& signature);

// EC sign/verify
// ----------------------------------------------------------------------------

/// Create a deterministic ECDSA signature using a private key.
BC_API bool sign(ec_signature& out, const ec_secret& secret,
    const hash_digest& hash);

/// Verify an EC signature using a compressed point.
BC_API bool verify_signature(const ec_compressed& point,
    const hash_digest& hash, const ec_signature& signature);

/// Verify an EC signature using an uncompressed point.
BC_API bool verify_signature(const ec_uncompressed& point,
    const hash_digest& hash, const ec_signature& signature);

/// Verify an EC signature using a potential point.
BC_API bool verify_signature(data_slice point, const hash_digest& hash,
    const ec_signature& signature);

// Recoverable sign/recover
// ----------------------------------------------------------------------------

/// Create a recoverable signature for use in message signing.
BC_API bool sign_recoverable(recoverable_signature& out,
    const ec_secret& secret, const hash_digest& hash);

/// Recover the compressed point from a recoverable message signature.
BC_API bool recover_public(ec_compressed& out,
    const recoverable_signature& recoverable, const hash_digest& hash);

/// Recover the uncompressed point from a recoverable message signature.
BC_API bool recover_public(ec_uncompressed& out,
    const recoverable_signature& recoverable, const hash_digest& hash);

} // namespace libbitcoin

#endif
