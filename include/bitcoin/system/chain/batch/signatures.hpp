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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_SIGNATURES_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_SIGNATURES_HPP

#include <atomic>
#include <span>
#include <bitcoin/system/chain/batch/ecdsa_signatures.hpp>
#include <bitcoin/system/chain/batch/schnorr_signatures.hpp>
#include <bitcoin/system/chain/batch/threshold.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_USE_OF_SPAN)

/// Thread-static per-block signature capture accumulators. An accumulator is
/// populated by one block's sequential connect (single-threaded, lock-free)
/// and bulk-committed to the corresponding store table upon its completion,
/// deinterleaving the AoS log into the store's SoA columns. Capacity is
/// retained across blocks (cleared, not freed) and released only by
/// signatures::purge() once batching subsides.

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
        const std::span<const ec_compressed>& keys,
        const std::span<const ec_signature>& sigs) const NOEXCEPT;

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

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
