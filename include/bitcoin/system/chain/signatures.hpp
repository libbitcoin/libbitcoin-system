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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SIGNATURES_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SIGNATURES_HPP

#include <atomic>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A container for passing signature triples into a caller's buffer.
/// When enabled and passed to machine::interpreter this causes
/// signature_verify() calls to return true in scripts for which this one call
/// fully determines success/failure of the script. This allows the caller to
/// batch validate the signatures and through its own correlation determine
/// which if any block (or transaction depending on correlation) has failed.
struct BC_API signatures
{
    /// Reporting enumeration for capture misses.
    enum class miss
    {
        /// Failed to capture an ecdsa signature.
        ecdsa,

        /// Failed to capture ecdsa signatures.
        multisig,

        /// Failed to capture a schnorr signature (single|multiple).
        schnorr,

        /// Failed to capture schnorr signatures (multiple >= 2^16).
        overflow
    };

    /// Threshold category.
    enum class category
    {
        single,
        equal,
        inequal,
        lesser,
        greater,
        not_lesser,
        not_greater,
        between
    };

    struct threshold_entries
    {
        struct entry
        {
            /// Digest is created in the sigop (sigop scope - must copy).
            hash_digest digest;

            /// Point is a stack element (script scope - use reference).
            cref<ec_xonly> point;

            /// Signature is a stack element (script scope - use reference).
            cref<ec_signature> sig;
        };

        /// Scoping requires that capture_.threshold(threshold_entries) does not
        /// retain a reference to point or sig (must copy or dispose the refs).
        inline bool push_entry(const hash_digest& digest,
            const cref<ec_xonly>& point,
            const cref<ec_signature>& sig) NOEXCEPT
        {
            BC_ASSERT(entries.size() == expected);
            entries.emplace_back(digest, point, sig);
            return entries.size() == expected;
        }

        std::vector<entry> entries{};
        opcode condition{};
        uint16_t minimum{};
        uint16_t maximum{};
        uint16_t expected{};
    };

    /// Reporting handlers.
    using fire_handler = std::function<void(miss)>;
    using log_handler = std::function<void(const script&)>;

    /// False implies script should execute because store cannot accept.
    using ecdsa_handler = std::function<bool(const hash_digest&,
        const ec_compressed&, const ec_signature&)>;
    using schnorr_handler = std::function<bool(const hash_digest&,
        const ec_xonly, const ec_signature&)>;
    using multisig_handler = std::function<bool(const hash_digest&,
        const ec_compresseds&, const ec_signatures&)>;

    /// False implies script must validate signatures or abort.
    using threshold_handler = std::function<void(const threshold_entries&)>;

    /// Default construction disables batching.
    const bool enabled{};

    /// Replace with operative handlers.
    /// Allows script interpreter to emit log info via caller.
    const log_handler log
    {
        [] (const script&) NOEXCEPT {}
    };
    const fire_handler fire
    {
        [] (miss) NOEXCEPT {}
    };

    /// Replace with operative handlers.
    /// Invoked with signature triples for capture in script interpreter.
    const ecdsa_handler ecdsa
    {
        [] (const hash_digest&, const ec_compressed&,
            const ec_signature&) NOEXCEPT
        {
            return false;
        }
    };
    const schnorr_handler schnorr
    {
        [] (const hash_digest&, const ec_xonly,
            const ec_signature&) NOEXCEPT
        {
            return false;
        }
    };
    const multisig_handler multisig
    {
        [] (const hash_digest&, const ec_compresseds&,
            const ec_signatures&) NOEXCEPT
        {
            return false;
        }
    };
    const threshold_handler threshold
    {
        [] (const threshold_entries&) NOEXCEPT
        {
            return false;
        }
    };
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
