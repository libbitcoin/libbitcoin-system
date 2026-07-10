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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_THRESHOLD_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_THRESHOLD_HPP

#include <bitcoin/system/chain/batch/cursor.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Script helper for schnorr threshold capture.
struct threshold
{
    /// Capture fabricates success for every sigop, and bip342 terminates the
    /// script on any failing non-empty signature, so a committed capture
    /// always implies the terminal comparison evaluates at exactly
    /// `expected`. This decides script satisfaction at capture time, before
    /// allocation, unsatisfiable spends decline capture and evaluate inline.
    static constexpr bool is_satisfied(opcode code, size_t expected,
        size_t minimum, size_t maximum) NOEXCEPT
    {
        switch (code)
        {
            // checksig denotes the tapscript multisig (checksigverify
            // chain) pattern, minimum == pair count (all must verify).
            case opcode::checksig:
            case opcode::numequal:
                return expected == minimum;

            // Verify variant pops both operands and pushes nothing, leaving
            // an empty stack, which can never satisfy clean-true [bip342].
            case opcode::numequalverify:
                return false;

            case opcode::numnotequal:
                return expected != minimum;
            case opcode::lessthan:
                return expected < minimum;
            case opcode::greaterthan:
                return expected > minimum;
            case opcode::lessthanorequal:
                return expected <= minimum;
            case opcode::greaterthanorequal:
                return expected >= minimum;

            // op_within is half-open: minimum <= x < maximum.
            case opcode::within:
                return (expected >= minimum) && (expected < maximum);

            // Includes the op_xor (not a threshold script) sentinel.
            default:
                return false;
        }
    }

    using cursor = chain::cursor<bool(const hash_digest&, const ec_xonly&,
        const ec_signature&)>;

    /// Open implies batching latched for this script (pending rows).
    cursor sink{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
