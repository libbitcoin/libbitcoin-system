/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/input.hpp>
#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/rule_fork.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction
{
public:
    typedef input::list ins;
    typedef output::list outs;
    typedef std::vector<transaction> list;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation
    {
        static const uint64_t undetermined_offset;

        bool simulate = false;
        uint64_t originator = 0;
        code error = error::success;
        chain_state::ptr state = nullptr;
        uint64_t offset = undetermined_offset;

        /// The tx exists in the store (don't create it).
        bool exists = false;

        /// An existing tx is valid at current fork state (don't validate).
        bool pooled = false;

        /// The block tx is confirmed in ancestry or pool tx pooled (reject).
        bool duplicate = false;
    };

    // Constructors.
    //-------------------------------------------------------------------------

    transaction();

    transaction(transaction&& other);
    transaction(const transaction& other);

    transaction(uint32_t version, uint32_t locktime, ins&& inputs,
        outs&& outputs);
    transaction(uint32_t version, uint32_t locktime, const ins& inputs,
        const outs& outputs);

    // Operators.
    //-------------------------------------------------------------------------

    transaction& operator=(transaction&& other);
    transaction& operator=(const transaction& other);

    bool operator==(const transaction& other) const;
    bool operator!=(const transaction& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static transaction factory(const data_chunk& data, bool wire=true, bool witness=false);
    static transaction factory(std::istream& stream, bool wire=true, bool witness=false);
    static transaction factory(reader& source, bool wire=true, bool witness=false);

    // Non-wire store deserializations to preserve hash.
    static transaction factory(reader& source, hash_digest&& hash, bool wire=true, bool witness=false);
    static transaction factory(reader& source, const hash_digest& hash, bool wire=true, bool witness=false);

    bool from_data(const data_chunk& data, bool wire=true, bool witness=false);
    bool from_data(std::istream& stream, bool wire=true, bool witness=false);
    bool from_data(reader& source, bool wire=true, bool witness=false);

    // Non-wire store deserializations to preserve hash.
    bool from_data(reader& source, hash_digest&& hash, bool wire=true, bool witness=false);
    bool from_data(reader& source, const hash_digest& hash, bool wire=true, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool wire=true, bool witness=false) const;
    void to_data(std::ostream& stream, bool wire=true, bool witness=false) const;
    void to_data(writer& sink, bool wire=true, bool witness=false) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    static size_t maximum_size(bool is_coinbase);
    size_t serialized_size(bool wire=true, bool witness=false) const;

    uint32_t version() const;
    void set_version(uint32_t value);

    uint32_t locktime() const;
    void set_locktime(uint32_t value);

    // Deprecated (unsafe).
    ins& inputs();

    const ins& inputs() const;
    void set_inputs(const ins& value);
    void set_inputs(ins&& value);

    // Deprecated (unsafe).
    outs& outputs();

    const outs& outputs() const;
    void set_outputs(const outs& value);
    void set_outputs(outs&& value);

    hash_digest outputs_hash() const;
    hash_digest inpoints_hash() const;
    hash_digest sequences_hash() const;
    hash_digest hash(bool witness=false) const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    uint64_t fees() const;
    point::list previous_outputs() const;
    point::list missing_previous_outputs() const;
    hash_list missing_previous_transactions() const;
    uint64_t total_input_value() const;
    uint64_t total_output_value() const;
    size_t signature_operations() const;
    size_t signature_operations(bool bip16, bool bip141) const;
    size_t weight() const;

    bool is_coinbase() const;
    bool is_null_non_coinbase() const;
    bool is_oversized_coinbase() const;
    bool is_mature(size_t height) const;
    bool is_overspent() const;
    bool is_internal_double_spend() const;
    bool is_confirmed_double_spend() const;
    bool is_dusty(uint64_t minimum_output_value) const;
    bool is_missing_previous_outputs() const;
    bool is_final(size_t block_height, uint32_t block_time) const;
    bool is_locked(size_t block_height, uint32_t median_time_past) const;
    bool is_locktime_conflict() const;
    bool is_segregated() const;

    code check(bool transaction_pool=true, bool retarget=true) const;
    code accept(bool transaction_pool=true) const;
    code accept(const chain_state& state, bool transaction_pool=true) const;
    code connect() const;
    code connect(const chain_state& state) const;
    code connect_input(const chain_state& state, size_t input_index) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation validation;

protected:
    void reset();
    void invalidate_cache() const;
    bool all_inputs_final() const;

private:
    typedef std::shared_ptr<hash_digest> hash_ptr;
    typedef boost::optional<uint64_t> optional_value;

    hash_ptr hash_cache() const;
    optional_value total_input_value_cache() const;
    optional_value total_output_value_cache() const;

    uint32_t version_;
    uint32_t locktime_;
    input::list inputs_;
    output::list outputs_;

    // These share a mutex as they are not expected to contend.
    mutable hash_ptr hash_;
    mutable hash_ptr witness_hash_;
    mutable hash_ptr outputs_hash_;
    mutable hash_ptr inpoints_hash_;
    mutable hash_ptr sequences_hash_;
    mutable upgrade_mutex hash_mutex_;

    // These share a mutex as they are not expected to contend.
    mutable optional_value total_input_value_;
    mutable optional_value total_output_value_;
    mutable boost::optional<bool> segregated_;
    mutable upgrade_mutex mutex_;
};

} // namespace chain
} // namespace libbitcoin

#endif
