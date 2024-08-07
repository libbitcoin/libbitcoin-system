/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/witness.hpp>

#include <algorithm>
#include <istream>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

using namespace system::machine;
static const auto checksig_script = script{ { opcode::checksig } };

// Constructors.
// ----------------------------------------------------------------------------

witness::witness() NOEXCEPT
  : witness(chunk_cptrs{}, false, zero)
{
}

witness::witness(data_stack&& stack) NOEXCEPT
  : witness(*to_shareds(std::move(stack)), true)
{
    // stack moved so cannot pass serialized_size(stack), order not guaranteed.
}

witness::witness(const data_stack& stack) NOEXCEPT
  : witness(*to_shareds(stack), true)
{
}

witness::witness(chunk_cptrs&& stack) NOEXCEPT
  : witness(std::move(stack), true)
{
    // stack moved so cannot pass serialized_size(stack), order not guaranteed.
}

witness::witness(const chunk_cptrs& stack) NOEXCEPT
  : witness(stack, true, serialized_size(stack))
{
}

witness::witness(const data_slice& data, bool prefix) NOEXCEPT
  : witness(stream::in::copy(data), prefix)
{
}

////witness::witness(stream::in::fast&& stream, bool prefix) NOEXCEPT
////  : witness(read::bytes::fast(stream), prefix)
////{
////}

witness::witness(stream::in::fast& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::fast(stream), prefix)
{
}

witness::witness(std::istream&& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::istream(stream), prefix)
{
}

witness::witness(std::istream& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::istream(stream), prefix)
{
}

witness::witness(reader&& source, bool prefix) NOEXCEPT
  : witness(source, prefix)
{
}

witness::witness(reader& source, bool prefix) NOEXCEPT
  : stack_(source.get_arena())
{
    assign_data(source, prefix);
}

witness::witness(const std::string& mnemonic) NOEXCEPT
  : witness(from_string(mnemonic))
{
}

// protected
witness::witness(chunk_cptrs&& stack, bool valid) NOEXCEPT
  : stack_(std::move(stack)), valid_(valid), size_(serialized_size(stack_))
{
}

// protected
witness::witness(const chunk_cptrs& stack, bool valid) NOEXCEPT
  : stack_(stack), valid_(valid), size_(serialized_size(stack))
{
}

// protected
witness::witness(const chunk_cptrs& stack, bool valid, size_t size) NOEXCEPT
  : stack_(stack), valid_(valid), size_(size)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool witness::operator==(const witness& other) const NOEXCEPT
{
    return deep_equal(stack_, other.stack_);
}

bool witness::operator!=(const witness& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

void witness::skip(reader& source, bool prefix) NOEXCEPT
{
    if (prefix)
    {
        const auto count = source.read_size(max_block_weight);

        for (size_t element = 0; element < count; ++element)
            source.read_bytes(source.read_size(max_block_weight));
    }
    else
    {
        while (!source.is_exhausted())
            source.read_bytes(source.read_size(max_block_weight));
    }
}

// private
void witness::assign_data(reader& source, bool prefix) NOEXCEPT
{
    size_ = zero;
    const auto& allocator = source.get_allocator();

    if (prefix)
    {
        const auto count = source.read_size(max_block_weight);
        stack_.reserve(count);

        for (size_t element = 0; element < count; ++element)
        {
            const auto size = source.read_size(max_block_weight);
            stack_.emplace_back(source.read_bytes_raw(size),
                allocator.deleter<data_chunk>(source.get_arena()));
            size_ = element_size(size_, stack_.back());
        }
    }
    else
    {
        while (!source.is_exhausted())
        {
            const auto size = source.read_size(max_block_weight);
            stack_.emplace_back(source.read_bytes_raw(size),
                allocator.deleter<data_chunk>(source.get_arena()));
            size_ = element_size(size_, stack_.back());
        }
    }

    valid_ = source;
}

inline bool is_push_token(const std::string& token) NOEXCEPT
{
    return token.size() > one && token.front() == '[' && token.back() == ']';
}

inline std::string remove_token_delimiters(const std::string& token) NOEXCEPT
{
    BC_ASSERT(token.size() > one);
    return std::string(std::next(token.begin()), std::prev(token.end()));
}

// static/private
witness witness::from_string(const std::string& mnemonic) NOEXCEPT
{
    // There is always one data element per non-empty string token.
    auto tokens = split(mnemonic);

    // Split always returns at least one token, and when trimming it will be
    // empty only if there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        tokens.clear();

    data_stack stack(tokens.size());
    auto data = stack.begin();

    // Create data stack from the split tokens.
    for (const auto& token: tokens)
    {
        if (!is_push_token(token) ||
            !decode_base16(*data++, remove_token_delimiters(token)))
            return {};
    }

    return { std::move(stack) };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk witness::to_data(bool prefix) const NOEXCEPT
{
    data_chunk data(serialized_size(prefix));
    stream::out::copy ostream(data);
    to_data(ostream, prefix);
    return data;
}

void witness::to_data(std::ostream& stream, bool prefix) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, prefix);
}

void witness::to_data(writer& sink, bool prefix) const NOEXCEPT
{
    // Witness prefix is an element count, not byte length (unlike script).
    if (prefix)
        sink.write_variable(stack_.size());

    // Tokens encoded as variable integer prefixed byte array (bip144).
    for (const auto& element: stack_)
    {
        sink.write_variable(element->size());
        sink.write_bytes(*element);
    }
}

std::string witness::to_string() const NOEXCEPT
{
    if (!valid_)
        return "(?)";

    std::string text;
    for (const auto& element: stack_)
        text += "[" + encode_base16(*element) + "] ";

    trim_right(text);
    return text;
}

// Properties.
// ----------------------------------------------------------------------------

bool witness::is_valid() const NOEXCEPT
{
    return valid_;
}

const chunk_cptrs& witness::stack() const NOEXCEPT
{
    return stack_;
}

// static/private
size_t witness::serialized_size(const chunk_cptrs& stack) NOEXCEPT
{
    return std::accumulate(stack.begin(), stack.end(), zero, element_size);
}

size_t witness::serialized_size(bool prefix) const NOEXCEPT
{
    // Witness prefix is an element count, not byte length (unlike script).
    // An empty stack is not a valid witnessed tx (no inputs) but a consistent
    // serialization is used independently by database so zero stack allowed.
    return prefix ? ceilinged_add(size_, variable_size(stack_.size())) : size_;
}

// Utilities.
// ----------------------------------------------------------------------------

// This is an internal optimization over using script::to_pay_key_hash_pattern.
inline operations to_pay_key_hash(data_chunk&& program) NOEXCEPT
{
    BC_ASSERT(program.size() == short_hash_size);

    return operations
    {
        { opcode::dup },
        { opcode::hash160 },
        { std::move(program), true },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

// out_script is only useful only for sigop counting.
// program_script is only used to determine witness_program type.
bool witness::extract_sigop_script(script& out_script,
    const script& program_script) const NOEXCEPT
{
    out_script = {};
    switch (program_script.version())
    {
        case script_version::zero:
        {
            switch (program_script.witness_program().size())
            {
                // Each p2wkh input is counted as 1 sigop (bip141).
                case short_hash_size:
                    out_script = checksig_script;
                    return true;

                // p2wsh sigops are counted as before for p2sh (bip141).
                case hash_size:
                    if (!stack_.empty())
                        out_script = { *stack_.back(), false };

                    return true;

                // Undefined v0 witness script, will not validate.
                default:
                    return true;
            }
        }

        // These versions are reserved for future extensions (bip141).
        case script_version::reserved:
            return true;

        // Return false only if this is not a witness script.
        case script_version::unversioned:
        default:
            return false;
    }
}

// Extract script and initial execution stack.
bool witness::extract_script(script::cptr& out_script,
    chunk_cptrs_ptr& out_stack, const script& program_script) const NOEXCEPT
{
    // Copy stack of shared const pointers for use as mutable witness stack.
    out_stack = std::make_shared<chunk_cptrs>(stack_);
    data_chunk program{ program_script.witness_program() };

    switch (program_script.version())
    {
        case script_version::zero:
        {
            switch (program.size())
            {
                // p2wkh
                // witness stack : <signature> <public-key>
                // input script  : (empty)
                // output script : <0> <20-byte-hash-of-public-key>
                case short_hash_size:
                {
                    // Create a pay-to-key-hash input script from the program.
                    // The hash160 of public key must match program (bip141).
                    out_script = to_shared<script>(to_pay_key_hash(
                        std::move(program)));

                    // Stack must be 2 elements (bip141).
                    return out_stack->size() == two;
                }

                // p2wsh
                // witness stack : <script> [stack-elements]
                // input script  : (empty)
                // output script : <0> <32-byte-hash-of-script>
                case hash_size:
                {
                    // The stack must consist of at least 1 element (bip141).
                    if (out_stack->empty())
                        return false;

                    // Input script is popped from the stack (bip141).
                    out_script = to_shared<script>(*pop(*out_stack), false);

                    // The sha256 of popped script must match program (bip141).
                    return std::equal(program.begin(), program.end(),
                        out_script->hash().begin());
                }

                // The witness extraction is invalid for v0.
                default:
                    return false;
            }
        }

        // These versions are reserved for future extensions (bip141).
        case script_version::reserved:
            return true;

        // The witness version is undefined.
        case script_version::unversioned:
        default:
            return false;
    }
}

BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

witness tag_invoke(json::value_to_tag<witness>,
    const json::value& value) NOEXCEPT
{
    return witness{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const witness& witness) NOEXCEPT
{
    value = witness.to_string();
}

BC_POP_WARNING()

witness::cptr tag_invoke(json::value_to_tag<witness::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<witness>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const witness::cptr& output) NOEXCEPT
{
    tag_invoke(tag, value, *output);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
