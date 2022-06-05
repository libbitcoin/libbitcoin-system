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

// Constructors.
// ----------------------------------------------------------------------------

script::script() noexcept
  : script(operations{}, false)
{
}

script::~script() noexcept
{
}

script::script(script&& other) noexcept
  : script(std::move(other.ops_), other.valid_)
{
}

script::script(const script& other) noexcept
  : script(other.ops_, other.valid_)
{
}

script::script(operations&& ops) noexcept
  : script(std::move(ops), true)
{
}

script::script(const operations& ops) noexcept
  : script(ops, true)
{
}

script::script(const data_slice& data, bool prefix) noexcept
  : script(stream::in::copy(data), prefix)
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
script::script(operations&& ops, bool valid) noexcept
  : ops_(std::move(ops)), valid_(valid), offset(ops_.begin())
{
}

// protected
script::script(const operations& ops, bool valid) noexcept
  : ops_(ops), valid_(valid), offset(ops_.begin())
{
}

// Operators.
// ----------------------------------------------------------------------------

script& script::operator=(script&& other) noexcept
{
    ops_ = std::move(other.ops_);
    valid_ = other.valid_;
    offset = ops_.begin();
    return *this;
}

script& script::operator=(const script& other) noexcept
{
    ops_ = other.ops_;
    valid_ = other.valid_;
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
        ops.emplace_back(source);

    if (prefix)
    {
        // Remove the stream limit.
        source.set_limit();

        // Stream was exhausted prior to reaching prefix size.
        if (source.get_position() - start != size)
            source.invalidate();
    }

    return { std::move(ops), source };
}

// static/private
script script::from_string(const std::string& mnemonic) noexcept
{
    // There is always one operation per non-empty string token.
    auto tokens = split(mnemonic);

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
        if (!ops.back().is_valid())
            return {};
    }

    return { std::move(ops) };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk script::to_data(bool prefix) const noexcept
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(prefix));
    stream::out::copy ostream(data);
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

    // Data serialization is affected by offset metadata.
    ////for (const auto& op: ops())
    for (auto op = offset; op != stop; ++op)
        op->to_data(sink);
}

std::string script::to_string(uint32_t active_forks) const noexcept
{
    auto first = true;
    std::ostringstream text;

    for (const auto& op: ops())
    {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

// Properties.
// ----------------------------------------------------------------------------

bool script::is_valid() const noexcept
{
    // Any byte vector is a valid script.
    // This is false only if the byte count did not match the size prefix.
    return valid_;
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

// Utilities (static).
// ----------------------------------------------------------------------------

bool script::is_push_only(const operations& ops) noexcept
{
    const auto push = [](const operation& op) noexcept
    {
        return op.is_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_relaxed_push(const operations& ops) noexcept
{
    const auto push = [&](const operation& op) noexcept
    {
        return op.is_relaxed_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: BIP34 requires coinbase input script to begin with one byte that
// indicates the height size. This is inconsistent with an extreme future where
// the size byte overflows. However satoshi actually requires nominal encoding.
//*****************************************************************************
bool script::is_coinbase_pattern(const operations& ops, size_t height) noexcept
{
    return !ops.empty()
        && ops[0].is_nominal_push()
        && ops[0].data() == number(height).data();
}

////// This is slightly more efficient because the script does not get parsed,
////// but the static template implementation is more self-explanatory.
////bool script::is_coinbase_pattern(size_t height) const
////{
////    const auto actual = to_data(false);
////
////    // Create the expected script as a non-minimal byte vector.
////    script compare(operations{ { number(height).data(), false } });
////    const auto expected = compare.to_data(false);
////
////    // Require the actual script start with the expected coinbase script.
////    return std::equal(expected.begin(), expected.end(), actual.begin());
////}

//*****************************************************************************
// CONSENSUS: this pattern is used to commit to bip141 witness data.
//*****************************************************************************
bool script::is_commitment_pattern(const operations& ops) noexcept
{
    static const auto header = to_big_endian(witness_head);

    // C++14: remove && ops[1].data().size() >= header.size() guard.
    // Bytes after commitment are optional with no consensus meaning (bip141).
    // Commitment is not executable so invalid trailing operations are allowed.
    return ops.size() > 1
        && ops[0].code() == opcode::op_return
        && ops[1].code() == opcode::push_size_36
        && ops[1].data().size() >= header.size()
        && std::equal(header.begin(), header.end(), ops[1].data().begin());
}

//*****************************************************************************
// CONSENSUS: this pattern is used in bip141 validation rules.
//*****************************************************************************
bool script::is_witness_program_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].data().size() >= min_witness_program
        && ops[1].data().size() <= max_witness_program;
}

// The satoshi client tests for 83 bytes total. This allows for the waste of
// one byte to represent up to 75 bytes using the push_one_size opcode.
// It also allows any number of push ops and limits it to 0 value and 1 per tx.
////bool script::is_pay_null_data_pattern(const operations& ops)
////{
////    static constexpr auto op_76 = static_cast<uint8_t>(opcode::push_one_size);
////
////    return ops.size() >= 2
////        && ops[0].code() == opcode::return_
////        && static_cast<uint8_t>(ops[1].code()) <= op_76
////        && ops[1].data().size() <= max_null_data_size;
////}

// Used by neutrino.
bool script::is_pay_op_return_pattern(const operations& ops) noexcept
{
    return !ops.empty()
        && ops[0].code() == opcode::op_return;
}

// The satoshi client enables configurable data size for policy.
bool script::is_pay_null_data_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && ops[0].code() == opcode::op_return
        && ops[1].is_minimal_push()
        && ops[1].data().size() <= max_null_data_size;
}

// TODO: expand this to the 20 signature op_check_multisig limit.
// The current 16 (or 20) limit does not affect server indexing because bare
// multisig is not indexable and p2sh multisig is byte-limited to 15 sigs.
// The satoshi client policy limit is 3 signatures for bare multisig.
bool script::is_pay_multisig_pattern(const operations& ops) noexcept
{
    static constexpr auto op_1 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_16 = static_cast<uint8_t>(opcode::push_positive_16);

    const auto op_count = ops.size();

    if (op_count < 4 || ops[op_count - 1].code() != opcode::checkmultisig)
        return false;

    const auto op_m = static_cast<uint8_t>(ops[0].code());
    const auto op_n = static_cast<uint8_t>(ops[op_count - 2].code());

    if (op_m < op_1 || op_m > op_n || op_n < op_1 || op_n > op_16)
        return false;

    const auto number = op_n - op_1 + 1u;
    const auto points = op_count - 3u;

    if (number != points)
        return false;

    for (auto op = std::next(ops.begin()); op != std::prev(ops.end(), 2); ++op)
        if (!is_public_key(op->data()))
            return false;

    return true;
}

// The satoshi client considers this non-standard for policy.
bool script::is_pay_public_key_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && is_public_key(ops[0].data())
        && ops[1].code() == opcode::checksig;
}

bool script::is_pay_key_hash_pattern(const operations& ops) noexcept
{
    return ops.size() == 5
        && ops[0].code() == opcode::dup
        && ops[1].code() == opcode::hash160
        && ops[2].data().size() == short_hash_size
        && ops[3].code() == opcode::equalverify
        && ops[4].code() == opcode::checksig;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_pay_script_hash_pattern(const operations& ops) noexcept
{
    return ops.size() == 3
        && ops[0].code() == opcode::hash160
        && ops[1].code() == opcode::push_size_20
        && ops[2].code() == opcode::equal;
}

bool script::is_pay_witness_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].is_push();
}

bool script::is_pay_witness_key_hash_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_20;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_pay_witness_script_hash_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_32;
}

// The first push is based on wacky satoshi op_check_multisig behavior that
// we must perpetuate, though it's appearance here is policy not consensus.
// Limiting to push_size_0 eliminates pattern ambiguity with little downside.
bool script::is_sign_multisig_pattern(const operations& ops) noexcept
{
    const auto endorsement = [](const operation& op) noexcept
    {
        return is_endorsement(op.data());
    };

    return ops.size() >= 2
        && ops[0].code() == opcode::push_size_0
        && std::all_of(std::next(ops.begin()), ops.end(), endorsement);
}

bool script::is_sign_public_key_pattern(const operations& ops) noexcept
{
    return ops.size() == 1
        && is_endorsement(ops[0].data());
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_sign_key_hash_pattern(const operations& ops) noexcept
{
    return ops.size() == 2
        && is_endorsement(ops[0].data())
        && is_public_key(ops[1].data());
}

// Ambiguous with is_sign_key_hash when second/last op is a public key.
// Ambiguous with is_sign_public_key_pattern when only op is an endorsement.
bool script::is_sign_script_hash_pattern(const operations& ops) noexcept
{
    return !ops.empty()
        && is_push_only(ops)
        && !ops.back().data().empty();
}

operations script::to_pay_null_data_pattern(const data_slice& data) noexcept
{
    if (data.size() > max_null_data_size)
        return {};

    return operations
    {
        { opcode::op_return },
        { to_chunk(data), false }
    };
}

operations script::to_pay_public_key_pattern(const data_slice& point) noexcept
{
    if (!is_public_key(point))
        return {};

    return operations
    {
        { to_chunk(point), false },
        { opcode::checksig }
    };
}

operations script::to_pay_key_hash_pattern(const short_hash& hash) noexcept
{
    return operations
    {
        { opcode::dup },
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

operations script::to_pay_script_hash_pattern(const short_hash& hash) noexcept
{
    return operations
    {
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equal }
    };
}

// TODO: limit to 20 for consistency with op_check_multisig.
operations script::to_pay_multisig_pattern(uint8_t signatures,
    const compressed_list& points) noexcept
{
    return to_pay_multisig_pattern(signatures,
        to_stack<ec_compressed_size>(points));
}

// TODO: expand this to a 20 signature limit.
// This supports up to 16 signatures, however check_multisig is limited to 20.
// The embedded script is limited to 520 bytes, an effective limit of 15 for
// p2sh multisig, which can be as low as 7 when using all uncompressed keys.
operations script::to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points) noexcept
{
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    static constexpr auto base = op_81 - 1;
    static constexpr auto max = op_96 - base;

    const auto m = signatures;
    const auto n = points.size();

    if (m < 1 || m > n || n < 1 || n > max)
        return operations();

    const auto op_m = static_cast<opcode>(m + base);
    const auto op_n = static_cast<opcode>(points.size() + base);

    operations ops;
    ops.reserve(points.size() + 3);
    ops.emplace_back(op_m);

    for (const auto& point: points)
    {
        if (!is_public_key(point))
            return {};

        ops.emplace_back(point, false);
    }

    ops.emplace_back(op_n);
    ops.emplace_back(opcode::checkmultisig);
    return ops;
}

operations script::to_pay_witness_pattern(uint8_t version,
    const data_slice& data) noexcept
{
    return
    {
        { operation::opcode_from_version(version) },
        { to_chunk(data), false },
    };
}

operations script::to_pay_witness_key_hash_pattern(
    const short_hash& hash) noexcept
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false },
    };
}

operations script::to_pay_witness_script_hash_pattern(
    const hash_digest& hash) noexcept
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false }
    };
}

// Utilities (non-static).
// ----------------------------------------------------------------------------

const data_chunk& script::witness_program() const noexcept
{
    static const data_chunk empty;
    return is_witness_program_pattern(ops()) ? ops()[1].data() : empty;
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
inline size_t multisig_sigops(bool accurate, opcode code) noexcept
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

    // There is no condition prior to the first opcode in a script.
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
