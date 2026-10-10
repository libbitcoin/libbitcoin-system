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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_SCHNORR_SIGNATURES_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_SCHNORR_SIGNATURES_HPP

#include <span>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_USE_OF_SPAN)

/// Accumulator of captured schnorr verification rows (fixed-width AoS).
/// Threshold rows are indistinguishable from single rows in the store (the
/// correlate column carries only the block link); counted for reporting.
class schnorr_signatures
{
public:
    /// AoS row mirroring the store's schnorr SoA columns.
    struct row
    {
        hash_digest digest;
        ec_xonly point;
        ec_signature signature;
    };

    /// Append one row (cannot decline).
    inline void append(const hash_digest& digest, const ec_xonly& point,
        const ec_signature& signature) NOEXCEPT
    {
        rows_.push_back(row{ digest, point, signature });
    }

    /// Count expected threshold rows (reporting only).
    inline void count_threshold(size_t rows) NOEXCEPT
    {
        thresholds_ += rows;
    }

    /// All captured rows, in capture order.
    inline std::span<const row> rows() const NOEXCEPT
    {
        return rows_;
    }

    /// Rows captured through threshold cursors (reporting only).
    inline size_t thresholds() const NOEXCEPT
    {
        return thresholds_;
    }

    /// True if no rows are captured.
    inline bool empty() const NOEXCEPT
    {
        return rows_.empty();
    }

    /// Verify all rows in place (store commit failure fallback).
    inline bool verify() const NOEXCEPT
    {
        return std::all_of(rows_.begin(), rows_.end(),
            [](const row& record) NOEXCEPT
            {
                return schnorr::verify_signature(record.point, record.digest,
                    record.signature);
            });
    }

    /// Reset for next block, retaining capacity.
    inline void clear() NOEXCEPT
    {
        rows_.clear();
        thresholds_ = zero;
    }

    /// Reset, releasing capacity (see signatures::purge).
    inline void purge() NOEXCEPT
    {
        clear();
        rows_.shrink_to_fit();
    }

private:
    std::vector<row> rows_{};
    size_t thresholds_{};
};

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
