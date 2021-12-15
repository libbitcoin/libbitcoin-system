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
    block();

    block(block&& other);
    block(const block& other);

    block(chain::header&& header, transactions&& txs);
    block(const chain::header& header, const transactions& txs);
    block(const chain::header::ptr& header, const transactions_ptr& txs);

    block(const data_slice& data, bool witness);
    block(std::istream& stream, bool witness);
    block(reader& source, bool witness);

    // Operators.
    // ------------------------------------------------------------------------

    block& operator=(block&& other);
    block& operator=(const block& other);

    bool operator==(const block& other) const;
    bool operator!=(const block& other) const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool witness) const;
    void to_data(std::ostream& stream, bool witness) const;
    void to_data(writer& sink, bool witness) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const;
    const chain::header& header() const;
    const chain::header::ptr header_ptr() const;
    const transactions_ptr transactions() const;
    hash_list transaction_hashes(bool witness) const;

    /// Computed properties.
    size_t weight() const;
    uint64_t fees() const;
    uint64_t claim() const;
    hash_digest hash() const;
    bool is_segregated() const;
    size_t serialized_size(bool witness) const;

    // Validation.
    // ------------------------------------------------------------------------

    /// Consensus checks (no DoS guards for block sync without headers first).
    code check() const;
    code accept(const context& state, size_t subsidy_interval,
        uint64_t initial_subsidy) const;
    code connect(const context& state) const;

protected:
    block(const chain::header::ptr& header, const transactions_ptr& txs,
        bool valid);

    // Check (context free).
    // ------------------------------------------------------------------------

    bool is_empty() const;
    bool is_oversized() const;
    bool is_first_non_coinbase() const;
    bool is_extra_coinbases() const;
    bool is_forward_reference() const;
    bool is_internal_double_spend() const;
    bool is_invalid_merkle_root() const;

    // TX: error::empty_transaction
    // TX: error::previous_output_null
    // TX: error::invalid_coinbase_script_size

    // Accept (contextual).
    // ------------------------------------------------------------------------

    bool is_overweight() const;
    bool is_invalid_coinbase_script(size_t height) const;
    bool is_hash_limit_exceeded() const;
    bool is_invalid_witness_commitment() const;

    // prevouts required
    bool is_overspent(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const;
    bool is_signature_operations_limited(bool bip16, bool bip141) const;

    // prevout confirmation state required
    bool is_unspent_coinbase_collision(size_t height) const;

    // TX: error::transaction_non_final (context)
    // TX: error::missing_previous_output (prevouts)
    // TX: error::spend_exceeds_value (prevouts)
    // TX: error::coinbase_maturity (prevouts)
    // TX: error::relative_time_locked (prevouts)
    // TX: error::unconfirmed_spend (prevout confirmation state)
    // TX: error::confirmed_double_spend (prevout confirmation state)

private:
    static block from_data(reader& source, bool witness);

    // context free
    hash_digest generate_merkle_root(bool witness) const;

    // contextual
    size_t non_coinbase_inputs() const;
    uint64_t reward(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const;

    // delegated
    code check_transactions() const;
    code connect_transactions(const context& state) const;
    code accept_transactions(const context& state) const;

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
