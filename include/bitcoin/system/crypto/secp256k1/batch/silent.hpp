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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SILENT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SILENT_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1/batch/scan.hpp>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {

namespace silent {

/// Spans match serialized columns, one element for each transaction.
struct BC_API batch
{
    std::span<const ec_compressed> sums;
    std::span<const ec_secret> hashes;

    /// Each out = hash * sum of the transaction, with valid set for each.
    /// The out and valid spans are sized to the batch. False if canceled.
    static bool compute(const std::span<ec_compressed>& out,
        const std::span<uint8_t>& valid, const stopper& cancel,
        const batch& batch) NOEXCEPT;

    /// Device is set if the batch was computed on the device, which is used
    /// only if gpu and available.
    static bool compute(const std::span<ec_compressed>& out,
        const std::span<uint8_t>& valid, bool& device, const stopper& cancel,
        const batch& batch, bool gpu) NOEXCEPT;

protected:
    /// Transactions computed by one task.
    static constexpr size_t chunk_rows = power2(10_size);
};

} // namespace silent
} // namespace system
} // namespace libbitcoin

#endif
