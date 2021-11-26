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
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/machine/interpreter.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace bc::system::chain;

static const chain::transaction default_tx_;
static const chain::script default_script_;

static default_allocator<chain::operation> no_fill_op_allocator{};

// Constructors.
//-----------------------------------------------------------------------------

program::program()
  : script_(default_script_),
    transaction_(default_tx_),
    input_index_(0),
    forks_(0),
    value_(0),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
}

program::program(const script& script)
  : script_(script),
    transaction_(default_tx_),
    input_index_(0),
    forks_(0),
    value_(0),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
}

program::program(const script& script, const chain::transaction& transaction,
    uint32_t index, uint32_t forks)
  : script_(script),
    transaction_(transaction),
    input_index_(index),
    forks_(forks),
    value_(max_uint64),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
    // This is guarded by is_invalid, and in the interpreter.
    BITCOIN_ASSERT(index < transaction.inputs().size());
}

// Condition, alternate, jump and operation_count are not copied.
program::program(const script& script, const chain::transaction& transaction,
    uint32_t index, uint32_t forks, data_stack&& stack, uint64_t value,
    script_version version)
  : script_(script),
    transaction_(transaction),
    input_index_(index),
    forks_(forks),
    value_(value),
    version_(version),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(std::move(stack))
{
    // This is guarded by is_invalid, and in the interpreter.
    BITCOIN_ASSERT(index < transaction.inputs().size());
}


// Condition, alternate, jump and operation_count are not copied.
program::program(const script& script, const program& other)
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(other.primary_)
{
}

// Condition, alternate, jump and operation_count are not moved.
program::program(const script& script, program&& other, bool)
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(std::move(other.primary_))
{
}

// Instructions.
//-----------------------------------------------------------------------------

code program::evaluate()
{
    return interpreter::run(*this);
}

code program::evaluate(const operation& op)
{
    return interpreter::run(op, *this);
}

// Constant registers.
//-----------------------------------------------------------------------------

// Check initial program state for validity (i.e. can evaluation return true).
bool program::is_invalid() const
{
    const auto bip141 = script::is_enabled(forks_, rule_fork::bip141_rule);
    const auto nops_rule = script::is_enabled(forks_, rule_fork::nops_rule);

    // TODO: nops rule must be enabled.
    return
        (/*nops_rule && */script_.is_oversized()) ||
        (input_index() > transaction_.inputs().size()) ||
        (bip141 && !chain::witness::is_push_size(primary_));
}

uint32_t program::forks() const
{
    return forks_;
}

uint32_t program::input_index() const
{
    return input_index_;
}

uint64_t program::value() const
{
    return value_;
}

script_version program::version() const
{
    return version_;
}

const chain::transaction& program::transaction() const
{
    return transaction_;
}

// Program registers.
//-----------------------------------------------------------------------------

program::op_iterator program::begin() const
{
    return script_.begin();
}

program::op_iterator program::jump() const
{
    return jump_;
}

program::op_iterator program::end() const
{
    return script_.end();
}

// Instructions.
//-----------------------------------------------------------------------------

inline bool operation_overflow(size_t count)
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
bool program::increment_op_count(const operation& op)
{
    // Addition is safe due to script size constraint.
    BITCOIN_ASSERT(max_size_t - one >= operation_count_);

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return !operation_overflow(operation_count_);
}

// This is guarded by script size.
bool program::increment_op_count(int32_t public_keys)
{
    static const auto max_keys = static_cast<int32_t>(max_script_public_keys);

    // bit.ly/2d1bsdB
    if (is_negative(public_keys) || public_keys > max_keys)
        return false;

    // Addition is safe due to script size constraint.
    BITCOIN_ASSERT(max_size_t - public_keys >= operation_count_);

    operation_count_ += public_keys;
    return !operation_overflow(operation_count_);
}

bool program::register_jump(const operation& op)
{
    if (script_.empty())
        return false;

    // This avoids std::find_if using equality operator override.
    const auto finder = [&op](const operation& operation)
    {
        return &operation == &op;
    };

    // This is not efficient (linear) but rarely used.
    jump_ = std::find_if(script_.begin(), script_.end(), finder);

    // This is not reachable if op is an element of script_.
    if (jump_ == script_.end())
        return false;

    std::advance(jump_, one);
    return true;
}

// Primary stack (push).
//-----------------------------------------------------------------------------

// push
void program::push(bool value)
{
    push_move(value ? value_type{ numbers::positive_1 } : value_type{});
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_move(value_type&& item)
{
    primary_.push_back(std::move(item));
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_copy(const value_type& item)
{
    primary_.push_back(item);
}

// Primary stack (pop).
//-----------------------------------------------------------------------------

// This must be guarded.
data_chunk program::pop()
{
    BITCOIN_ASSERT(!empty());
    const auto value = primary_.back();
    primary_.pop_back();
    return value;
}

bool program::pop(int32_t& out_value)
{
    number value;
    if (!pop(value))
        return false;

    out_value = value.int32();
    return true;
}

bool program::pop(number& out_number, size_t maxiumum_size)
{
    return !empty() && out_number.set_data(pop(), maxiumum_size);
}

bool program::pop_binary(number& first, number& second)
{
    // The right hand side number is at the top of the stack.
    return pop(first) && pop(second);
}

bool program::pop_ternary(number& first, number& second, number& third)
{
    // The upper bound is at stack top, lower bound next, value next.
    return pop(first) && pop(second) && pop(third);
}

// Determines if popped value is valid post-pop stack index and returns index.
bool program::pop_position(stack_iterator& out_position)
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
bool program::pop(data_stack& section, int32_t signed_count)
{
    if (is_negative(signed_count))
        return false;

    const auto count = static_cast<uint32_t>(signed_count);

    if (size() < count)
        return false;

    for (size_t index = 0; index < count; ++index)
        section.push_back(pop());

    return true;
}

// Primary push/pop optimizations (active).
//-----------------------------------------------------------------------------

// pop1/pop2/.../pop[index]/push[index]/.../push2/push1/push[index]
void program::duplicate(size_t index)
{
    push_copy(item(index));
}

// pop1/pop2/push1/push2
void program::swap(size_t index_left, size_t index_right)
{
    // TODO: refactor to allow DRY without const_cast here.
    std::swap(
        const_cast<data_stack::value_type&>(item(index_left)),
        const_cast<data_stack::value_type&>(item(index_right)));
}

// pop1/pop2/.../pop[pos-1]/pop[pos]/push[pos-1]/.../push2/push1
void program::erase(const stack_iterator& position)
{
    primary_.erase(position);
}

// pop1/pop2/.../pop[i]/pop[first]/.../pop[last]/push[i]/.../push2/push1
void program::erase(const stack_iterator& first,
    const stack_iterator& last)
{
    primary_.erase(first, last);
}

// Primary push/pop optimizations (passive).
//-----------------------------------------------------------------------------

// Reversed byte order in this example (big-endian).
// []               : false (empty)
// [00 00 00 00 00] : false (+zero)
// [80 00 00 00 00] : false (-zero)
// [42 00 00 00 00] : true
// [00 80 00 00 00] : true

// private
bool program::stack_to_bool(bool clean) const
{
    const auto& top = primary_.back();

    if (top.empty() || (clean && primary_.size() != one))
        return false;

    auto not_zero = [](uint8_t value)
    {
        return value != numbers::positive_0;
    };

    auto non_zero = [](uint8_t value) 
    {
        return (value & ~numbers::negative_sign) != numbers::positive_0;
    };

    return non_zero(top.back()) ||
        std::any_of(top.begin(), std::prev(top.end()), not_zero);
}

size_t program::size() const
{
    return primary_.size();
}

bool program::empty() const
{
    return primary_.empty();
}

// This must be guarded (intended for interpreter internal use).
bool program::stack_true(bool clean) const
{
    BITCOIN_ASSERT(!empty());
    return stack_to_bool(clean);
}

// This is safe to call when empty (intended for completion handlers).
bool program::stack_result(bool clean) const
{
    return !empty() && stack_true(clean);
}

bool program::is_stack_overflow() const
{
    // bit.ly/2cowHlP
    // Addition is safe due to script size metadata.
    return size() + alternate_.size() > max_stack_size;
}

bool program::if_(const operation& op) const
{
    return op.is_conditional() || succeeded();
}

// This must be guarded.
const data_stack::value_type& program::item(size_t index) /*const*/
{
    return *position(index);
}

bool program::top(number& out_number, size_t maxiumum_size)
{
    return !empty() && out_number.set_data(item(0), maxiumum_size);
}

// This must be guarded.
program::stack_iterator program::position(size_t index) /*const*/
{
    // Decrementing 1 (prev) makes the stack index zero-based (unlike satoshi).
    BITCOIN_ASSERT(index < size());
    return std::prev(primary_.end(), ++index);
}

// Alternate stack.
//-----------------------------------------------------------------------------

bool program::empty_alternate() const
{
    return alternate_.empty();
}

void program::push_alternate(value_type&& value)
{
    alternate_.push_back(std::move(value));
}

// This must be guarded.
program::value_type program::pop_alternate()
{
    BITCOIN_ASSERT(!alternate_.empty());
    const auto value = alternate_.back();
    alternate_.pop_back();
    return value;
}

// Conditional stack.
//-----------------------------------------------------------------------------

void program::open(bool value)
{
    negative_count_ += (value ? 0 : 1);
    condition_.push_back(value);
}

// This must be guarded.
void program::negate()
{
    BITCOIN_ASSERT(!closed());
    const auto value = condition_.back();
    negative_count_ += (value ? 1 : -1);
    condition_.back() = !value;

    // Optimized above to avoid succeeded loop.
    ////condition_.back() = !condition_.back();
}

// This must be guarded.
void program::close()
{
    BITCOIN_ASSERT(!closed());
    const auto value = condition_.back();
    negative_count_ += (value ? 0 : -1);
    condition_.pop_back();

    // Optimized above to avoid succeeded loop.
    ////condition_.pop_back();
}

bool program::closed() const
{
    return condition_.empty();
}

bool program::succeeded() const
{
    return is_zero(negative_count_);

    // Optimized above to avoid succeeded loop.
    ////const auto is_true = [](bool value) { return value; };
    ////return std::all_of(condition_.begin(), condition_.end(), true);
}

// Subscript.
//-----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: Witness v0 scripts are not stripped (bip143/v0).
// Subscripts are not evaluated, they are limited to signature hash creation.
// ****************************************************************************
chain::script program::subscript() const
{
    // TODO: Retain the script as a shared object. Currently both subscript
    // TODO: methods return either a reference or a constructed copy. This
    // TODO: necessitates returning a copy in either case. This copy can be
    // TODO: eliminated by passing a shared_from_this() generated pointer in
    // TODO: one case and a constructed shared pointer instance in the other.
    if (jump() == begin())
        return script_;

    // TODO: Construct script on operations shared pointer and offset parameter.
    // TODO: if offset provided, all iteration starts at offset point. This
    // TODO: precludes copying operations in the case of a jump without mutate.
    operation::list sub(std::distance(jump(), end()), no_fill_op_allocator);
    std::copy(jump(), end(), sub.begin());
    return { sub };
}

// ****************************************************************************
// CONSENSUS: Endorsement and code separator stripping are always performed in
// conjunction and are limited to non-witness signature hash subscripts.
// The order of operations is inconsequential, as they are all removed.
// Subscripts are not evaluated, they are limited to signature hash creation.
// ****************************************************************************
chain::script program::subscript(const endorsements& endorsements) const
{
    const auto sub = subscript();

    // bip143: op stripping is not applied to bip141 v0 scripts.
    // The bip141 fork sets the version property, so this is a distinct check.
    if (script::is_enabled(forks(), rule_fork::bip143_rule))
        return sub;

    // TODO: Construct opcode with shared pointer to data, and stack with
    // TODO: shared pointers to opcode data values. This eliminates both the
    // TODO: stack copy of script data elements and copy here into operations.
    // TODO: the endorsement delete would then just reduce the reference count.

    // Transform endorsements into the set of operators, and op_codeseparator.
    const auto strip = create_delete_ops(endorsements);

    // If no intersection, nothing to strip.
    if (!intersecting(sub.operations(), strip))
        return script_;

    // Copy script operations for mutation.
    return { difference(sub.operations(), strip) };
}

// TODO: use sighash and key to generate signature in sign mode.
bool program::prepare(ec_signature& signature, data_chunk& key,
    hash_digest& hash, const system::endorsement& endorsement) const
{
    uint8_t flags;
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(flags, distinguished, endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    hash = signature_hash(subscript({ endorsement }), flags);

    // Parse DER signature into an EC signature (bip66 sets strict).
    const auto bip66 = script::is_enabled(forks(), rule_fork::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// TODO: use sighash and key to generate signature in sign mode.
bool program::prepare(ec_signature& signature, data_chunk& key,
    hash_digest& hash, hash_cache& cache, const system::endorsement& endorsement,
    const script& subscript) const
{
    uint8_t flags;
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(flags, distinguished, endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    hash = signature_hash(cache, subscript, flags);

    // Parse DER signature into an EC signature (bip66 sets strict).
    const auto bip66 = script::is_enabled(forks(), rule_fork::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// Private.
//-----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: non-minimal endorsement operation encoding required.
// ****************************************************************************
chain::operation::list program::create_delete_ops(const endorsements& data)
{
    // Create set of endorsement push data ops and a codeseparator op.
    operation::list strip(add1(data.size()), no_fill_op_allocator);

    // C++17: Parallel policy.
    std::transform(data.begin(), data.end(), strip.begin(),
        [](const endorsement& data)
        {
            return operation{ data, false };
        });

    strip.emplace_back(opcode::codeseparator);
    return strip;
}

hash_digest program::signature_hash(const script& subscript, uint8_t flags) const
{
    // The bip141 fork establishes witness version, hashing is a distinct fork.
    const auto bip143 = script::is_enabled(forks(), rule_fork::bip143_rule);

    // bip143: the method of signature hashing is changed for v0 scripts.
    return script::generate_signature_hash(transaction(), input_index(),
        subscript, value(), flags, version(), bip143);
}

// Caches signature hashes in a map against sighash flags.
// Prevents recomputation in the common case where flags are the same.
hash_digest program::signature_hash(hash_cache& cache, const script& subscript,
    uint8_t flags) const
{
    const auto it = cache.find(flags);
    if (it != cache.end())
        return it->second;

    // TODO: change to pointer cache with general conversion of push data.
    auto hash = signature_hash(subscript, flags);
    cache[flags] = hash;
    return hash;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
