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
#ifndef LIBBITCOIN__CHAIN_WITNESS_HPP
#define LIBBITCOIN__CHAIN_WITNESS_HPP

#include <cstddef>
#include <istream>
#include <string>
#include <bitcoin/chain_script.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/witness.hpp>
#include <bitcoin/machine_operation_iterator.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/utility_data_stack.hpp>
#include <bitcoin/utility_data_chunk.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_witness
{
public:
//    typedef machine::operation operation;
//    typedef utility_data_stack::const_iterator iterator;

    // Constructors.
    //-------------------------------------------------------------------------

    chain_witness();

//    chain_witness(witness&& other);
    chain_witness(const chain_witness& other);

//    chain_witness(utility_data_stack&& stack);
    chain_witness(const utility_data_stack& stack);

//    chain_witness(utility_data_chunk&& encoded, bool prefix);
    chain_witness(const utility_data_chunk& encoded, bool prefix);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
//    chain_witness& operator=(chain_witness&& other);
//    chain_witness& operator=(const chain_witness& other);
    chain_witness& assign(const chain_witness& other);

//    bool operator==(const chain_witness& other) const;
    bool eq(const chain_witness& other) const;
//    bool operator!=(const chain_witness& other) const;

    // Deserialization (from chain_witness stack).
    //-------------------------------------------------------------------------
    // Prefixed data assumed valid here though caller may confirm with is_valid.

    static chain_witness factory(const utility_data_chunk& encoded, bool prefix);
//    static chain_witness factory(std::istream& stream, bool prefix);
//    static chain_witness factory(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    bool from_data(const utility_data_chunk& encoded, bool prefix);
//    bool from_data(std::istream& stream, bool prefix);
//    bool from_data(reader& source, bool prefix);

    /// The chain_witness deserialized ccording to count and size prefixing.
    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool prefix) const;
//    void to_data(std::ostream& stream, bool prefix) const;
//    void to_data(writer& sink, bool prefix) const;

    std::string to_string() const;

    // Iteration.
    //-------------------------------------------------------------------------

    void clear();
    bool empty() const;
    size_t size() const;
    const utility_data_chunk& front() const;
    const utility_data_chunk& back() const;
    machine_operation_iterator begin() const;
    machine_operation_iterator end() const;
//    const utility_data_chunk& operator[](size_t index) const;
    const utility_data_chunk& get(size_t index) const;
    void set(size_t index, utility_data_chunk&) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool prefix) const;
    const utility_data_stack& stack() const;

    // Utilities.
    //-------------------------------------------------------------------------

    static bool is_push_size(const utility_data_stack& stack);
    static bool is_reserved_pattern(const utility_data_stack& stack);

    bool extract_sigop_script(chain_script& out_script,
        const chain_script& program_script) const;
    bool extract_embedded_script(chain_script& out_script, utility_data_stack& out_stack,
        const chain_script& program_script) const;

    // Validation.
    //-------------------------------------------------------------------------

    error_code verify(const chain_transaction& tx, uint32_t input_index, uint32_t forks,
        const chain_script& program_script, uint64_t value) const;

public:
	chain::witness getValue() {
		return value;
	}

	void setValue(chain::witness value) {
		this->value = value;
	}
private:
	chain::witness value;

//protected:
//    // So that input may call reset from its own.
//    friend class input;
//
//    void reset();

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif
