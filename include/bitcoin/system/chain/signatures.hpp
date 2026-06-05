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
    using ecdsa_handler = std::function<void(const hash_digest&,
        const ec_compressed&, const ec_signature&)>;

    using schnorr_handler = std::function<void(const hash_digest&,
        const ec_xonly, const ec_signature&)>;

    using multisig_handler = std::function<void(const hash_digest&,
        const ec_compresseds&, const ec_signatures&)>;

    /// Invoked with signature verification triples in script interpreter.
    const ecdsa_handler ecdsa{};
    const schnorr_handler schnorr{};
    const multisig_handler multisig{};

    /// Default construction disables batching.
    const bool enabled{};

    /// Diagnostic counters.
    mutable std::atomic<size_t> batched_ecdsa{};
    mutable std::atomic<size_t> unbatched_ecdsa{};
    mutable std::atomic<size_t> batched_schnorr{};
    mutable std::atomic<size_t> unbatched_schnorr{};
    mutable std::atomic<size_t> batched_multisig{};
    mutable std::atomic<size_t> unbatched_multisig{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
