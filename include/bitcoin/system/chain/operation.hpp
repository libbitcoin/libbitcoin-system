/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OPERATION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OPERATION_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API operation
{
public:
    typedef std::vector<operation> list;
    typedef std::shared_ptr<operation> ptr;

    typedef list::const_iterator iterator;

    // Constructors.
    //-------------------------------------------------------------------------

    operation();

    operation(operation&& other);
    operation(const operation& other);

    operation(opcode code);

    /// These construct from push-data, not serialized operations (no codes).
    /// When minimal is true the data is interpreted as minimally-encoded push.
    operation(data_chunk&& push_data, bool minimal);
    operation(const data_chunk& push_data, bool minimal);

    /// These deserialize operations (with codes), not from push-data.
    operation(const data_chunk& op_data);
    operation(std::istream& stream);
    operation(reader& source);

    // Operators.
    //-------------------------------------------------------------------------

    operation& operator=(operation&& other);
    operation& operator=(const operation& other);

    bool operator==(const operation& other) const;
    bool operator!=(const operation& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    /// These serialize operations (with codes), not to push-data.
    bool from_data(const data_chunk& encoded);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);

    bool from_string(const std::string& mnemonic);

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;

    std::string to_string(uint32_t active_forks) const;

    // Properties.
    //-------------------------------------------------------------------------

    size_t serialized_size() const;

    /// Get the op code [0..255].
    opcode code() const;

    /// Get the data, empty if not a push code.
    const data_chunk& data() const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Compute nominal data opcode based on size alone.
    static opcode opcode_from_size(size_t size);

    /// Compute the minimal data opcode for a given chunk of data.
    /// Caller should clear data if converting to non-payload opcode.
    static opcode minimal_opcode_from_data(const data_chunk& data);

    /// Compute the nominal data opcode for a given chunk of data.
    /// Restricted to sized data, avoids conversion to numeric opcodes.
    static opcode nominal_opcode_from_data(const data_chunk& data);

    /// Convert the [0..16] value to the corresponding opcode (or undefined).
    static opcode opcode_from_version(uint8_t value);

    /// Convert the [1..16] value to the corresponding opcode (or undefined).
    static opcode opcode_from_positive(uint8_t value);

    /// Convert the opcode to the corresponding [1..16] value (or undefined).
    static uint8_t opcode_to_positive(opcode code);

    /// Categories of opcodes.
    static bool is_push(opcode code);
    static bool is_payload(opcode code);
    static bool is_counted(opcode code);
    static bool is_version(opcode code);
    static bool is_numeric(opcode code);
    static bool is_positive(opcode code);
    static bool is_invalid(opcode code);
    static bool is_reserved(opcode code);
    static bool is_conditional(opcode code);
    static bool is_relaxed_push(opcode code);

    /// Categories of operations.
    bool is_push() const;
    bool is_payload() const;
    bool is_counted() const;
    bool is_version() const;
    bool is_numeric() const;
    bool is_positive() const;
    bool is_invalid() const;
    bool is_reserved() const;
    bool is_conditional() const;
    bool is_relaxed_push() const;
    bool is_oversized() const;
    bool is_minimal_push() const;
    bool is_nominal_push() const;
    bool is_underflow() const;

protected:
    static uint32_t read_data_size(opcode code, reader& source);

    operation(opcode code, data_chunk&& data, bool underflow);
    operation(opcode code, const data_chunk& data, bool underflow);

    opcode opcode_from_data(const data_chunk& data, bool minimal);
    void reset();

private:
    opcode code_;
    data_chunk data_;
    bool underflow_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif