/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_MACHINE_PROGRAM_IPP
#define LIBBITCOIN_MACHINE_PROGRAM_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/machine/number.hpp>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace machine {

// Constant registers.
//-----------------------------------------------------------------------------

bool program::is_valid() const
{
    // An invalid sequence indicates a failure deserializing operations.
    return script_.is_valid_operations() &&
        (script_.satoshi_content_size() <= max_script_size);
}

uint32_t program::forks() const
{
    return forks_;
}

uint32_t program::input_index() const
{
    return input_index_;
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

size_t program::operation_count() const
{
    return operation_count_;
}

// Instructions.
//-----------------------------------------------------------------------------

inline bool operation_overflow(size_t count)
{
    return count > max_counted_ops;
}

bool program::increment_operation_count(const operation& op)
{
    // Addition is safe due to script size validation.
    if (operation::is_counted(op.code()))
        ++operation_count_;

    return !operation_overflow(operation_count_);
}

bool program::increment_multisig_public_key_count(int32_t count)
{
    // bit.ly/2d1bsdB
    if (count < 0 || count > max_script_public_key_count)
        return false;

    // Addition is safe due to script size validation.
    operation_count_ += count;
    return !operation_overflow(operation_count_);
}

bool program::set_jump_register(const operation& op, int32_t offset)
{
    if (script_.empty())
        return false;

    const auto finder = [&op](const operation& operation)
    {
        return &operation == &op;
    };

    // This is not efficient but is simplifying and subscript is rarely used.
    // Otherwise we must track the program counter through each evaluation.
    jump_ = std::find_if(script_.begin(), script_.end(), finder);

    if (jump_ == script_.end())
        return false;

    // This does not require a guard because op_codeseparator can only increment.
    // Even if the opcode is last in the sequnce the increment is valid (end).
    BITCOIN_ASSERT_MSG(offset == 1, "unguarded jump offset");

    jump_ += offset;
    return true;
}

// Primary stack (push).
//-----------------------------------------------------------------------------

// push
void program::push(bool value)
{
    push_move(value ? value_type{ number::positive_1 } : value_type{});
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_move(value_type&& item)
{
    primary_.emplace_back(std::move(item));
}

// Be explicit about the intent to move or copy, to get compiler help.
void program::push_copy(const value_type& item)
{
    primary_.emplace_back(item);
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
    int32_t index;
    if (!pop(index))
        return false;

    // Ensure the index is within bounds.
    if (index < 0 || index >= size())
        return false;

    out_position = position(static_cast<size_t>(index));
    return true;
}

// pop1/pop2/.../pop[count]
bool program::pop(data_stack& section, size_t count)
{
    if (size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
        section.emplace_back(pop());

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

bool program::stack_to_bool() const
{
    const auto& back = primary_.back();
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

bool program::empty() const
{
    return primary_.empty();
}

// This must be guarded (intended for interpreter internal use).
bool program::stack_true() const
{
    BITCOIN_ASSERT(!empty());
    return stack_to_bool();
}

// This is safe to call when empty (intended for completion handlers).
bool program::stack_result() const
{
    return !empty() && stack_true();
}

bool program::is_stack_overflow() const
{
    // bit.ly/2cowHlP
    // Addition is safe due to script size validation.
    return size() + alternate_.size() > max_stack_size;
}

bool program::if_(const operation& op) const
{
    // Skip operation if failed and the operator is unconditional.
    return op.is_conditional() || succeeded();
}

const data_stack::value_type& program::item(size_t index) /*const*/
{
    return *position(index);
}

program::stack_iterator program::position(size_t index) /*const*/
{
    // Subtracting 1 makes the stack indexes zero-based (unlike satoshi).
    BITCOIN_ASSERT(index < size());
    return (primary_.end() - 1) - index;
}

// Pop jump-to-end, push all back, use to construct a script.
operation::list program::subscript() const
{
    operation::list ops;

    for (auto op = jump(); op != end(); ++op)
        ops.push_back(*op);

    return ops;
}

size_t program::size() const
{
    return primary_.size();
}

// Alternate stack.
//-----------------------------------------------------------------------------

bool program::empty_alternate() const
{
    return alternate_.empty();
}

void program::push_alternate(value_type&& value)
{
    alternate_.emplace_back(std::move(value));
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
    condition_.push_back(value);
}

// This must be guarded.
void program::negate()
{
    BITCOIN_ASSERT(!closed());
    condition_.back() = !condition_.back();
}

// This must be guarded.
void program::close()
{
    BITCOIN_ASSERT(!closed());
    condition_.pop_back();
}

bool program::closed() const
{
    return condition_.empty();
}

bool program::succeeded() const
{
    const auto is_true = [](bool value) { return value; };
    return std::all_of(condition_.begin(), condition_.end(), is_true);
}

} // namespace machine
} // namespace libbitcoin

#endif
