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

#include <atomic>
#include <span>
#include <bitcoin/system/chain/batch/multisig.hpp>
#include <bitcoin/system/chain/batch/threshold.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A capture context passed into machine::interpreter. When enabled, sigops
/// in fully-determining scripts stream signature rows into caller storage
/// and fabricate success, the caller batch-validates and correlates block
/// failure. Any store decline (allocation or commit) sets `faulted` and the
/// affected sigop falls through to inline verification, so block validity
/// is always fully determined, `faulted` is store-health telemetry only.
struct BC_API signatures
{
    /// Reporting enumeration for capture misses.
    enum class miss { ecdsa, multisig, schnorr };

    /// Reporting handlers.
    using log_handler = std::function<void(const script&)>;
    using fire_handler = std::function<void(miss, size_t)>;

    /// Single-sig captures, false implies write was not committed (fault).
    using ecdsa_handler = std::function<bool(const hash_digest&,
        const ec_compressed&, const ec_signature&)>;
    using schnorr_handler = std::function<bool(const hash_digest&,
        const ec_xonly&, const ec_signature&)>;

    /// Multisig capture: one bulk write per group, keys.size() = n,
    /// sigs.size() = m; store expands the band.
    using multisig_handler = std::function<bool(const hash_digest&,
        std::span<const ec_compressed>, std::span<const ec_signature>)>;

    /// Threshold capture: allocate all rows, stream through the cursor.
    using threshold_handler = std::function<
        chain::threshold::cursor(size_t rows)>;

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
    const ecdsa_handler ecdsa
    {
        [](const hash_digest&, const ec_compressed&,
            const ec_signature&) NOEXCEPT
        {
            return false;
        }
    };
    const schnorr_handler schnorr
    {
        [](const hash_digest&, const ec_xonly&, const ec_signature&) NOEXCEPT
        {
            return false;
        }
    };
    const multisig_handler multisig
    {
        [](const hash_digest&, std::span<const ec_compressed>,
            std::span<const ec_signature>) NOEXCEPT
        {
            return false;
        }
    };
    const threshold_handler threshold
    {
        // max_size_t cursor implies possibly recoverable fault.
        [](size_t) NOEXCEPT
        {
            return chain::threshold::cursor{};
        }
    };

    /// A store decline occurred during capture (block validity intact).
    mutable std::atomic_bool faulted{};

    /// Signatures were batched for the block.
    mutable std::atomic_bool batched{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
