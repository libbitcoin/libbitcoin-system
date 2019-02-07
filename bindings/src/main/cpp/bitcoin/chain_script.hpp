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
#ifndef LIBBITCOIN__CHAIN_SCRIPT_HPP
#define LIBBITCOIN__CHAIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/chain_transaction.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/chain_transaction.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/math_hash_digest.hpp>
#include <bitcoin/math_ec_signature.hpp>
#include <bitcoin/math_ec_secret.hpp>
#include <bitcoin/math_endorsement.hpp>
#include <bitcoin/math_point_list.hpp>
#include <bitcoin/math_short_hash.hpp>
#include <bitcoin/machine_operation_iterator.hpp>
#include <bitcoin/bitcoin/machine/rule_fork.hpp>
#include <bitcoin/bitcoin/machine/script_pattern.hpp>
#include <bitcoin/bitcoin/machine/script_version.hpp>
#include <bitcoin/utility_data_chunk.hpp>
#include <bitcoin/utility_data_slice.hpp>
#include <bitcoin/utility_data_stack.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>
#include <bitcoin/chain_script_operation_list.hpp>

namespace libbitcoin {
namespace api {

//class chain_transaction;
//class witness;

class BC_API chain_script : public libbitcoin::chain::script
{
public:
//    typedef machine::operation operation;
//    typedef machine::rule_fork rule_fork;
//    typedef machine::script_pattern script_pattern;
//    typedef machine::script_version script_version;

    // Constructors.
    //-------------------------------------------------------------------------

    chain_script();

//    script(script&& other);
    chain_script(const chain_script& other);

//    script(chain_script_operation_list&& ops);
    chain_script(const chain_script_operation_list& ops);

//    script(data_chunk&& encoded, bool prefix);
    chain_script(const utility_data_chunk& encoded, bool prefix);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
//    script& operator=(script&& other);
    chain_script& assign(const chain_script& other);
//    script& operator=(const script& other);

//    bool operator==(const script& other) const;
    bool eq(const chain_script& other) const;
//    bool operator!=(const script& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static chain_script factory(const utility_data_chunk& encoded, bool prefix);
//    static script factory(std::istream& stream, bool prefix);
//    static script factory(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    bool from_data(const utility_data_chunk& encoded, bool prefix);
//    bool from_data(std::istream& stream, bool prefix);
//    bool from_data(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
//    void from_operations(chain_script_operation_list&& ops);
    void from_operations(const chain_script_operation_list& ops);
    bool from_string(const std::string& mnemonic);

    /// A script object is valid if the byte count matches the prefix.
    bool is_valid() const;

    /// Script operations is valid if all push ops have the predicated size.
    bool is_valid_operations() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool prefix) const;
//    void to_data(std::ostream& stream, bool prefix) const;
//    void to_data(writer& sink, bool prefix) const;

    std::string to_string(uint32_t active_forks) const;

    // Iteration.
    //-------------------------------------------------------------------------

    void clear();
    bool empty() const;
    size_t size() const;
    const operation& front() const;
    const operation& back() const;
    machine_operation_iterator begin() const;
    machine_operation_iterator end() const;
    const operation& getElement(size_t index) const;
//    const operation& operator[](size_t index) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool prefix) const;
    const chain_script_operation_list& operations() const;

    // Signing.
    //-------------------------------------------------------------------------

	static math_hash_digest generate_signature_hash(
			const chain_transaction& tx, uint32_t input_index,
			const chain_script& script_code, uint8_t sighash_type,
			script_version version = script_version::unversioned,
			uint64_t value = max_uint64);

	static bool check_signature(const math_ec_signature& signature,
			uint8_t sighash_type, const utility_data_chunk& public_key,
			const chain_script& script_code, const chain_transaction& tx,
			uint32_t input_index, script_version version =
					script_version::unversioned, uint64_t value = max_uint64);

    static bool create_endorsement(math_endorsement& out, const math_ec_secret& secret,
        const script& prevout_script, const chain_transaction& tx,
        uint32_t input_index, uint8_t sighash_type,
        script_version version=script_version::unversioned,
        uint64_t value=max_uint64);

    // Utilities (static).
    //-------------------------------------------------------------------------

    /// libbitcoin::api::chain_transaction helpers.
    static math_hash_digest to_outputs(const chain_transaction& tx);
    static math_hash_digest to_inpoints(const chain_transaction& tx);
    static math_hash_digest to_sequences(const chain_transaction& tx);

    /// Determine if the fork is enabled in the active forks set.
    static bool is_enabled(uint32_t active_forks, rule_fork fork)
    {
        return (fork & active_forks) != 0;
    }

    /// Consensus patterns.
    static bool is_push_only(const chain_script_operation_list& ops);
    static bool is_relaxed_push(const chain_script_operation_list& ops);
    static bool is_coinbase_pattern(const chain_script_operation_list& ops, size_t height);
    static bool is_commitment_pattern(const chain_script_operation_list& ops);
    static bool is_witness_program_pattern(const chain_script_operation_list& ops);


    /// Common output patterns (psh and pwsh are also consensus).
    static bool is_pay_null_data_pattern(const chain_script_operation_list& ops);
    static bool is_pay_multisig_pattern(const chain_script_operation_list& ops);
    static bool is_pay_public_key_pattern(const chain_script_operation_list& ops);
    static bool is_pay_key_hash_pattern(const chain_script_operation_list& ops);
    static bool is_pay_script_hash_pattern(const chain_script_operation_list& ops);
    static bool is_pay_witness_script_hash_pattern(const chain_script_operation_list& ops);

    /// Common input patterns (skh is also consensus).
    static bool is_sign_multisig_pattern(const chain_script_operation_list& ops);
    static bool is_sign_public_key_pattern(const chain_script_operation_list& ops);
    static bool is_sign_key_hash_pattern(const chain_script_operation_list& ops);
    static bool is_sign_script_hash_pattern(const chain_script_operation_list& ops);

    /// Stack factories.
    static chain_script_operation_list to_pay_null_data_pattern(utility_data_slice data);
    static chain_script_operation_list to_pay_public_key_pattern(utility_data_slice point);
    static chain_script_operation_list to_pay_key_hash_pattern(const math_short_hash& hash);
    static chain_script_operation_list to_pay_script_hash_pattern(const math_short_hash& hash);
    static chain_script_operation_list to_pay_multisig_pattern(uint8_t signatures,
        const math_point_list& points);
    static chain_script_operation_list to_pay_multisig_pattern(uint8_t signatures,
        const utility_data_stack& points);

    // Utilities (non-static).
    //-------------------------------------------------------------------------

    /// Common pattern detection.
    utility_data_chunk witness_program() const;
    script_version version() const;
    script_pattern pattern() const;
    script_pattern input_pattern() const;
    script_pattern output_pattern() const;

    /// Consensus computations.
    size_t sigops(bool accurate) const;
    void find_and_delete(const utility_data_stack& endorsements);
    bool is_unspendable() const;

    // Validation.
    //-------------------------------------------------------------------------

    // This obtains the previous output from metadata.
    static error_code verify(const chain_transaction& tx, uint32_t input_index,
        uint32_t forks);

    static error_code verify(const chain_transaction& tx, uint32_t input_index,
        uint32_t forks, const chain_script& prevout_script, uint64_t value);

//protected:
//    // So that input and output may call reset from their own.
//    friend class input;
//    friend class output;
//
//    void reset();
//    bool is_pay_to_witness(uint32_t forks) const;
//    bool is_pay_to_script_hash(uint32_t forks) const;
//
//private:
//    static size_t serialized_size(const chain_script_operation_list& ops);
//    static data_chunk operations_to_data(const chain_script_operation_list& ops);
//    static hash_digest generate_unversioned_signature_hash(
//        const chain_transaction& tx, uint32_t input_index,
//        const script& script_code, uint8_t sighash_type);
//    static hash_digest generate_version_0_signature_hash(const chain_transaction& tx,
//        uint32_t input_index, const script& script_code, uint64_t value,
//        uint8_t sighash_type);
//
//    void find_and_delete_(const data_chunk& endorsement);
//
//    chain_script_operation_list& operations_move();
//    const chain_script_operation_list& operations_copy() const;
//
//    // These are protected by mutex.
//    mutable chain_script_operation_list operations_;
//    mutable bool cached_;
//    mutable upgrade_mutex mutex_;
//
//    data_chunk bytes_;
//    bool valid_;
};

} // namespace api
} // namespace libbitcoin

#endif
