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
#include <bitcoin/system/chain/script.hpp>

#include <algorithm>
#include <sstream>
#include <utility>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// static
// TODO: would be inlined but machine is a circular include.
//*****************************************************************************
// CONSENSUS: BIP34 requires coinbase input script to begin with one byte
// that indicates height size. This is inconsistent with an extreme future
// where the size byte overflows. However satoshi actually requires nominal
// encoding.
//*************************************************************************
bool script::is_coinbase_pattern(const operations& ops, size_t height) NOEXCEPT
{
    using namespace machine::number;
    return !ops.empty()
        && ops[0].is_nominal_push()
        && ops[0].data() == chunk::from_integer(to_unsigned(height));
}

// Constructors.
// ----------------------------------------------------------------------------

script::script() NOEXCEPT
  : script(operations{}, false, false, false, false, zero)
{
}

script::~script() NOEXCEPT
{
}

script::script(script&& other) NOEXCEPT
  : script(
      std::move(other.ops_),
      other.valid_,
      other.easier_,
      other.failer_,
      other.roller_,
      other.size_)
{
}

script::script(const script& other) NOEXCEPT
  : script(
      other.ops_,
      other.valid_,
      other.easier_,
      other.failer_,
      other.roller_,
      other.size_)
{
}

script::script(operations&& ops) NOEXCEPT
  : script(from_operations(std::move(ops)))
{
}

script::script(const operations& ops) NOEXCEPT
  : script(from_operations(ops))
{
}

script::script(stream::in::fast&& stream, bool prefix) NOEXCEPT
  : script(read::bytes::fast(stream), prefix)
{
}

script::script(stream::in::fast& stream, bool prefix) NOEXCEPT
  : script(read::bytes::fast(stream), prefix)
{
}

script::script(std::istream&& stream, bool prefix) NOEXCEPT
  : script(read::bytes::istream(stream), prefix)
{
}

script::script(std::istream& stream, bool prefix) NOEXCEPT
  : script(read::bytes::istream(stream), prefix)
{
}

script::script(reader&& source, bool prefix) NOEXCEPT
  : script(source, prefix)
{
}

script::script(reader& source, bool prefix) NOEXCEPT
  : ops_(source.get_arena())
{
    assign_data(source, prefix);
}

script::script(const std::string& mnemonic) NOEXCEPT
  : script(from_string(mnemonic))
{
}

// protected
script::script(const operations& ops, bool valid, bool easier, bool failer,
    bool roller, size_t size) NOEXCEPT
  : ops_(ops),
    valid_(valid),
    easier_(easier),
    failer_(failer),
    roller_(roller),
    size_(size),
    offset(ops_.begin())
{
}

// Operators.
// ----------------------------------------------------------------------------

script& script::operator=(script&& other) NOEXCEPT
{
    ops_ = std::move(other.ops_);
    valid_ = other.valid_;
    easier_ = other.easier_;
    failer_ = other.failer_;
    roller_ = other.roller_;
    size_ = other.size_;
    offset = ops_.begin();
    return *this;
}

script& script::operator=(const script& other) NOEXCEPT
{
    ops_ = other.ops_;
    valid_ = other.valid_;
    easier_ = other.easier_;
    failer_ = other.failer_;
    roller_ = other.roller_;
    size_ = other.size_;
    offset = ops_.begin();
    return *this;
}

bool script::operator==(const script& other) const NOEXCEPT
{
    return size_ == other.size_
        && ops_ == other.ops_;
}

bool script::operator!=(const script& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
size_t script::op_count(reader& source) NOEXCEPT
{
    // Stream errors reset by set_position so trap here.
    if (!source)
        return zero;

    const auto start = source.get_read_position();
    auto count = zero;

    // This is expensive (1.1%) but far less than vector reallocs (11.6%).
    while (operation::count_op(source))
        ++count;

    source.set_position(start);
    return count;
}

// static/private
script script::from_operations(operations&& ops) NOEXCEPT
{
    constexpr auto valid = true;
    auto easier = false;
    auto failer = false;
    auto roller = false;

    for (const auto& op: ops)
    {
        easier |= op.is_success();
        failer |= op.is_invalid();
        roller |= op.is_roller();
    }

    const auto size = serialized_size(ops);
    return { std::move(ops), valid, easier, failer, roller, size };
}

// static/private
script script::from_operations(const operations& ops) NOEXCEPT
{
    constexpr auto valid = true;
    auto easier = false;
    auto failer = false;
    auto roller = false;

    for (const auto& op : ops)
    {
        easier |= op.is_success();
        failer |= op.is_invalid();
        roller |= op.is_roller();
    }

    const auto size = serialized_size(ops);
    return { ops, valid, easier, failer, roller, size };
}

// private
void script::assign_data(reader& source, bool prefix) NOEXCEPT
{
    easier_ = false;
    failer_ = false;
    roller_ = false;
    size_t expected{};

    if (prefix)
    {
        expected = source.read_size();
        source.set_limit(expected);
    }

    ops_.reserve(op_count(source));
    const auto start = source.get_read_position();

    while (!source.is_exhausted())
    {
        ops_.emplace_back(source);
        const auto& op = ops_.back();
        easier_ |= op.is_success();
        failer_ |= op.is_invalid();
        roller_ |= op.is_roller();
    }

    size_ = source.get_read_position() - start;

    if (prefix)
    {
        source.set_limit();
        if (size_ != expected)
            source.invalidate();
    }

    valid_ = source;
    offset = ops_.begin();
}

// static/private
script script::from_string(const std::string& mnemonic) NOEXCEPT
{
    constexpr auto valid = true;
    auto easier = false;
    auto failer = false;
    auto roller = false;

    // There is always one operation per non-empty string token.
    auto tokens = split(mnemonic);

    // Split always returns at least one token, and when trimming it will be
    // empty only if there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        tokens.clear();

    operations ops{};
    ops.reserve(tokens.size());

    // Create an op list from the split tokens.
    for (const auto& token: tokens)
    {
        ops.emplace_back(token);
        const auto& op = ops.back();
        easier |= op.is_success();
        failer |= op.is_invalid();
        roller |= op.is_roller();

        // This is a deserialization failure, not just an invalid code.
        if (!ops.back().is_valid())
            return {};
    }

    const auto size = serialized_size(ops);
    return { std::move(ops), valid, easier, failer, roller, size };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk script::to_data(bool prefix) const NOEXCEPT
{
    data_chunk data(serialized_size(prefix));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, prefix);
    return data;
}

void script::to_data(std::ostream& stream, bool prefix) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, prefix);
}

// see also: subscript.to_data().
void script::to_data(writer& sink, bool prefix) const NOEXCEPT
{
    if (prefix)
        sink.write_variable(serialized_size(false));

    // Data serialization is affected by offset metadata.
    for (iterator op{ offset }; op != ops().end(); ++op)
        op->to_data(sink);
}

std::string script::to_string(uint32_t active_flags) const NOEXCEPT
{
    auto first = true;
    std::ostringstream text;

    // Throwing stream aborts.
    for (const auto& op: ops())
    {
        text << (first ? "" : " ") << op.to_string(active_flags);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

void script::clear_offset() const NOEXCEPT
{
    offset = ops_.begin();
}

// Properties.
// ----------------------------------------------------------------------------

bool script::is_valid() const NOEXCEPT
{
    // Any byte vector is a valid script.
    // This is false only if the byte count did not match the size prefix.
    return valid_;
}

bool script::is_roller() const NOEXCEPT
{
    return roller_;
};

bool script::is_prefail() const NOEXCEPT
{
    // Script contains an invalid opcode and will fail evaluation.
    return failer_;
}

bool script::is_prevalid() const NOEXCEPT
{
    // Script contains a success opcode and will pass evaluation (tapscript).
    return easier_;
}

bool script::is_underflow() const NOEXCEPT
{
    // Prefail implies an invalid code and a non-empty op stack.
    return is_prefail() && ops_.back().is_underflow();
}

bool script::is_oversized() const NOEXCEPT
{
    return serialized_size(false) > max_script_size;
}

// An unspendable script is any that can provably not be spent under any
// circumstance. This allows for exclusion of the output as unspendable.
// The criteria below are not comprehensive but are fast to evaluate.
bool script::is_unspendable() const NOEXCEPT
{
    if (ops_.empty())
        return false;

    const auto& code = ops_.front().code();

    // There is no condition prior to the first opcode in a script, so
    // is_reserved must be checked. is_invalid short-circuits evaluation for
    // scripts that fail to parse, but would otherwise be caught in evaluation.
    return operation::is_reserved(code) || operation::is_invalid(code);
}

const operations& script::ops() const NOEXCEPT
{
    return ops_;
}

// Consensus (witness::extract_script) and Electrum server payments key.
hash_digest script::hash() const NOEXCEPT
{
    hash_digest hash{};
    stream::out::fast stream{ hash };
    hash::sha256::fast sink(stream);
    to_data(sink, false);
    sink.flush();
    return hash;
}

// static/private
size_t script::serialized_size(const operations& ops) NOEXCEPT
{
    return std::accumulate(ops.begin(), ops.end(), zero, op_size);
}

size_t script::serialized_size(bool prefix) const NOEXCEPT
{
    // Recompute it serialization has been affected by offset metadata.
    const auto size = (offset == ops_.begin()) ? size_ :
        std::accumulate(offset, ops_.end(), zero, op_size);

    return prefix ? ceilinged_add(size, variable_size(size)) : size;
}

BC_POP_WARNING()
BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

script tag_invoke(json::value_to_tag<script>,
    const json::value& value) NOEXCEPT
{
    return script{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const script& script) NOEXCEPT
{
    value = script.to_string(flags::all_rules);
}

BC_POP_WARNING()

script::cptr tag_invoke(json::value_to_tag<script::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<script>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const script::cptr& script) NOEXCEPT
{
    tag_invoke(tag, value, *script);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
