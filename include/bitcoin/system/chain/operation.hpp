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
    // Constructors.
    // ------------------------------------------------------------------------

    /// Default operation is any invalid opcode with underflow set.
    operation() noexcept;

    operation(operation&& other) noexcept;
    operation(const operation& other) noexcept;

    /// Use data constructors for push_data ops.
    operation(opcode code) noexcept;

    /// These construct from push-data, not serialized operations (no codes).
    /// When minimal is true the data is interpreted as minimally-encoded push.
    operation(data_chunk&& push_data, bool minimal) noexcept;
    operation(const data_chunk& push_data, bool minimal) noexcept;
    operation(chunk_ptr push_data, bool minimal) noexcept;

    /// These deserialize operations (with codes), not from push-data.
    operation(const data_slice& op_data) noexcept;
    operation(std::istream&& stream) noexcept;
    operation(std::istream& stream) noexcept;
    operation(reader&& source) noexcept;
    operation(reader& source) noexcept;

    /// Literal string is disallowed, as it conflicts with const data_slice&.
    /// An invalid serialization returns op(any_invalid
    explicit operation(const std::string& mnemonic) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    operation& operator=(operation&& other) noexcept;
    operation& operator=(const operation& other) noexcept;

    bool operator==(const operation& other) const noexcept;
    bool operator!=(const operation& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const noexcept;
    void to_data(std::ostream& stream) const noexcept;
    void to_data(writer& sink) const noexcept;

    std::string to_string(uint32_t active_forks) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    opcode code() const noexcept;
    const data_chunk& data() const noexcept;
    chunk_ptr data_ptr() const noexcept;

    /// Computed properties.
    size_t serialized_size() const noexcept;

    // Utilities.
    // ------------------------------------------------------------------------

    /// Compute nominal data opcode based on size alone.
    static opcode opcode_from_size(size_t size) noexcept;

    /// Compute the minimal data opcode for a given chunk of data.
    /// Caller should clear data if converting to non-payload opcode.
    static opcode minimal_opcode_from_data(const data_chunk& data) noexcept;

    /// Compute the nominal data opcode for a given chunk of data.
    /// Restricted to sized data, avoids conversion to numeric opcodes.
    static opcode nominal_opcode_from_data(const data_chunk& data) noexcept;

    /// Convert the [0..16] value to the corresponding opcode (or undefined).
    static opcode opcode_from_version(uint8_t value) noexcept;

    /// Convert the [1..16] value to the corresponding opcode (or undefined).
    static opcode opcode_from_positive(uint8_t value) noexcept;

    /// Convert the opcode to the corresponding [1..16] value (or undefined).
    static uint8_t opcode_to_positive(opcode code) noexcept;

    /// Categories of opcodes.
    static bool is_push(opcode code) noexcept;
    static bool is_payload(opcode code) noexcept;
    static bool is_counted(opcode code) noexcept;
    static bool is_version(opcode code) noexcept;
    static bool is_numeric(opcode code) noexcept;
    static bool is_positive(opcode code) noexcept;
    static bool is_invalid(opcode code) noexcept;
    static bool is_reserved(opcode code) noexcept;
    static bool is_conditional(opcode code) noexcept;
    static bool is_relaxed_push(opcode code) noexcept;

    /// Categories of operations.
    /// The is_invalid() method pertains only to opcode consensus validity.
    bool is_push() const noexcept;
    bool is_payload() const noexcept;
    bool is_counted() const noexcept;
    bool is_version() const noexcept;
    bool is_numeric() const noexcept;
    bool is_positive() const noexcept;
    bool is_invalid() const noexcept;
    bool is_reserved() const noexcept;
    bool is_conditional() const noexcept;
    bool is_relaxed_push() const noexcept;
    bool is_oversized() const noexcept;
    bool is_minimal_push() const noexcept;
    bool is_nominal_push() const noexcept;
    bool is_underflow() const noexcept;

protected:
    operation(opcode code, chunk_ptr push_data_ptr, bool underflow) noexcept;

private:
    // So script may call count_op.
    friend class script;

    static operation from_data(reader& source) noexcept;
    static operation from_push_data(const chunk_ptr& data, bool minimal) noexcept;
    static operation from_string(const std::string& mnemonic) noexcept;

    static chunk_ptr no_data() noexcept;
    static chunk_ptr any_data() noexcept;
    static bool count_op(reader& source) noexcept;
    static uint32_t read_data_size(opcode code, reader& source) noexcept;
    static opcode opcode_from_data(const data_chunk& push_data, bool minimal) noexcept;

    // Operation should not be stored as shared (adds 16 bytes).
    // copy: 8 + 2 * 64 + 1 = 18 bytes (vs. 16 when shared).
    opcode code_;
    chunk_ptr data_;
    bool underflow_;
};

typedef std::vector<operation> operations;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
