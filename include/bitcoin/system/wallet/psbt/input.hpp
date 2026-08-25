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
#ifndef LIBBITCOIN_SYSTEM_WALLET_PSBT_INPUT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_PSBT_INPUT_HPP

#include <optional>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/wallet/psbt/entry.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

/// A per-input map, as defined by BIP174/BIP370.
struct BC_API input
{
    typedef std_vector<input> list;

    /// Parse the map, false on malformed field or duplicate key.
    bool from_data(reader& source, uint32_t version) NOEXCEPT;

    /// Serialize the map, including its terminator.
    void to_data(writer& sink, uint32_t version) const NOEXCEPT;

    /// The input carries final signature script and/or witness.
    bool is_final() const NOEXCEPT;

    /// The spent output from either utxo form (null if unavailable).
    chain::output::cptr prevout(uint32_t index) const NOEXCEPT;

    /// Merge the fields of the other input (BIP174 combiner).
    void combine(const input& other) NOEXCEPT;

    // BIP174.
    chain::transaction::cptr non_witness_utxo{};
    chain::output::cptr witness_utxo{};
    entry::list partial_signatures{};
    std::optional<uint32_t> sighash_type{};
    chain::script::cptr embedded_script{};
    chain::script::cptr witness_script{};
    derivation::list derivations{};
    chain::script::cptr final_script_sig{};
    chain::witness::cptr final_script_witness{};

    // BIP370.
    std::optional<hash_digest> previous_txid{};
    std::optional<uint32_t> output_index{};
    std::optional<uint32_t> sequence{};
    std::optional<uint32_t> required_time_locktime{};
    std::optional<uint32_t> required_height_locktime{};

    // Defined non-structural (proofs, taproot) and unknown fields.
    entry::list others{};
};

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
