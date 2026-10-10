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

/// Span matches serialized buffer, a row for each output of a transaction.
/// Rows of one transaction are contiguous and share one sum and input hash.
struct BC_API batch
{
#pragma pack(push, 1)
    struct row_t
    {
        scan::batch::prefix prefix;
        ec_compressed sum;
        ec_secret hash;
    };
#pragma pack(pop)

    std::span<const row_t> rows;

    /// Each out = hash * sum of the row, valid set for each row, computed
    /// once for each run of rows of equal sum and hash. False if canceled.
    static bool compute(std_vector<ec_compressed>& out, data_chunk& valid,
        const stopper& cancel, const batch& batch) NOEXCEPT;

    /// Device is set if the batch was computed on the device.
    static bool compute(std_vector<ec_compressed>& out, data_chunk& valid,
        bool& device, const stopper& cancel, const batch& batch) NOEXCEPT;

protected:
    /// Transactions computed by one task.
    static constexpr size_t chunk_rows = power2(10_size);

    /// Computations of at least this many transactions run on the device
    /// where available.
    static constexpr size_t device_rows = power2(16_size);
};

} // namespace silent
} // namespace system
} // namespace libbitcoin

#endif
