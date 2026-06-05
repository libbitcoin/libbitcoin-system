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
namespace schnorr {
    
/// Span matches serialized buffer.
struct BC_API signatures
{
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;
    using span = std::span<const signatures>;
    static links verify(const span& batch, bool turbo) NOEXCEPT;

    hash_digest digest;
    ec_xonly point;
    ec_signature signature;
    link id;

protected:
    static data_chunk batch(const span& batch, bool turbo) NOEXCEPT;
    static links correlate(const data_chunk& out, const span& batch) NOEXCEPT;
};

} // namespace schnorr

namespace ecdsa {

/// Span matches serialized buffer.
struct BC_API signatures
{
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;
    using span = std::span<const signatures>;
    static links verify(const span& batch, bool turbo) NOEXCEPT;

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    link id;

protected:
    static data_chunk batch(const span& batch, bool turbo) NOEXCEPT;
    static links correlate(const data_chunk& out, const span& batch) NOEXCEPT;
};

} // namespace ecdsa

namespace multisig {
    
/// Span matches serialized buffer.
struct BC_API signatures
{
    using link = data_array<3>;
    using link_t = unsigned_type<sizeof(link)>;
    using links = std::vector<link_t>;
    using span = std::span<const signatures>;
    static links verify(const span& batch, bool turbo)  NOEXCEPT;

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    uint8_t pair;
    uint16_t set;
    link id;

protected:
    static data_chunk batch(const span& batch, bool turbo) NOEXCEPT;
    static links correlate(const data_chunk& out, const span& batch) NOEXCEPT;
};

} // namespace multisig
} // namespace system
} // namespace libbitcoin

#pragma pack(pop)

#endif
