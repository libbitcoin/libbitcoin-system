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
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/mutex.hpp>
#include <bitcoin/system/optional.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

class settings;

namespace chain {

class BC_API block
{
public:
    typedef std::vector<block> list;
    typedef std::shared_ptr<block> ptr;

    // Constructors.
    //-------------------------------------------------------------------------

    block();

    block(block&& other);
    block(const block& other);

    block(chain::header&& header, transaction::list&& transactions);
    block(const chain::header& header, const transaction::list& transactions);

    block(const data_chunk& data, bool witness=false);
    block(std::istream& stream, bool witness=false);
    block(reader& source, bool witness=false);

    // Operators.
    //-------------------------------------------------------------------------

    block& operator=(block&& other);
    block& operator=(const block& other);

    bool operator==(const block& other) const;
    bool operator!=(const block& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    bool from_data(const data_chunk& data, bool witness=false);
    bool from_data(std::istream& stream, bool witness=false);
    bool from_data(reader& source, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool witness=false) const;
    void to_data(std::ostream& stream, bool witness=false) const;
    void to_data(writer& sink, bool witness=false) const;

    hash_list to_hashes(bool witness=false) const;

    // Properties.
    //-------------------------------------------------------------------------

    size_t serialized_size(bool witness=false) const;

    const chain::header& header() const;
    const transaction::list& transactions() const;

    hash_digest hash() const;

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    static uint64_t subsidy(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42);

    hash_digest generate_merkle_root(bool witness=false) const;
    size_t total_non_coinbase_inputs() const;
    size_t total_inputs() const;
    size_t weight() const;

    bool is_extra_coinbases() const;
    bool is_hash_limit_exceeded() const;
    bool is_final(size_t height, uint32_t block_time) const;
    bool is_distinct_transaction_set() const;
    bool is_valid_coinbase_script(size_t height) const;
    bool is_valid_witness_commitment() const;
    bool is_forward_reference() const;
    bool is_internal_double_spend() const;
    bool is_valid_merkle_root() const;
    bool is_segregated() const;

    code check(uint64_t max_money, uint32_t timestamp_limit_seconds,
        uint32_t proof_of_work_limit, bool scrypt=false,
        bool header=true) const;
    code check_transactions(uint64_t max_money) const;
    code accept(const chain_state& state, const settings& settings,
        bool transactions=true, bool header=true) const;
    code accept_transactions(const chain_state& state) const;
    ////code connect(const chain_state& state) const;
    ////code connect_transactions(const chain_state& state) const;

protected:
    void reset();

    block(chain::header&& header, transaction::list&& transactions, bool valid);
    block(const chain::header& header, const transaction::list& transactions,
        bool valid);

private:
    chain::header header_;
    transaction::list transactions_;
    bool valid_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
