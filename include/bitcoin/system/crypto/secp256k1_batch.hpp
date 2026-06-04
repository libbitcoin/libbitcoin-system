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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

#pragma pack(push, 1)

namespace libbitcoin {
namespace system {

/// schnorr
/// ---------------------------------------------------------------------------
namespace schnorr {

struct BC_API triple
{
    using token = data_array<3>;
    using tokens = std::vector<token>;
    static constexpr size_t id_size = sizeof(token);

    hash_digest digest;
    ec_xonly point;
    ec_signature signature;
    token identifier;
};

using triples = std::span<const triple>;

/// Verify Schnorr signature of hash by associated secret of the x-only point.
BC_API bool verify_signature(const triple& single) NOEXCEPT;

/// Verify Schnorr signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated token, otherwise empty.
BC_API triple::tokens verify_signatures(const triples& batch,
    bool turbo) NOEXCEPT;

} // namespace schnorr

/// ecdsa
/// ---------------------------------------------------------------------------
namespace ecdsa {

struct BC_API triple
{
    using token = data_array<3>;
    using tokens = std::vector<token>;
    static constexpr size_t id_size = sizeof(token);

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    token identifier;
};

using triples = std::span<const triple>;

/// Verify ECDSA signature of hash by associated secret of the point.
BC_API bool verify_signature(const triple& single) NOEXCEPT;

/// Verify ECDSA signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated token, otherwise empty.
BC_API triple::tokens verify_signatures(const triples& batch,
    bool turbo) NOEXCEPT;

} // namespace ecdsa

/// multisig (ecdsa)
/// ---------------------------------------------------------------------------
namespace multisig {

struct BC_API triple
{
    using token = data_array<3>;
    using tokens = std::vector<token>;

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    token identifier;
    uint16_t set;
    uint8_t pair;

    static constexpr size_t id_size = sizeof(token) + sizeof(set) +
        sizeof(pair);
};

using triples = std::span<const triple>;

/// Verify ECDSA signature of hash by associated secret of the point.
BC_API bool verify_signature(const triple& single) NOEXCEPT;

/// Verify ECDSA signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated token, otherwise empty.
BC_API triple::tokens verify_signatures(const triples& batch,
    bool NOT_ULTRAFAST(turbo)) NOEXCEPT;

} // namespace multisig

} // namespace system
} // namespace libbitcoin

#pragma pack(pop)

#endif
