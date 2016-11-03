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
#include <bitcoin/bitcoin/chain/script/operation.hpp>

#include <algorithm>
#include <numeric>
#include <bitcoin/bitcoin/chain/script/rule_fork.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/chain/script/script_pattern.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

namespace libbitcoin {
namespace chain {

// The failed-state code must not pass execution, so it must be 'disabled'.
// Otherwise the script processor would be required to check the stack state.
static constexpr auto invalid_code = opcode::disabled_xor;

// Constructors.
//-----------------------------------------------------------------------------

operation::operation()
  : code_(invalid_code), valid_(false)
{
}

operation::operation(operation&& other)
  : operation(other.code_, std::move(other.data_), other.valid_)
{
}

operation::operation(const operation& other)
  : operation(other.code_, other.data_, other.valid_)
{
}

operation::operation(opcode code)
  : code_(code), valid_(true)
{
}

operation::operation(data_chunk&& uncoded, bool minimal)
  : code_(minimal ? opcode_from_data(uncoded) :
        opcode_from_size(uncoded.size())),
    data_(std::move(uncoded)),
    valid_(!is_oversized())
{
    if (!valid_)
        reset();

    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && is_numeric(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

operation::operation(const data_chunk& uncoded, bool minimal)
  : code_(minimal ? opcode_from_data(uncoded) :
        opcode_from_size(uncoded.size())),
    data_(uncoded),
    valid_(!is_oversized())
{
    if (!valid_)
        reset();

    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && is_numeric(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

// protected
operation::operation(opcode code, data_chunk&& data, bool valid)
  : code_(code), data_(std::move(data)), valid_(valid)
{
}

// protected
operation::operation(opcode code, const data_chunk& data, bool valid)
  : code_(code), data_(data), valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

operation& operation::operator=(operation&& other)
{
    code_ = other.code_;
    data_ = std::move(other.data_);
    valid_ = other.valid_;
    return *this;
}

operation& operation::operator=(const operation& other)
{
    code_ = other.code_;
    data_ = other.data_;
    valid_ = other.valid_;
    return *this;
}

bool operation::operator==(const operation& other) const
{
    return (code_ == other.code_) && (data_ == other.data_);
}

bool operation::operator!=(const operation& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
operation operation::factory_from_data(const data_chunk& encoded)
{
    operation instance;
    instance.from_data(encoded);
    return instance;
}

// static
operation operation::factory_from_data(std::istream& stream)
{
    operation instance;
    instance.from_data(stream);
    return instance;
}

// static
operation operation::factory_from_data(reader& source)
{
    operation instance;
    instance.from_data(source);
    return instance;
}

bool operation::from_data(const data_chunk& encoded)
{
    data_source istream(encoded);
    return from_data(istream);
}

bool operation::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

// The iterator requires that the opcode read is the opcode written.
bool operation::from_data(reader& source)
{
    reset();

    valid_ = true;
    code_ = static_cast<opcode>(source.read_byte());
    const auto size = read_data_size(code_, source);

    // It is slightly more performant to skip a zero byte read and assignment.
    if (size != 0)
        data_ = source.read_bytes(size);

    if (!source)
        reset();

    return valid_;
}

static bool is_push_token(const std::string& token)
{
    return token.size() > 1 && token.front() == '[' && token.back() == ']';
}

// The removal of spaces inside of data is a compatability break with v2.
static std::string trim_push(const std::string& token)
{
    return std::string(token.begin() + 1, token.end() - 1);
}

bool operation::from_string(const std::string& mnemonic)
{
    reset();

    if (is_push_token(mnemonic))
    {
        if (decode_base16(data_, trim_push(mnemonic)) && !is_oversized())
        {
            // The produces the minimal data encoding.
            code_ = opcode_from_data(data_);
            valid_ = true;

            // Revert data if opcode_from_data produced a numeric encoding.
            if (is_numeric(code_))
            {
                data_.clear();
                data_.shrink_to_fit();
            }
        }
    }
    else if (opcode_from_string(code_, mnemonic))
    {
        valid_ = true;
    }

    if (!valid_)
        reset();

    return valid_;
}

bool operation::is_valid() const
{
    return valid_;
}

// protected
void operation::reset()
{
    code_ = invalid_code;
    data_.clear();
    valid_ = false;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk operation::to_data() const
{
    data_chunk data;
    data.reserve(serialized_size());
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void operation::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void operation::to_data(writer& sink) const
{
    static constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);
    const auto size = data_.size();
    const auto code = static_cast<uint8_t>(code_);
    sink.write_byte(code);

    switch (code_)
    {
        case opcode::push_one_size:
            sink.write_byte(static_cast<uint8_t>(size));
            break;
        case opcode::push_two_size:
            sink.write_2_bytes_little_endian(static_cast<uint16_t>(size));
            break;
        case opcode::push_four_size:
            sink.write_4_bytes_little_endian(static_cast<uint32_t>(size));
            break;
        default:
            break;
    }

    sink.write_bytes(data_);
}

// The removal of spaces inside of data is a compatability break with v2.
std::string operation::to_string(uint32_t active_forks) const
{
    if (!valid_)
        return "<invalid>";

    if (data_.empty())
        return opcode_to_string(code_, active_forks);

    return "[" + encode_base16(data_) + "]";
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

uint64_t operation::serialized_size() const
{
    const auto size = sizeof(uint8_t) + data_.size();

    switch (code_)
    {
        case opcode::push_one_size:
            return sizeof(uint8_t) + size;
        case opcode::push_two_size:
            return sizeof(uint16_t) + size;
        case opcode::push_four_size:
            return sizeof(uint32_t) + size;
        default:
            return size;

    }
}

opcode operation::code() const
{
    return code_;
}

const data_chunk& operation::data() const
{
    return data_;
}

// Utilities.
//-------------------------------------------------------------------------
// static

// private
uint32_t operation::read_data_size(opcode code, reader& source)
{
    static constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

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
// CONSENSUS: this encoding is consensus critical due to find_and_delete.
//*****************************************************************************
opcode operation::opcode_from_size(size_t size)
{
    BITCOIN_ASSERT(size <= max_uint32);
    static constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    if (size <= op_75)
        return static_cast<opcode>(size);
    else if (size <= max_uint8)
        return opcode::push_one_size;
    else if(size <= max_uint16)
        return opcode::push_two_size;
    else
        return opcode::push_four_size;
}

opcode operation::opcode_from_data(const data_chunk& data)
{
    const auto size = data.size();

    if (size != 1)
        return opcode_from_size(size);

    const auto code = static_cast<opcode>(data.front());
    return is_numeric(code) ? code : opcode_from_size(size);
}

opcode operation::opcode_from_positive(uint8_t value)
{
    BITCOIN_ASSERT(value >= script_number::positive_1);
    BITCOIN_ASSERT(value <= script_number::positive_16);
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<opcode>(value + op_81 - 1);
}

uint8_t operation::opcode_to_positive(opcode code)
{
    BITCOIN_ASSERT(is_positive(code));
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<uint8_t>(code)-op_81 - 1;
}

// [0..79, 81..96]
bool operation::is_push(opcode code)
{
    static constexpr auto op_80 = static_cast<uint8_t>(opcode::reserved_80);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96 && value != op_80;
}

// [97..255]
bool operation::is_counted(opcode code)
{
    static constexpr auto op_97 = static_cast<uint8_t>(opcode::nop);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_97;
}

// [-1, 1..16]
bool operation::is_numeric(opcode code)
{
    return is_positive(code) || code == opcode::push_negative_1;
}

// [1..16]
bool operation::is_positive(opcode code)
{
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_81 && value <= op_96;
}

bool operation::is_conditional(opcode code)
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

bool operation::is_disabled(opcode code)
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

        //*********************************************************************
        // CONSENSUS: reserved conditionals are effectively disabled.
        // These are in the conditional range yet are not handled.
        // as a result satoshi always processes them in the op swtich.
        // This causes them to always fail as unhandled. "VER" is outside the
        // satoshi conditional range test so it does not exhibit this behavior.
        //*********************************************************************
        case opcode::disabled_verif:
        case opcode::disabled_vernotif:
            return true;
        default:
            return false;
    }
}

// Validation.
//-------------------------------------------------------------------------

bool operation::is_push() const
{
    return is_push(code_);
}

bool operation::is_counted() const
{
    return is_counted(code_);
}

bool operation::is_positive() const
{
    return is_positive(code_);
}

bool operation::is_disabled() const
{
    return is_disabled(code_);
}

bool operation::is_conditional() const
{
    return is_conditional(code_);
}

bool operation::is_oversized() const
{
    // bit.ly/2eSDkOJ
    return data_.size() > max_push_data_size;
}

} // namespace chain
} // namespace libbitcoin
