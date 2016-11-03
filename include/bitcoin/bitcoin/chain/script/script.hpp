/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_CHAIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/chain/script/evaluation_context.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/operation_stack.hpp>
#include <bitcoin/bitcoin/chain/script/rule_fork.hpp>
#include <bitcoin/bitcoin/chain/script/script_pattern.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class transaction;

class BC_API script
{
public:
    // Constructors.
    //-------------------------------------------------------------------------

    script();

    script(script&& other);
    script(const script& other);

    script(operation_stack&& ops);
    script(const operation_stack& ops);

    script(data_chunk&& bytes, bool prefix);
    script(const data_chunk& bytes, bool prefix);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
    script& operator=(script&& other);
    script& operator=(const script& other);

    bool operator==(const script& other) const;
    bool operator!=(const script& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static script factory_from_data(const data_chunk& data, bool prefix);
    static script factory_from_data(std::istream& stream, bool prefix);
    static script factory_from_data(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    bool from_data(const data_chunk& data, bool prefix);
    bool from_data(std::istream& stream, bool prefix);
    bool from_data(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    void from_stack(const operation_stack& ops);
    bool from_string(const std::string& mnemonic);

    /// A script object is valid if the byte count matches the prefix.
    bool is_valid() const;

    /// A script stack is valid if all push ops have the predicated size.
    bool is_valid_stack() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    std::string to_string(uint32_t active_forks) const;

    // Iteration.
    //-------------------------------------------------------------------------

    bool empty() const;
    size_t size() const;
    const operation& front() const;
    const operation& back() const;
    operation::const_iterator begin() const;
    operation::const_iterator end() const;
    const operation& operator[](std::size_t index) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    uint64_t satoshi_content_size() const;
    uint64_t serialized_size(bool prefix) const;

    // Signing.
    //-------------------------------------------------------------------------

    static hash_digest generate_signature_hash(const transaction& tx,
        uint32_t input_index, const script& script_code, uint8_t sighash_type);

    static bool check_signature(const ec_signature& signature,
        uint8_t sighash_type, const data_chunk& public_key,
        const script& script_code, const transaction& tx,
        uint32_t input_index);

    static bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& prevout_script, const transaction& tx,
        uint32_t input_index, uint8_t sighash_type);

    // Utilities.
    //-------------------------------------------------------------------------

    static bool is_enabled(uint32_t active_forks, rule_fork flag);

    script_pattern pattern() const;
    size_t sigops(bool embedded) const;
    size_t pay_script_hash_sigops(const script& prevout_script) const;

    // Remove values from the stack using satoshi's awful find_and_delete.
    void purge(const data_stack& endorsements);

    // Validation.
    //-------------------------------------------------------------------------

    static code verify(const transaction& tx, uint32_t input_index,
        uint32_t flags);

    static code verify(const transaction& tx, uint32_t input_index,
        const script& prevout_script, uint32_t flags);

protected:
    // So that input and output may call reset from their own.
    friend class input;
    friend class output;

    void reset();
    bool is_relaxed_push_data_only() const;
    bool is_relaxed_push_data(opcode code) const;
    bool is_pay_to_script_hash(uint32_t flags) const;
    const operation_stack& stack() const;
    void find_and_delete(const data_chunk& endorsement);

private:
    static size_t serialized_size(const operation_stack& ops);
    static data_chunk stack_to_data(const operation_stack& ops);
    static code pay_hash(const transaction& tx, uint32_t input_index,
        const script& input_script, evaluation_context& input_context);

    data_chunk bytes_;
    bool valid_;

    // These are protected by mutex.
    mutable bool cached_;
    mutable operation_stack stack_;
    mutable upgrade_mutex mutex_;
};

} // namespace chain
} // namespace libbitcoin

#endif
