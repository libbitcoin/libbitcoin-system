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
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>

#pragma pack(push, 1)

namespace libbitcoin {
namespace system {

namespace batched {
using link = data_array<3>;
using link_t = unsigned_type<sizeof(link)>;
using links_t = std::vector<link_t>;
constexpr link terminal{ 0xff, 0xff, 0xff };
} // namespace batched

namespace ecdsa {

/// Span matches serialized buffer.
struct BC_API batch
{
    using span = std::span<const batch>;
    static batched::links_t verify(const stopper& cancel,
        const span& batch) NOEXCEPT;

    hash_digest digest;
    ec_compressed point;
    ec_signature signature;
    uint8_t pair;
    uint16_t group;
    batched::link id;

protected:
    using multisig_matrix = std::array<uint16_t, bits<uint16_t>>;
    static bool meets_threshold(uint8_t signatures, uint8_t keys,
        const multisig_matrix& successes) NOEXCEPT;
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const span& in) NOEXCEPT;
    static data_chunk evaluate(const stopper& cancel,
        const span& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const span& batch) NOEXCEPT;
};

} // namespace ecdsa

namespace schnorr {
    
/// Span matches serialized buffer.
struct BC_API batch
{
    using span = std::span<const batch>;
    static batched::links_t verify(const stopper& cancel,
        const span& batch) NOEXCEPT;

    hash_digest digest;
    ec_xonly point;
    ec_signature signature;
    uint8_t category;
    uint16_t pair;
    uint16_t group;
    batched::link id;

protected:
    static bool meets_threshold(uint8_t category, size_t successes,
        size_t minimum, size_t maximum) NOEXCEPT;
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const span& in) NOEXCEPT;
    static data_chunk evaluate(const stopper& cancel,
        const span& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const span& batch) NOEXCEPT;
};

} // namespace schnorr

namespace silent
{

/// Span matches serialized buffer.
struct BC_API batch
{
    using prefix = data_array<8>;
    using tx_link = data_array<4>;
    using tx_link_t = unsigned_type<sizeof(tx_link)>;
    ////using tx_links_t = std::vector<tx_link_t>;
    using handler = std::function<void(const code&, tx_link_t)>;

    std::span<const prefix> prefixes;
    std::span<const ec_compressed> compresseds;
    std::span<const tx_link> correlates;

    static void scan(const stopper& cancel, const batch& batch,
        const ec_secret& scan_key, const handler& callback,
        bool turbo) NOEXCEPT;

protected:
    static bool get_match(tx_link_t& out, const batch& batch,
        size_t row, const ec_secret& scan_key) NOEXCEPT;
};

} // namespace silent
} // namespace system
} // namespace libbitcoin

#pragma pack(pop)

#endif
