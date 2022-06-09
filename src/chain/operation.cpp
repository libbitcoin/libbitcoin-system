
/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <memory>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/enums/numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Gotta set something when invalid minimal result, test is_valid.
static constexpr auto any_invalid = opcode::op_xor;

// static
chunk_cptr operation::no_data_ptr() noexcept
{
    static const auto empty = to_shared<data_chunk>();
    return empty;
}

// static
chunk_cptr operation::any_data_ptr() noexcept
{
    // Push data is not possible with an invalid code, combination is invalid.
    static const auto any = to_shared<data_chunk>({ 0x42 });
    return any;
}

// Constructors.
// ----------------------------------------------------------------------------

operation::operation() noexcept
  : operation(any_invalid, any_data_ptr(), false)
{
}

// If code is push data the data member will be inconsistent (empty).
operation::operation(opcode code) noexcept
  : operation(code, no_data_ptr(), false)
{
}

operation::operation(data_chunk&& push_data, bool minimal) noexcept
  : operation(from_push_data(to_shared(std::move(push_data)), minimal))
{
}

operation::operation(const data_chunk& push_data, bool minimal) noexcept
  : operation(from_push_data(to_shared(push_data), minimal))
{
}

operation::operation(const chunk_cptr& push_data, bool minimal) noexcept
  : operation(from_push_data(push_data, minimal))
{
}

operation::operation(const data_slice& op_data) noexcept
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : operation(stream::in::copy(op_data))
    BC_POP_WARNING()
{
}

operation::operation(std::istream&& stream) noexcept
  : operation(read::bytes::istream(stream))
{
}

operation::operation(std::istream& stream) noexcept
  : operation(read::bytes::istream(stream))
{
}

operation::operation(reader&& source) noexcept
  : operation(from_data(source))
{
}

operation::operation(reader& source) noexcept
  : operation(from_data(source))
{
}

operation::operation(const std::string& mnemonic) noexcept
  : operation(from_string(mnemonic))
{
}

// protected
operation::operation(opcode code, const chunk_cptr& push_data,
    bool underflow) noexcept
  : code_(code), data_(push_data), underflow_(underflow)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool operation::operator==(const operation& other) const noexcept
{
    // Ccompare data values not pointers.
    return (code_ == other.code_)
        && (*data_ == *other.data_)
        && (underflow_ == other.underflow_);
}

bool operation::operator!=(const operation& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
operation operation::from_data(reader& source) noexcept
{
    // Guard against resetting a previously-invalid stream.
    if (!source)
        return {};

    // If stream is not empty then a non-data opcode will always deserialize.
    // A push-data opcode may indicate more bytes than are available. In this
    // case the the script is invalid, but it may not be evaluated, such as
    // with a coinbase input. So if an operation fails to deserialize it is
    // re-read and retained as an "underflow" operation. An underflow op
    // serializes as data only, and fails evaluation. Only the last operation
    // in a script could become an underflow, which may possibly contain the
    // entire script. This retains the read position in case of underflow.
    const auto start = source.get_position();

    // Size of a push-data opcode is not retained, as this is inherent in data.
    auto code = static_cast<opcode>(source.read_byte());
    const auto size = read_data_size(code, source);

    // Guard against resetting the invalidated stream.
    if (size > max_block_size)
    {
        source.invalidate();
        return {};
    }

    auto push = to_shared(source.read_bytes(size));
    const auto underflow = !source;

    // This requires that provided stream terminates at the end of the script.
    // When passing ops as part of a stream longer than the script, such as for
    // a transaction, caller should apply source.set_limit(prefix_size), and
    // clear the stream limit upon return. Stream invalidation and set_position
    // do not alter a stream limit, it just behaves as a smaller stream buffer.
    // Without a limit, source.read_bytes() below consumes the remaining stream.
    if (underflow)
    {
        code = any_invalid;
        source.set_position(start);
        push = to_shared(source.read_bytes());
    }

    // All byte vectors are deserializable, stream indicates own failure.
    return { code, push, underflow };
}

// static/private
operation operation::from_push_data(const chunk_cptr& data,
    bool minimal) noexcept
{
    const auto code = opcode_from_data(*data, minimal);

    // Minimal interpretation affects only single byte push data.
    // Revert data if (minimal) opcode_from_data produced a numeric encoding.
    const auto push = is_payload(code) ? data : no_data_ptr();

    return { code, push, false };
}

inline bool is_push_token(const std::string& token) noexcept
{
    return token.size() > one && token.front() == '[' && token.back() == ']';
}

inline bool is_text_token(const std::string& token) noexcept
{
    return token.size() > one && token.front() == '\'' && token.back() == '\'';
}

inline bool is_underflow_token(const std::string& token) noexcept
{
    return token.size() > one && token.front() == '<' && token.back() == '>';
}

inline std::string remove_token_delimiters(const std::string& token) noexcept
{
    BC_ASSERT(token.size() > one);
    return std::string(std::next(token.begin()), std::prev(token.end()));
}

inline string_list split_push_token(const std::string& token) noexcept
{
    return split(remove_token_delimiters(token), ".", false, false);
}

static bool opcode_from_data_prefix(opcode& out_code,
    const std::string& prefix, const data_chunk& push_data) noexcept
{
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);
    const auto size = push_data.size();
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
    const std::string& token) noexcept
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

// private/static
operation operation::from_string(const std::string& mnemonic) noexcept
{
    data_chunk chunk;
    auto valid = false;
    auto underflow = false;

    // Always defined below, but this fixes warning.
    opcode code{ opcode::op_xor };

    if (is_push_token(mnemonic))
    {
        // Data encoding uses single token with one or two parts.
        const auto parts = split_push_token(mnemonic);

        if (parts.size() == 1)
        {
            // Extract operation using nominal data size decoding.
            if ((valid = decode_base16(chunk, parts.front())))
                code = nominal_opcode_from_data(chunk);
        }
        else if (parts.size() == 2)
        {
            // Extract operation using explicit data size decoding.

            // More efficient [] dereference is guarded above.
            BC_PUSH_WARNING(USE_GSL_AT)
            valid = decode_base16(chunk, parts[1]) &&
                opcode_from_data_prefix(code, parts[0], chunk);
            BC_POP_WARNING()
        }
    }
    else if (is_text_token(mnemonic))
    {
        // Extract operation using nominal data size decoding.
        chunk = to_chunk(remove_token_delimiters(mnemonic));
        code = nominal_opcode_from_data(chunk);
        valid = true;
    }
    else if (is_underflow_token(mnemonic))
    {
        // code is ignored for underflow ops.
        underflow = true;
        code = any_invalid;
        valid = decode_base16(chunk, remove_token_delimiters(mnemonic));
    }
    else if (opcode_from_mnemonic(code, mnemonic))
    {
        // Any push code may have empty data, so this is presumed here.
        // No data is obtained here from a push opcode (use push/text tokens).
        valid = true;
    }
    else if (data_from_decimal(chunk, mnemonic))
    {
        // opcode_from_mnemonic captures [-1, 0, 1..16] integers, others here.
        code = nominal_opcode_from_data(chunk);
        valid = true;
    }

    if (!valid)
        return {};

    return { code, to_shared(std::move(chunk)), underflow };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk operation::to_data() const noexcept
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream);
    return data;
}

void operation::to_data(std::ostream& stream) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void operation::to_data(writer& sink) const noexcept
{
    // Underflow is op-undersized data, it is serialized with no opcode.
    // An underflow could only be a final token in a script deserialization.
    if (is_underflow())
    {
        sink.write_bytes(*data_);
    }
    else
    {
        const auto size = data_->size();
        sink.write_byte(static_cast<uint8_t>(code_));

        switch (code_)
        {
            case opcode::push_one_size:
                sink.write_byte(narrow_cast<uint8_t>(size));
                break;
            case opcode::push_two_size:
                sink.write_2_bytes_little_endian(narrow_cast<uint16_t>(size));
                break;
            case opcode::push_four_size:
                sink.write_4_bytes_little_endian(
                    possible_narrow_cast<uint32_t>(size));
                break;
            default:
            break;
        }

        sink.write_bytes(*data_);
    }
}

// To String.
// ----------------------------------------------------------------------------

static std::string opcode_to_prefix(opcode code,
    const data_chunk& data) noexcept
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

std::string operation::to_string(uint32_t active_forks) const noexcept
{
    if (!is_valid())
        return "(?)";

    if (underflow_)
        return "<" + encode_base16(*data_) + ">";

    if (data_->empty())
        return opcode_to_mnemonic(code_, active_forks);

    // Data encoding uses single token with explicit size prefix as required.
    return "[" + opcode_to_prefix(code_, *data_) + encode_base16(*data_) + "]";
}

// Properties.
// ----------------------------------------------------------------------------

bool operation::is_valid() const noexcept
{
    // Push data not possible with any is_invalid, combination is invalid.
    // This is necessary because there can be no invalid sentinel value.
    return !(code_ == any_invalid && !underflow_ && !data_->empty());
}

opcode operation::code() const noexcept
{
    return code_;
}

const data_chunk& operation::data() const noexcept
{
    return *data_;
}

const chunk_cptr& operation::data_ptr() const noexcept
{
    return data_;
}

size_t operation::serialized_size() const noexcept
{
    static constexpr auto op_size = sizeof(uint8_t);
    const auto size = data_->size();

    if (underflow_)
        return size;

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

// Utilities.
// ----------------------------------------------------------------------------

// static/private
// Advances stream, returns true unless exhausted.
// Does not advance to end position in the case of underflow operation.
bool operation::count_op(reader& source) noexcept
{
    if (source.is_exhausted())
        return false;

    const auto code = static_cast<opcode>(source.read_byte());
    source.skip_bytes(read_data_size(code, source));
    return true;
}

// static/private
uint32_t operation::read_data_size(opcode code, reader& source) noexcept
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
//*****************************************************************************
opcode operation::opcode_from_size(size_t size) noexcept
{
    BC_ASSERT(size <= max_uint32);
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

opcode operation::minimal_opcode_from_data(const data_chunk& data) noexcept
{
    const auto size = data.size();

    if (size == one)
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

opcode operation::nominal_opcode_from_data(const data_chunk& data) noexcept
{
    return opcode_from_size(data.size());
}

// static/private
opcode operation::opcode_from_data(const data_chunk& data,
    bool minimal) noexcept
{
    return minimal ? minimal_opcode_from_data(data) :
        nominal_opcode_from_data(data);
}

opcode operation::opcode_from_version(uint8_t value) noexcept
{
    BC_ASSERT(value <= numbers::positive_16);
    return (value == numbers::positive_0) ? opcode::push_size_0 :
        operation::opcode_from_positive(value);
}

opcode operation::opcode_from_positive(uint8_t value) noexcept
{
    BC_ASSERT(value >= numbers::positive_1);
    BC_ASSERT(value <= numbers::positive_16);
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<opcode>(value + sub1(op_81));
}

uint8_t operation::opcode_to_positive(opcode code) noexcept
{
    BC_ASSERT(is_positive(code));
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<uint8_t>(code) - add1(op_81);
}

// Categories of opcodes.
// ----------------------------------------------------------------------------

// opcode: [101-102, 126-129, 131-134, 141-142, 149-153]
// ****************************************************************************
// CONSENSUS: These are invalid even if evaluation is precluded by conditional.
// ****************************************************************************
bool operation::is_invalid(opcode code) noexcept
{
    // C++14: switch in constexpr.
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
bool operation::is_reserved(opcode code) noexcept
{
    constexpr auto op_185 = opcode::nop10;

    // C++14: switch in constexpr.
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
            return code > op_185;
    }
}

// opcode: [99-100, 103-104]
bool operation::is_conditional(opcode code) noexcept
{
    // C++14: switch in constexpr.
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
////// opcode: [0-96]
////bool operation::is_relaxed_push(opcode code) noexcept
////{
////    constexpr auto op_96 = opcode::push_positive_16;
////    return code <= op_96;
////}

// Categories of operations.
// ----------------------------------------------------------------------------

bool operation::is_push() const noexcept
{
    return is_push(code_);
}

bool operation::is_payload() const noexcept
{
    return is_payload(code_);
}

bool operation::is_counted() const noexcept
{
    return is_counted(code_);
}

bool operation::is_version() const noexcept
{
    return is_version(code_);
}

bool operation::is_numeric() const noexcept
{
    return is_numeric(code_);
}

bool operation::is_positive() const noexcept
{
    return is_positive(code_);
}

bool operation::is_invalid() const noexcept
{
    return is_invalid(code_);
}

bool operation::is_reserved() const noexcept
{
    return is_reserved(code_);
}

bool operation::is_conditional() const noexcept
{
    return is_conditional(code_);
}

bool operation::is_relaxed_push() const noexcept
{
    return is_relaxed_push(code_);
}

bool operation::is_oversized() const noexcept
{
    // Rule imposed by [0.3.6] soft fork.
    return data_->size() > max_push_data_size;
}

bool operation::is_minimal_push() const noexcept
{
    return code_ == minimal_opcode_from_data(*data_);
}

bool operation::is_nominal_push() const noexcept
{
    return code_ == nominal_opcode_from_data(*data_);
}

// ****************************************************************************
// CONSENSUS: An underflow is sized op-undersized data. This is valid as long
// as the operation is not executed. For example, coinbase input scripts.
// ****************************************************************************
bool operation::is_underflow() const noexcept
{
    return underflow_;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

operation tag_invoke(json::value_to_tag<operation>,
    const json::value& value) noexcept
{
    return operation{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const operation& operation) noexcept
{
    value = operation.to_string(forks::all_rules);
}

BC_POP_WARNING()

operation::cptr tag_invoke(json::value_to_tag<operation::cptr>,
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<operation>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const operation::cptr& operation) noexcept
{
    tag_invoke(tag, value, *operation);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
