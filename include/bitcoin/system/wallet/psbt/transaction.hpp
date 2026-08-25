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
#ifndef LIBBITCOIN_SYSTEM_WALLET_PSBT_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_WALLET_PSBT_TRANSACTION_HPP

#include <optional>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/psbt/entry.hpp>
#include <bitcoin/system/wallet/psbt/input.hpp>
#include <bitcoin/system/wallet/psbt/output.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

/// A partially signed bitcoin transaction, as defined by BIP174 (version 0)
/// and BIP370 (version 2). Finalization covers legacy and version 0 witness
/// programs; taproot fields are parsed and preserved but not satisfied.
class BC_API transaction
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(transaction);

    static const uint32_t version_0;
    static const uint32_t version_2;

    /// Constructors.
    transaction() NOEXCEPT;
    transaction(const data_chunk& decoded) NOEXCEPT;
    transaction(const std::string& encoded) NOEXCEPT;
    transaction(const chain::transaction& unsigned_tx) NOEXCEPT;
    transaction(uint32_t tx_version) NOEXCEPT;

    /// Operators.
    bool operator==(const transaction& other) const NOEXCEPT;
    bool operator!=(const transaction& other) const NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, transaction& to);
    friend std::ostream& operator<<(std::ostream& out,
        const transaction& of) NOEXCEPT;

    /// Cast operators.
    operator bool() const NOEXCEPT;

    /// Serializers.
    std::string encoded() const NOEXCEPT;
    data_chunk to_chunk() const NOEXCEPT;

    /// Accessors.
    uint32_t version() const NOEXCEPT;
    uint32_t tx_version() const NOEXCEPT;
    const xpub::list& xpubs() const NOEXCEPT;
    const input::list& inputs() const NOEXCEPT;
    const output::list& outputs() const NOEXCEPT;
    const entry::list& others() const NOEXCEPT;
    input::list& inputs() NOEXCEPT;
    output::list& outputs() NOEXCEPT;
    const chain::transaction& unsigned_tx() const NOEXCEPT;
    const std::optional<uint8_t>& tx_modifiable() const NOEXCEPT;
    const std::optional<uint32_t>& fallback_locktime() const NOEXCEPT;

    /// All inputs are finalized.
    bool is_final() const NOEXCEPT;

    /// The spent output of the input (null if unavailable).
    chain::output::cptr prevout(size_t input) const NOEXCEPT;

    /// The fee implied by input prevouts and outputs (null if unavailable).
    std::optional<uint64_t> fee() const NOEXCEPT;

    /// The computed lock time (BIP370), or the unsigned tx lock time.
    /// Unsatisfiable input lock time requirements return nullopt.
    std::optional<uint32_t> locktime() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// Merge the fields of another view of the same transaction.
    bool combine(const transaction& other) NOEXCEPT;

    /// Merge the inputs and outputs of a distinct transaction.
    bool join(const transaction& other) NOEXCEPT;

    /// Convert complete signature sets into final scripts, false if any
    /// input cannot be finalized (finalized inputs are retained).
    bool finalize() NOEXCEPT;

    /// The network transaction (invalid unless is_final()).
    chain::transaction extract() const NOEXCEPT;

private:
    /// Factories.
    static transaction from_data(const data_chunk& decoded) NOEXCEPT;
    static transaction from_string(const std::string& encoded) NOEXCEPT;
    static transaction from_transaction(const chain::transaction& tx) NOEXCEPT;

    bool parse(reader& source) NOEXCEPT;
    bool parse_global(reader& source) NOEXCEPT;
    bool finalize(input& in, uint32_t index) NOEXCEPT;
    chain::input::cptr extract_input(size_t index) const NOEXCEPT;

    /// Members.
    bool valid_{};
    uint32_t version_{};
    uint32_t tx_version_{};
    chain::transaction tx_{};
    xpub::list xpubs_{};
    input::list inputs_{};
    output::list outputs_{};
    entry::list others_{};
    std::optional<uint8_t> tx_modifiable_{};
    std::optional<uint32_t> fallback_locktime_{};
};

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
