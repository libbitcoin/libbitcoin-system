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
#ifndef LIBBITCOIN_SYSTEM_WALLET_PSBT_OUTPUT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_PSBT_OUTPUT_HPP

#include <optional>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/psbt/entry.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

/// A per-output map, as defined by BIP174/BIP370.
struct BC_API output
{
    typedef std_vector<output> list;

    /// Parse the map, false on malformed field or duplicate key.
    bool from_data(reader& source, uint32_t version) NOEXCEPT;

    /// Serialize the map, including its terminator.
    void to_data(writer& sink, uint32_t version) const NOEXCEPT;

    /// Merge the fields of the other output (BIP174 combiner).
    void combine(const output& other) NOEXCEPT;

    // BIP174.
    chain::script::cptr redeem_script{};
    chain::script::cptr witness_script{};
    derivation::list derivations{};

    // BIP370.
    std::optional<uint64_t> amount{};
    chain::script::cptr script{};

    // Defined non-structural (taproot) and unknown fields.
    entry::list others{};
};

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
