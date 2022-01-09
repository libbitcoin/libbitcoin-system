/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BLOCK_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BLOCK_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

class settings;

namespace chain {

class BC_API block
{
public:
    typedef std::shared_ptr<const block> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default block is an invalid object.
    block() noexcept;

    block(block&& other) noexcept;
    block(const block& other) noexcept;

    block(chain::header&& header, transactions&& txs) noexcept;
    block(const chain::header& header, const transactions& txs) noexcept;
    block(const chain::header::ptr& header,
        const transactions_ptr& txs) noexcept;

    block(const data_slice& data, bool witness) noexcept;
    block(std::istream&& stream, bool witness) noexcept;
    block(std::istream& stream, bool witness) noexcept;
    block(reader&& source, bool witness) noexcept;
    block(reader& source, bool witness) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    block& operator=(block&& other) noexcept;
    block& operator=(const block& other) noexcept;

    bool operator==(const block& other) const noexcept;
    bool operator!=(const block& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool witness) const noexcept;
    void to_data(std::ostream& stream, bool witness) const noexcept;
    void to_data(writer& sink, bool witness) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    const chain::header& header() const noexcept;
    const chain::header::ptr header_ptr() const noexcept;
    const transactions_ptr transactions() const noexcept;
    hash_list transaction_hashes(bool witness) const noexcept;

    /// Computed properties.
    size_t weight() const noexcept;
    uint64_t fees() const noexcept;
    uint64_t claim() const noexcept;
    hash_digest hash() const noexcept;
    bool is_segregated() const noexcept;
    size_t serialized_size(bool witness) const noexcept;

    // Validation.
    // ------------------------------------------------------------------------

    /// Consensus checks (no DoS guards for block sync without headers first).
    code check() const noexcept;
    code accept(const context& state, size_t subsidy_interval,
        uint64_t initial_subsidy) const noexcept;
    code connect(const context& state) const noexcept;

protected:
    block(const chain::header::ptr& header, const transactions_ptr& txs,
        bool valid) noexcept;

    // Check (context free).
    // ------------------------------------------------------------------------

    bool is_empty() const noexcept;
    bool is_oversized() const noexcept;
    bool is_first_non_coinbase() const noexcept;
    bool is_extra_coinbases() const noexcept;
    bool is_forward_reference() const noexcept;
    bool is_internal_double_spend() const noexcept;
    bool is_invalid_merkle_root() const noexcept;

    // TX: error::empty_transaction
    // TX: error::previous_output_null
    // TX: error::invalid_coinbase_script_size

    // Accept (contextual).
    // ------------------------------------------------------------------------

    bool is_overweight() const noexcept;
    bool is_invalid_coinbase_script(size_t height) const noexcept;
    bool is_hash_limit_exceeded() const noexcept;
    bool is_invalid_witness_commitment() const noexcept;

    // prevouts required
    bool is_overspent(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const noexcept;
    bool is_signature_operations_limited(bool bip16,
        bool bip141) const noexcept;

    // prevout confirmation state required
    bool is_unspent_coinbase_collision(size_t height) const noexcept;

    // TX: error::transaction_non_final (context)
    // TX: error::missing_previous_output (prevouts)
    // TX: error::spend_exceeds_value (prevouts)
    // TX: error::coinbase_maturity (prevouts)
    // TX: error::relative_time_locked (prevouts)
    // TX: error::unconfirmed_spend (prevout confirmation state)
    // TX: error::confirmed_double_spend (prevout confirmation state)

private:
    static block from_data(reader& source, bool witness) noexcept;

    // context free
    hash_digest generate_merkle_root(bool witness) const noexcept;

    // contextual
    size_t non_coinbase_inputs() const noexcept;
    uint64_t reward(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const noexcept;

    // delegated
    code check_transactions() const noexcept;
    code accept_transactions(const context& state) const noexcept;
    code connect_transactions(const context& state) const noexcept;

    // Block should be stored as shared (adds 16 bytes).
    // copy: 4 * 64 + 1 = 33 bytes (vs. 16 when shared).
    chain::header::ptr header_;
    transactions_ptr txs_;
    bool valid_;
};

typedef std::vector<block> blocks;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
