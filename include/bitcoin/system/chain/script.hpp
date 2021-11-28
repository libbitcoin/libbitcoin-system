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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/system/chain/enums/rule_fork.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/sighash_algorithm.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/mutex.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class transaction;

class BC_API script
{
public:
    typedef std::vector<script> list;
    typedef std::shared_ptr<script> ptr;

    // Constructors.
    //-------------------------------------------------------------------------

    script();

    script(script&& other);
    script(const script& other);

    script(operation::list&& ops);
    script(const operation::list& ops);

    script(const data_chunk& encoded, bool prefix);
    script(std::istream& stream, bool prefix);
    script(reader& source, bool prefix);

    // Operators.
    //-------------------------------------------------------------------------

    script& operator=(script&& other);
    script& operator=(const script& other);

    bool operator==(const script& other) const;
    bool operator!=(const script& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    /// Deserialization invalidates the iterator.
    bool from_data(const data_chunk& encoded, bool prefix);
    bool from_data(std::istream& stream, bool prefix);
    bool from_data(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    bool from_string(const std::string& mnemonic);

    /// A script object is valid if the byte count matches the prefix.
    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    std::string to_string(uint32_t active_forks) const;

    // Iteration.
    //-------------------------------------------------------------------------

    void clear();
    bool empty() const;
    size_t size() const;
    const operation& front() const;
    const operation& back() const;
    operation::iterator begin() const;
    operation::iterator end() const;
    const operation& operator[](size_t index) const;

    // Properties.
    //-------------------------------------------------------------------------

    size_t serialized_size(bool prefix) const;
    const operation::list& operations() const;
    hash_digest to_payments_key() const;

    // Signing.
    //-------------------------------------------------------------------------

    static hash_digest generate_signature_hash(const transaction& tx,
        uint32_t index, const script& subscript, uint64_t value, uint8_t flags,
        script_version version, bool bip143);

    static bool check_signature(const ec_signature& signature,
        const data_slice& public_key, const script& subscript,
        const transaction& tx, uint32_t index, uint64_t value, uint8_t flags,
        script_version version, bool bip143);

    static bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& prevout_script, const transaction& tx,
        uint32_t index, uint64_t value, uint8_t flags, script_version version,
        bool bip143);

    // Utilities (static).
    //-------------------------------------------------------------------------

    /// Determine if the fork is enabled in the active forks set.
    static bool is_enabled(uint32_t active_forks, rule_fork fork);

    /// Consensus patterns.
    static bool is_push_only(const operation::list& ops);
    static bool is_relaxed_push(const operation::list& ops);
    static bool is_coinbase_pattern(const operation::list& ops, size_t height);
    static bool is_commitment_pattern(const operation::list& ops);
    static bool is_witness_program_pattern(const operation::list& ops);

    /// Common output patterns (psh and pwsh are also consensus).
    static bool is_pay_null_data_pattern(const operation::list& ops);
    static bool is_pay_multisig_pattern(const operation::list& ops);
    static bool is_pay_public_key_pattern(const operation::list& ops);
    static bool is_pay_key_hash_pattern(const operation::list& ops);
    static bool is_pay_script_hash_pattern(const operation::list& ops);
    static bool is_pay_witness_pattern(const operation::list& ops);
    static bool is_pay_witness_key_hash_pattern(const operation::list& ops);
    static bool is_pay_witness_script_hash_pattern(const operation::list& ops);

    /// Common input patterns (skh is also consensus).
    static bool is_sign_multisig_pattern(const operation::list& ops);
    static bool is_sign_public_key_pattern(const operation::list& ops);
    static bool is_sign_key_hash_pattern(const operation::list& ops);
    static bool is_sign_script_hash_pattern(const operation::list& ops);

    /// Stack factories.
    static operation::list to_pay_null_data_pattern(const data_slice& data);
    static operation::list to_pay_public_key_pattern(const data_slice& point);
    static operation::list to_pay_key_hash_pattern(const short_hash& hash);
    static operation::list to_pay_script_hash_pattern(const short_hash& hash);
    static operation::list to_pay_multisig_pattern(uint8_t signatures,
        const compressed_list& points);
    static operation::list to_pay_multisig_pattern(uint8_t signatures,
        const data_stack& points);
    static  operation::list to_pay_witness_pattern(uint8_t version,
        const data_slice& data);
    static operation::list to_pay_witness_key_hash_pattern(
        const short_hash& hash);
    static operation::list to_pay_witness_script_hash_pattern(
        const hash_digest& hash);

    // Utilities (non-static).
    //-------------------------------------------------------------------------

    /// Common pattern detection.
    const data_chunk& witness_program() const;
    script_version version() const;
    script_pattern pattern() const;
    script_pattern input_pattern() const;
    script_pattern output_pattern() const;

    /// Consensus computations.
    size_t sigops(bool accurate) const;
    bool is_oversized() const;
    bool is_unspendable() const;

    // Validation.
    //-------------------------------------------------------------------------

    /// This overload obtains the previous output from metadata.
    ////static code verify(const transaction& tx, uint32_t index, uint32_t forks);
    static code verify(const transaction& tx, uint32_t index, uint32_t forks,
        const script& prevout_script, uint64_t value);

protected:
    // So inputs and output may reset the member.
    friend class input;
    friend class output;

    script(operation::list&& ops, bool valid);
    script(const operation::list& ops, bool valid);

    void reset();
    bool is_pay_to_witness(uint32_t forks) const;
    bool is_pay_to_script_hash(uint32_t forks) const;

private:
    static hash_digest generate_unversioned_signature_hash(const transaction& tx,
        uint32_t index, const script& subscript, uint8_t flags);
    static hash_digest generate_version_0_signature_hash(const transaction& tx,
        uint32_t index, const script& subscript, uint64_t value, uint8_t flags,
        bool bip143);

    mutable operation::list ops_;
    bool valid_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
