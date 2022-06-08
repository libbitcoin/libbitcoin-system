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

// Pass input const_iterator vs. input&.
// Don't store input, index, sequence, or final.
// Just obtain from the iterator, exposed as protected methods.
// Index is obtained from the std::distance(inputs.begin, it), though this
// requires that the iterator is an interator of the inputs vector. It is a
// runtime failure if the iterator is null/end and is a compile time failure to
// compare an from of a different collection (i.e. via std::distance).

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
// ============================================================================

// Utilities.
// ----------------------------------------------------------------------------

// Check initial program state for validity (i.e. can evaluation return true).
bool program::is_invalid() const noexcept
{
    // TODO: nops rule must be enabled in tests and config.
    ////const auto nops_rule = is_enabled(forks::nops_rule);
    const auto bip141 = is_enabled(forks::bip141_rule);

    return
        (/*nops_rule && */script_->is_oversized()) ||
        (bip141 && !chain::witness::is_push_size(*primary_));
}

bool program::is_enabled(chain::forks rule) const noexcept
{
    return to_bool(forks_ & rule);
}

bool program::is_stack_true(stack clean) const noexcept
{
    return !is_empty() && stack_to_bool(clean);
}

//  Accumulators.
// ----------------------------------------------------------------------------

constexpr bool operation_overflow(size_t count) noexcept
{
    // ************************************************************************
    // CONSENSUS:
    // Satoshi compares the count to 200 with a composed postfix increment,
    // which makes the actual maximum 201, not the presumably-intended 200.
    // The code was later revised to make this explicit, by use of a prefix
    // increment against a limit of 201.
    // ************************************************************************
    return count > max_counted_ops;
}

bool program::ops_increment(const operation& op) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(sub1(max_size_t) >= operation_count_);

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return !operation_overflow(operation_count_);
}

bool program::ops_increment(int32_t public_keys) noexcept
{
    constexpr auto max_keys = possible_narrow_sign_cast<int32_t>(
        max_script_public_keys);

    // bit.ly/2d1bsdB
    if (is_negative(public_keys) || public_keys > max_keys)
        return false;

    // Addition is safe due to script size constraint.
    BC_ASSERT(max_size_t - public_keys >= operation_count_);

    operation_count_ += public_keys;
    return !operation_overflow(operation_count_);
}

// Program registers.
// ----------------------------------------------------------------------------

program::op_iterator program::begin() const noexcept
{
    return script_->ops().begin();
}

program::op_iterator program::end() const noexcept
{
    return script_->ops().end();
}

// Protected (interpreter friend).
// ============================================================================

// Constant registers.
// ----------------------------------------------------------------------------

bool program::is_final() const noexcept
{
    return (*input_)->is_final();
}

uint32_t program::sequence() const noexcept
{
    return (*input_)->sequence();
}

const chain::transaction& program::transaction() const noexcept
{
    return transaction_;
}

// Primary stack (push).
// ----------------------------------------------------------------------------

void program::push(bool value) noexcept
{
    // It is safe to share instances of these shared pointers across threads.
    static const auto true_ = to_shared<data_chunk>({ numbers::positive_1 });
    static const auto false_ = to_shared<data_chunk>(data_chunk{});

    push(value ? true_ : false_);
}

void program::push(data_chunk&& item) noexcept
{
    push(to_shared<data_chunk>(std::move(item)));
}

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

// Primary stack (pop).
// ----------------------------------------------------------------------------

// public (transaction)
chunk_cptr program::pop() noexcept
{
    // This must be guarded (is_relaxed_push is checked in transaction).
    BC_ASSERT(!is_empty());

    // This must be a pointer copy, as the pointer is about to be destroyed.
    const chunk_cptr value{ primary_->back() };
    drop();
    return value;
}

void program::drop() noexcept
{
    // This must be guarded (is_relaxed_push is checked in transaction).
    BC_ASSERT(!is_empty());

    primary_->pop_back();
}

bool program::pop(int32_t& out_value) noexcept
{
    number value;
    if (!pop(value))
        return false;

    out_value = value.int32();
    return true;
}

bool program::pop(number& out_number, size_t maxiumum_size) noexcept
{
    return !is_empty() && out_number.set_data(*pop(), maxiumum_size);
}

// True if popped value is valid post-pop stack index.
bool program::pop_index(size_t& index) noexcept
{
    int32_t signed_index;
    if (!pop(signed_index))
        return false;

    if (is_negative(signed_index))
        return false;

    index = sign_cast<size_t>(signed_index);
    return index < size();
}

bool program::pop_binary(number& left, number& right) noexcept
{
    // The right hand side operand is at the top of the stack.
    return pop(right) && pop(left);
}

bool program::pop_ternary(number& upper, number& lower, number& value) noexcept
{
    // The upper bound is at stack top, lower bound next, value next.
    return pop(upper) && pop(lower) && pop(value);
}

// pop1/pop2/.../pop[count]
bool program::pop(chunk_cptrs& section, int32_t signed_count) noexcept
{
    if (is_negative(signed_count))
        return false;

    const auto count = sign_cast<uint32_t>(signed_count);

    if (size() < count)
        return false;

    section.reserve(count);
    for (size_t index = 0; index < count; ++index)
        section.push_back(pop());

    return true;
}

// Primary stack push/pop non-const functions (optimizations).
// ----------------------------------------------------------------------------

// pop1/pop2/push1/push2
void program::swap(size_t left, size_t right) noexcept
{
    // This must be guarded.
    BC_ASSERT(std::max(left, right) < size());

    // Stack index is zero-based, last element is "top".
    std::swap(
        *std::prev(primary_->end(), add1(left)),
        *std::prev(primary_->end(), add1(right)));
}

// pop1/pop2/.../pop[pos-1]/pop[pos]/push[pos-1]/.../push2/push1
void program::erase(size_t index) noexcept
{
    // This must be guarded.
    BC_ASSERT(index < size());

    // Stack index is zero-based, last element is "top".
    primary_->erase(std::prev(primary_->end(), add1(index)));
}

// Primary stack push/pop const functions.
// ----------------------------------------------------------------------------

const chunk_cptr& program::item(size_t index) const noexcept
{
    // This must be guarded.
    BC_ASSERT(index < size());

    // Stack index is zero-based (zero is top).
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

bool program::get_top(number& out_number, size_t maxiumum_size) const noexcept
{
    return !is_empty() && out_number.set_data(*item(zero), maxiumum_size);
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

void program::open(bool value) noexcept
{
    ////negative_condition_count_ += (value ? 0 : 1);

    condition_.push_back(value);
}

void program::reopen() noexcept
{
    // This must be guarded.
    BC_ASSERT(!is_closed());

    ////const auto value = condition_.back();
    ////negative_condition_count_ += (value ? 1 : -1);
    ////condition_.back() = !value;

    condition_.back() = !condition_.back();
}

void program::close() noexcept
{
    // This must be guarded.
    BC_ASSERT(!is_closed());

    ////const auto value = condition_.back();
    ////negative_condition_count_ += (value ? 0 : -1);
    ////condition_.pop_back();

    condition_.pop_back();
}

bool program::is_closed() const noexcept
{
    return condition_.empty();
}

bool program::is_succeess() const noexcept
{
    ////return is_zero(negative_condition_count_);

    const auto is_true = [](bool value) noexcept { return value; };

    return std::all_of(condition_.begin(), condition_.end(), is_true);
}

bool program::if_(const operation& op) const noexcept
{
    // Conditional op execution is not predicated on conditional stack.
    return op.is_conditional() || is_succeess();
}

// Signature validation helpers.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. However, execution of any one given
// script instance is not safe for concurrent execution (unnecessary scenario).
bool program::set_subscript(const operation& op) noexcept
{
    if (script_->ops().empty())
        return false;

    const auto stop = script_->ops().end();

    // This avoids std::find_if using equality operator override.
    const auto finder = [&op](const operation& operation) noexcept
    {
        return &operation == &op;
    };

    // This is not efficient (linear) but rarely used.
    script_->offset = std::find_if(script_->ops().begin(), stop, finder);

    // This is not reachable if op is an element of script_.
    if (script_->offset == stop)
        return false;

    // Advance the offset to the op following the found code separator.
    std::advance(script_->offset, one);
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
// ============================================================================

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

// Reversed byte order in this example (big-endian).
// []               : false (empty)
// [00 00 00 00 00] : false (+zero)
// [80 00 00 00 00] : false (-zero)
// [42 00 00 00 00] : true
// [00 80 00 00 00] : true
bool program::stack_to_bool(stack clean) const noexcept
{
    const auto& top = primary_->back();

    if (top->empty() || ((clean == stack::clean) && (primary_->size() != one)))
        return false;

    const auto non_zero = [](uint8_t value) noexcept
    {
        return value != numbers::positive_0;
    };

    const auto zero_magnitude = [](uint8_t value) noexcept
    {
        constexpr auto sign_mask = bit_not<uint8_t>(numbers::negative_sign);
        return bit_and<uint8_t>(value, sign_mask) == numbers::positive_0;
    };

    return !zero_magnitude(top->back()) ||
        std::any_of(top->begin(), std::prev(top->end()), non_zero);
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
