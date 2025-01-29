/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
    DEFAULT_COPY_MOVE_DESTRUCT(operation);

    typedef std::shared_ptr<const operation> cptr;

    /// Utilities.
    /// -----------------------------------------------------------------------

    static constexpr uint8_t opcode_to_positive(opcode code) NOEXCEPT;
    static constexpr size_t opcode_to_maximum_size(opcode code) NOEXCEPT;
    static constexpr opcode opcode_from_size(size_t size) NOEXCEPT;
    static constexpr opcode opcode_from_version(uint8_t value) NOEXCEPT;
    static constexpr opcode opcode_from_positive(uint8_t value) NOEXCEPT;

    static constexpr opcode minimal_opcode_from_data(
        const data_chunk& data) NOEXCEPT;
    static constexpr opcode nominal_opcode_from_data(
        const data_chunk& data) NOEXCEPT;

    /// Categories of opcodes.
    /// -----------------------------------------------------------------------

    static constexpr bool is_relaxed_push(opcode code) NOEXCEPT;
    static constexpr bool is_push(opcode code) NOEXCEPT;
    static constexpr bool is_payload(opcode code) NOEXCEPT;
    static constexpr bool is_counted(opcode code) NOEXCEPT;
    static constexpr bool is_positive(opcode code) NOEXCEPT;
    static constexpr bool is_version(opcode code) NOEXCEPT;
    static constexpr bool is_numeric(opcode code) NOEXCEPT;
    static constexpr bool is_number(opcode code) NOEXCEPT;
    static constexpr bool is_invalid(opcode code) NOEXCEPT;
    static constexpr bool is_conditional(opcode code) NOEXCEPT;
    static constexpr bool is_reserved(opcode code) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default operation is any invalid opcode with underflow set.
    operation() NOEXCEPT;

    /// Use data constructors for push_data ops.
    operation(opcode code) NOEXCEPT;

    /// These construct from push-data, not serialized operations (no codes).
    /// When minimal is true the data is interpreted as minimally-encoded push.
    operation(data_chunk&& push_data, bool minimal) NOEXCEPT;
    operation(const data_chunk& push_data, bool minimal) NOEXCEPT;
    operation(const chunk_cptr& push_data, bool minimal) NOEXCEPT;

    /// These deserialize operations (with codes), not from push-data.
    operation(const data_slice& op_data) NOEXCEPT;
    ////operation(stream::in::fast&& stream) NOEXCEPT;
    operation(stream::in::fast& stream) NOEXCEPT;
    operation(std::istream&& stream) NOEXCEPT;
    operation(std::istream& stream) NOEXCEPT;
    operation(reader&& source) NOEXCEPT;
    operation(reader& source) NOEXCEPT;

    // TODO: move to config serialization wrapper.
    // TODO: a byte-deserialized operation cannot be invalid unless empty.
    // TODO: byte size cannot overflow, but can underflow (remains valid).
    // TODO: but mnemonic decoding may be invalid. moving this to config
    // TODO: allows string decoding to throw, consistent with other config.
    /// Literal string is disallowed, as it conflicts with const data_slice&.
    explicit operation(const std::string& mnemonic) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const operation& other) const NOEXCEPT;
    bool operator!=(const operation& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    // TODO: move to config serialization wrapper.
    std::string to_string(uint32_t active_flags) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    opcode code() const NOEXCEPT;
    const data_chunk& data() const NOEXCEPT;
    const chunk_cptr& data_ptr() const NOEXCEPT;

    /// Computed properties.
    size_t serialized_size() const NOEXCEPT;

    /// Categories of operations.
    /// -----------------------------------------------------------------------

    /// The is_invalid() method pertains only to opcode consensus validity and
    /// should not be confused with the inversion of is_valid (serialization).
    bool is_invalid() const NOEXCEPT;
    bool is_push() const NOEXCEPT;
    bool is_payload() const NOEXCEPT;
    bool is_counted() const NOEXCEPT;
    bool is_version() const NOEXCEPT;
    bool is_numeric() const NOEXCEPT;
    bool is_positive() const NOEXCEPT;
    bool is_reserved() const NOEXCEPT;
    bool is_conditional() const NOEXCEPT;
    bool is_relaxed_push() const NOEXCEPT;
    bool is_minimal_push() const NOEXCEPT;
    bool is_nominal_push() const NOEXCEPT;
    bool is_underflow() const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;
    bool is_underclaimed() const NOEXCEPT;

protected:
    operation(opcode code, const chunk_cptr& push_data_ptr,
        bool underflow) NOEXCEPT;

private:
    // So script may call count_op.
    friend class script;
    static bool count_op(reader& source) NOEXCEPT;

    static operation from_push_data(const chunk_cptr& data,
        bool minimal) NOEXCEPT;

    // TODO: move to config serialization wrapper.
    static operation from_string(const std::string& mnemonic) NOEXCEPT;

    static const data_chunk& no_data() NOEXCEPT;
    static const chunk_cptr& no_data_cptr() NOEXCEPT;
    static const chunk_cptr& any_data_cptr() NOEXCEPT;
    static uint32_t read_data_size(opcode code, reader& source) NOEXCEPT;
    static inline opcode opcode_from_data(const data_chunk& push_data,
        bool minimal) NOEXCEPT
    {
        return minimal ? minimal_opcode_from_data(push_data) :
            nominal_opcode_from_data(push_data);
    }

    bool data_empty() const NOEXCEPT;
    size_t data_size() const NOEXCEPT;
    const data_chunk& get_data() const NOEXCEPT;
    const chunk_cptr& get_data_cptr() const NOEXCEPT;
    void assign_data(reader& source) NOEXCEPT;

    // Operation should not be stored as shared (adds 16 bytes).
    // copy: 8 + 2 * 64 + 1 = 18 bytes (vs. 16 when shared).
    opcode code_;
    chunk_cptr data_;
    bool underflow_;
};

typedef std_vector<operation> operations;

DECLARE_JSON_VALUE_CONVERTORS(operation);
DECLARE_JSON_VALUE_CONVERTORS(operation::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/operation.ipp>

namespace std
{
template<>
struct hash<bc::system::chain::operation>
{
    size_t operator()(const bc::system::chain::operation& value) const NOEXCEPT
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data());
    }
};
} // namespace std

#endif
