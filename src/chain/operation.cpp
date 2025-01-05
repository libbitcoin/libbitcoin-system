
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
#include <bitcoin/system/chain/operation.hpp>

#include <algorithm>
#include <memory>
#include <bitcoin/system/chain/enums/numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Gotta set something when invalid minimal result, test is_valid.
static constexpr auto any_invalid = opcode::op_xor;

// Null data helpers.
// ----------------------------------------------------------------------------

// static/private
const data_chunk& operation::no_data() NOEXCEPT
{
    static const data_chunk empty_data{};
    return empty_data;
}

// static/private
const chunk_cptr& operation::no_data_cptr() NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    static const std::shared_ptr<const data_chunk> empty
    {
        new const data_chunk{}
    };
    BC_POP_WARNING()
    return empty;
}

// static/private
// Push data is not possible with an invalid code, combination is invalid.
const chunk_cptr& operation::any_data_cptr() NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    static const std::shared_ptr<const data_chunk> any
    {
        new const data_chunk{ 0x42 }
    };
    BC_POP_WARNING()
    return any;
}

bool operation::data_empty() const NOEXCEPT
{
    return !data_ || data_->empty();
}

size_t operation::data_size() const NOEXCEPT
{
    return data_ ? data_->size() : zero;
}

const data_chunk& operation::get_data() const NOEXCEPT
{
    return data_ ? *data_ : no_data();
}

const chunk_cptr& operation::get_data_cptr() const NOEXCEPT
{
    return data_ ? data_ : no_data_cptr();
}

// Constructors.
// ----------------------------------------------------------------------------

operation::operation() NOEXCEPT
  : operation(any_invalid, any_data_cptr(), false)
{
}

// If code is push data the data member will be inconsistent (empty).
operation::operation(opcode code) NOEXCEPT
  : operation(code, nullptr, false)
{
}

operation::operation(data_chunk&& push_data, bool minimal) NOEXCEPT
  : operation(from_push_data(to_shared(std::move(push_data)), minimal))
{
}

operation::operation(const data_chunk& push_data, bool minimal) NOEXCEPT
  : operation(from_push_data(to_shared(push_data), minimal))
{
}

operation::operation(const chunk_cptr& push_data, bool minimal) NOEXCEPT
  : operation(from_push_data(push_data, minimal))
{
}

operation::operation(const data_slice& op_data) NOEXCEPT
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : operation(stream::in::copy(op_data))
    BC_POP_WARNING()
{
}

////operation::operation(stream::in::fast&& stream) NOEXCEPT
////  : operation(read::bytes::fast(stream))
////{
////}

operation::operation(stream::in::fast& stream) NOEXCEPT
  : operation(read::bytes::fast(stream))
{
}

operation::operation(std::istream&& stream) NOEXCEPT
  : operation(read::bytes::istream(stream))
{
}

operation::operation(std::istream& stream) NOEXCEPT
  : operation(read::bytes::istream(stream))
{
}

operation::operation(reader&& source) NOEXCEPT
  : operation(source)
{
}

operation::operation(reader& source) NOEXCEPT
{
    assign_data(source);
}

operation::operation(const std::string& mnemonic) NOEXCEPT
  : operation(from_string(mnemonic))
{
}

// protected
operation::operation(opcode code, const chunk_cptr& push_data,
    bool underflow) NOEXCEPT
  : code_(code), data_(push_data), underflow_(underflow)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool operation::operator==(const operation& other) const NOEXCEPT
{
    return (code_ == other.code_)
        && (data_ == other.data_ || get_data() == other.get_data())
        && (underflow_ == other.underflow_);
}

bool operation::operator!=(const operation& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// private
void operation::assign_data(reader& source) NOEXCEPT
{
    auto& allocator = source.get_allocator();

    // Guard against resetting a previously-invalid stream.
    if (!source)
    {
        INPLACE(&data_, data_chunk, allocator, nullptr);
        return;
    }

    // If stream is not empty then a non-data opcode will always deserialize.
    // A push-data opcode may indicate more bytes than are available. In this
    // case the the script is invalid, but it may not be evaluated, such as
    // with a coinbase input. So if an operation fails to deserialize it is
    // re-read and retained as an "underflow" operation. An underflow op
    // serializes as data only, and fails evaluation. Only the last operation
    // in a script could become an underflow, which may possibly contain the
    // entire script. This retains the read position in case of underflow.
    const auto start = source.get_read_position();

    // Size of a push-data opcode is not retained, as this is inherent in data.
    code_ = static_cast<opcode>(source.read_byte());
    const auto size = read_data_size(code_, source);

    // read_bytes only guarded from excessive allocation by stream limit.
    if (size > max_block_size)
        source.invalidate();

    // An invalid source.read_bytes_raw returns nullptr.
    INPLACE(&data_, data_chunk, allocator, source.read_bytes_raw(size));
    underflow_ = !source;

    // This requires that provided stream terminates at the end of the script.
    // When passing ops as part of a stream longer than the script, such as for
    // a transaction, caller should apply source.set_limit(prefix_size), and
    // clear the stream limit upon return. Stream invalidation and set_position
    // do not alter a stream limit, it just behaves as a smaller stream buffer.
    // Without a limit, source.read_bytes() below consumes the remaining stream.
    if (underflow_)
    {
        code_ = any_invalid;
        source.set_position(start);

        // An invalid source.read_bytes_raw returns nullptr.
        data_.reset(POINTER(data_chunk, allocator, source.read_bytes_raw()));
    }

    // All byte vectors are deserializable, stream indicates own failure.
}

// static/private
operation operation::from_push_data(const chunk_cptr& data,
    bool minimal) NOEXCEPT
{
    const auto code = opcode_from_data(*data, minimal);

    // Minimal interpretation affects only single byte push data.
    // Revert data if (minimal) opcode_from_data produced a numeric encoding.
    const auto push = is_payload(code) ? data : nullptr;

    return { code, push, false };
}

inline bool is_push_token(const std::string& token) NOEXCEPT
{
    return token.size() > one && token.front() == '[' && token.back() == ']';
}

inline bool is_text_token(const std::string& token) NOEXCEPT
{
    return token.size() > one && token.front() == '\'' && token.back() == '\'';
}

inline bool is_underflow_token(const std::string& token) NOEXCEPT
{
    return token.size() > one && token.front() == '<' && token.back() == '>';
}

inline std::string remove_token_delimiters(const std::string& token) NOEXCEPT
{
    BC_ASSERT(token.size() > one);
    return std::string(std::next(token.begin()), std::prev(token.end()));
}

inline string_list split_push_token(const std::string& token) NOEXCEPT
{
    return split(remove_token_delimiters(token), ".", false, false);
}

static bool opcode_from_data_prefix(opcode& out_code,
    const std::string& prefix, const data_chunk& push_data) NOEXCEPT
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
    const std::string& token) NOEXCEPT
{
    // Deserialization to a number can convert random text to zero.
    if (!is_ascii_numeric(token))
        return false;

    int64_t value;
    if (!deserialize(value, token))
        return false;

    out_data = machine::number::chunk::from_integer(value);
    return true;
}

// private/static
operation operation::from_string(const std::string& mnemonic) NOEXCEPT
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

        if (parts.size() == one)
        {
            // Extract operation using nominal data size decoding.
            if ((valid = decode_base16(chunk, parts.front())))
                code = nominal_opcode_from_data(chunk);
        }
        else if (parts.size() == two)
        {
            // Extract operation using explicit data size decoding.

            // More efficient [] dereference is guarded above.
            BC_PUSH_WARNING(NO_ARRAY_INDEXING)
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

data_chunk operation::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size());

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream);
    return data;
}

void operation::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void operation::to_data(writer& sink) const NOEXCEPT
{
    // Underflow is op-undersized data, it is serialized with no opcode.
    // An underflow could only be a final token in a script deserialization.
    if (is_underflow())
    {
        sink.write_bytes(get_data());
    }
    else
    {
        const auto size = data_size();
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

        sink.write_bytes(get_data());
    }
}

// To String.
// ----------------------------------------------------------------------------

static std::string opcode_to_prefix(opcode code,
    const data_chunk& data) NOEXCEPT
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

std::string operation::to_string(uint32_t active_flags) const NOEXCEPT
{
    if (!is_valid())
        return "(?)";

    if (underflow_)
        return "<" + encode_base16(get_data()) + ">";

    if (data_empty())
        return opcode_to_mnemonic(code_, active_flags);

    // Data encoding uses single token with explicit size prefix as required.
    return "[" + opcode_to_prefix(code_, get_data()) +
        encode_base16(get_data()) + "]";
}

// Properties.
// ----------------------------------------------------------------------------

bool operation::is_valid() const NOEXCEPT
{
    // Push data not possible with any is_invalid, combination is invalid.
    // This is necessary because there can be no invalid sentinel value.
    return !(code_ == any_invalid && !underflow_ && !data_empty());
}

opcode operation::code() const NOEXCEPT
{
    return code_;
}

const data_chunk& operation::data() const NOEXCEPT
{
    return get_data();
}

const chunk_cptr& operation::data_ptr() const NOEXCEPT
{
    return get_data_cptr();
}

size_t operation::serialized_size() const NOEXCEPT
{
    static constexpr auto op_size = sizeof(uint8_t);
    const auto size = data_size();

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
bool operation::count_op(reader& source) NOEXCEPT
{
    if (source.is_exhausted())
        return false;

    const auto code = static_cast<opcode>(source.read_byte());
    source.skip_bytes(read_data_size(code, source));
    return true;
}

// static/private
uint32_t operation::read_data_size(opcode code, reader& source) NOEXCEPT
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

// Categories of operations.
// ----------------------------------------------------------------------------

bool operation::is_invalid() const NOEXCEPT
{
    return is_invalid(code_);
}

bool operation::is_push() const NOEXCEPT
{
    return is_push(code_);
}

bool operation::is_payload() const NOEXCEPT
{
    return is_payload(code_);
}

bool operation::is_counted() const NOEXCEPT
{
    return is_counted(code_);
}

bool operation::is_version() const NOEXCEPT
{
    return is_version(code_);
}

bool operation::is_numeric() const NOEXCEPT
{
    return is_numeric(code_);
}

bool operation::is_positive() const NOEXCEPT
{
    return is_positive(code_);
}

bool operation::is_reserved() const NOEXCEPT
{
    return is_reserved(code_);
}

bool operation::is_conditional() const NOEXCEPT
{
    return is_conditional(code_);
}

bool operation::is_relaxed_push() const NOEXCEPT
{
    return is_relaxed_push(code_);
}

bool operation::is_minimal_push() const NOEXCEPT
{
    return code_ == minimal_opcode_from_data(get_data());
}

bool operation::is_nominal_push() const NOEXCEPT
{
    return code_ == nominal_opcode_from_data(get_data());
}

bool operation::is_oversized() const NOEXCEPT
{
    // Rule max_push_data_size imposed by [0.3.6] soft fork.
    return data_size() > max_push_data_size;
}

bool operation::is_underclaimed() const NOEXCEPT
{
    return data_size() > operation::opcode_to_maximum_size(code_);
}

// ****************************************************************************
// CONSENSUS: An underflow is sized op-undersized data. This is valid as long
// as the operation is not executed. For example, coinbase input scripts.
// ****************************************************************************
bool operation::is_underflow() const NOEXCEPT
{
    return underflow_;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

operation tag_invoke(json::value_to_tag<operation>,
    const json::value& value) NOEXCEPT
{
    return operation{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const operation& operation) NOEXCEPT
{
    value = operation.to_string(flags::all_rules);
}

BC_POP_WARNING()

operation::cptr tag_invoke(json::value_to_tag<operation::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<operation>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const operation::cptr& operation) NOEXCEPT
{
    tag_invoke(tag, value, *operation);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
