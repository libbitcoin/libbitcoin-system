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
#pragma pack(push, 1)
    struct correlate_t
    {
        batched::link id;
        uint8_t pair;
        uint16_t group;
    };
#pragma pack(pop)

    static batched::links_t verify(const stopper& cancel,
        const batch& batch) NOEXCEPT;

    std::span<const correlate_t> correlates;
    std::span<const hash_digest> digests;
    std::span<const ec_compressed> points;
    std::span<const ec_signature> signatures;

protected:
    using multisig_matrix = std::array<uint16_t, bits<uint16_t>>;
    static bool meets_threshold(uint8_t signatures, uint8_t keys,
        const multisig_matrix& successes) NOEXCEPT;
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const batch& in) NOEXCEPT;
    static data_chunk evaluate(const stopper& cancel,
        const batch& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const batch& batch) NOEXCEPT;
};

} // namespace ecdsa

namespace schnorr {
    
/// Span matches serialized buffer.
struct BC_API batch
{
#pragma pack(push, 1)
    struct correlate_t
    {
        batched::link id;
        uint8_t category;
        uint16_t pair;
        uint16_t group;
    };
#pragma pack(pop)

    static batched::links_t verify(const stopper& cancel,
        const batch& batch) NOEXCEPT;

    std::span<const correlate_t> correlates;
    std::span<const hash_digest> digests;
    std::span<const ec_xonly> points;
    std::span<const ec_signature> signatures;

protected:
    static bool meets_threshold(uint8_t category, size_t successes,
        size_t minimum, size_t maximum) NOEXCEPT;
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const batch& in) NOEXCEPT;
    static data_chunk evaluate(const stopper& cancel,
        const batch& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const batch& batch) NOEXCEPT;
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

    std::span<const tx_link> correlates;
    std::span<const prefix> prefixes;
    std::span<const ec_compressed> points;

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

#endif
