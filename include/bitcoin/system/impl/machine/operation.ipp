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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_OPERATION_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_OPERATION_IPP

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/machine/opcode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

static BC_CONSTEXPR auto invalid_code = opcode::disabled_xor;

// Constructors.
//-----------------------------------------------------------------------------

inline operation::operation()
  : code_(invalid_code), valid_(false)
{
    // The failed-state code must be disabled so it will never pass evaluation.
    BITCOIN_ASSERT(is_disabled());
}

inline operation::operation(operation&& other)
  : operation(other.code_, std::move(other.data_), other.valid_)
{
}

inline operation::operation(const operation& other)
  : operation(other.code_, other.data_, other.valid_)
{
}

inline operation::operation(data_chunk&& uncoded, bool minimal)
  : code_(opcode_from_data(uncoded, minimal)),
    data_(std::move(uncoded)),
    valid_(!is_oversized())
{
    if (!valid_)
        reset();

    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && !is_payload(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

inline operation::operation(const data_chunk& uncoded, bool minimal)
  : code_(opcode_from_data(uncoded, minimal)),
    data_(uncoded),
    valid_(!is_oversized())
{
    if (!valid_)
        reset();

    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && !is_payload(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

inline operation::operation(opcode code)
  : code_(code), valid_(true)
{
}

// protected
inline operation::operation(opcode code, data_chunk&& data, bool valid)
  : code_(code), data_(std::move(data)), valid_(valid)
{
}

// protected
inline operation::operation(opcode code, const data_chunk& data, bool valid)
  : code_(code), data_(data), valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

inline operation& operation::operator=(operation&& other)
{
    code_ = other.code_;
    data_ = std::move(other.data_);
    valid_ = other.valid_;
    return *this;
}

inline operation& operation::operator=(const operation& other)
{
    code_ = other.code_;
    data_ = other.data_;
    valid_ = other.valid_;
    return *this;
}

inline bool operation::operator==(const operation& other) const
{
    return (code_ == other.code_) && (data_ == other.data_);
}

inline bool operation::operator!=(const operation& other) const
{
    return !(*this == other);
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

inline size_t operation::serialized_size() const
{
    static BC_CONSTEXPR auto op_size = sizeof(uint8_t);
    const auto size = data_.size();

    switch (code_)
    {
        case opcode::push_one_size:
            return op_size + sizeof(uint8_t) + size;
        case opcode::push_two_size:
            return op_size + sizeof(uint16_t) + size;
        case opcode::push_four_size:
            return op_size + sizeof(uint32_t) + size;
        default:
            return op_size + size;
    }
}

inline opcode operation::code() const
{
    return code_;
}

inline const data_chunk& operation::data() const
{
    return data_;
}

// Utilities.
//-----------------------------------------------------------------------------

// private
//*****************************************************************************
// CONSENSUS: op data size is limited to 520 bytes, which requires no more
// than two bytes to encode. However the four byte encoding can represent
// a value of any size, so remains valid despite the data size limit.
//*****************************************************************************
inline uint32_t operation::read_data_size(opcode code, reader& source)
{
    BC_CONSTEXPR auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    switch (code)
    {
        case opcode::push_one_size:
            return source.read_byte();
        case opcode::push_two_size:
            return source.read_2_bytes_little_endian();
        case opcode::push_four_size:
            return source.read_4_bytes_little_endian();
        default:
            const auto byte = static_cast<uint8_t>(code);
            return byte <= op_75 ? byte : 0;
    }
}

//*****************************************************************************
// CONSENSUS: this is non-minial but consensus critical due to find_and_delete.
// Presumably this was just an oversight in the original script encoding.
//*****************************************************************************
inline opcode operation::opcode_from_size(size_t size)
{
    BITCOIN_ASSERT(size <= max_uint32);
    BC_CONSTEXPR auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    if (size <= op_75)
        return static_cast<opcode>(size);
    else if (size <= max_uint8)
        return opcode::push_one_size;
    else if (size <= max_uint16)
        return opcode::push_two_size;
    else
        return opcode::push_four_size;
}

inline opcode operation::minimal_opcode_from_data(const data_chunk& data)
{
    const auto size = data.size();

    if (size == 1)
    {
        const auto value = data.front();

        if (value == number::negative_1)
            return opcode::push_negative_1;

        if (value == number::positive_0)
            return opcode::push_size_0;

        if (value >= number::positive_1 && value <= number::positive_16)
            return opcode_from_positive(value);
    }

    // Nominal encoding is minimal for multiple bytes and non-numeric values.
    return opcode_from_size(size);
}

inline opcode operation::nominal_opcode_from_data(const data_chunk& data)
{
    return opcode_from_size(data.size());
}

inline opcode operation::opcode_from_version(uint8_t value)
{
    BITCOIN_ASSERT(value <= positive_16);
    return (value == number::positive_0) ? opcode::push_size_0 :
        operation::opcode_from_positive(value);
}

inline opcode operation::opcode_from_data(const data_chunk& data,
    bool minimal)
{
    return minimal ? minimal_opcode_from_data(data) :
        nominal_opcode_from_data(data);
}

inline opcode operation::opcode_from_positive(uint8_t value)
{
    BITCOIN_ASSERT(value >= number::positive_1);
    BITCOIN_ASSERT(value <= number::positive_16);
    BC_CONSTEXPR auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<opcode>(value + op_81 - 1);
}

inline uint8_t operation::opcode_to_positive(opcode code)
{
    BITCOIN_ASSERT(is_positive(code));
    BC_CONSTEXPR auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<uint8_t>(code) - op_81 + 1;
}

// opcode: [0-79, 81-96]
inline bool operation::is_push(opcode code)
{
    BC_CONSTEXPR auto op_80 = static_cast<uint8_t>(opcode::reserved_80);
    BC_CONSTEXPR auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96 && value != op_80;
}

// opcode: [1-78]
inline bool operation::is_payload(opcode code)
{
    BC_CONSTEXPR auto op_1 = static_cast<uint8_t>(opcode::push_size_1);
    BC_CONSTEXPR auto op_78 = static_cast<uint8_t>(opcode::push_four_size);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_1 && value <= op_78;
}

// opcode: [97-255]
inline bool operation::is_counted(opcode code)
{
    BC_CONSTEXPR auto op_97 = static_cast<uint8_t>(opcode::nop);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_97;
}

// stack: [[], 1-16]
inline bool operation::is_version(opcode code)
{
    return code == opcode::push_size_0 || is_positive(code);
}

// stack: [-1, 1-16]
inline bool operation::is_numeric(opcode code)
{
    return is_positive(code) || code == opcode::push_negative_1;
}

// stack: [1-16]
inline bool operation::is_positive(opcode code)
{
    BC_CONSTEXPR auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    BC_CONSTEXPR auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_81 && value <= op_96;
}

// opcode: [80, 98, 137, 138, 186-255]
inline bool operation::is_reserved(opcode code)
{
    BC_CONSTEXPR auto op_186 = static_cast<uint8_t>(opcode::reserved_186);

    switch (code)
    {
        case opcode::reserved_80:
        case opcode::reserved_98:
        case opcode::reserved_137:
        case opcode::reserved_138:
            return true;
        default:
            return static_cast<uint8_t>(code) >= op_186;
    }
}

//*****************************************************************************
// CONSENSUS: the codes VERIF and VERNOTIF are in the conditional range yet are
// not handled. As a result satoshi always processes them in the op switch.
// This causes them to always fail as unhandled. It is misleading that the
// satoshi test cases refer to these as reserved codes. These two codes behave
// exactly as the explicitly disabled codes. On the other hand VER is not within
// the satoshi conditional range test so it is in fact reserved. Presumably
// this was an unintended consequence of range testing enums.
//*****************************************************************************
inline bool operation::is_disabled(opcode code)
{
    switch (code)
    {
        case opcode::disabled_cat:
        case opcode::disabled_substr:
        case opcode::disabled_left:
        case opcode::disabled_right:
        case opcode::disabled_invert:
        case opcode::disabled_and:
        case opcode::disabled_or:
        case opcode::disabled_xor:
        case opcode::disabled_mul2:
        case opcode::disabled_div2:
        case opcode::disabled_mul:
        case opcode::disabled_div:
        case opcode::disabled_mod:
        case opcode::disabled_lshift:
        case opcode::disabled_rshift:
        case opcode::disabled_verif:
        case opcode::disabled_vernotif:
            return true;
        default:
            return false;
    }
}

//*****************************************************************************
// CONSENSUS: in order to properly treat VERIF and VERNOTIF as disabled (see
// is_disabled comments) those codes must not be included here.
//*****************************************************************************
inline bool operation::is_conditional(opcode code)
{
    switch (code)
    {
        case opcode::if_:
        case opcode::notif:
        case opcode::else_:
        case opcode::endif:
            return true;
        default:
            return false;
    }
}

//*****************************************************************************
// CONSENSUS: this test explicitly includes the satoshi 'reserved' code.
// This affects the operation count in p2sh script evaluation.
// Presumably this was an unintended consequence of range testing enums.
//*****************************************************************************
// opcode: [0-96]
inline bool operation::is_relaxed_push(opcode code)
{
    BC_CONSTEXPR auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96;
}

inline bool operation::is_push() const
{
    return is_push(code_);
}

inline bool operation::is_counted() const
{
    return is_counted(code_);
}

inline bool operation::is_version() const
{
    return is_version(code_);
}

inline bool operation::is_positive() const
{
    return is_positive(code_);
}

inline bool operation::is_disabled() const
{
    return is_disabled(code_);
}

inline bool operation::is_conditional() const
{
    return is_conditional(code_);
}

inline bool operation::is_relaxed_push() const
{
    return is_relaxed_push(code_);
}

inline bool operation::is_oversized() const
{
    // bit.ly/2eSDkOJ
    return data_.size() > max_push_data_size;
}

inline bool operation::is_minimal_push() const
{
    return code_ == minimal_opcode_from_data(data_);
}

inline bool operation::is_nominal_push() const
{
    return code_ == nominal_opcode_from_data(data_);
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
