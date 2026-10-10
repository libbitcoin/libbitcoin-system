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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_ECDSA_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_ECDSA_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/batch/verify.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {

namespace ecdsa {

/// Spans match serialized buffers, a row for each correlate.
struct BC_API batch
{
#pragma pack(push, 1)
    struct correlate_t
    {
        batched::link id;
        uint8_t pair;
        uint16_t group;
    };

    struct row_t
    {
        hash_digest digest;
        ec_compressed point;
        ec_signature signature;
    };
#pragma pack(pop)

    static batched::links_t verify(const stopper& cancel,
        const batch& batch) NOEXCEPT;

    /// Device is set if the batch was verified on the device.
    static batched::links_t verify(bool& device, const stopper& cancel,
        const batch& batch) NOEXCEPT;

    std::span<const correlate_t> correlates;
    std::span<const row_t> rows;

protected:
    using multisig_matrix = std::array<uint16_t, bits<uint16_t>>;
    static bool meets_threshold(uint8_t signatures, uint8_t keys,
        const multisig_matrix& successes) NOEXCEPT;
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const batch& in) NOEXCEPT;
    static data_chunk evaluate(bool& device, const stopper& cancel,
        const batch& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const batch& batch) NOEXCEPT;
};

} // namespace ecdsa
} // namespace system
} // namespace libbitcoin

#endif
