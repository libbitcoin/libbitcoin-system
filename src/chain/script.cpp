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
#include <bitcoin/system/chain/script.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <utility>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace bc::system::machine;

// static (should be inline or constexpr).
// TODO: Avoiding circular include on machine.
bool script::is_coinbase_pattern(const operations& ops,
    size_t height) noexcept
{
    // TODO: number::chunk::from_int constexpr?
    return !ops.empty()
        && ops[0].is_nominal_push()
        && ops[0].data() == number::chunk::from_integer(to_unsigned(height));
}

// Constructors.
// ----------------------------------------------------------------------------

script::script() noexcept
  : script(operations{}, false, false)
{
}

script::~script() noexcept
{
}

script::script(script&& other) noexcept
  : script(std::move(other.ops_), other.valid_, other.prefail_)
{
}

script::script(const script& other) noexcept
  : script(other.ops_, other.valid_, other.prefail_)
{
}

// Prefail is false.
script::script(operations&& ops) noexcept
  : script(std::move(ops), true, false)
{
}

// Prefail is false.
script::script(const operations& ops) noexcept
  : script(ops, true, false)
{
}

script::script(operations&& ops, bool prefail) noexcept
    : script(std::move(ops), true, prefail)
{
}

script::script(const operations& ops, bool prefail) noexcept
    : script(ops, true, prefail)
{
}

script::script(const data_slice& data, bool prefix) noexcept
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : script(stream::in::copy(data), prefix)
    BC_POP_WARNING()
{
}

script::script(std::istream&& stream, bool prefix) noexcept
  : script(read::bytes::istream(stream), prefix)
{
}

script::script(std::istream& stream, bool prefix) noexcept
  : script(read::bytes::istream(stream), prefix)
{
}

script::script(reader&& source, bool prefix) noexcept
  : script(from_data(source, prefix))
{
}

script::script(reader& source, bool prefix) noexcept
  : script(from_data(source, prefix))
{
}

script::script(const std::string& mnemonic) noexcept
  : script(from_string(mnemonic))
{
}

// protected
script::script(operations&& ops, bool valid, bool prefail) noexcept
  : ops_(std::move(ops)), valid_(valid), prefail_(prefail), offset(ops_.begin())
{
}

// protected
script::script(const operations& ops, bool valid, bool prefail) noexcept
  : ops_(ops), valid_(valid), prefail_(prefail), offset(ops_.begin())
{
}

// Operators.
// ----------------------------------------------------------------------------

script& script::operator=(script&& other) noexcept
{
    ops_ = std::move(other.ops_);
    valid_ = other.valid_;
    prefail_ = other.prefail_;
    offset = ops_.begin();
    return *this;
}

script& script::operator=(const script& other) noexcept
{
    ops_ = other.ops_;
    valid_ = other.valid_;
    prefail_ = other.prefail_;
    offset = ops_.begin();
    return *this;
}

bool script::operator==(const script& other) const noexcept
{
    return (ops_ == other.ops_);
}

bool script::operator!=(const script& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
size_t script::op_count(reader& source) noexcept
{
    const auto start = source.get_position();
    auto count = zero;

    while (operation::count_op(source))
        ++count;

    // Stream errors ignored, caught in from_data.
    source.set_position(start);
    return count;
}

// static/private
script script::from_data(reader& source, bool prefix) noexcept
{
    auto size = zero;
    auto start = zero;
    auto prefail = false;

    if (prefix)
    {
        size = source.read_size();
        start = source.get_position();

        // Limit the number of bytes that ops may consume.
        source.set_limit(size);
    }

    operations ops;
    ops.reserve(op_count(source));

    while (!source.is_exhausted())
    {
        ops.emplace_back(source);
        prefail |= ops.back().is_invalid();
    }

    if (prefix)
    {
        // Remove the stream limit.
        source.set_limit();

        // Stream was exhausted prior to reaching prefix size.
        if (source.get_position() - start != size)
            source.invalidate();
    }

    return { std::move(ops), source, prefail };
}

// static/private
script script::from_string(const std::string& mnemonic) noexcept
{
    // There is always one operation per non-empty string token.
    auto tokens = split(mnemonic);
    auto prefail = false;

    // Split always returns at least one token, and when trimming it will be
    // empty only if there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        tokens.clear();

    operations ops;
    ops.reserve(tokens.size());

    // Create an op list from the split tokens.
    for (const auto& token: tokens)
    {
        ops.emplace_back(token);
        prefail |= ops.back().is_invalid();

        // This is a deserialization failure, not just an invalid code.
        if (!ops.back().is_valid())
            return {};
    }

    return { std::move(ops), prefail };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk script::to_data(bool prefix) const noexcept
{
    data_chunk data(serialized_size(prefix), no_fill_byte_allocator);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream, prefix);
    return data;
}

void script::to_data(std::ostream& stream, bool prefix) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out, prefix);
}

// see also: subscript.to_data().
void script::to_data(writer& sink, bool prefix) const noexcept
{
    if (prefix)
        sink.write_variable(serialized_size(false));

    const auto stop = ops().end();

    // The iterator must be copied.
    BC_PUSH_WARNING(USE_REFERENCE)

    // Data serialization is affected by offset metadata.
    for (auto op = offset; op != stop; ++op)
        op->to_data(sink);

    BC_POP_WARNING()
}

std::string script::to_string(uint32_t active_forks) const noexcept
{
    auto first = true;
    std::ostringstream text;

    // Throwing stream aborts.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

    for (const auto& op: ops())
    {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();

    BC_POP_WARNING()
}


// Properties.
// ----------------------------------------------------------------------------

bool script::is_valid() const noexcept
{
    // Any byte vector is a valid script.
    // This is false only if the byte count did not match the size prefix.
    return valid_;
}

bool script::is_prefail() const noexcept
{
    // The script contains an invalid opcode and will thus fail evaluation.
    return prefail_;
}

const operations& script::ops() const noexcept
{
    return ops_;
}

// Consensus (witness::extract_script) and Electrum server payments key.
hash_digest script::hash() const noexcept
{
    hash_digest sha256{};
    hash::sha256::copy sink(sha256);
    to_data(sink, false);
    sink.flush();
    return sha256;
}

size_t script::serialized_size(bool prefix) const noexcept
{
    const auto op_size = [](size_t total, const operation& op) noexcept
    {
        return total + op.serialized_size();
    };

    // Data serialization is affected by offset metadata.
    ////auto size = std::accumulate(ops_.begin(), ops_.end(), zero, op_size);
    auto size = std::accumulate(offset, ops_.end(), zero, op_size);

    if (prefix)
        size += variable_size(size);

    return size;
}

// Utilities.
// ----------------------------------------------------------------------------

const data_chunk& script::witness_program() const noexcept
{
    static const data_chunk empty;

    BC_PUSH_WARNING(USE_GSL_AT)
    return is_witness_program_pattern(ops()) ? ops()[1].data() : empty;
    BC_POP_WARNING()
}

script_version script::version() const noexcept
{
    if (!is_witness_program_pattern(ops()))
        return script_version::unversioned;

    switch (ops_.front().code())
    {
        case opcode::push_size_0:
            return script_version::zero;
        default:
            return script_version::reserved;
    }
}

// Caller should test for is_sign_script_hash_pattern when sign_key_hash result
// as it is possible for an input script to match both patterns.
script_pattern script::pattern() const noexcept
{
    const auto input = output_pattern();
    return input == script_pattern::non_standard ? input_pattern() : input;
}

// Output patterns are mutually and input unambiguous.
// The bip141 coinbase pattern is not tested here, must test independently.
script_pattern script::output_pattern() const noexcept
{
    if (is_pay_key_hash_pattern(ops()))
        return script_pattern::pay_key_hash;

    if (is_pay_script_hash_pattern(ops()))
        return script_pattern::pay_script_hash;

    if (is_pay_null_data_pattern(ops()))
        return script_pattern::pay_null_data;

    if (is_pay_public_key_pattern(ops()))
        return script_pattern::pay_public_key;

    // Limited to 16 signatures though op_check_multisig allows 20.
    if (is_pay_multisig_pattern(ops()))
        return script_pattern::pay_multisig;

    return script_pattern::non_standard;
}

// A sign_key_hash result always implies sign_script_hash as well.
// The bip34 coinbase pattern is not tested here, must test independently.
script_pattern script::input_pattern() const noexcept
{
    if (is_sign_key_hash_pattern(ops()))
        return script_pattern::sign_key_hash;

    // This must follow is_sign_key_hash_pattern for ambiguity comment to hold.
    if (is_sign_script_hash_pattern(ops()))
        return script_pattern::sign_script_hash;

    if (is_sign_public_key_pattern(ops()))
        return script_pattern::sign_public_key;

    if (is_sign_multisig_pattern(ops()))
        return script_pattern::sign_multisig;

    return script_pattern::non_standard;
}

bool script::is_pay_to_witness(uint32_t forks) const noexcept
{
    // This is an optimization over using script::pattern.
    return is_enabled(forks, forks::bip141_rule) &&
        is_witness_program_pattern(ops());
}

bool script::is_pay_to_script_hash(uint32_t forks) const noexcept
{
    // This is an optimization over using script::pattern.
    return is_enabled(forks, forks::bip16_rule) &&
        is_pay_script_hash_pattern(ops());
}

// Count 1..16 multisig accurately for embedded (bip16) and witness (bip141).
constexpr size_t multisig_sigops(bool accurate, opcode code) noexcept
{
    return accurate && operation::is_positive(code) ?
        operation::opcode_to_positive(code) : multisig_default_sigops;
}

constexpr bool is_single_sigop(opcode code) noexcept
{
    return code == opcode::checksig || code == opcode::checksigverify;
}

constexpr bool is_multiple_sigop(opcode code) noexcept
{
    return code == opcode::checkmultisig || code == opcode::checkmultisigverify;
}

size_t script::sigops(bool accurate) const noexcept
{
    auto total = zero;
    auto preceding = opcode::push_negative_1;

    for (const auto& op: ops())
    {
        const auto code = op.code();

        if (is_single_sigop(code))
            total = ceilinged_add(total, one);
        else if (is_multiple_sigop(code))
            total = ceilinged_add(total, multisig_sigops(accurate, preceding));

        preceding = code;
    }

    return total;
}

bool script::is_oversized() const noexcept
{
    return serialized_size(false) > max_script_size;
}

// An unspendable script is any that can provably not be spent under any
// circumstance. This allows for exclusion of the output as unspendable.
// The criteria below are not comprehensive but are fast to evaluate.
bool script::is_unspendable() const noexcept
{
    if (ops_.empty())
        return false;

    const auto& code = ops_.front().code();

    // There is no condition prior to the first opcode in a script, so
    // is_reserved must be checked. is_invalid short-circuits evaluation for
    // scripts that fail to parse, but would otherwise be caught in evaluation.
    return operation::is_reserved(code) || operation::is_invalid(code);
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

script tag_invoke(json::value_to_tag<script>,
    const json::value& value) noexcept
{
    return script{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const script& script) noexcept
{
    value = script.to_string(forks::all_rules);
}

BC_POP_WARNING()

script::cptr tag_invoke(json::value_to_tag<script::cptr>,
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<script>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const script::cptr& script) noexcept
{
    tag_invoke(tag, value, *script);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
