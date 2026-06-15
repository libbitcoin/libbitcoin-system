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
#include <bitcoin/system/crypto/secp256k1.hpp>

#include <algorithm>
#include <secp256k1.h>
#include <bitcoin/system/crypto/der_parser.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {
namespace ecdsa {

// local
// ----------------------------------------------------------------------------
// `ec_signature` is an alias for secp256k1_ecdsa_signature (private format).
// The output of ecdsa_signature_parse_der_lax (via decode_signature) is in a
// fixed-width (64 byte) form but is not in a standard/public form. The output
// of secp256k1_ecdsa_signature_normalize (in verify_signature) is also in a
// private format.

static constexpr auto ec_success = 1;

// Return indicates modified, not success/fail.
// BIP62 required low-s signatures, but that is not active.
// secp256k1_ecdsa_verify rejects non-normalized (low-s) signatures, but
// bitcoin does not have such a limitation, so we always normalize.
// This normalization produces secp256k1_ecdsa_signature (private format).
static bool normalize_signature(const secp256k1_context* context,
    ec_signature& out, const ec_signature& in) NOEXCEPT
{
    static constexpr auto modified = 1;

    // Return indicates modified, not success/fail.
    return secp256k1_ecdsa_signature_normalize(context,
        pointer_cast<secp256k1_ecdsa_signature>(out.data()),
            pointer_cast<const secp256k1_ecdsa_signature>(in.data()))
                == modified;
}

// ec_signature is an alias for secp256k1_ecdsa_signature (private format).
static bool verify_signature(const secp256k1_context* context,
    const secp256k1_pubkey& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    ec_signature normal;
    /* bool */ normalize_signature(context, normal, signature);

    return secp256k1_ecdsa_verify(context,
        pointer_cast<secp256k1_ecdsa_signature>(normal.data()),
            hash.data(), &point) == ec_success;
}

// ECDSA parse/encode
// ----------------------------------------------------------------------------

// ec_signature is an alias for secp256k1_ecdsa_signature (private format).
bool encode_signature(der_signature& out,
    const ec_signature& signature) NOEXCEPT
{
    const auto sign = pointer_cast<const secp256k1_ecdsa_signature>(
        signature.data());

    const auto context = ec_context_sign::context();
    auto size = max_der_signature_size;
    out.resize(size);

    if (secp256k1_ecdsa_signature_serialize_der(context, out.data(), &size,
        sign) != ec_success)
        return false;

    out.resize(size);
    return true;
}

// ec_signature is an alias for secp256k1_ecdsa_signature (private format).
bool decode_signature(ec_signature& out, const data_slice& der_signature,
    bool strict) NOEXCEPT
{
    // BIP66: strict parse is not called for when signature is empty.
    if (der_signature.empty())
        return false;

    // BIP66: requires a specific (non-standard) DER signature encoding.
    if (strict && !is_valid_bip66_signature_encoding(der_signature))
        return false;

    const auto context = ec_context_verify::context();
    auto parsed = pointer_cast<secp256k1_ecdsa_signature>(out.data());

    // ************************************************************************
    // CONSENSUS: This function parses DER with various errors as allowed by
    // Bitcoin prior to activation of BIP66. This attempts to codify the lax
    // rules applied by version(s) of OpenSSL in use up to that time.
    // ************************************************************************
    return ecdsa_signature_parse_der_lax(context, parsed, der_signature.data(),
        der_signature.size());
}

// Return indicates modified, not success/fail.
// Convert a secp256k1_ecdsa_signature to low-s secp256k1_ecdsa_signature.
bool normalize_signature(ec_signature& out, const ec_signature& in) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    return normalize_signature(context, out, in) == ec_success;
}

// Convert a secp256k1_ecdsa_signature to low-s big-endian canonical signature.
bool canonicalize_signature(ec_signature& out, const ec_signature& in) NOEXCEPT
{
    const auto context = ec_context_sign::context();

    ec_signature normal;
    /* bool */ normalize_signature(context, normal, in);

    return secp256k1_ecdsa_signature_serialize_compact(context, out.data(),
        pointer_cast<secp256k1_ecdsa_signature>(normal.data()))
            == ec_success;
}

// ECDSA sign/verify signature
// ----------------------------------------------------------------------------
// It is recommended to verify a signature after signing.

// Produces low-s private form `secp256k1_ecdsa_signature` (usable in verify).
bool sign(ec_signature& out, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    const auto context = ec_context_sign::context();

    return secp256k1_ecdsa_sign(context,
        pointer_cast<secp256k1_ecdsa_signature>(out.data()), hash.data(),
            secret.data(), secp256k1_nonce_function_rfc6979, nullptr)
                == ec_success;
}

// Expects private form `secp256k1_ecdsa_signature` (converts to low-s).
// This expected signature form is the output of decode_signature(...).
bool verify_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    const auto context = ec_context_verify::context();

    secp256k1_pubkey pubkey;
    return ec_public_key_parse(context, pubkey, point) &&
        verify_signature(context, pubkey, hash, signature);
}

// Expects private form `secp256k1_ecdsa_signature` (converts to low-s).
// This expected signature form is the output of decode_signature(...).
bool verify_signature(const ec_compressed& compressed,
    const hash_digest& hash, const ec_signature& signature) NOEXCEPT
{
    const auto context = ec_context_verify::context();

    secp256k1_pubkey pubkey;
    return ec_public_key_parse(context, pubkey, compressed) &&
        verify_signature(context, pubkey, hash, signature);
}

} // namespace ecdsa
} // namespace system
} // namespace libbitcoin
