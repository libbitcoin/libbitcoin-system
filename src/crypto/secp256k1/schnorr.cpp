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

#include <secp256k1.h>
#include <secp256k1_schnorrsig.h>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {
namespace schnorr {

static constexpr auto ec_success = 1;

// Schnorr parse/sign/verify
// ----------------------------------------------------------------------------
// `ec_signature` is a public format for schnorr.

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
bool verify_signature(const data_chunk& x_point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    constexpr auto size = ec_xonly_size;
    if (x_point.size() != size)
        return false;

    const auto& public_key = unsafe_array_cast<uint8_t, size>(x_point.data());
    return verify_signature(public_key, hash, signature);
}

// BIP341: A Taproot signature is a 64-byte Schnorr sig, as defined in BIP340.
bool verify_signature(const ec_xonly& x_point, const hash_digest& hash,
    const ec_signature& signature) NOEXCEPT
{
    secp256k1_xonly_pubkey pubkey;
    const auto context = ec_context_verify::context();

    return
        secp256k1_xonly_pubkey_parse(context, &pubkey, x_point.data()) ==
            ec_success && 
        secp256k1_schnorrsig_verify(context, signature.data(), hash.data(),
            hash_size, &pubkey) == ec_success;
}

// BIP341: If q != x(Q) or c[0] & 1 != y(Q) mod 2, fail.
bool verify_commitment(const ec_xonly& internal_key, const hash_digest& tweak,
    const ec_xonly& tweaked_key, bool tweaked_key_parity) NOEXCEPT
{
    secp256k1_xonly_pubkey pubkey;
    const auto context = ec_context_verify::context();
    const auto parity = to_int(tweaked_key_parity);

    return
        secp256k1_xonly_pubkey_parse(context, &pubkey, internal_key.data()) ==
            ec_success &&
        secp256k1_xonly_pubkey_tweak_add_check(context, tweaked_key.data(),
            parity, &pubkey, tweak.data()) == ec_success;
}

} // namespace schnorr
} // namespace system
} // namespace libbitcoin
