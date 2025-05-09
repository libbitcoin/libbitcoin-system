/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <utility>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <secp256k1_schnorrsig.h>
#include <bitcoin/system/crypto/der_parser.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"

namespace libbitcoin {
namespace system {

static constexpr auto ec_success = 1;
static constexpr auto maximum_recovery_id = 3;
static constexpr auto compressed_even = 0x02_u8;
static constexpr auto compressed_odd = 0x03_u8;
static constexpr auto uncompressed = 0x04_u8;
static constexpr auto hybrid_even = 0x06_u8;
static constexpr auto hybrid_odd = 0x07_u8;

// Local functions.
// ----------------------------------------------------------------------------
// The templates allow strong typing of private keys without redundant code.

constexpr int to_flags(bool compressed) NOEXCEPT
{
    return compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED;
}

static bool parse(const secp256k1_context* context, secp256k1_pubkey& out,
    const data_slice& point) NOEXCEPT
{
    if (point.empty())
        return false;

    // secp256k1_ec_pubkey_parse supports compressed (33 bytes, header byte 0x02
    // or 0x03), uncompressed (65 bytes, header byte 0x04), or hybrid (65 bytes,
    // header byte 0x06 or 0x07) format public keys.
    return secp256k1_ec_pubkey_parse(context, &out, point.data(), point.size())
        == ec_success;
}

static bool parse(const secp256k1_context* context,
    std::vector<secp256k1_pubkey>& out, const compressed_list& points) NOEXCEPT
{
    out.resize(points.size());
    auto key = out.begin();

    for (const auto& point: points)
        if (!parse(context, *key++, point))
            return false;

    return true;
}

// Create an array of secp256k1_pubkey pointers for secp256k1 call.
static std::vector<const secp256k1_pubkey*> to_pointers(
    const std::vector<secp256k1_pubkey>& keys) NOEXCEPT
{
    std::vector<const secp256k1_pubkey*> pointers(keys.size());

    std::transform(keys.begin(), keys.end(), pointers.begin(),
        [](const secp256k1_pubkey& point) NOEXCEPT
        {
            return &point;
        });

    return pointers;
}

template <size_t Size>
static bool serialize(const secp256k1_context* context, data_array<Size>& out,
    const secp256k1_pubkey point) NOEXCEPT
{
    auto size = Size;
    constexpr auto flags = to_flags(Size == ec_compressed_size);
    secp256k1_ec_pubkey_serialize(context, out.data(), &size, &point, flags);
    return size == Size;
}

// parse, add, serialize
template <size_t Size>
static bool ec_add(const secp256k1_context* context, data_array<Size>& in_out,
    const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_add(context, &pubkey, secret.data()) ==
            ec_success && serialize(context, in_out, pubkey);
}

// parse, multiply, serialize
template <size_t Size>
static bool ec_multiply(const secp256k1_context* context,
    data_array<Size>& in_out, const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_mul(context, &pubkey, secret.data()) ==
            ec_success && serialize(context, in_out, pubkey);
}

// parse, negate, serialize
template <size_t Size>
static bool ec_negate(const secp256k1_context* context,
    data_array<Size>& in_out) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_negate(context, &pubkey) == ec_success &&
        serialize(context, in_out, pubkey);
}

// create, serialize (secrets are normal)
template <size_t Size>
static bool secret_to_public(const secp256k1_context* context,
    data_array<Size>& out, const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_create(context, &pubkey, secret.data()) ==
        ec_success && serialize(context, out, pubkey);
}

// parse, recover, serialize
template <size_t Size>
static bool recover_public(const secp256k1_context* context,
    data_array<Size>& out, const recoverable_signature& recoverable,
    const hash_digest& hash) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    secp256k1_ecdsa_recoverable_signature sign;
    const auto recovery_id = static_cast<int>(recoverable.recovery_id);
    return
        secp256k1_ecdsa_recoverable_signature_parse_compact(context,
            &sign, recoverable.signature.data(), recovery_id) == ec_success &&
        secp256k1_ecdsa_recover(context, &pubkey, &sign, hash.data()) ==
            ec_success && serialize(context, out, pubkey);
}

// parsed - normalize, verify
static bool verify_signature(const secp256k1_context* context,
    const secp256k1_pubkey& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    const auto parsed = pointer_cast<const secp256k1_ecdsa_signature>(
        signature.data());

    // BIP62 required low-s signatures, but that is not active.
    // secp256k1_ecdsa_verify rejects non-normalized (low-s) signatures, but
    // bitcoin does not have such a limitation, so we always normalize.
    secp256k1_ecdsa_signature normal;
    secp256k1_ecdsa_signature_normalize(context, &normal, parsed);

    return secp256k1_ecdsa_verify(context, &normal, hash.data(), &point) ==
        ec_success;
}

// Add EC values
// ----------------------------------------------------------------------------

bool ec_add(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

bool ec_add(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

// secrets are normal
bool ec_add(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_add(context, left.data(), right.data())
        == ec_success;
}

bool ec_add(ec_compressed& left, const ec_compressed& right) NOEXCEPT
{
    return ec_sum(left, { left, right });
}

bool ec_add(ec_compressed& left, const ec_uncompressed& right) NOEXCEPT
{
    ec_compressed out;
    return compress(out, right) && ec_add(left, out);
}

// parse, combine, serialize
bool ec_sum(ec_compressed& out, const compressed_list& points) NOEXCEPT
{
    if (points.empty())
        return false;

    const auto context = ec_context_verify::context();

    std::vector<secp256k1_pubkey> keys;
    if (!parse(context, keys, points))
        return false;

    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_combine(context, &pubkey,
        to_pointers(keys).data(), points.size()) == ec_success &&
        serialize(context, out, pubkey);
}

// Multiply EC values
// ----------------------------------------------------------------------------

bool ec_multiply(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

bool ec_multiply(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

// secrets are normal
bool ec_multiply(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_mul(context, left.data(), right.data()) ==
        ec_success;
}

// Negate EC values
// ----------------------------------------------------------------------------

// secrets are normal
bool ec_negate(ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_negate(context, scalar.data()) == ec_success;
}

bool ec_negate(ec_compressed& point) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_negate(context, point);
}

bool ec_negate(ec_uncompressed& point) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_negate(context, point);
}

// Convert keys
// ----------------------------------------------------------------------------

bool compress(ec_compressed& out, const ec_uncompressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return parse(context, pubkey, point) && serialize(context, out, pubkey);
}

bool decompress(ec_uncompressed& out, const ec_compressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return parse(context, pubkey, point) && serialize(context, out, pubkey);
}

bool secret_to_public(ec_compressed& out, const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

bool secret_to_public(ec_uncompressed& out, const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

// Verify keys
// ----------------------------------------------------------------------------

bool verify(const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_verify(context, secret.data()) == ec_success;
}

bool verify(const data_slice& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return parse(context, pubkey, point);
}

// Detect public keys
// ----------------------------------------------------------------------------

bool is_even_key(const ec_compressed& point) NOEXCEPT
{
    return point.front() == ec_even_sign;
}

bool is_compressed_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_compressed_size)
        return false;

    const auto first = point.front();
    return first == compressed_even || first == compressed_odd;
}

bool is_uncompressed_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_uncompressed_size)
        return false;

    const auto first = point.front();
    return first == uncompressed;
}

bool is_hybrid_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_uncompressed_size)
        return false;

    const auto first = point.front();
    return first == hybrid_even || first == hybrid_odd;
}

bool is_public_key(const data_slice& point) NOEXCEPT
{
    return is_compressed_key(point) || is_uncompressed_key(point);
}

bool is_endorsement(const endorsement& endorsement) NOEXCEPT
{
    const auto size = endorsement.size();
    return size >= min_endorsement_size && size <= max_endorsement_size;
}

// ECDSA parse/encode/sign/verify signature
// ----------------------------------------------------------------------------
// It is recommended to verify a signature after signing.

bool parse_endorsement(uint8_t& sighash_flags, data_slice& der_signature,
    const endorsement& endorsement) NOEXCEPT
{
    if (endorsement.empty())
        return false;

    sighash_flags = endorsement.back();
    der_signature = { endorsement.begin(), std::prev(endorsement.end()) };
    return true;
}

bool parse_signature(ec_signature& out, const data_slice& der_signature,
    bool strict) NOEXCEPT
{
    // BIP66: strict parse is not called for when signature is empty.
    if (der_signature.empty())
        return false;

    // BIP66: requires strict and minimal DER signature encoding.
    if (strict && !is_valid_signature_encoding(der_signature))
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

// serialize?
bool sign(ec_signature& out, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    const auto signature = pointer_cast<secp256k1_ecdsa_signature>(out.data());

    return secp256k1_ecdsa_sign(context, signature, hash.data(), secret.data(),
        secp256k1_nonce_function_rfc6979, nullptr) == ec_success;
}

bool verify_signature(const data_slice& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();

    return parse(context, pubkey, point) && verify_signature(context, pubkey,
        hash, signature);
}

// ECDSA recoverable sign/recover
// ----------------------------------------------------------------------------
// It is recommended to verify a signature after signing.

bool sign_recoverable(recoverable_signature& out, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    int recovery_id{};
    const auto context = ec_context_sign::context();
    secp256k1_ecdsa_recoverable_signature signature;

    const auto result =
        secp256k1_ecdsa_sign_recoverable(context, &signature, hash.data(),
            secret.data(), secp256k1_nonce_function_rfc6979, nullptr) ==
            ec_success &&
        secp256k1_ecdsa_recoverable_signature_serialize_compact(context,
            out.signature.data(), &recovery_id, &signature) == ec_success;

    if (is_negative(recovery_id) || recovery_id > maximum_recovery_id)
        return false;

    out.recovery_id = narrow_sign_cast<uint8_t>(recovery_id);
    return result;
}

bool recover_public(ec_compressed& out,
    const recoverable_signature& recoverable, const hash_digest& hash) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return recover_public(context, out, recoverable, hash);
}

bool recover_public(ec_uncompressed& out,
    const recoverable_signature& recoverable, const hash_digest& hash) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return recover_public(context, out, recoverable, hash);
}

// Schnorr parse/sign/verify
// ----------------------------------------------------------------------------

namespace schnorr {

bool parse(uint8_t& sighash_flags, ec_signature& signature,
    const endorsement& endorsement) NOEXCEPT
{
    switch (endorsement.size())
    {
        // BIP341: if [sighash byte] is omitted the resulting signatures are 64
        // bytes, and [default == 0] mode is implied (implies SIGHASH_ALL).
        case signature_size:
            sighash_flags = 0;
            break;

        // BIP341: signature has sighash byte appended in the usual fashion.
        // BIP341: zero is invalid sighash, must be explicit to prevent mally.
        case add1(signature_size):
            sighash_flags = endorsement.back();
            if (is_zero(sighash_flags)) return false;
            break;

        // BIP341: A Taproot signature is a 64-byte Schnorr signature.
        default:
            return false;
    }

    // TODO: optimize unnecessary copy (e.g. could return reference).
    signature = unsafe_array_cast<uint8_t, signature_size>(endorsement.data());
    return true;
}

// It is recommended to verify a signature after signing.
bool sign(ec_signature& out, const ec_secret& secret,
    const hash_digest& hash, const hash_digest& auxiliary) NOEXCEPT
{
    secp256k1_keypair keypair;
    const auto context = ec_context_sign::context();

    return 
        secp256k1_keypair_create(context, &keypair, secret.data()) ==
            ec_success &&
        secp256k1_schnorrsig_sign32(context, out.data(), hash.data(), &keypair,
            auxiliary.data()) == ec_success;
}

// BIP341: A Taproot signature is a 64-byte Schnorr sig, as defined in BIP340.
bool verify_signature(const data_slice& x_point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    if (x_point.size() != hash_size)
        return false;

    secp256k1_xonly_pubkey pubkey;
    const auto context = ec_context_verify::context();

    return
        secp256k1_xonly_pubkey_parse(context, &pubkey, x_point.data()) ==
            ec_success && 
        secp256k1_schnorrsig_verify(context, signature.data(),  hash.data(),
            hash_size, &pubkey) == ec_success;
}

} // namespace schnorr
} // namespace system
} // namespace libbitcoin
