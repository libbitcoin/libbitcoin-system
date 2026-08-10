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
#include <bitcoin/system/crypto/secp256k1.hpp>

#include <secp256k1.h>
#include <secp256k1_ellswift.h>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {
namespace ellswift {

static constexpr auto ec_success = 1;

// ElligatorSwift create/decode/exchange (bip324)
// ----------------------------------------------------------------------------

bool create(ec_ellswift& out, const ec_secret& secret,
    const hash_digest& auxiliary) NOEXCEPT
{
    const auto context = ec_context_sign::context();

    return secp256k1_ellswift_create(context, out.data(), secret.data(),
        auxiliary.data()) == ec_success;
}

bool decode(ec_compressed& out, const ec_ellswift& key) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();

    return
        secp256k1_ellswift_decode(context, &pubkey, key.data()) ==
            ec_success &&
        ec_public_key_serialize(context, out, pubkey);
}

// BIP324: v2_ecdh, the tagged hash of the x-only shared point and the two
// encoded public keys, initiating party first (party a).
bool exchange(hash_digest& out, const ec_secret& secret,
    const ec_ellswift& key_a, const ec_ellswift& key_b,
    bool responding) NOEXCEPT
{
    const auto context = ec_context_verify::context();

    return secp256k1_ellswift_xdh(context, out.data(), key_a.data(),
        key_b.data(), secret.data(), to_int(responding),
        secp256k1_ellswift_xdh_hash_function_bip324, nullptr) == ec_success;
}

} // namespace ellswift
} // namespace system
} // namespace libbitcoin
