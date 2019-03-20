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
#ifndef LIBBITCOIN__MACHINE_OPERATION_HPP
#define LIBBITCOIN__MACHINE_OPERATION_HPP

//#include <cstddef>
//#include <cstdint>
//#include <iostream>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <machine_opcode.hpp>
#include <machine_script_pattern.hpp>
#include <utility_data_chunk.hpp>
#include <utility_reader.hpp>
#include <utility_writer.hpp>

namespace libbitcoin {
//namespace machine {
namespace api {

class BC_API machine_operation
{
public:
//    typedef std::vector<operation> list;
//    typedef list::const_iterator iterator;

    // Constructors.
    //-------------------------------------------------------------------------

    machine_operation();

    machine_operation(machine_operation&& other);
//    machine_operation(const machine_operation& other);

    machine_operation(utility_data_chunk&& uncoded, bool minimal=true);
//    machine_operation(const utility_data_chunk& uncoded, bool minimal=true);

    machine_operation(machine_opcode code);

    // Operators.
    //-------------------------------------------------------------------------

//    machine_operation& operator=(machine_operation&& other);
    machine_operation& assign(machine_operation&& other);
//    machine_operation& operator=(const machine_operation& other);

//    bool operator==(const machine_operation& other) const;
    bool eq(const machine_operation& other) const;
//    bool operator!=(const machine_operation& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static machine_operation factory(const utility_data_chunk& encoded);
//    static machine_operation factory(std::istream& stream);
//    static machine_operation factory(reader& source);

    bool from_data(const utility_data_chunk& encoded);
//    bool from_data(std::istream& stream);
//    bool from_data(reader& source);

    bool from_string(const std::string& mnemonic);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data() const;
//    void to_data(std::ostream& stream) const;
//    void to_data(writer& sink) const;

    std::string to_string(uint32_t active_forks) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size() const;

    /// Get the op code [0..255], if is_valid is consistent with data.
    machine_opcode code() const;

    /// Get the data, empty if not a push code or if invalid.
    const utility_data_chunk& data() const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Compute nominal data machine_opcode based on size alone.
    static machine_opcode opcode_from_size(size_t size);

    /// Compute the minimal data opcode for a given chunk of data.
    /// Caller should clear data if converting to non-payload opcode.
    static machine_opcode minimal_opcode_from_data(const utility_data_chunk& data);

    /// Compute the nominal data opcode for a given chunk of data.
    /// Restricted to sized data, avoids conversion to numeric opcodes.
    static machine_opcode nominal_opcode_from_data(const utility_data_chunk& data);

    /// Convert the [1..16] value to the corresponding opcode (or undefined).
    static machine_opcode opcode_from_positive(uint8_t value);

    /// Convert the opcode to the corresponding [1..16] value (or undefined).
    static uint8_t opcode_to_positive(machine_opcode code);

    /// Categories of opcodes.
    static bool is_push(machine_opcode code);
    static bool is_payload(machine_opcode code);
    static bool is_counted(machine_opcode code);
    static bool is_version(machine_opcode code);
    static bool is_numeric(machine_opcode code);
    static bool is_positive(machine_opcode code);
    static bool is_reserved(machine_opcode code);
    static bool is_disabled(machine_opcode code);
    static bool is_conditional(machine_opcode code);
    static bool is_relaxed_push(machine_opcode code);

    /// Categories of operations.
    bool is_push() const;
    bool is_counted() const;
    bool is_version() const;
    bool is_positive() const;
    bool is_disabled() const;
    bool is_conditional() const;
    bool is_relaxed_push() const;
    bool is_oversized() const;
    bool is_minimal_push() const;
    bool is_nominal_push() const;

    machine::operation getValue() {
        return value;
    }

    void setValue(machine::operation value) {
        this->value = value;
    }
//protected:
//    machine_operation(machine_opcode code, utility_data_chunk&& data, bool valid);
//    machine_operation(machine_opcode code, const utility_data_chunk& data, bool valid);
//    static uint32_t read_data_size(machine_opcode code, reader& source);
//    machine_opcode opcode_from_data(const utility_data_chunk& data, bool minimal);
//    void reset();

private:
    machine::operation value;
//    opcode code_;
//    data_chunk data_;
//    bool valid_;
};

} // namespace api
//} // namespace machine
} // namespace libbitcoin

//#include <bitcoin/bitcoin/impl/machine/operation.ipp>

#endif
