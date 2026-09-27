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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_INTERFACE_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_INTERFACE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

// Based on:
// github.com/bitcoin-core/secp256k1 (include/*.h, the used subset)

// Flags are those of secp256k1.h, token for token, so either header may
// follow the other.
#define SECP256K1_FLAGS_TYPE_MASK ((1 << 8) - 1)
#define SECP256K1_FLAGS_TYPE_CONTEXT (1 << 0)
#define SECP256K1_FLAGS_TYPE_COMPRESSION (1 << 1)
#define SECP256K1_FLAGS_BIT_CONTEXT_VERIFY (1 << 8)
#define SECP256K1_FLAGS_BIT_CONTEXT_SIGN (1 << 9)
#define SECP256K1_FLAGS_BIT_CONTEXT_DECLASSIFY (1 << 10)
#define SECP256K1_FLAGS_BIT_COMPRESSION (1 << 8)
#define SECP256K1_CONTEXT_NONE (SECP256K1_FLAGS_TYPE_CONTEXT)
#define SECP256K1_CONTEXT_VERIFY (SECP256K1_FLAGS_TYPE_CONTEXT | SECP256K1_FLAGS_BIT_CONTEXT_VERIFY)
#define SECP256K1_CONTEXT_SIGN (SECP256K1_FLAGS_TYPE_CONTEXT | SECP256K1_FLAGS_BIT_CONTEXT_SIGN)
#define SECP256K1_EC_COMPRESSED (SECP256K1_FLAGS_TYPE_COMPRESSION | SECP256K1_FLAGS_BIT_COMPRESSION)
#define SECP256K1_EC_UNCOMPRESSED (SECP256K1_FLAGS_TYPE_COMPRESSION)
#define SECP256K1_TAG_PUBKEY_EVEN 0x02
#define SECP256K1_TAG_PUBKEY_ODD 0x03
#define SECP256K1_TAG_PUBKEY_UNCOMPRESSED 0x04
#define SECP256K1_TAG_PUBKEY_HYBRID_EVEN 0x06
#define SECP256K1_TAG_PUBKEY_HYBRID_ODD 0x07

namespace libbitcoin {
namespace system {
namespace secp256k1 {

/// libsecp256k1 compatible interface to the local implementation, for the
/// functions used by the library. Operations on secret values are blinded
/// but variable time. Opaque types other than the ecdsa signature are
/// implementation defined.

/// Types.
/// ---------------------------------------------------------------------------

struct secp256k1_context;

struct secp256k1_pubkey
{
    data_array<64> data;
};

/// r then s, each as four native 64 bit words, least significant first.
struct secp256k1_ecdsa_signature
{
    data_array<64> data;
};

struct secp256k1_ecdsa_recoverable_signature
{
    data_array<65> data;
};

struct secp256k1_xonly_pubkey
{
    data_array<64> data;
};

struct secp256k1_keypair
{
    data_array<96> data;
};

using secp256k1_nonce_function = int(*)(uint8_t* nonce32,
    const uint8_t* msg32, const uint8_t* key32, const uint8_t* algo16,
    void* data, unsigned int attempt);

using secp256k1_ellswift_xdh_hash_function = int(*)(uint8_t* output,
    const uint8_t* x32, const uint8_t* ell_a64, const uint8_t* ell_b64,
    void* data);

/// RFC6979 nonce derivation (with HMAC-SHA256).
BC_API extern const secp256k1_nonce_function
    secp256k1_nonce_function_rfc6979;

/// BIP324 tagged hash of both encodings and the shared x coordinate.
BC_API extern const secp256k1_ellswift_xdh_hash_function
    secp256k1_ellswift_xdh_hash_function_bip324;

/// Context (secp256k1.h).
/// ---------------------------------------------------------------------------

BC_API secp256k1_context* secp256k1_context_create(unsigned int flags) NOEXCEPT;
BC_API void secp256k1_context_destroy(secp256k1_context* ctx) NOEXCEPT;

/// Keys (secp256k1.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_ec_pubkey_parse(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const uint8_t* input, size_t inputlen) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_serialize(const secp256k1_context* ctx,
    uint8_t* output, size_t* outputlen, const secp256k1_pubkey* pubkey,
    unsigned int flags) NOEXCEPT;

BC_API int secp256k1_ec_seckey_verify(const secp256k1_context* ctx,
    const uint8_t* seckey) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_create(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const uint8_t* seckey) NOEXCEPT;

BC_API int secp256k1_ec_seckey_negate(const secp256k1_context* ctx,
    uint8_t* seckey) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_negate(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey) NOEXCEPT;

BC_API int secp256k1_ec_seckey_tweak_add(const secp256k1_context* ctx,
    uint8_t* seckey, const uint8_t* tweak32) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_tweak_add(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const uint8_t* tweak32) NOEXCEPT;

BC_API int secp256k1_ec_seckey_tweak_mul(const secp256k1_context* ctx,
    uint8_t* seckey, const uint8_t* tweak32) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_tweak_mul(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const uint8_t* tweak32) NOEXCEPT;

BC_API int secp256k1_ec_pubkey_combine(const secp256k1_context* ctx,
    secp256k1_pubkey* out, const secp256k1_pubkey* const* ins,
    size_t n) NOEXCEPT;

/// ECDSA (secp256k1.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_ecdsa_signature_parse_compact(
    const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig,
    const uint8_t* input64) NOEXCEPT;

BC_API int secp256k1_ecdsa_signature_serialize_der(
    const secp256k1_context* ctx, uint8_t* output, size_t* outputlen,
    const secp256k1_ecdsa_signature* sig) NOEXCEPT;

BC_API int secp256k1_ecdsa_signature_serialize_compact(
    const secp256k1_context* ctx, uint8_t* output64,
    const secp256k1_ecdsa_signature* sig) NOEXCEPT;

BC_API int secp256k1_ecdsa_signature_normalize(const secp256k1_context* ctx,
    secp256k1_ecdsa_signature* sigout,
    const secp256k1_ecdsa_signature* sigin) NOEXCEPT;

BC_API int secp256k1_ecdsa_verify(const secp256k1_context* ctx,
    const secp256k1_ecdsa_signature* sig, const uint8_t* msghash32,
    const secp256k1_pubkey* pubkey) NOEXCEPT;

BC_API int secp256k1_ecdsa_sign(const secp256k1_context* ctx,
    secp256k1_ecdsa_signature* sig, const uint8_t* msghash32,
    const uint8_t* seckey, secp256k1_nonce_function noncefp,
    const void* ndata) NOEXCEPT;

/// ECDSA recovery (secp256k1_recovery.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_ecdsa_recoverable_signature_parse_compact(
    const secp256k1_context* ctx, secp256k1_ecdsa_recoverable_signature* sig,
    const uint8_t* input64, int recid) NOEXCEPT;

BC_API int secp256k1_ecdsa_recoverable_signature_serialize_compact(
    const secp256k1_context* ctx, uint8_t* output64, int* recid,
    const secp256k1_ecdsa_recoverable_signature* sig) NOEXCEPT;

BC_API int secp256k1_ecdsa_sign_recoverable(const secp256k1_context* ctx,
    secp256k1_ecdsa_recoverable_signature* sig, const uint8_t* msghash32,
    const uint8_t* seckey, secp256k1_nonce_function noncefp,
    const void* ndata) NOEXCEPT;

BC_API int secp256k1_ecdsa_recover(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const secp256k1_ecdsa_recoverable_signature* sig,
    const uint8_t* msghash32) NOEXCEPT;

/// Extra keys (secp256k1_extrakeys.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_xonly_pubkey_parse(const secp256k1_context* ctx,
    secp256k1_xonly_pubkey* pubkey, const uint8_t* input32) NOEXCEPT;

BC_API int secp256k1_xonly_pubkey_tweak_add_check(
    const secp256k1_context* ctx, const uint8_t* tweaked_pubkey32,
    int tweaked_pk_parity, const secp256k1_xonly_pubkey* internal_pubkey,
    const uint8_t* tweak32) NOEXCEPT;

BC_API int secp256k1_keypair_create(const secp256k1_context* ctx,
    secp256k1_keypair* keypair, const uint8_t* seckey) NOEXCEPT;

/// Schnorr (secp256k1_schnorrsig.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_schnorrsig_sign32(const secp256k1_context* ctx,
    uint8_t* sig64, const uint8_t* msg32, const secp256k1_keypair* keypair,
    const uint8_t* aux_rand32) NOEXCEPT;

BC_API int secp256k1_schnorrsig_verify(const secp256k1_context* ctx,
    const uint8_t* sig64, const uint8_t* msg, size_t msglen,
    const secp256k1_xonly_pubkey* pubkey) NOEXCEPT;

/// ElligatorSwift (secp256k1_ellswift.h).
/// ---------------------------------------------------------------------------

BC_API int secp256k1_ellswift_decode(const secp256k1_context* ctx,
    secp256k1_pubkey* pubkey, const uint8_t* ell64) NOEXCEPT;

BC_API int secp256k1_ellswift_create(const secp256k1_context* ctx,
    uint8_t* ell64, const uint8_t* seckey32, const uint8_t* auxrnd32) NOEXCEPT;

BC_API int secp256k1_ellswift_xdh(const secp256k1_context* ctx,
    uint8_t* output, const uint8_t* ell_a64, const uint8_t* ell_b64,
    const uint8_t* seckey32, int party,
    secp256k1_ellswift_xdh_hash_function hashfp, void* data) NOEXCEPT;

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
