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
#include <bitcoin/system/chain/operation.hpp>

#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/enums/numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Gotta set something when invalid minimal result, test is_valid.
static constexpr auto any_invalid = opcode::op_xor;

// Constructors.
//-----------------------------------------------------------------------------

// Always invalid (no code has been specified).
operation::operation()
  : code_(any_invalid), valid_(false)
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

// Invalid only if minimal encoding required and not satisfied.
operation::operation(data_chunk&& uncoded, bool minimal)
  : code_(opcode_from_data(uncoded, minimal)), data_(std::move(uncoded)), valid_(true)
{
    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && !is_payload(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

// Invalid only if minimal encoding required and not satisfied.
operation::operation(const data_chunk& uncoded, bool minimal)
  : code_(opcode_from_data(uncoded, minimal)), data_(uncoded), valid_(true)
{
    // Revert data if opcode_from_data produced a numeric encoding.
    if (minimal && !is_payload(code_))
    {
        data_.clear();
        data_.shrink_to_fit();
    }
}

// Always valid (all codes are valid in this sense).
operation::operation(opcode code)
  : code_(code), valid_(true)
{
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
    return
        (code_ == other.code_) &&
        (data_ == other.data_) &&
        (valid_ == other.valid_);
}

bool operation::operator!=(const operation& other) const
{
    return !(*this == other);
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

//*****************************************************************************
// CONSENSUS: hashed serialization honors the opcode indication of data size,
// even if it is not minimally-encoded.
//*****************************************************************************
size_t operation::serialized_size() const
{
    static constexpr auto op_size = sizeof(uint8_t);
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

opcode operation::code() const
{
    return code_;
}

const data_chunk& operation::data() const
{
    return data_;
}

// Utilities.
//-----------------------------------------------------------------------------

// private
//*****************************************************************************
// CONSENSUS: Given that hash serialization honors non-minimal encoding, we
// accept it and do not normalize it upon deserialization or serialization.
//*****************************************************************************
uint32_t operation::read_data_size(opcode code, reader& source)
{
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

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
// CONSENSUS: non-minial encoding is consensus critical due to find_and_delete.
// Presumably this was just an oversight in the original hash serialization.
//*****************************************************************************
opcode operation::opcode_from_size(size_t size)
{
    BITCOIN_ASSERT(size <= max_uint32);
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    if (size <= op_75)
        return static_cast<opcode>(size);
    else if (size <= max_uint8)
        return opcode::push_one_size;
    else if (size <= max_uint16)
        return opcode::push_two_size;
    else
        return opcode::push_four_size;
}

opcode operation::minimal_opcode_from_data(const data_chunk& data)
{
    const auto size = data.size();

    if (size == 1)
    {
        const auto value = data.front();

        if (value == numbers::negative_1)
            return opcode::push_negative_1;

        if (value == numbers::positive_0)
            return opcode::push_size_0;

        if (value >= numbers::positive_1 &&
            value <= numbers::positive_16)
            return opcode_from_positive(value);
    }

    // Nominal encoding is minimal for multiple bytes and non-numeric values.
    return opcode_from_size(size);
}

opcode operation::nominal_opcode_from_data(const data_chunk& data)
{
    return opcode_from_size(data.size());
}

opcode operation::opcode_from_version(uint8_t value)
{
    BITCOIN_ASSERT(value <= numbers::positive_16);
    return (value == numbers::positive_0) ? opcode::push_size_0 :
        operation::opcode_from_positive(value);
}

opcode operation::opcode_from_data(const data_chunk& data,
    bool minimal)
{
    return minimal ? minimal_opcode_from_data(data) :
        nominal_opcode_from_data(data);
}

opcode operation::opcode_from_positive(uint8_t value)
{
    BITCOIN_ASSERT(value >= numbers::positive_1);
    BITCOIN_ASSERT(value <= numbers::positive_16);
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<opcode>(value + op_81 - 1);
}

uint8_t operation::opcode_to_positive(opcode code)
{
    BITCOIN_ASSERT(is_positive(code));
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<uint8_t>(code) - op_81 + 1;
}

// opcode: [0-79, 81-96]
bool operation::is_push(opcode code)
{
    constexpr auto op_80 = static_cast<uint8_t>(opcode::reserved_80);
    constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96 && value != op_80;
}

// opcode: [1-78]
bool operation::is_payload(opcode code)
{
    constexpr auto op_1 = static_cast<uint8_t>(opcode::push_size_1);
    constexpr auto op_78 = static_cast<uint8_t>(opcode::push_four_size);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_1 && value <= op_78;
}

// opcode: [97-255]
bool operation::is_counted(opcode code)
{
    constexpr auto op_97 = static_cast<uint8_t>(opcode::nop);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_97;
}

// stack: [[], 1-16]
bool operation::is_version(opcode code)
{
    return code == opcode::push_size_0 || is_positive(code);
}

// stack: [-1, 1-16]
bool operation::is_numeric(opcode code)
{
    return is_positive(code) || code == opcode::push_negative_1;
}

// stack: [1-16]
bool operation::is_positive(opcode code)
{
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value >= op_81 && value <= op_96;
}

// opcode: [101-102, 126-129, 131-134, 141-142, 149-153]
// ****************************************************************************
// CONSENSUS: These are invalid even if evaluation is precluded by conditional.
// ****************************************************************************
bool operation::is_invalid(opcode code)
{
    switch (code)
    {
        // Demoted to invalid by [0.3.6] soft fork.
        case opcode::op_verif:
        case opcode::op_vernotif:

        // Demoted to invalid by [0.3.10] soft fork.
        case opcode::op_cat:
        case opcode::op_substr:
        case opcode::op_left:
        case opcode::op_right:
        case opcode::op_invert:
        case opcode::op_and:
        case opcode::op_or:
        case opcode::op_xor:
        case opcode::op_mul2:
        case opcode::op_div2:
        case opcode::op_mul:
        case opcode::op_div:
        case opcode::op_mod:
        case opcode::op_lshift:
        case opcode::op_rshift:
            return true;
        default:
            return false;
    }
}

// opcode: [80, 98, 106, 137-138, 186-255]
// ****************************************************************************
// CONSENSUS: These are invalid unless evaluation is precluded by conditional.
//
// Satoshi test cases incorrectly refer to op_ver and op_verif as "reserved".
// Reserved refers to codes that are unused but conditionally acceptable. When
// the conditional operator skips over them, the script may be valid. On the
// other hand, "disabled" codes are unconditionally invalid - such as op_cat.
// The "disabled" codes are in a group outside of the evaluation switch, which
// makes their unconditional invalidity obvious. The other two disabled codes
// are not so obvious in behavior, and misidentified in satoshi test vectors:
//
// These fail because the scripts are unconditional with both reserved and
// disabled codes, yet they are all referred to as reserved.
// { "1", "ver", "op_ver is reserved" }
// { "1", "verif", "op_verif is reserved" }
// { "1", "vernotif", "op_vernotif is reserved" }
// { "1", "reserved", "op_reserved is reserved" }
// { "1", "reserved1", "op_reserved1 is reserved" }
// { "1", "reserved2", "op_reserved2 is reserved" }
//
// These fail because the scripts either execute conditionally invalid codes
// (op_ver) or include unconditionally invalid codes, without execution
// (op_verif, op_vernotif). The comments are correct, contradicting the above.
// { "1", "if ver else 1 endif", "ver is reserved" }
// { "0", "if verif else 1 endif", "verif illegal everywhere" }
// { "0", "if else 1 else verif endif", "verif illegal everywhere" }
// { "0", "if vernotif else 1 endif", "vernotif illegal everywhere" }
// { "0", "if else 1 else vernotif endif", "vernotif illegal everywhere" }
//
// These fail regardless of conditional exclusion because they are also
// disabled codes.
// { "'a' 'b'", "cat", "cat disabled" }
// { "'a' 'b' 0", "if cat else 1 endif", "cat disabled" }
// { "'abc' 1 1", "substr", "substr disabled" }
// { "'abc' 1 1 0", "if substr else 1 endif", "substr disabled" }
// { "'abc' 2 0", "if left else 1 endif", "left disabled" }
// { "'abc' 2 0", "if right else 1 endif", "right disabled" }
// { "'abc'", "if invert else 1 endif", "invert disabled" }
// { "1 2 0 if and else 1 endif", "nop", "and disabled" }
// { "1 2 0 if or else 1 endif", "nop", "or disabled" }
// { "1 2 0 if xor else 1 endif", "nop", "xor disabled" }
// { "2 0 if 2mul else 1 endif", "nop", "2mul disabled" }
// { "2 0 if 2div else 1 endif", "nop", "2div disabled" }
// { "2 2 0 if mul else 1 endif", "nop", "mul disabled" }
// { "2 2 0 if div else 1 endif", "nop", "div disabled" }
// { "2 2 0 if mod else 1 endif", "nop", "mod disabled" }
// { "2 2 0 if lshift else 1 endif", "nop", "lshift disabled" }
// { "2 2 0 if rshift else 1 endif", "nop", "rshift disabled" }
//
// The reason that op_verif and op_vernotif are unconditionally invalid (and
// therefore behave exactly as "disabled" codes is that they are conditionals.
// Note that op_ver is not a conditional, so despite the similar name, when it
// was disabled it became a "reserved" code. The former conditonals are not
// excludable by remaining conditions. They pass this condition:
//      else if (fExec || (OP_IF <= opcode && opcode <= OP_ENDIF))
// which evaluates all codes, yet there were removed from the evaluation. They
// were part of this switch case, but now this break is not hit by them:
//      case OP_IF:
//      case OP_NOTIF: {...}
//          break;
// So they fall through to the conditional default case, just as any reserved
// code would, and halrt the evaluation with the bad_opcode error:
//      default:
//          return set_error(serror, SCRIPT_ERR_BAD_OPCODE);
// Yet because they do not bypass any conditional evaluation, they hit this
// default unconditionally. So they are "disabled" codes. We use the term
// "invalid" above because of the confusion that can cause. The only truly
// "reserved" codes are the op_nop# codes, which were promoted by a hard fork.
// ****************************************************************************
bool operation::is_reserved(opcode code)
{
    constexpr auto op_185 = static_cast<uint8_t>(opcode::nop10);

    switch (code)
    {
        // Demoted to reserved by [0.3.6] soft fork.
        case opcode::op_ver:
        case opcode::op_return:

        // Unimplemented.
        case opcode::reserved_80:
        case opcode::reserved_137:
        case opcode::reserved_138:
            return true;
        default:
            return static_cast<uint8_t>(code) > op_185;
    }
}

// opcode: [99-100, 103-104]
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

//*****************************************************************************
// CONSENSUS: this test explicitly includes the satoshi 'reserved' code.
// This affects the operation count in p2sh script evaluation.
// Presumably this was an unintended consequence of range testing enums.
//*****************************************************************************
// opcode: [0-96]
bool operation::is_relaxed_push(opcode code)
{
    constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96;
}

bool operation::is_push() const
{
    return is_push(code_);
}

bool operation::is_counted() const
{
    return is_counted(code_);
}

bool operation::is_version() const
{
    return is_version(code_);
}

bool operation::is_positive() const
{
    return is_positive(code_);
}

bool operation::is_invalid() const
{
    return is_invalid(code_);
}

bool operation::is_conditional() const
{
    return is_conditional(code_);
}

bool operation::is_relaxed_push() const
{
    return is_relaxed_push(code_);
}

bool operation::is_oversized() const
{
    // Rule imposed by [0.3.6] soft fork.
    return data_.size() > max_push_data_size;
}

bool operation::is_minimal_push() const
{
    return code_ == minimal_opcode_from_data(data_);
}

bool operation::is_nominal_push() const
{
    return code_ == nominal_opcode_from_data(data_);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
operation operation::factory(const data_chunk& encoded)
{
    operation instance;
    instance.from_data(encoded);
    return instance;
}

// static
operation operation::factory(std::istream& stream)
{
    operation instance;
    instance.from_data(stream);
    return instance;
}

// static
operation operation::factory(reader& source)
{
    operation instance;
    instance.from_data(source);
    return instance;
}

bool operation::from_data(const data_chunk& encoded)
{
    stream::in::copy istream(encoded);
    return from_data(istream);
}

bool operation::from_data(std::istream& stream)
{
    read::bytes::istream source(stream);
    return from_data(source);
}

bool operation::from_data(reader& source)
{
    valid_ = true;
    code_ = static_cast<opcode>(source.read_byte());
    const auto size = read_data_size(code_, source);

    // The max_script_size and max_push_data_size constants limit evaluation,
    // but not all scripts evaluate, so use max_block_size to guard memory 
    // allocation here. Interesting question before max_block_size soft fork.
    if (size > max_block_size)
        source.invalidate();
    else
        data_ = source.read_bytes(size);

    if (!source)
        reset();

    return valid_;
}

inline bool is_push_token(const std::string& token)
{
    return token.size() > 1 && token.front() == '[' && token.back() == ']';
}

inline bool is_text_token(const std::string& token)
{
    return token.size() > 1 && token.front() == '\'' && token.back() == '\'';
}

inline std::string remove_token_delimiters(const std::string& token)
{
    BITCOIN_ASSERT(token.size() > 1);
    return std::string(std::next(token.begin()), std::prev(token.end()));
}

inline string_list split_push_token(const std::string& token)
{
    return split(remove_token_delimiters(token), ".", false, false);
}

static bool opcode_from_data_prefix(opcode& out_code,
    const std::string& prefix, const data_chunk& data)
{
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);
    const auto size = data.size();
    out_code = operation::opcode_from_size(size);

    if (prefix == "0")
    {
        return size <= op_75;
    }
    else if (prefix == "1")
    {
        out_code = opcode::push_one_size;
        return size <= max_uint8;
    }
    else if (prefix == "2")
    {
        out_code = opcode::push_two_size;
        return size <= max_uint16;
    }
    else if (prefix == "4")
    {
        out_code = opcode::push_four_size;
        return size <= max_uint32;
    }

    return false;
}

static bool data_from_decimal(data_chunk& out_data,
    const std::string& token)
{
    // Deserialization to a number can convert random text to zero.
    if (!is_ascii_numeric(token))
        return false;

    int64_t value;
    if (!deserialize(value, token))
        return false;

    out_data = machine::number(value).data();
    return true;
}

// The removal of spaces in v3 data is a compatibility break with our v2.
bool operation::from_string(const std::string& mnemonic)
{
    reset();

    if (is_push_token(mnemonic))
    {
        // Data encoding uses single token with one or two parts.
        const auto parts = split_push_token(mnemonic);

        if (parts.size() == 1)
        {
            // Extract operation using nominal data size encoding.
            if ((valid_ = decode_base16(data_, parts.front())))
                code_ = nominal_opcode_from_data(data_);
        }
        else if (parts.size() == 2)
        {
            // Extract operation using minimal data size encoding.
            valid_ = decode_base16(data_, parts[1]) &&
                opcode_from_data_prefix(code_, parts[0], data_);
        }
    }
    else if (is_text_token(mnemonic))
    {
        // Extract operation using nominal data size encoding.
        data_ = to_chunk(remove_token_delimiters(mnemonic));
        code_ = nominal_opcode_from_data(data_);
        valid_ = true;
    }
    else if (opcode_from_mnemonic(code_, mnemonic))
    {
        // Any push code may have empty data, so this is presumed here.
        // No data is obtained here from a push opcode (use push/text tokens).
        valid_ = true;
    }
    else if (data_from_decimal(data_, mnemonic))
    {
        // opcode_from_mnemonic captures [-1, 0, 1..16] integers, others here.
        code_ = nominal_opcode_from_data(data_);
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
    code_ = any_invalid;
    data_.clear();
    valid_ = false;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk operation::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void operation::to_data(std::ostream& stream) const
{
    write::bytes::ostream out(stream);
    to_data(out);
}

//*****************************************************************************
// CONSENSUS: Non-minimal encoding is allowed by consensus hashing.
//*****************************************************************************
void operation::to_data(writer& sink) const
{
    const auto size = data_.size();

    sink.write_byte(static_cast<uint8_t>(code_));

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

static std::string opcode_to_prefix(opcode code, const data_chunk& data)
{
    // If opcode is minimal for a size-based encoding, do not set a prefix.
    if (code == operation::opcode_from_size(data.size()))
        return "";

    switch (code)
    {
        case opcode::push_one_size:
            return "1.";
        case opcode::push_two_size:
            return "2.";
        case opcode::push_four_size:
            return "4.";
        default:
            return "0.";
    }
}

// The removal of spaces in v3 data is a compatibility break with our v2.
std::string operation::to_string(uint32_t active_forks) const
{
    if (!valid_)
        return "<invalid>";

    if (data_.empty())
        return opcode_to_mnemonic(code_, active_forks);

    // Data encoding uses single token with explicit size prefix as required.
    return "[" + opcode_to_prefix(code_, data_) + encode_base16(data_) + "]";
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
