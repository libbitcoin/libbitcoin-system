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
#ifndef LIBBITCOIN__CHAIN_TRANSACTION_HPP
#define LIBBITCOIN__CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/chain_chain_state.hpp>
#include <bitcoin/chain_input.hpp>
#include <bitcoin/chain_input_list.hpp>
#include <bitcoin/chain_output.hpp>
#include <bitcoin/chain_output_list.hpp>
#include <bitcoin/chain_point.hpp>
#include <bitcoin/chain_point_list.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/error_code.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/math_hash_digest.hpp>
#include <bitcoin/math_hash_list.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/rule_fork.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace api {

using namespace libbitcoin::chain;

class BC_API chain_transaction : public transaction
{
public:
//    typedef std::vector<transaction> list;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
//    struct validation
//    {
//        /// This is a non-consensus sentinel value.
//        static const uint64_t unlinked;
//
//        uint64_t originator = 0;
//        chain_state::ptr state = nullptr;
//
//        /// Tx currently exists at the link (in any state).
//        uint64_t link = unlinked;
//
//        /// Tx existed before being written (in the current pass).
//        bool existed = false;
//
//        /// Tx is in a candidate chain block (and valid there).
//        bool candidate = false;
//
//        /// Tx is in a confirmed chain block at given height (and valid there).
//        bool confirmed = false;
//
//        /// There is no distiction between a tx that can be valid under some
//        /// forks and one that cannot be valid under any forks. The only
//        /// criteria for storage is deserialization and DoS protection. The
//        /// latter is provided by pool validation or containing block PoW.
//        /// A transaction that is deconfirmed is set to unverified, which is
//        /// simply a storage space optimization. This results in revalidation
//        /// in the case where the transaction may be confirmed again.
//        /// If verified the tx has been validated relative to given forks.
//        bool verified = false;
//    };

    // Constructors.
    //-------------------------------------------------------------------------

    chain_transaction();

//    transaction(transaction&& other);
    chain_transaction(const transaction& other);

//    transaction(uint32_t version, uint32_t locktime, input::list&& inputs,
//        output::list&& outputs);
    chain_transaction(uint32_t version, uint32_t locktime, const libbitcoin::api::chain_input_list& inputs,
        const chain_output_list& outputs);

    // Operators.
    //-------------------------------------------------------------------------

//    transaction& operator=(transaction&& other);
//    transaction& operator=(const transaction& other);
    transaction& assign(const transaction& other);

    bool eq(const transaction& other) const;
//    bool operator==(const transaction& other) const;
//    bool operator!=(const transaction& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static transaction factory(const libbitcoin::api::error_code& data, bool wire=true, bool witness=false);
//    static transaction factory(std::istream& stream, bool wire=true, bool witness=false);
//    static transaction factory(reader& source, bool wire=true, bool witness=false);

    // Non-wire store deserializations to preserve hash.
//    static transaction factory(reader& source, libbitcoin::api::math_hash_digest&& hash,bool wire=true, bool witness=false);
//    static transaction factory(reader& source, const libbitcoin::api::math_hash_digest& hash,bool wire=true, bool witness=false);

    bool from_data(const libbitcoin::api::error_code& data, bool wire=true, bool witness=false);
//    bool from_data(std::istream& stream, bool wire=true, bool witness=false);
//    bool from_data(reader& source, bool wire=true, bool witness=false);

    // Non-wire store deserializations to preserve hash.
//    bool from_data(reader& source, libbitcoin::api::math_hash_digest&& hash, bool wire=true, bool witness=false);
//    bool from_data(reader& source, const libbitcoin::api::math_hash_digest& hash, bool wire=true, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    libbitcoin::api::error_code to_data(bool wire=true, bool witness=false) const;
//    void to_data(std::ostream& stream, bool wire=true, bool witness=false) const;
//    void to_data(writer& sink, bool wire=true, bool witness=false) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    static size_t maximum_size(bool is_coinbase);
    size_t serialized_size(bool wire=true, bool witness=false) const;

    uint32_t version() const;
    void set_version(uint32_t value);

    uint32_t locktime() const;
    void set_locktime(uint32_t value);

    // Deprecated (unsafe).
//    input::list& inputs();

    const chain_input_list& inputs() const;
    void set_inputs(const chain_input_list& value);
//    void set_inputs(input::list&& value);

    // Deprecated (unsafe).
//    output::list& outputs();

    const chain_output_list& outputs() const;
    void set_outputs(const chain_output_list& value);
//    void set_outputs(output::list&& value);

    libbitcoin::api::math_hash_digest outputs_hash() const;
    libbitcoin::api::math_hash_digest inpoints_hash() const;
    libbitcoin::api::math_hash_digest sequences_hash() const;
    libbitcoin::api::math_hash_digest hash(bool witness=false) const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    uint64_t fees() const;
    chain_point_list previous_outputs() const;
    chain_point_list missing_previous_outputs() const;
    libbitcoin::math::api::math_hash_list missing_previous_transactions() const;
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

    libbitcoin::api::error_code check(uint64_t max_money, bool transaction_pool=true) const;
    libbitcoin::api::error_code accept(bool transaction_pool=true) const;
    libbitcoin::api::error_code accept(const chain_chain_state& state, bool transaction_pool=true) const;
    libbitcoin::api::error_code connect() const;
    libbitcoin::api::error_code connect(const chain_chain_state& state) const;
    libbitcoin::api::error_code connect_input(const chain_chain_state& state, size_t input_index) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation metadata;

protected:
    void reset();
    void invalidate_cache() const;
    bool all_inputs_final() const;

//private:
//    typedef std::shared_ptr<libbitcoin::api::math_hash_digest> hash_ptr;
//    typedef boost::optional<uint64_t> optional_value;
//
//    hash_ptr hash_cache() const;
//    optional_value total_input_value_cache() const;
//    optional_value total_output_value_cache() const;
//
//    uint32_t version_;
//    uint32_t locktime_;
//    input::list inputs_;
//    output::list outputs_;
//
//    // These share a mutex as they are not expected to contend.
//    mutable hash_ptr hash_;
//    mutable hash_ptr witness_hash_;
//    mutable hash_ptr outputs_hash_;
//    mutable hash_ptr inpoints_hash_;
//    mutable hash_ptr sequences_hash_;
//    mutable upgrade_mutex hash_mutex_;
//
//    // These share a mutex as they are not expected to contend.
//    mutable optional_value total_input_value_;
//    mutable optional_value total_output_value_;
//    mutable boost::optional<bool> segregated_;
//    mutable upgrade_mutex mutex_;
};

} // api
} // namespace libbitcoin

#endif
