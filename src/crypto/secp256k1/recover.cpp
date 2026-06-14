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
#include <secp256k1_recovery.h>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {
namespace ecdsa {
    
// local
// ----------------------------------------------------------------------------

static constexpr auto ec_success = 1;

template <size_t Size>
static bool recover_public(const secp256k1_context* context,
    data_array<Size>& out, const recoverable_signature& recoverable,
    const hash_digest& hash) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    secp256k1_ecdsa_recoverable_signature sign;
    const auto recovery_id = sign_cast<int>(recoverable.recovery_id);
    return
        secp256k1_ecdsa_recoverable_signature_parse_compact(context,
            &sign, recoverable.signature.data(), recovery_id) == ec_success &&
        secp256k1_ecdsa_recover(context, &pubkey, &sign, hash.data()) ==
            ec_success && ec_public_key_serialize(context, out, pubkey);
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
            out.signature.data(), &recovery_id, &signature) ==
            ec_success;

    static constexpr auto maximum_recovery_id = 3;
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

} // namespace ecdsa
} // namespace system
} // namespace libbitcoin
