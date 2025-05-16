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
#include <bitcoin/system/chain/witness.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

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
  : witness(stack, true, serialized_size(stack, false))
{
}

witness::witness(stream::in::fast&& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::fast(stream), prefix)
{
}

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
  : stack_(std::move(stack)), valid_(valid),
    size_(serialized_size(stack_, false))
{
}

// protected
witness::witness(const chunk_cptrs& stack, bool valid) NOEXCEPT
  : stack_(stack), valid_(valid), size_(serialized_size(stack_, false))
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

static inline size_t element_size(const chunk_cptr& element) NOEXCEPT
{
    // Each witness is prefixed with number of elements [bip144].
    const auto size = element->size();
    return ceilinged_add(variable_size(size), size);
};

// static
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
    auto& allocator = source.get_allocator();

    const auto push_witness = [&allocator, &source, this]() NOEXCEPT
    {
        // If read_bytes_raw returns nullptr invalid source is implied.
        const auto size = source.read_size(max_block_weight);
        const auto bytes = source.read_bytes_raw(size);
        if (is_null(bytes))
            return false;

        stack_.emplace_back(POINTER(data_chunk, allocator, bytes));
        size_ = ceilinged_add(size_, element_size(stack_.back()));
        return true;
    };

    if (prefix)
    {
        const auto count = source.read_size(max_block_weight);
        stack_.reserve(count);

        for (size_t element = 0; element < count; ++element)
            if (!push_witness())
                break;
    }
    else
    {
        while (!source.is_exhausted())
            if (!push_witness())
                break;
    }

    valid_ = source;
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk witness::to_data(bool prefix) const NOEXCEPT
{
    data_chunk data(serialized_size(prefix));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, prefix);
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

    // Tokens encoded as variable integer prefixed byte array [bip144].
    for (const auto& element: stack_)
    {
        sink.write_variable(element->size());
        sink.write_bytes(*element);
    }
}

// Text.
// ----------------------------------------------------------------------------

static inline bool is_push_token(const std::string& token) NOEXCEPT
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

const annex& witness::annex() const NOEXCEPT
{
    return annex_;
}

const chunk_cptrs& witness::stack() const NOEXCEPT
{
    return stack_;
}

// static
size_t witness::serialized_size(const chunk_cptrs& stack, bool prefix) NOEXCEPT
{
    const auto size = std::accumulate(stack.begin(), stack.end(), zero,
        [](size_t total, const chunk_cptr& element) NOEXCEPT
        {
            return ceilinged_add(total, element_size(element));
        });

    return prefix ? ceilinged_add(variable_size(stack.size()), size) : size;
}

size_t witness::serialized_size(bool prefix) const NOEXCEPT
{
    // Witness prefix is an element count, not byte length (unlike script).
    // An empty stack is not a valid witnessed tx (no inputs) but a consistent
    // serialization is used independently by database so zero stack allowed.
    return prefix ? ceilinged_add(variable_size(stack_.size()), size_) : size_;
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
