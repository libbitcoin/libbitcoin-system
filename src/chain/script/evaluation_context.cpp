/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/script/evaluation_context.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace chain {
    
// Fixed tuning parameters, max_stack_size ensures no reallocation.
static constexpr size_t stack_capactity = max_stack_size;
static constexpr size_t alternate_capactity = max_stack_size;
static constexpr size_t condition_capactity = max_stack_size;

// Constructors.
//-----------------------------------------------------------------------------

// Iterators must be set by set_script.
evaluation_context::evaluation_context(const chain::transaction& transaction,
    uint32_t input_index, uint32_t flags)
  : condition_(condition_capactity),
    operation_count_{0},
    flags_(flags),
    input_index_(input_index),
    transaction_(transaction)
{
    stack_.reserve(stack_capactity);
    alternate_.reserve(alternate_capactity);
}

// Condition, alternate jump and op_count are not coppied.
evaluation_context::evaluation_context(const evaluation_context& other)
  : stack_(other.stack_),
    condition_(condition_capactity),
    operation_count_{0},
    flags_(other.flags_),
    input_index_(other.input_index_),
    transaction_(other.transaction_)
{
    // Regarding capacity preservation on copy assignment: bit.ly/2eFbXHF
    stack_.reserve(stack_capactity);
    alternate_.reserve(alternate_capactity);
}

// Condition, alternate jump and op_count are not moved.
evaluation_context::evaluation_context(evaluation_context&& other, bool)
  : stack_(std::move(other.stack_)),
    condition_(condition_capactity),
    operation_count_{0},
    flags_(other.flags_),
    input_index_(other.input_index_),
    transaction_(other.transaction_)
{
    // Regarding capacity preservation on move assignment: bit.ly/2eFbXHF
    stack_.reserve(stack_capactity);
    alternate_.reserve(alternate_capactity);
}

// Program counter.
//-----------------------------------------------------------------------------

evaluation_context::op_iterator evaluation_context::begin() const
{
    return begin_;
}

evaluation_context::op_iterator evaluation_context::jump() const
{
    return jump_;
}

evaluation_context::op_iterator evaluation_context::end() const
{
    return end_;
}

// Instructions.
//-----------------------------------------------------------------------------

// This is just deferred construction, call only once.
bool evaluation_context::set_script(const script& script)
{
    // bit.ly/2c9HzmN
    if (script.satoshi_content_size() > max_script_size)
        return false;

    begin_ = script.begin();
    jump_ = script.begin();
    end_ = script.end();
    return true;
}

void evaluation_context::set_jump(op_iterator instruction)
{
    // The begin_ member could be overloaded for this since it is never reused.
    // But the cost of the proper abstraction is just a few bytes.
    jump_ = instruction;
}

// Operation count.
//-----------------------------------------------------------------------------

inline bool operation_overflow(size_t count)
{
    return count > max_counted_ops;
}

bool evaluation_context::update_operation_count(const operation& op)
{
    // Addition is safe due to script size validation.
    if (operation::is_counted(op.code()))
        ++operation_count_;

    return !operation_overflow(operation_count_);
}

bool evaluation_context::update_pubkey_count(int32_t multisig_pubkeys)
{
    // bit.ly/2d1bsdB
    if (multisig_pubkeys < 0 || multisig_pubkeys > max_script_public_key_count)
        return false;

    // Addition is safe due to script size validation.
    operation_count_ += multisig_pubkeys;
    return !operation_overflow(operation_count_);
}

/// Properties.
//-----------------------------------------------------------------------------

uint32_t evaluation_context::flags() const
{
    return flags_;
}

uint32_t evaluation_context::input_index() const
{
    return input_index_;
}

const chain::transaction& evaluation_context::transaction() const
{
    return transaction_;
}

/// Stack info.
//-----------------------------------------------------------------------------

// Pop jump-to-end, push all back, use to construct a script.
script evaluation_context::subscript() const
{
    operation_stack ops;

    for (auto op = jump(); op != end(); ++op)
        ops.push_back(*op);

    return script(std::move(ops));
}

const data_stack::value_type& evaluation_context::item(size_t index) /*const*/
{
    return *position(index);
}

evaluation_context::stack_iterator evaluation_context::position(
    size_t index) /*const*/
{
    // Subtracting 1 makes the stack indexes zero-based (unlike satoshi).
    BITCOIN_ASSERT(index < stack_.size());
    return (stack_.end() - 1) - index;
}

bool evaluation_context::is_short_circuited(const operation& op) const
{
    return !(operation::is_conditional(op.code()) || condition_.succeeded());
}

bool evaluation_context::is_stack_overflow() const
{
    // bit.ly/2cowHlP
    // Addition is safe due to script size validation.
    return stack_.size() + alternate_.size() > max_stack_size;
}

// private
bool evaluation_context::stack_to_bool() const
{
    const auto& back = stack_.back();
    if (back.empty())
        return false;

    const auto last = back.end() - 1;
    for (auto it = back.begin(); it != back.end(); ++it)
    {
        if (*it != 0)
        {
            // It's not non-zero it's the terminating negative sentinel.
            return !(it == last && *it == number::negative_0);
        }
    }

    return false;
}

// This call must be guarded (do not return false if empty, for op handlers).
bool evaluation_context::stack_true() const
{
    BITCOIN_ASSERT(!stack_.empty());
    return stack_to_bool();
}

// This is safe to call when empty (for completion handlers).
bool evaluation_context::stack_false() const
{
    return empty() || !stack_true();
}

bool evaluation_context::empty() const
{
    return stack_.empty();
}

size_t evaluation_context::size() const
{
    return stack_.size();
}

/// Stack pop.
//-----------------------------------------------------------------------------

// This call must be guarded.
data_chunk evaluation_context::pop()
{
    BITCOIN_ASSERT(!stack_.empty());
    const auto value = stack_.back();
    stack_.pop_back();
    return value;
}

bool evaluation_context::pop(int32_t& out_value)
{
    number value;
    if (!pop(value))
        return false;

    out_value = value.int32();
    return true;
}

bool evaluation_context::pop(number& out_number, size_t maxiumum_size)
{
    return !stack_.empty() && out_number.set_data(pop(), maxiumum_size);
}

bool evaluation_context::pop_binary(number& first, number& second)
{
    // The right hand side number is at the top of the stack.
    return pop(first) && pop(second);
}

bool evaluation_context::pop_ternary(number& first, number& second,
    number& third)
{
    // The upper bound is at stack top, lower bound next, value next.
    return pop(first) && pop(second) && pop(third);
}

// Determines if popped value is valid post-pop stack index and returns index.
bool evaluation_context::pop_position(stack_iterator& out_position)
{
    int32_t index;
    if (!pop(index))
        return false;

    // Ensure the index is within bounds.
    const auto size = stack_.size();
    if (index < 0 || index >= size)
        return false;

    out_position = position(static_cast<size_t>(index));
    return true;
}

// pop1/pop2/.../pop[count]
bool evaluation_context::pop(data_stack& section, size_t count)
{
    if (stack_.size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
        section.emplace_back(pop());

    return true;
}

// pop1/pop2/.../pop[pos-1]/pop[pos]/push[pos-1]/.../push2/push1
void evaluation_context::erase(const stack_iterator& position)
{
    stack_.erase(position);
}

// pop1/pop2/.../pop[i]/pop[first]/.../pop[last]/push[i]/.../push2/push1
void evaluation_context::erase(const stack_iterator& first,
    const stack_iterator& last)
{
    stack_.erase(first, last);
}

/// Stack push.
//-----------------------------------------------------------------------------

// push
void evaluation_context::push(bool value)
{
    if (value)
        push_move({ number::positive_1 });
    else
        push_move({});
}

// Be explicit about the intent to move or copy, to get compiler help.
void evaluation_context::push_move(value_type&& item)
{
    stack_.emplace_back(std::move(item));
}

// Be explicit about the intent to move or copy, to get compiler help.
void evaluation_context::push_copy(const value_type& item)
{
    stack_.emplace_back(item);
}

// pop1/pop2/.../pop[index]/push[index]/.../push2/push1/push[index]
void evaluation_context::duplicate(size_t index)
{
    push_copy(item(index));
}

// pop1/pop2/push1/push2
void evaluation_context::swap(size_t index_left, size_t index_right)
{
    // TODO: refactor to allow DRY without const_cast here.
    std::swap(
        const_cast<data_stack::value_type&>(item(index_left)),
        const_cast<data_stack::value_type&>(item(index_right)));
}

/// Alternate stack.
//-----------------------------------------------------------------------------

bool evaluation_context::empty_alternate() const
{
    return alternate_.empty();
}

evaluation_context::value_type evaluation_context::pop_alternate()
{
    BITCOIN_ASSERT(!alternate_.empty());
    const auto value = alternate_.back();
    alternate_.pop_back();
    return value;
}

void evaluation_context::push_alternate(value_type&& value)
{
    alternate_.emplace_back(std::move(value));
}

/// Conditional stack.
//-----------------------------------------------------------------------------

void evaluation_context::close()
{
    condition_.close();
}

void evaluation_context::negate()
{
    condition_.negate();
}

void evaluation_context::open(bool value)
{
    return condition_.open(value);
}

bool evaluation_context::closed() const
{
    return condition_.closed();
}

bool evaluation_context::succeeded() const
{
    return condition_.succeeded();
}

} // namespace chain
} // namespace libbitcoin
