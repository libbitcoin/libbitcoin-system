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
#ifndef LIBBITCOIN_MACHINE_OPERATION_HPP
#define LIBBITCOIN_MACHINE_OPERATION_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/script_pattern.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace machine {

class BC_API operation
{
public:
    typedef std::vector<operation> list;
    typedef std::vector<operation>::const_iterator iterator;

    // Constructors.
    //-------------------------------------------------------------------------

    inline operation();

    inline operation(operation&& other);
    inline operation(const operation& other);

    inline operation(data_chunk&& uncoded, bool minimal = true);
    inline operation(const data_chunk& uncoded, bool minimal = true);

    inline operation(opcode code);

    // Operators.
    //-------------------------------------------------------------------------

    inline operation& operator=(operation&& other);
    inline operation& operator=(const operation& other);

    inline bool operator==(const operation& other) const;
    inline bool operator!=(const operation& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static operation factory_from_data(const data_chunk& encoded);
    static operation factory_from_data(std::istream& stream);
    static operation factory_from_data(reader& source);

    bool from_data(const data_chunk& encoded);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);

    bool from_string(const std::string& mnemonic);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    data_chunk to_data() const;

    std::string to_string(uint32_t active_forks) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    inline size_t serialized_size() const;

    /// Get the op code [0..255], if is_valid is consistent with data.
    inline opcode code() const;

    /// Get the data, empty if not a push code or if invalid.
    inline const data_chunk& data() const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Compute the minimal data opcode based on size alone.
    static inline opcode opcode_from_size(size_t size);

    /// Compute the minimal data opcode for a given chunk of data.
    /// If a numeric code is used then corresponding data must be set to empty.
    static inline opcode opcode_from_data(const data_chunk& data);

    /// Convert the [1..16] value to the corresponding opcode (or undefined).
    static inline opcode opcode_from_positive(uint8_t value);

    /// Convert the opcode to the corresponding [1..16] value (or undefined).
    static inline uint8_t opcode_to_positive(opcode code);

    /// Categories of opcodes.
    static inline bool is_push(opcode code);
    static inline bool is_counted(opcode code);
    static inline bool is_numeric(opcode code);
    static inline bool is_positive(opcode code);
    static inline bool is_disabled(opcode code);
    static inline bool is_conditional(opcode code);
    static inline bool is_relaxed_push(opcode code);

    // Validation.
    //-------------------------------------------------------------------------

    /// Categories of opcodes.
    inline bool is_push() const;
    inline bool is_counted() const;
    inline bool is_positive() const;
    inline bool is_disabled() const;
    inline bool is_conditional() const;
    inline bool is_relaxed_push() const;

    /// Validate the data against the code.
    inline bool is_oversized() const;

protected:
    inline operation(opcode code, data_chunk&& data, bool valid);
    inline operation(opcode code, const data_chunk& data, bool valid);
    inline static uint32_t read_data_size(opcode code, reader& source);
    inline opcode opcode_from_data(const data_chunk& uncoded, bool minimal);
    void reset();

private:
    opcode code_;
    data_chunk data_;
    bool valid_;
};

} // namespace machine
} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/machine/operation.ipp>

#endif
