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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_SIGNATURES_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_SIGNATURES_HPP

#include <algorithm>
#include <atomic>
#include <iterator>
#include <span>
#include <vector>
#include <bitcoin/system/chain/batch/multisig.hpp>
#include <bitcoin/system/chain/batch/threshold.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Thread-static per-block signature capture accumulators. An accumulator is
/// populated by one block's sequential connect (single-threaded, lock-free)
/// and bulk-committed to the corresponding store table upon its completion,
/// deinterleaving the AoS log into the store's SoA columns. Capacity is
/// retained across blocks (cleared, not freed) and released only by
/// signatures::purge() once batching subsides.

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
        rows_ = {};
        thresholds_ = zero;
    }

private:
    std::vector<row> rows_{};
    size_t thresholds_{};
};

/// Accumulator of captured ecdsa verification groups (variable-width AoS).
/// Each record is one group: a single sigop (1 of 1) or a multisig band
/// (m of n). The group id is the record ordinal, capped at max_uint16. The
/// store expands each group to its banded rows upon commit.
class ecdsa_signatures
{
public:
    /// Append one group record, false implies group capacity (decline).
    inline bool append(const hash_digest& digest,
        std::span<const ec_compressed> keys,
        std::span<const ec_signature> sigs) NOEXCEPT
    {
        const auto sigs_count = sigs.size();
        const auto keys_count = keys.size();
        BC_ASSERT(multisig::check(sigs_count, keys_count));

        if (groups_ > max_uint16)
            return false;

        log_.push_back(possible_narrow_cast<uint8_t>(sigs_count));
        log_.push_back(possible_narrow_cast<uint8_t>(keys_count));
        append(digest);
        for (const auto& key: keys) append(key);
        for (const auto& sig: sigs) append(sig);

        ++groups_;
        rows_ += multisig::rows(sigs_count, keys_count);
        if (is_one(sigs_count) && is_one(keys_count)) ++singles_;
        else keys_ += keys_count;
        return true;
    }

    /// Visit each group in capture order (group id is the visit ordinal).
    /// Handler: void(const hash_digest&, std::span<const ec_compressed>,
    /// std::span<const ec_signature>).
    template <typename Handler>
    inline void for_each(Handler&& handler) const NOEXCEPT
    {
        auto it = log_.data();
        const auto end = std::next(it, to_signed(log_.size()));
        while (it != end)
        {
            const size_t sigs_count = *it++;
            const size_t keys_count = *it++;
            const auto& digest = unsafe_array_cast<uint8_t, hash_size>(it);
            std::advance(it, to_signed(hash_size));
            const std::span<const ec_compressed> keys
            {
                pointer_cast<const ec_compressed>(it), keys_count
            };
            std::advance(it, to_signed(keys_count * ec_compressed_size));
            const std::span<const ec_signature> sigs
            {
                pointer_cast<const ec_signature>(it), sigs_count
            };
            std::advance(it, to_signed(sigs_count * ec_signature_size));
            handler(digest, keys, sigs);
        }
    }

    /// Group (record) count, the group id domain.
    inline size_t groups() const NOEXCEPT
    {
        return groups_;
    }

    /// Total banded row expansion across all groups (store rows).
    inline size_t rows() const NOEXCEPT
    {
        return rows_;
    }

    /// Single sigop (1 of 1) group count (reporting only).
    inline size_t singles() const NOEXCEPT
    {
        return singles_;
    }

    /// Total keys across multisig groups (reporting only).
    inline size_t multisig_keys() const NOEXCEPT
    {
        return keys_;
    }

    /// True if no groups are captured.
    inline bool empty() const NOEXCEPT
    {
        return is_zero(groups_);
    }

    /// Verify all groups in place (store commit failure fallback). Each group
    /// applies the sequential op_checkmultisig trial (1 of 1 degenerates to a
    /// single verify), exactly the evaluation its capture fabricated.
    bool verify() const NOEXCEPT
    {
        auto success = true;
        for_each([&](const hash_digest& digest,
            std::span<const ec_compressed> keys,
            std::span<const ec_signature> sigs) NOEXCEPT
        {
            success = verify_group(digest, keys, sigs) && success;
        });

        return success;
    }

    /// Reset for next block, retaining capacity.
    inline void clear() NOEXCEPT
    {
        log_.clear();
        groups_ = zero;
        rows_ = zero;
        singles_ = zero;
        keys_ = zero;
    }

    /// Reset, releasing capacity (see signatures::purge).
    inline void purge() NOEXCEPT
    {
        log_ = {};
        groups_ = zero;
        rows_ = zero;
        singles_ = zero;
        keys_ = zero;
    }

protected:
    template <size_t Size>
    inline void append(const data_array<Size>& bytes) NOEXCEPT
    {
        log_.insert(log_.end(), bytes.begin(), bytes.end());
    }

    static bool verify_group(const hash_digest& digest,
        std::span<const ec_compressed> keys,
        std::span<const ec_signature> sigs) NOEXCEPT
    {
        auto key = keys.begin();
        auto sig = sigs.begin();
        while ((sig != sigs.end()) &&
            (std::distance(key, keys.end()) >= std::distance(sig, sigs.end())))
        {
            if (ecdsa::verify_signature(*key, digest, *sig)) ++sig;
            ++key;
        }

        return sig == sigs.end();
    }

private:
    data_chunk log_{};
    size_t groups_{};
    size_t rows_{};
    size_t singles_{};
    size_t keys_{};
};

/// A capture context passed into machine::interpreter. When enabled, sigops
/// in fully-determining scripts append signature rows to this thread's
/// accumulators and fabricate success. The caller bulk-commits the
/// accumulators to store upon block connect completion and batch-validates
/// at drain, correlating block failure. Appends cannot decline on store
/// state; the only decline is the ecdsa group id domain (uint16), so the
/// affected sigop falls through to inline verification and block validity is
/// always fully determined (`faulted` is telemetry only).
struct BC_API signatures
{
    /// Reporting enumeration for capture misses.
    enum class miss { ecdsa, multisig, schnorr };

    /// Reporting handlers.
    using log_handler = std::function<void(const script&)>;
    using fire_handler = std::function<void(miss, size_t)>;

    /// This thread's accumulators (self-registered upon first access).
    static ecdsa_signatures& ecdsa_rows() NOEXCEPT;
    static schnorr_signatures& schnorr_rows() NOEXCEPT;

    /// Release the capacity of all threads' accumulators (batching
    /// subsided). Caller must exclude capture (e.g. post to the validation
    /// strand with zero validation backlog), as threads append without
    /// locking.
    static void purge() NOEXCEPT;

    /// Single-sig captures, false implies decline (ecdsa group id domain).
    bool ecdsa(const hash_digest& digest, const ec_compressed& key,
        const ec_signature& signature) const NOEXCEPT;
    bool schnorr(const hash_digest& digest, const ec_xonly& point,
        const ec_signature& signature) const NOEXCEPT;

    /// Multisig capture: one group record, keys.size() = n, sigs.size() = m;
    /// the store expands the band upon commit.
    bool multisig(const hash_digest& digest,
        std::span<const ec_compressed> keys,
        std::span<const ec_signature> sigs) const NOEXCEPT;

    /// Threshold capture: open a cursor streaming rows to this thread's
    /// schnorr accumulator (cannot decline).
    chain::threshold::cursor threshold(size_t rows) const NOEXCEPT;

    /// Default construction disables batching.
    const bool enabled{};

    /// Replace with operative handlers.
    const log_handler log
    {
        [](const script&) NOEXCEPT {}
    };
    const fire_handler fire
    {
        [](miss, size_t) NOEXCEPT {}
    };

    /// A capture decline occurred (block validity intact, telemetry only).
    mutable std::atomic_bool faulted{};

    /// Signatures were batched for the block.
    mutable std::atomic_bool batched{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
