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
#include <bitcoin/system/machine/program.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/interpreter.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace system::chain;

constexpr auto unused_value = max_uint64;
constexpr auto unused_version = script_version::unversioned;

inline chunk_cptrs_ptr default_stack() noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<chunk_cptrs>();
    BC_POP_WARNING()
}

inline chunk_cptrs_ptr copy_stack(const chunk_cptrs& stack) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<chunk_cptrs>(chunk_cptrs{ stack });
    BC_POP_WARNING()
}

// Constructors.
// ----------------------------------------------------------------------------

// Input script run (default/empty stack).
program::program(const chain::transaction& tx, const input_iterator& input,
     uint32_t forks) noexcept
  : program(tx, input, (*input)->script_ptr(), forks, unused_value,
      unused_version, default_stack())
{
}

// Legacy p2sh or prevout script run (copied input stack - use first).
program::program(const program& other, const script::cptr& script) noexcept
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    primary_(copy_stack(*other.primary_))
{
}

// Legacy p2sh or prevout script run (moved input stack - use last).
program::program(program&& other, const script::cptr& script) noexcept
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    primary_(std::move(other.primary_))
{
}

// Witness script run (witness-initialized stack).
program::program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, script_version version,
    const chunk_cptrs_ptr& stack) noexcept
  : program(tx, input, script, forks, (*input)->prevout->value(), version,
      stack)
{
}

// protected
program::program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, uint64_t value,
    script_version version, const chunk_cptrs_ptr& stack) noexcept
  : transaction_(tx),
    input_(input),
    script_(script),
    forks_(forks),
    value_(value),
    version_(version),
    primary_(stack)
{
}

// Public.
// ----------------------------------------------------------------------------

bool program::is_valid() const noexcept
{
    // TODO: nops rule must first be enabled in tests and config.
    ////const auto nops_rule = is_enabled(forks::nops_rule);
    constexpr auto nops_rule = true;
    const auto bip141 = is_enabled(forks::bip141_rule);

    return
        // nops_rule establishes script size limit.
        (!nops_rule || !script_->is_oversized()) &&

        // bip_141 introduces an initialized stack, so must validate.
        (!bip141    || chain::witness::is_push_size(*primary_));
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// Moving a shared pointer to the stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
void program::push(chunk_cptr&& item) noexcept
BC_POP_WARNING()
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(std::move(item));
    BC_POP_WARNING()
}

void program::push(const chunk_cptr& item) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(item);
    BC_POP_WARNING()
}

void program::push_chunk(data_chunk&& item) noexcept
{
    // TODO: retain ownership.
    push(to_shared<data_chunk>(std::move(item)));
}

void program::push_bool(bool value) noexcept
{
    // TODO: variant - push(value).
    if (value)
        push_numeric(1);
    else
        push_chunk({});
}

void program::push_length(size_t value) noexcept
{
    // This is guarded by stack size and push data limits.
    BC_ASSERT_MSG(value <= max_int64, "integer overflow");

    push_numeric(possible_narrow_sign_cast<int64_t>(value));
}

void program::push_numeric(int64_t value) noexcept
{
    push_number(number{ value });
}

void program::push_number(const number& value) noexcept
{
    // TODO: change to int64_t variant and remove this overload.
    ////push(value.int64());
    push(to_shared<data_chunk>(value.data()));
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

chunk_cptr program::pop() noexcept
{
    // This must be guarded (is_relaxed_push is checked in transaction).
    BC_ASSERT(!is_empty());

    // This must be a pointer copy, as the pointer is about to be destroyed.
    const chunk_cptr value{ primary_->back() };
    drop();
    return value;
}

bool program::pop(chunk_cptrs& items, size_t count) noexcept
{
    if (size() < count)
        return false;

    items.reserve(count);
    for (size_t index = 0; index < count; ++index)
        items.push_back(pop());

    return true;
}

void program::drop() noexcept
{
    // This must be guarded (is_relaxed_push is checked in transaction).
    BC_ASSERT(!is_empty());

    primary_->pop_back();
}

bool program::pop_signed_four_bytes(int32_t& out_value) noexcept
{
    number value;
    if (!pop_number_four_bytes(value))
        return false;

    // number::to_int32 presumes 4 byte pop.
    out_value = value.to_int32();
    return true;
}

// True if popped value is valid post-pop stack index (precluded if size < 2).
bool program::pop_index_four_bytes(size_t& out_value) noexcept
{
    int32_t signed_value;
    if (!pop_signed_four_bytes(signed_value))
        return false;

    if (is_negative(signed_value))
        return false;

    out_value = sign_cast<size_t>(signed_value);
    return out_value < size();
}

// TODO: variant - change to pop int32_t.
bool program::pop_number_four_bytes(number& out_value) noexcept
{
    return !is_empty() && out_value.set_data(*pop(), max_number_size_four);
}

// TODO: variant - change to pop int64_t.
bool program::pop_number_five_bytes(number& out_value) noexcept
{
    return !is_empty() && out_value.set_data(*pop(), max_number_size_four);
}

// TODO: variant - change to pop int32_t.
bool program::pop_binary_four_bytes(number& left, number& right) noexcept
{
    // The right hand side operand is at the top of the stack.
    return pop_number_four_bytes(right) && pop_number_four_bytes(left);
}

// TODO: variant - change to pop int32_t.
bool program::pop_ternary_four_bytes(number& upper, number& lower,
    number& value) noexcept
{
    // The upper bound is at stack top, lower bound next, value next.
    return pop_number_four_bytes(upper) && pop_number_four_bytes(lower) &&
        pop_number_four_bytes(value);
}

// ****************************************************************************
// CONSENSUS:
// Note that elsewhere numeric opcodes are limited to
// operands in the range -2**31+1 to 2**31-1, however it is
// legal for opcodes to produce results exceeding that
// range. This limitation is implemented by CScriptNum's
// default 4-byte limit.
//
// If we kept to that limit we'd have a year 2038 problem,
// even though the nLockTime field in transactions
// themselves is uint32 which only becomes meaningless
// after the year 2106.
//
// Thus as a special case we tell CScriptNum to accept up
// to 5-byte bignums, which are good until 2**39-1, well
// beyond the 2**32-1 limit of the nLockTime field itself.
// ****************************************************************************
bool program::peek_top_unsigned_five_bytes(uint64_t& out_value) const noexcept
{
    number value;
    if (is_empty() || !value.set_data(*item(zero), max_number_size_five) ||
        value.is_negative())
        return false;

    // number::to_int40 presumes 5 byte pop.
    out_value = sign_cast<uint64_t>(value.to_int40());
    return true;
}

// Primary stack push/pop non-const functions (optimizations).
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

void program::swap(size_t left, size_t right) noexcept
{
    // This must be guarded.
    BC_ASSERT(std::max(left, right) < size());

    std::swap(
        *std::prev(primary_->end(), add1(left)),
        *std::prev(primary_->end(), add1(right)));
}

void program::erase(size_t index) noexcept
{
    // This must be guarded.
    BC_ASSERT(index < size());

    primary_->erase(std::prev(primary_->end(), add1(index)));
}

// Primary stack push/pop const functions.
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

const chunk_cptr& program::item(size_t index) const noexcept
{
    // This must be guarded.
    BC_ASSERT(index < size());

    return *std::prev(primary_->end(), add1(index));
}

size_t program::size() const noexcept
{
    return primary_->size();
}

bool program::is_empty() const noexcept
{
    return primary_->empty();
}

bool program::is_stack_overflow() const noexcept
{
    // bit.ly/2cowHlP
    // Addition is safe due to script size constraints.
    return size() + alternate_.size() > max_stack_size;
}

constexpr bool is_non_zero(uint8_t value) noexcept
{
    return value != numbers::number_0;
};

bool program::stack_to_bool(stack clean) const noexcept
{
    // Reversed byte order in this example (big-endian).
    // -                : false (empty stack)
    // []               : false (empty stack top)
    // [00 00 00 00 00] : false (+zero)
    // [80 00 00 00 00] : false (-zero)
    // [42 00 00 00 00] : true
    // [00 80 00 00 00] : true

    // This must be guarded.
    BC_ASSERT(!primary_->empty());

    if ((clean == stack::clean) && (primary_->size() != one))
        return false;

    const auto& top = *primary_->back();

    if (top.empty())
        return false;

    if (!number::is_sign_byte(top.back()))
        return true;

    // Given the assumption of proper encoding, the first non-sign byte of any
    // stack number is non-zero, however chunk may have any value, so test all.
    return std::any_of(std::next(top.begin()), std::prev(top.end()),
        is_non_zero);
}

// Alternate stack.
// ----------------------------------------------------------------------------

bool program::is_alternate_empty() const noexcept
{
    return alternate_.empty();
}

// Moving a shared pointer to the alternate stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
void program::push_alternate(chunk_cptr&& value) noexcept
BC_POP_WARNING()
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    alternate_.push_back(std::move(value));
    BC_POP_WARNING()
}

chunk_cptr program::pop_alternate() noexcept
{
    // This must be guarded.
    BC_ASSERT(!alternate_.empty());

    // This must be a pointer copy, as the pointer is about to be destroyed.
    const chunk_cptr value{ alternate_.back() };
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------
// Condition count additions are guarded by script size limit.

void program::begin_if(bool value) noexcept
{
    // Optimize is_succeess().
    negative_condition_count_ += (value ? 0 : 1);

    condition_.push_back(value);
}

// ****************************************************************************
// CONSENSUS: "You may have noticed the strange behavior of Bitcoin's ELSE
// statement. Bitcoin allows one to switch between true and false conditions
// several times. For example, the following script is valid and leaves the
// value 2 on the stack: 1 OP_IF OP_ELSE OP_ELSE 2 OP_ENDIF"
// bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
// ****************************************************************************
void program::else_if() noexcept
{
    // This must be guarded.
    BC_ASSERT(!is_balanced());

    // Optimize is_succeess().
    negative_condition_count_ += (condition_.back() ? 1 : -1);

    condition_.back() = !condition_.back();
}

void program::end_if() noexcept
{
    // This must be guarded.
    BC_ASSERT(!is_balanced());

    // Optimize is_succeess().
    negative_condition_count_ += (condition_.back() ? 0 : -1);

    condition_.pop_back();
}

bool program::is_balanced() const noexcept
{
    return condition_.empty();
}

bool program::is_succeess() const noexcept
{
    ////const auto is_true = [](bool value) noexcept { return value; };
    ////return std::all_of(condition_.begin(), condition_.end(), is_true);

    // Optimization changes O(n) search [for every operation] to O(1).
    // bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
    return is_zero(negative_condition_count_);
}

bool program::if_(const operation& op) const noexcept
{
    // Conditional op execution is not predicated on conditional stack.
    return op.is_conditional() || is_succeess();
}

//  Accumulators.
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS:
// Satoshi compares the count to 200 with a composed postfix increment, which
// makes the actual maximum 201, not the presumably-intended 200. The code was
// later revised to make this explicit, by use of a prefix increment against a
// limit of 201.
// ****************************************************************************
constexpr bool operation_overflow(size_t count) noexcept
{
    return count > max_counted_ops;
}

bool program::ops_increment(const operation& op) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(sub1(max_size_t) >= operation_count_);

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return operation_count_ <= max_counted_ops;
}

bool program::ops_increment(size_t public_keys) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(max_size_t - public_keys >= operation_count_);

    operation_count_ += public_keys;
    return !operation_overflow(operation_count_);
}

// Signature validation helpers.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. However, execution of any one given
// script instance is not safe for concurrent execution (unnecessary scenario).
bool program::set_subscript(const op_iterator& op) noexcept
{
    // End is not reachable if op is an element of script_.
    if (script_->ops().empty() || op == script_->ops().end())
        return false;

    // Advance the offset to the op following the found code separator.
    script_->offset = std::next(op);
    return true;
}

// ****************************************************************************
// CONSENSUS: Endorsement and code separator stripping are always performed in
// conjunction and are limited to non-witness signature hash subscripts.
// The order of operations is inconsequential, as they are all removed.
// Subscripts are not evaluated, they are limited to signature hash creation.
// ****************************************************************************
script::cptr program::subscript(
    const chunk_cptrs& endorsements) const noexcept
{
    // bip141: establishes the version property.
    // bip143: op stripping is not applied to bip141 v0 scripts.
    if (is_enabled(forks::bip143_rule) && version_ == script_version::zero)
        return script_;

    // Transform endorsements into the set of operators, and op_codeseparator.
    const auto strip = create_strip_ops(endorsements);
    const auto stop = script_->ops().end();

    // If none of the strip ops are found, return the subscript.
    if (!intersecting(script_->offset, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    BC_PUSH_WARNING(NO_NEW_DELETE)
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return to_shared(new script{ difference(script_->offset, stop, strip) });
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// TODO: use sighash and key to generate signature in sign mode.
bool program::prepare(ec_signature& signature, const data_chunk&,
    hash_digest& hash, const chunk_cptr& endorsement) const noexcept
{
    uint8_t flags;
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(flags, distinguished, *endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    hash = signature_hash(*subscript({ endorsement }), flags);

    // Parse DER signature into an EC signature (bip66 sets strict).
    const auto bip66 = is_enabled(forks::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// TODO: use sighash and key to generate signature in sign mode.
bool program::prepare(ec_signature& signature, const data_chunk&,
    hash_cache& cache, uint8_t& flags, const data_chunk& endorsement,
    const script& sub) const noexcept
{
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(flags, distinguished, endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    signature_hash(cache, sub, flags);

    // Parse DER signature into an EC signature (bip66 sets strict).
    const auto bip66 = is_enabled(forks::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// Private.
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: nominal endorsement operation encoding required.
// ****************************************************************************
chain::operations program::create_strip_ops(
    const chunk_cptrs& endorsements) noexcept
{
    constexpr auto non_mininal = false;
    operations strip;

    // Subsequent emplaces are non-allocating, but still noexcept(false).
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    strip.reserve(add1(endorsements.size()));

    for (const auto& push: endorsements)
        strip.emplace_back(push, non_mininal);

    strip.emplace_back(opcode::codeseparator);
    BC_POP_WARNING()

    return strip;
}

hash_digest program::signature_hash(const script& sub,
    uint8_t flags) const noexcept
{
    // The bip141 fork establishes witness version, hashing is a distinct fork.
    const auto bip143 = is_enabled(forks::bip143_rule);

    // bip143: the method of signature hashing is changed for v0 scripts.
    return transaction_.signature_hash(input_, sub, value_, flags, version_,
        bip143);
}

// Caches signature hashes in a map against sighash flags.
// Prevents recomputation in the common case where flags are the same.
void program::signature_hash(hash_cache& cache, const script& sub,
    uint8_t flags) const noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    if (cache.find(flags) == cache.end())
        cache.emplace(flags, signature_hash(sub, flags));
    BC_POP_WARNING()
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
