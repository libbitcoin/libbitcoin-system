/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

constexpr int to_flags(bool compressed) NOEXCEPT
{
    return compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED;
}

// Helper functions.
// ----------------------------------------------------------------------------
// The templates allow strong typing of private keys without redundant code.

bool parse(const secp256k1_context* context, secp256k1_pubkey& out,
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

bool parse(const secp256k1_context* context, std::vector<secp256k1_pubkey>& out,
    const compressed_list& points) NOEXCEPT
{
    out.resize(points.size());
    auto key = out.begin();

    for (const auto& point: points)
        if (!parse(context, *key++, point))
            return false;

    return true;
}

// Create an array of secp256k1_pubkey pointers for secp256k1 call.
std::vector<const secp256k1_pubkey*> to_pointers(
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
bool serialize(const secp256k1_context* context, data_array<Size>& out,
    const secp256k1_pubkey point) NOEXCEPT
{
    auto size = Size;
    constexpr auto flags = to_flags(Size == ec_compressed_size);
    secp256k1_ec_pubkey_serialize(context, out.data(), &size, &point, flags);
    return size == Size;
}

// parse, add, serialize
template <size_t Size>
bool ec_add(const secp256k1_context* context, data_array<Size>& in_out,
    const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_add(context, &pubkey, secret.data()) ==
            ec_success && serialize(context, in_out, pubkey);
}

// parse, multiply, serialize
template <size_t Size>
bool ec_multiply(const secp256k1_context* context, data_array<Size>& in_out,
    const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_mul(context, &pubkey, secret.data()) ==
            ec_success && serialize(context, in_out, pubkey);
}

// parse, negate, serialize
template <size_t Size>
bool ec_negate(const secp256k1_context* context,
    data_array<Size>& in_out) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_negate(context, &pubkey) == ec_success &&
        serialize(context, in_out, pubkey);
}

// create, serialize (secrets are normal)
template <size_t Size>
bool secret_to_public(const secp256k1_context* context, data_array<Size>& out,
    const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_create(context, &pubkey, secret.data()) ==
        ec_success && serialize(context, out, pubkey);
}

// parse, recover, serialize
template <size_t Size>
bool recover_public(const secp256k1_context* context, data_array<Size>& out,
    const recoverable_signature& recoverable, const hash_digest& hash) NOEXCEPT
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
bool verify_signature(const secp256k1_context* context,
    const secp256k1_pubkey& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    const auto parsed = pointer_cast<const secp256k1_ecdsa_signature>(
        signature.data());

    secp256k1_ecdsa_signature normal;
    secp256k1_ecdsa_signature_normalize(context, &normal, parsed);

    // BIP62 required low-s signatures, but that is not active.
    // secp256k1_ecdsa_verify rejects non-normalized (low-s) signatures, but
    // bitcoin does not have such a limitation, so we always normalize.
    return secp256k1_ecdsa_verify(context, &normal, hash.data(), &point) ==
        ec_success;
}

// Add EC values
// ----------------------------------------------------------------------------

bool ec_add(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

bool ec_add(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

// secrets are normal
bool ec_add(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_add(context, left.data(),
        right.data()) == 1;
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

    auto const* context = ec_context_verify::context();

    std::vector<secp256k1_pubkey> keys;
    if (!parse(context, keys, points))
        return false;

    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_combine(context, &pubkey,
        to_pointers(keys).data(), points.size()) == 
            ec_success && serialize(context, out, pubkey);
}

// Multiply EC values
// ----------------------------------------------------------------------------

bool ec_multiply(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

bool ec_multiply(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

// secrets are normal
bool ec_multiply(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_mul(context, left.data(),
        right.data()) == ec_success;
}

// Negate EC values
// ----------------------------------------------------------------------------

// secrets are normal
bool ec_negate(ec_secret& scalar) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return secp256k1_ec_seckey_negate(context, scalar.data()) == ec_success;
}

bool ec_negate(ec_compressed& point) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_negate(context, point);
}

bool ec_negate(ec_uncompressed& point) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return ec_negate(context, point);
}

// Convert keys
// ----------------------------------------------------------------------------

bool compress(ec_compressed& out, const ec_uncompressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    auto const* context = ec_context_verify::context();
    return parse(context, pubkey, point) && serialize(context, out, pubkey);
}

bool decompress(ec_uncompressed& out, const ec_compressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    auto const* context = ec_context_verify::context();
    return parse(context, pubkey, point) && serialize(context, out, pubkey);
}

bool secret_to_public(ec_compressed& out, const ec_secret& secret) NOEXCEPT
{
    auto const* context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

bool secret_to_public(ec_uncompressed& out, const ec_secret& secret) NOEXCEPT
{
    auto const* context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

// Verify keys
// ----------------------------------------------------------------------------

bool verify(const ec_secret& secret) NOEXCEPT
{
    auto const* context = ec_context_verify::context();
    return secp256k1_ec_seckey_verify(context, secret.data()) == ec_success;
}

bool verify(const data_slice& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    auto const* context = ec_context_verify::context();
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

// DER parse/encode
// ----------------------------------------------------------------------------

bool parse_endorsement(uint8_t& sighash_flags, data_slice& der_signature,
    const endorsement& endorsement) NOEXCEPT
{
    if (endorsement.empty())
        return false;

    sighash_flags = endorsement.back();
    der_signature = { endorsement.begin(), std::prev(endorsement.end()) };
    return true;
}

// BIP66 requires strict DER signature encoding.
bool parse_signature(ec_signature& out, const data_slice& der_signature,
    bool strict) NOEXCEPT
{
    if (der_signature.empty())
        return false;

    auto const* context = ec_context_verify::context();
    auto parsed = pointer_cast<secp256k1_ecdsa_signature>(out.data());

    if (strict)
        return secp256k1_ecdsa_signature_parse_der(context, parsed,
            der_signature.data(), der_signature.size()) == ec_success;

    return ecdsa_signature_parse_der_lax(context, parsed,
        der_signature.data(), der_signature.size());
}

bool encode_signature(der_signature& out,
    const ec_signature& signature) NOEXCEPT
{
    const auto sign = pointer_cast<const secp256k1_ecdsa_signature>(
        signature.data());

    auto const* context = ec_context_sign::context();
    auto size = max_der_signature_size;
    out.resize(size);

    if (secp256k1_ecdsa_signature_serialize_der(context, out.data(), &size,
        sign) != ec_success)
        return false;

    out.resize(size);
    return true;
}

// EC sign/verify
// ----------------------------------------------------------------------------

// create (serialize???) (secrets are normal)
bool sign(ec_signature& out, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    const auto signature = pointer_cast<secp256k1_ecdsa_signature>(
        out.data());

    return (secp256k1_ecdsa_sign(context, signature, hash.data(), secret.data(),
        secp256k1_nonce_function_rfc6979, nullptr) == ec_success);
}

// parse<>, verify<>
bool verify_signature(const data_slice& point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();

    return parse(context, pubkey, point) &&
        verify_signature(context, pubkey, hash, signature);
}

// Recoverable sign/recover
// ----------------------------------------------------------------------------

// sign, serialize (secrets are normal)
bool sign_recoverable(recoverable_signature& out, const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    int recovery_id = 0;
    auto const* context = ec_context_sign::context();
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

} // namespace system
} // namespace libbitcoin
