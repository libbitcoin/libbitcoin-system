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

static const transaction default_transaction{};

// Constructors.
// ----------------------------------------------------------------------------

program::program(const script::ptr& script, const chain::transaction& tx,
    uint32_t index, uint32_t forks) noexcept
  : script_(script),
    transaction_(tx),
    input_index_(index),
    forks_(forks),
    value_(max_uint64),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0)
{
    // This is guarded by is_invalid, and in the interpreter.
    BC_ASSERT(index < transaction_.inputs()->size());
}

// Reuse copied program stack .
program::program(const script::ptr& script, const program& other) noexcept
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    negative_count_(0),
    operation_count_(0),
    primary_(other.primary_)
{
}

// Reuse moved program stack.
program::program(const script::ptr& script, program&& other) noexcept
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    negative_count_(0),
    operation_count_(0),
    primary_(std::move(other.primary_))
{
}

// Condition, alternate, jump and operation_count are not copied.
program::program(const script::ptr& script, const chain::transaction& tx,
    uint32_t index, uint32_t forks, chunk_ptrs&& stack, uint64_t value,
    script_version version) noexcept
  : script_(script),
    transaction_(tx),
    input_index_(index),
    forks_(forks),
    value_(value),
    version_(version),
    negative_count_(0),
    operation_count_(0),
    primary_(std::move(stack))
{
    // This is guarded by is_invalid, and in the interpreter.
    BC_ASSERT(index < tx.inputs()->size());
}

// Utilities.
// ----------------------------------------------------------------------------

// Check initial program state for validity (i.e. can evaluation return true).
bool program::is_invalid() const noexcept
{
    const auto bip141 = is_enabled(forks::bip141_rule);
    ////const auto nops_rule = is_enabled(forks::nops_rule);

    // TODO: nops rule must be enabled.
    return
        (/*nops_rule && */script_->is_oversized()) ||
        (input_index_ > transaction_.inputs()->size()) ||
        (bip141 && !chain::witness::is_push_size(primary_));
}

bool program::is_enabled(chain::forks rule) const noexcept
{
    return to_bool(forks_ & rule);
}

// Constant registers.
// ----------------------------------------------------------------------------

// This must be guarded (intended for interpreter internal use).
const input& program::input() const noexcept
{
    // This is guarded by is_invalid().
    BC_ASSERT(input_index_ < transaction().inputs()->size());
    return *(*transaction_.inputs())[input_index_];
}

const chain::transaction& program::transaction() const noexcept
{
    return transaction_;
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

// Instructions.
// ----------------------------------------------------------------------------

inline bool operation_overflow(size_t count) noexcept
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

// This is guarded by script size.
bool program::increment_op_count(const operation& op) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(max_size_t - one >= operation_count_);

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return !operation_overflow(operation_count_);
}

// This is guarded by script size.
bool program::increment_op_count(int32_t public_keys) noexcept
{
    static const auto max_keys = static_cast<int32_t>(max_script_public_keys);

    // bit.ly/2d1bsdB
    if (is_negative(public_keys) || public_keys > max_keys)
        return false;

    // Addition is safe due to script size constraint.
    BC_ASSERT(max_size_t - public_keys >= operation_count_);

    operation_count_ += public_keys;
    return !operation_overflow(operation_count_);
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// push
void program::push(bool value) noexcept
{
    push_move(value ? data_chunk{ numbers::positive_1 } : data_chunk{});
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_move(data_chunk&& item) noexcept
{
    push_ref(to_shared<data_chunk>(std::move(item)));
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_copy(const data_chunk& item) noexcept
{
    push_ref(to_shared<data_chunk>(item));
}

void program::push_ref(chunk_ptr&& item) noexcept
{
    primary_.push_back(std::move(item));
}

void program::push_ref(const chunk_ptr& item) noexcept
{
    primary_.push_back(item);
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

// This must be guarded.
data_chunk program::pop() noexcept
{
    return *pop_ref();
}

// This must be guarded.
chunk_ptr program::pop_ref() noexcept
{
    BC_ASSERT(!empty());
    const auto value = primary_.back();
    primary_.pop_back();
    return value;
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
    return !empty() && out_number.set_data(*pop_ref(), maxiumum_size);
}

bool program::pop_binary(number& first, number& second) noexcept
{
    // The right hand side number is at the top of the stack.
    return pop(first) && pop(second);
}

bool program::pop_ternary(number& first, number& second, number& third) noexcept
{
    // The upper bound is at stack top, lower bound next, value next.
    return pop(first) && pop(second) && pop(third);
}

// Determines if popped value is valid post-pop stack index and returns index.
bool program::pop_position(stack_iterator& out_position) noexcept
{
    int32_t signed_index;
    if (!pop(signed_index))
        return false;

    // Ensure the index is within bounds.

    if (is_negative(signed_index))
        return false;

    const auto index = static_cast<uint32_t>(signed_index);

    if (index >= size())
        return false;

    out_position = position(index);
    return true;
}

// pop1/pop2/.../pop[count]
bool program::pop(chunk_ptrs& section, int32_t signed_count) noexcept
{
    if (is_negative(signed_count))
        return false;

    const auto count = static_cast<uint32_t>(signed_count);

    if (size() < count)
        return false;

    section.reserve(count);
    for (size_t index = 0; index < count; ++index)
        section.push_back(pop_ref());

    return true;
}

// Primary push/pop optimizations (active).
// ----------------------------------------------------------------------------

// pop1/pop2/.../pop[index]/push[index]/.../push2/push1/push[index]
void program::duplicate(size_t index) noexcept
{
    push_ref(item(index));
}

// pop1/pop2/push1/push2
void program::swap(size_t left, size_t right) noexcept
{
    // position(index) is const, but must have non-const iterators here.
    std::swap(
        *std::prev(primary_.end(), ++left),
        *std::prev(primary_.end(), ++right));
}

// pop1/pop2/.../pop[pos-1]/pop[pos]/push[pos-1]/.../push2/push1
void program::erase(const stack_iterator& position) noexcept
{
    primary_.erase(position);
}

// pop1/pop2/.../pop[i]/pop[first]/.../pop[last]/push[i]/.../push2/push1
void program::erase(const stack_iterator& first,
    const stack_iterator& last) noexcept
{
    primary_.erase(first, last);
}

// Primary push/pop optimizations (passive).
// ----------------------------------------------------------------------------

// Reversed byte order in this example (big-endian).
// []               : false (empty)
// [00 00 00 00 00] : false (+zero)
// [80 00 00 00 00] : false (-zero)
// [42 00 00 00 00] : true
// [00 80 00 00 00] : true

// private
bool program::stack_to_bool(bool clean) const noexcept
{
    const auto& top = primary_.back();

    if (top->empty() || (clean && primary_.size() != one))
        return false;

    auto not_zero = [](uint8_t value) noexcept
    {
        return value != numbers::positive_0;
    };

    auto non_zero = [](uint8_t value)  noexcept
    {
        return (value & ~numbers::negative_sign) != numbers::positive_0;
    };

    return non_zero(top->back()) ||
        std::any_of(top->begin(), std::prev(top->end()), not_zero);
}

size_t program::size() const noexcept
{
    return primary_.size();
}

bool program::empty() const noexcept
{
    return primary_.empty();
}

// This must be guarded (intended for interpreter internal use).
bool program::stack_true(bool clean) const noexcept
{
    BC_ASSERT(!empty());
    return stack_to_bool(clean);
}

// This is safe to call when empty (intended for completion handlers).
bool program::stack_result(bool clean) const noexcept
{
    return !empty() && stack_true(clean);
}

bool program::is_stack_overflow() const noexcept
{
    // bit.ly/2cowHlP
    // Addition is safe due to script size metadata.
    return size() + alternate_.size() > max_stack_size;
}

bool program::if_(const operation& op) const noexcept
{
    return op.is_conditional() || succeeded();
}

bool program::top(number& out_number, size_t maxiumum_size) const noexcept
{
    return !empty() && out_number.set_data(*item(zero), maxiumum_size);
}

// This must be guarded.
program::stack_iterator program::position(size_t index) const noexcept
{
    BC_ASSERT(index < size());

    // Stack index is zero-based (zero is top).
    return std::prev(primary_.end(), ++index);
}

// This must be guarded.
chunk_ptr program::item(size_t index) const noexcept
{
    // Stack index is zero-based (zero is top).
    return *position(index);
}

// Alternate stack.
// ----------------------------------------------------------------------------

bool program::empty_alternate() const noexcept
{
    return alternate_.empty();
}

void program::push_alternate(chunk_ptr&& value) noexcept
{
    alternate_.push_back(std::move(value));
}

// This must be guarded.
chunk_ptr program::pop_alternate() noexcept
{
    BC_ASSERT(!alternate_.empty());
    const auto value = alternate_.back();
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------

void program::open(bool value) noexcept
{
    negative_count_ += (value ? 0 : 1);
    condition_.push_back(value);
}

// This must be guarded.
void program::negate() noexcept
{
    BC_ASSERT(!closed());
    const auto value = condition_.back();
    negative_count_ += (value ? 1 : -1);
    condition_.back() = !value;

    // Optimized above to avoid succeeded loop.
    ////condition_.back() = !condition_.back();
}

// This must be guarded.
void program::close() noexcept
{
    BC_ASSERT(!closed());
    const auto value = condition_.back();
    negative_count_ += (value ? 0 : -1);
    condition_.pop_back();

    // Optimized above to avoid succeeded loop.
    ////condition_.pop_back();
}

bool program::closed() const noexcept
{
    return condition_.empty();
}

bool program::succeeded() const noexcept
{
    return is_zero(negative_count_);

    // Optimized above to avoid succeeded loop.
    ////const auto is_true = [](bool value) noexcept { return value; };
    ////return std::all_of(condition_.begin(), condition_.end(), true);
}

// Subscript.
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
script::ptr program::subscript(const chunk_ptrs& endorsements) const noexcept
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
    return to_shared(new script{ difference(script_->offset, stop, strip) });
}

// TODO: use sighash and key to generate signature in sign mode.
bool program::prepare(ec_signature& signature, const data_chunk&,
    hash_digest& hash, const chunk_ptr& endorsement) const noexcept
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
    const chunk_ptrs& endorsements) noexcept
{
    constexpr auto non_mininal = false;

    operations strip;
    strip.reserve(add1(endorsements.size()));

    for (const auto& push: endorsements)
        strip.emplace_back(push, non_mininal);

    strip.emplace_back(opcode::codeseparator);
    return strip;
}

hash_digest program::signature_hash(const script& sub,
    uint8_t flags) const noexcept
{
    // The bip141 fork establishes witness version, hashing is a distinct fork.
    const auto bip143 = is_enabled(forks::bip143_rule);

    // bip143: the method of signature hashing is changed for v0 scripts.
    return transaction_.signature_hash(input_index_, sub, value_, flags,
        version_, bip143);
}

// Caches signature hashes in a map against sighash flags.
// Prevents recomputation in the common case where flags are the same.
void program::signature_hash(hash_cache& cache, const script& sub,
    uint8_t flags) const noexcept
{
    if (cache.find(flags) == cache.end())
        cache.emplace(flags, signature_hash(sub, flags));
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
