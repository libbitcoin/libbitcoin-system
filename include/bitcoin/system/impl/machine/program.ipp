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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_IPP

#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Public.
// ----------------------------------------------------------------------------

TEMPLATE
inline bool CLASS::
is_true(bool clean_stack) const NOEXCEPT
{
    return (!clean_stack || is_stack_clean()) && !is_stack_empty() &&
        peek_bool_();
}

TEMPLATE
inline const data_chunk& CLASS::
pop() NOEXCEPT
{
    BC_ASSERT_MSG(!is_stack_empty(), "pop from empty stack");

    return *pop_chunk_();
}

// Non-public.
// ============================================================================

TEMPLATE
INLINE error::script_error_t CLASS::
initialize() const NOEXCEPT
{
    // TODO: nops rule must first be enabled in tests and config.
    constexpr auto nops = true; ////is_enabled(flags::nops_rule);
    const auto bip141 = is_enabled(flags::bip141_rule);
    const auto bip342 = is_enabled(flags::bip342_rule);

    // Apply stack element limit (520) to initial witness [bip141][bip342].
    if (bip141 && witness_ && !chain::witness::is_push_size(*witness_))
        return error::invalid_witness_stack;

    // Script size limit (10,000) [0.3.7+], removed [bip342].
    if (!bip342 && nops && script_->is_oversized())
        return error::invalid_script_size;

    // Stacks element limit (1,000) applied to initial stack [bip342].
    if (bip342 && is_stack_overflow())
        return error::invalid_stack_size;

    // Succeed if any success code, one overrides all codes [bip342].
    if (bip342 && script_->is_prevalid())
        return error::prevalid_script;

    // Fail if last op underflow, lower priority than success codes [bip342].
    if (script_->is_underflow())
        return error::invalid_script;

    // Fail if any op invalid (invalid codes reduced in tapscript).
    // Should be after underflow check since underflow is also an invalid op.
    // Promoted success codes are not reachable here due to is_prevalid above.
    // So only op_verif/op_vernotif (unpromoted invalids) are caught here for
    // tapscript, otherwise is_prevalid bypassed and all invalids caught here.
    if (script_->is_prefail())
        return error::prefail_script;

    return error::script_success;
}

TEMPLATE
INLINE typename CLASS::op_iterator CLASS::
begin() const NOEXCEPT
{
    return script_->ops().begin();
}

TEMPLATE
INLINE typename CLASS::op_iterator CLASS::
end() const NOEXCEPT
{
    return script_->ops().end();
}

TEMPLATE
INLINE const chain::transaction& CLASS::
tx() const NOEXCEPT
{
    return transaction_;
}

TEMPLATE
INLINE const chain::input& CLASS::
input() const NOEXCEPT
{
    return **input_;
}

TEMPLATE
INLINE bool CLASS::
is_enabled(flags flag) const NOEXCEPT
{
    return to_bool(flags_ & flag);
}

// Primary stack (conversions).
// ----------------------------------------------------------------------------

// static
TEMPLATE
INLINE bool CLASS::
equal_chunks(const stack_variant& left, const stack_variant& right) NOEXCEPT
{
    return primary_stack::equal_chunks(left, right);
}

TEMPLATE
INLINE bool CLASS::
peek_bool_() const NOEXCEPT
{
    return primary_.peek_bool();
}

TEMPLATE
INLINE chunk_xptr CLASS::
peek_chunk_() const NOEXCEPT
{
    return primary_.peek_chunk();
}

TEMPLATE
INLINE size_t CLASS::
peek_size() const NOEXCEPT
{
    return primary_.peek_size();
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// This is the only source of push (write) tethering.
TEMPLATE
INLINE void CLASS::
push_chunk(data_chunk&& datum) NOEXCEPT
{
    primary_.push(std::move(datum));
}

// Passing data_chunk& would be poor interface design, as it would allow
// derived callers to (unsafely) store raw pointers to unshared data_chunk.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)
TEMPLATE
INLINE void CLASS::
push_chunk(const chunk_cptr& datum) NOEXCEPT
BC_POP_WARNING()
BC_POP_WARNING()
{
    primary_.emplace_chunk(datum.get());
}

// private
TEMPLATE
INLINE void CLASS::
push_chunk(const chunk_xptr& datum) NOEXCEPT
{
    primary_.emplace_chunk(datum);
}

TEMPLATE
INLINE void CLASS::
push_bool(bool value) NOEXCEPT
{
    primary_.emplace_boolean(value);
}

TEMPLATE
INLINE void CLASS::
push_signed64(int64_t value) NOEXCEPT
{
    primary_.emplace_integer(value);
}

TEMPLATE
INLINE void CLASS::
push_length(size_t value) NOEXCEPT
{
    // This is guarded by stack size and push data limits.
    BC_ASSERT_MSG(value <= max_int64, "integer overflow");

    push_signed64(possible_narrow_sign_cast<int64_t>(value));
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

// This tethers a chunk if the stack value is not chunk.
TEMPLATE
INLINE chunk_xptr CLASS::
pop_chunk_() NOEXCEPT
{
    const auto value = peek_chunk_();
    drop_();
    return value;
}

// This tethers chunks if the stack values are not chunk.
TEMPLATE
INLINE bool CLASS::
pop_chunks(chunk_xptrs& data, size_t count) NOEXCEPT
{
    if (stack_size() < count)
        return false;

    data.reserve(count);
    for (size_t index = 0; index < count; ++index)
        data.push_back(pop_chunk_());

    return true;
}

TEMPLATE
INLINE bool CLASS::
pop_strict_bool_() NOEXCEPT
{
    const auto value = primary_.peek_strict_bool();
    drop_();
    return value;
}

TEMPLATE
INLINE bool CLASS::
pop_bool_(bool& value, bool minimal) NOEXCEPT
{
    if (!minimal)
        value = peek_bool_();
    else if (!primary_.peek_minimal_bool(value))
        return false;

    drop_();
    return true;
}

TEMPLATE
INLINE bool CLASS::
pop_signed32_(int32_t& value) NOEXCEPT
{
    const auto result = peek_signed32_(value);
    drop_();
    return result;
}

TEMPLATE
INLINE bool CLASS::
pop_signed32(int32_t& value) NOEXCEPT
{
    if (is_stack_empty())
        return false;

    return pop_signed32_(value);
}

TEMPLATE
INLINE bool CLASS::
pop_binary32(int32_t& left, int32_t& right) NOEXCEPT
{
    if (stack_size() < 2)
        return false;

    // The right hand side operand is at the top of the stack.
    return pop_signed32_(right) && pop_signed32_(left);
}

TEMPLATE
INLINE bool CLASS::
pop_ternary32(int32_t& upper, int32_t& lower,
    int32_t& value) NOEXCEPT
{
    if (stack_size() < 3)
        return false;

    // The upper bound is at stack top, lower bound next, value next.
    return pop_signed32_(upper) && pop_signed32_(lower) &&
        pop_signed32_(value);
}

// ****************************************************************************
// CONSENSUS: Satoshi limits this value to the int32_t domain (getint()).
// This value is only used for stack indexing (key/sig counts & pick/roll).
// The upper bound of int32_t always exceeds the possible stack size, which
// is checked downstream. Similarly, a negative causes a downstream script
// failure. As such it is sufficient to fail on non-idexability here,
// allowing the value to be returned as a valid and unsigned stack index.
// ****************************************************************************
TEMPLATE
INLINE bool CLASS::
pop_index32(size_t& index) NOEXCEPT
{
    int32_t value;
    if (!pop_signed32(value))
        return false;

    if (is_negative(value))
        return false;

    // Cast guarded by stack size.
    index = limit<size_t>(value);

    // True if popped value valid post-pop stack index (precluded if size < 2).
    return index < stack_size();
}

// private
TEMPLATE
INLINE bool CLASS::
peek_signed32_(int32_t& value) const NOEXCEPT
{
    return primary_.peek_signed4(value);
}

// private
TEMPLATE
INLINE bool CLASS::
peek_signed40_(int64_t& value) const NOEXCEPT
{
    return primary_.peek_signed5(value);
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.sequence allows use of the full unsigned 32 bit domain, without use of
// the negative range.
// ****************************************************************************
TEMPLATE
INLINE bool CLASS::
peek_unsigned32(uint32_t& value) const NOEXCEPT
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_(signed64) || is_negative(signed64))
        return false;

    // 32 bits are used in unsigned input.sequence compare.
    value = narrow_sign_cast<uint32_t>(signed64);
    return true;
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.locktime allows use of the full unsigned 32 bit domain, without use of
// the negative range. Otherwise a 2038 limit (vs. the inherent 2106 limit)
// would have been introduced.
// ****************************************************************************
TEMPLATE
INLINE bool CLASS::
peek_unsigned40(uint64_t& value) const NOEXCEPT
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_(signed64) || is_negative(signed64))
        return false;

    // 40 bits are usable in unsigned tx.locktime compare.
    value = sign_cast<uint64_t>(signed64);
    return true;
}

// Primary stack (variant - index).
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

// This swaps the variant elements of the stack vector.
TEMPLATE
INLINE void CLASS::
swap_(size_t left_index, size_t right_index) NOEXCEPT
{
    primary_.swap(left_index, right_index);
}

TEMPLATE
INLINE void CLASS::
erase_(size_t index) NOEXCEPT
{
    primary_.erase(index);
}

TEMPLATE
INLINE const stack_variant& CLASS::
peek_(size_t index) const NOEXCEPT
{
    return primary_.peek(index);
}

// Primary stack (variant - top).
// ----------------------------------------------------------------------------

TEMPLATE
INLINE void CLASS::
drop_() NOEXCEPT
{
    primary_.drop();
}

TEMPLATE
INLINE void CLASS::
push_variant(const stack_variant& vary) NOEXCEPT
{
    primary_.push(vary);
}

TEMPLATE
INLINE const stack_variant& CLASS::
peek_() const NOEXCEPT
{
    return primary_.top();
}

TEMPLATE
INLINE stack_variant CLASS::
pop_() NOEXCEPT
{
    return primary_.pop();
}

// Primary stack state (untyped).
// ----------------------------------------------------------------------------

TEMPLATE
INLINE size_t CLASS::
stack_size() const NOEXCEPT
{
    return primary_.size();
}

TEMPLATE
INLINE bool CLASS::
is_stack_empty() const NOEXCEPT
{
    return primary_.empty();
}

TEMPLATE
INLINE bool CLASS::
is_stack_overflow() const NOEXCEPT
{
    // Addition is safe due to stack size constraint.
    // Limit of 1000 elements in stack and altstack remains [bip342]. 
    return (stack_size() + alternate_.size()) > chain::max_unified_stack_size;
}

// private
TEMPLATE
INLINE bool CLASS::
is_stack_clean() const NOEXCEPT
{
    return is_one(stack_size());
}

// Alternate stack.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE bool CLASS::
is_alternate_empty() const NOEXCEPT
{
    return alternate_.empty();
}

// Moving a shared pointer to the alternate stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
TEMPLATE
INLINE void CLASS::
push_alternate(stack_variant&& vary) NOEXCEPT
BC_POP_WARNING()
{
    alternate_.push_back(std::move(vary));
}

TEMPLATE
INLINE stack_variant CLASS::
pop_alternate_() NOEXCEPT
{
    BC_ASSERT(!alternate_.empty());

    stack_variant value{ std::move(alternate_.back()) };
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE void CLASS::
begin_if(bool value) NOEXCEPT
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(value || !is_add_overflow(negative_conditions_, one));

    negative_conditions_ += (value ? 0 : 1);
    condition_.push_back(value);
}

// ****************************************************************************
// CONSENSUS: "You may have noticed the strange behavior of Bitcoin's ELSE
// statement. Bitcoin allows one to switch between true and false conditions
// several times. For example, the following script is valid and leaves the
// value 2 on the stack: 1 OP_IF OP_ELSE OP_ELSE 2 OP_ENDIF"
// bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
// ****************************************************************************
TEMPLATE
INLINE void CLASS::
else_if_() NOEXCEPT
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    // Addition is safe due to script size constraint.
    BC_ASSERT(condition_.back() || !is_add_overflow(negative_conditions_, one));

    negative_conditions_ += (condition_.back() ? 1 : -1);
    condition_.back() = !condition_.back();
}

TEMPLATE
INLINE void CLASS::
end_if_() NOEXCEPT
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    negative_conditions_ += (condition_.back() ? 0 : -1);
    condition_.pop_back();
}

TEMPLATE
INLINE bool CLASS::
is_balanced() const NOEXCEPT
{
    return condition_.empty();
}

TEMPLATE
INLINE bool CLASS::
is_succeess() const NOEXCEPT
{
    // Optimization changes O(n) search [for every operation] to O(1).
    // bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
    return is_zero(negative_conditions_);
}

TEMPLATE
INLINE bool CLASS::
if_(const operation& op) const NOEXCEPT
{
    // Conditional op execution is not predicated on conditional stack.
    return op.is_conditional() || is_succeess();
}

//  Accumulator.
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS:
// Satoshi compares the count to 200 with a composed postfix increment, which
// makes the actual maximum 201, not the presumably-intended 200. The code was
// later revised to make this explicit, by use of a prefix increment against a
// limit of 201.
// ****************************************************************************
INLINE constexpr bool operation_count_exceeded(size_t count) NOEXCEPT
{
    return count > chain::max_counted_ops;
}

TEMPLATE
INLINE bool CLASS::
sigops_increment() NOEXCEPT
{
    BC_ASSERT(is_enabled(flags::bip342_rule));

    // Executing signature op with non-empty signature decrements budget by 50.
    // If the budget falls below zero script fails immediately [bip342].

    // Budget initialized add1(50) to make it zero-based, avoiding signed type.
    budget_ = floored_subtract(budget_, chain::signature_cost);
    return !is_zero(budget_);
}

TEMPLATE
INLINE bool CLASS::
ops_increment(const operation& op) NOEXCEPT
{
    // Non-push opcodes limit of 201 per script does not apply [bip342].
    if (is_enabled(flags::bip342_rule))
        return true;

    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operations_, one));

    if (operation::is_counted(op.code()))
        ++operations_;

    return operations_ <= chain::max_counted_ops;
}

TEMPLATE
INLINE bool CLASS::
ops_increment(size_t public_keys) NOEXCEPT
{
    // Non-push opcodes limit of 201 per script does not apply [bip342].
    if (is_enabled(flags::bip342_rule))
        return true;

    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operations_, public_keys));

    operations_ += public_keys;
    return !operation_count_exceeded(operations_);
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
