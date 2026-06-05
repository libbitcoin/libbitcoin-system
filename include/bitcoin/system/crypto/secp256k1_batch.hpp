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
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;
    static constexpr size_t metadata_size = sizeof(link);

    hash_digest digest;
    ec_xonly point;
    ec_signature signature;
    link id;
};
using triples = std::span<const triple>;
} // namespace schnorr

/// Verify Schnorr signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated index, otherwise empty.
BC_API schnorr::triple::links verify_signatures(const schnorr::triples& batch,
    bool turbo) NOEXCEPT;

/// ecdsa
/// ---------------------------------------------------------------------------
namespace ecdsa {
struct BC_API triple
{
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;
    static constexpr size_t metadata_size = sizeof(link);

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    link id;
};
using triples = std::span<const triple>;
} // namespace ecdsa

/// Verify ECDSA signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated index, otherwise empty.
BC_API ecdsa::triple::links verify_signatures(const ecdsa::triples& batch,
    bool turbo) NOEXCEPT;

/// multisig (ecdsa)
/// ---------------------------------------------------------------------------
namespace multisig {
struct BC_API triple
{
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    uint8_t pair;
    uint16_t set;
    link id;

    static constexpr size_t metadata_size = sizeof(link) + sizeof(set) +
        sizeof(pair);
};
using triples = std::span<const triple>;
} // namespace multisig

/// Verify ECDSA signatures of { message, public key, signature } triples.
/// Failed rows are identified by return of associated index, otherwise empty.
BC_API multisig::triple::links verify_signatures(
    const multisig::triples& batch, bool turbo) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#pragma pack(pop)

#endif
