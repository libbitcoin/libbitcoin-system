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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <iterator>
#include <type_traits>
#include <utility>
#include <variant>
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
using namespace system::error;

constexpr auto unused_value = max_uint64;
constexpr auto unused_version = script_version::unversioned;

// Hash results and int/bool->chunks are saved using a shared_ptr vector.
// The tether is not garbage-collected (until destruct) as this is a space-
// time performance tradeoff. The maximum number of constructable chunks is
// bound by the script size limit. A standard in/out script pair tethers
// only one chunk, the computed hash.
//
// The following script operations will ALWAYS tether chunks, as the result
// of a computed hash is *pushed* to the weak pointer (xptr) variant stack.
//
// op_ripemd160         (1)
// op_sha1              (1)
// op_sha256            (1)
// op_hash160           (1)
// op_hash256           (1)
//
// The following script operations will ONLY tether chunks in the case where
// the *popped* element was originally bool/int64_t but required as chunk.
// This is never the case in standard scripts.
//
// op_ripemd160         (0..1)
// op_sha1              (0..1)
// op_sha256            (0..1)
// op_hash160           (0..1)
// op_hash256           (0..1)
// op_size              (0..1)
// op_check_sig         (0..2, and m (endorsements) + n (keys))
// op_check_sig_verify  (0..2, and m (endorsements) + n (keys))

// Constructors.
// ----------------------------------------------------------------------------

// Input script run (default/empty stack).
// 'tx' must remain in scope, this holds state referenced by weak pointers.
// This expecation is guaranteed by the retained tx reference.
template <typename Stack>
inline program<Stack>::
program(const chain::transaction& tx, const input_iterator& input,
     uint32_t forks) noexcept
  : transaction_(tx),
    input_(input),
    script_((*input)->script_ptr()),
    forks_(forks),
    value_(unused_value),
    version_(unused_version),
    tether_(),
    witness_(),
    primary_()
{
}

// Legacy p2sh or prevout script run (copied input stack - use first).
// 'other' must remain in scope, this holds state referenced by weak pointers.
// This expecation is guaranteed by the retained transaction_ member reference
// and copy of other.tether_ (not tx state).
template <typename Stack>
inline program<Stack>::
program(const program& other, const script::cptr& script) noexcept
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    tether_(other.tether_),
    witness_(),
    primary_(other.primary_)
{
}

// Legacy p2sh or prevout script run (moved input stack - use last).
template <typename Stack>
inline program<Stack>::
program(program&& other, const script::cptr& script) noexcept
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    tether_(std::move(other.tether_)),
    witness_(),
    primary_(std::move(other.primary_))
{
}

// Witness script run (witness-initialized stack).
// 'tx', 'input' (and iterated chain::input), and 'witness' must remain in
// scope, as these hold chunk state referenced by weak pointers. This
// expecation is guaranteed by the retained tx and input reference. A reference
// to witness is explicitly retained to guarantee the lifetime of its elements.
template <typename Stack>
inline program<Stack>::
program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, script_version version,
    const chunk_cptrs_ptr& witness) noexcept
  : transaction_(tx),
    input_(input),
    script_(script),
    forks_(forks),
    value_((*input)->prevout->value()),
    version_(version),
    tether_(),
    witness_(witness),
    primary_(pointer_cast<variant>(*witness))
{
}

// Public.
// ----------------------------------------------------------------------------

template <typename Stack>
inline bool program<Stack>::
is_true(bool clean_stack) const noexcept
{
    return (!clean_stack || is_stack_clean()) && !is_stack_empty() &&
        peek_bool_unsafe();
}

template <typename Stack>
inline const data_chunk& program<Stack>::pop() noexcept
{
    BC_ASSERT_MSG(!is_stack_empty(), "pop from empty stack");

    return *pop_chunk_unsafe();
}

// Non-public.
// ============================================================================

template <typename Stack>
inline bool program<Stack>::
is_prefail() const noexcept
{
    return script_->is_prefail();
}

template <typename Stack>
inline typename program<Stack>::op_iterator program<Stack>::
begin() const noexcept
{
    return script_->ops().begin();
}

template <typename Stack>
inline typename program<Stack>::op_iterator program<Stack>::
end() const noexcept
{
    return script_->ops().end();
}

template <typename Stack>
inline const chain::input& program<Stack>::
input() const noexcept
{
    return **input_;
}

template <typename Stack>
inline const chain::transaction& program<Stack>::
transaction() const noexcept
{
    return transaction_;
}

template <typename Stack>
inline bool program<Stack>::
is_enabled(chain::forks rule) const noexcept
{
    return to_bool(forks_ & rule);
}


// TODO: only perform is_push_size check on witness initialized stack.
// TODO: others are either empty or presumed push_size from prevout script run.
template <typename Stack>
inline script_error_t program<Stack>::
validate() const noexcept
{
    // TODO: nops rule must first be enabled in tests and config.
    const auto bip141 = is_enabled(forks::bip141_rule);

    // The script was determined by the parser to contain an invalid opcode.
    if (is_prefail())
        return error::prefail_script;

    // bip_141 introduces an initialized stack, so must validate.
    if (bip141 && witness_ && !witness::is_push_size(*witness_))
        return error::invalid_witness_stack;

    // The nops_rule establishes script size limit.
    return script_->is_oversized() ? error::invalid_script_size :
        error::script_success;
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// This is the only source of push (write) tethering.
template <typename Stack>
inline void program<Stack>::
push_chunk(data_chunk&& datum) noexcept
{
    primary_.push_back(make_external(std::move(datum), tether_));
}

// Passing data_chunk& would be poor interface design, as it would allow
// derived callers to (unsafely) store raw pointers to unshared data_chunk.
// While a shared pointer could be removed from scope after this call, the
// design expects that all shared pointers have the lifetime of this class,
// While data_chunks are constructed as a consequence of hashing operations.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)
template <typename Stack>
inline void program<Stack>::
push_chunk(const chunk_cptr& datum) noexcept
BC_POP_WARNING()
BC_POP_WARNING()
{
    primary_.emplace_back(datum.get());
}

// private
template <typename Stack>
inline void program<Stack>::
push_chunk(const chunk_xptr& datum) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_.emplace_back(datum);
    BC_POP_WARNING()
}

template <typename Stack>
inline void program<Stack>::
push_bool(bool value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_.emplace_back(value);
    BC_POP_WARNING()
}

template <typename Stack>
inline void program<Stack>::
push_signed64(int64_t value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_.emplace_back(value);
    BC_POP_WARNING()
}

template <typename Stack>
inline void program<Stack>::
push_length(size_t value) noexcept
{
    // This is guarded by stack size and push data limits.
    BC_ASSERT_MSG(value <= max_int64, "integer overflow");
    push_signed64(possible_narrow_sign_cast<int64_t>(value));
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

// This tethers a chunk if the stack value is not chunk.
template <typename Stack>
inline chunk_xptr program<Stack>::
pop_chunk_unsafe() noexcept
{
    const auto value = peek_chunk_unsafe();
    drop_unsafe();
    return value;
}

// This tethers chunks if the stack values are not chunk.
template <typename Stack>
inline bool program<Stack>::
pop_chunks(chunk_xptrs& data, size_t count) noexcept
{
    if (stack_size() < count)
        return false;

    data.reserve(count);
    for (size_t index = 0; index < count; ++index)
        data.push_back(pop_chunk_unsafe());

    return true;
}

template <typename Stack>
inline bool program<Stack>::
pop_bool_unsafe() noexcept
{
    const auto value = peek_bool_unsafe();
    drop_unsafe();
    return value;
}

template <typename Stack>
inline bool program<Stack>::
pop_strict_bool_unsafe() noexcept
{
    const auto value = peek_strict_bool_unsafe();
    drop_unsafe();
    return value;
}

// private
template <typename Stack>
inline bool program<Stack>::
pop_signed32_unsafe(int32_t& value) noexcept
{
    const auto result = peek_signed32_unsafe(value);
    drop_unsafe();
    return result;
}

template <typename Stack>
inline bool program<Stack>::
pop_signed32(int32_t& value) noexcept
{
    if (is_stack_empty())
        return false;

    return pop_signed32_unsafe(value);
}

template <typename Stack>
inline bool program<Stack>::
pop_binary32(int32_t& left, int32_t& right) noexcept
{
    if (stack_size() < 2)
        return false;

    // The right hand side operand is at the top of the stack.
    return pop_signed32_unsafe(right) && pop_signed32_unsafe(left);
}

template <typename Stack>
inline bool program<Stack>::
pop_ternary32(int32_t& upper, int32_t& lower,
    int32_t& value) noexcept
{
    if (stack_size() < 3)
        return false;

    // The upper bound is at stack top, lower bound next, value next.
    return pop_signed32_unsafe(upper) && pop_signed32_unsafe(lower) &&
        pop_signed32_unsafe(value);
}

// True if popped value is valid post-pop stack index (precluded if size < 2).
template <typename Stack>
inline bool program<Stack>::
pop_index32(size_t& index) noexcept
{
    int32_t value;
    if (!pop_signed32(value))
        return false;

    if (is_negative(value))
        return false;

    // Cast guarded by stack size.
    index = limit<size_t>(value);
    return index < stack_size();
}

// Primary stack (peek).
// ----------------------------------------------------------------------------

// static
// Integers are unconstrained as these are stack chunk equality comparisons.
template <typename Stack>
constexpr bool program<Stack>::equal_chunks(const variant& left,
    const variant& right) noexcept
{
    static_assert(std::variant_size_v<program<Stack>::variant> == 3);

    using namespace number;
    enum { bool_, int64_, pchunk_ };
    auto out = true;

    // Methods bound at compile time (free).
    // One runtime switch on variant index (cheap).
    // bool/int conversions are compile-time (free).
    // chunk conversions reduce to conventional bitcoin design.
    std::visit(program::overload
    {
        // This is never executed in standard scripts.
        [&](bool vary) noexcept
        {
            switch (right.index())
            {
                case bool_:
                    out = std::get<bool>(right) == vary;
                    break;
                case int64_:
                    out = std::get<int64_t>(right) == boolean::to_int(vary);
                    break;
                default:
                case pchunk_:
                    out = *std::get<chunk_xptr>(right) == chunk::from_bool(vary);
            }
        },

        // This is never executed in standard scripts.
        [&](int64_t vary) noexcept
        {
            switch (right.index())
            {
                case bool_:
                    out = boolean::to_int(std::get<bool>(right)) == vary;
                    break;
                case int64_:
                    out = std::get<int64_t>(right) == vary;
                    break;
                default:
                case pchunk_:
                    out = *std::get<chunk_xptr>(right) == chunk::from_int(vary);
            }
        },

        // This is the canonical use case.
        [&](chunk_xptr vary) noexcept
        {
            switch (right.index())
            {
                case bool_:
                    // This is never executed in standard scripts.
                    out = chunk::from_bool(std::get<bool>(right)) == *vary;
                    break;
                case int64_:
                    // This is never executed in standard scripts.
                    out = chunk::from_int(std::get<int64_t>(right)) == *vary;
                    break;
                default:
                case pchunk_:
                    // This is the canonical use case.
                    out = *std::get<chunk_xptr>(right) == *vary;
            }
        }
    }, left);

    return out;
}

// This is the only source of peek/pop (read) tethering.
template <typename Stack>
inline chunk_xptr program<Stack>::
peek_chunk_unsafe() const noexcept
{
    using namespace number;
    chunk_xptr item{};

    std::visit(overload
    {
        [&, this](bool vary) noexcept
        {
            // This is never executed in standard scripts.
            item = make_external(chunk::from_bool(vary), tether_);
        },
        [&](int64_t vary) noexcept
        {
            // This is never executed in standard scripts.
            item = make_external(chunk::from_int(vary), tether_);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is the canonical use case.
            item = vary;
        }
    }, peek_variant_unsafe());

    return item;
}

template <typename Stack>
inline bool program<Stack>::
peek_bool_unsafe() const noexcept
{
    using namespace number;
    bool item{};

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is the canonical use case.
            item = vary;
        },
        [&](int64_t vary) noexcept
        {
            // This is never executed in standard scripts.
            item = boolean::to_bool(vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is never executed in standard scripts.
            item = boolean::from_chunk(*vary);
        }
    }, peek_variant_unsafe());
    return item;
}

// private
template <typename Stack>
inline bool program<Stack>::
peek_strict_bool_unsafe() const noexcept
{
    using namespace number;
    bool item{};

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is the canonical use case (after bip147).
            item = vary;
        },
        [&](int64_t vary) noexcept
        {
            // This may be executed in standard scripts (before bip147).
            item = boolean::to_bool(vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This may be executed in standard scripts (before bip147).
            item = boolean::strict_from_chunk(*vary);
        }
    }, peek_variant_unsafe());
    return item;
}

// private
// Generalized integer peek for varying bit widths up to 64.
// Chunk to integer conversions are constrained by caller (32 or 40 bits).
template <typename Stack>
template<size_t Bytes, typename Integer,
    if_signed_integer<Integer>,
    if_integral_integer<Integer>,
    if_not_greater<Bytes, sizeof(Integer)>>
inline bool program<Stack>::
peek_signed_unsafe(Integer& value) const noexcept
{
    using namespace number;
    bool result{ true };

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is never executed in standard scripts.
            value = boolean::to_int<Bytes>(vary);
        },
        [&](int64_t vary) noexcept
        {
            // This is the canonical use case (bounds check only).
            result = integer<Bytes>::from_int(value, vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is never executed in standard scripts.
            result = integer<Bytes>::from_chunk(value , *vary);
        }
    }, peek_variant_unsafe());
    return result;
}

// private
template <typename Stack>
inline bool program<Stack>::
peek_signed32_unsafe(int32_t& value) const noexcept
{
    constexpr auto bytes4 = sizeof(int32_t);
    return peek_signed_unsafe<bytes4>(value);
}

// private
template <typename Stack>
inline bool program<Stack>::
peek_signed40_unsafe(int64_t& value) const noexcept
{
    constexpr auto bytes5 = sizeof(int32_t) + sizeof(int8_t);
    return peek_signed_unsafe<bytes5>(value);
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.sequence allows use of the full unsigned 32 bit domain, without use of
// the negative range.
// ****************************************************************************
template <typename Stack>
inline bool program<Stack>::
peek_unsigned32(uint32_t& value) const noexcept
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_unsafe(signed64) || is_negative(value))
        return false;

    // 32 bits are used in unsigned input.sequence compare.
    value = narrow_sign_cast<uint32_t>(signed64);
    return true;
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.locktime allows use of the full unsigned 32 bit domain, without use of
// the negative range. Otherwise a 2038 limit (beyond the inherent 2106 limit)
// would have been introduced.
// ****************************************************************************
template <typename Stack>
inline bool program<Stack>::
peek_unsigned40(uint64_t& value) const noexcept
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_unsafe(signed64) || is_negative(value))
        return false;

    // 40 bits are usable in unsigned tx.locktime compare.
    value = sign_cast<uint64_t>(signed64);
    return true;
}

// Primary stack (variant).
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

template <typename Stack>
inline void program<Stack>::
drop_unsafe() noexcept
{
    BC_ASSERT(!is_stack_empty());
    primary_.pop_back();
}

// This swaps the variant elements of the stack vector.
template <typename Stack>
inline void program<Stack>::
swap_unsafe(size_t left_index, size_t right_index) noexcept
{
    BC_ASSERT(left_index < stack_size() && right_index < stack_size());
    std::swap(
        primary_.at(sub1(stack_size()) - left_index),
        primary_.at(sub1(stack_size()) - right_index));
}

template <typename Stack>
inline void program<Stack>::
erase_unsafe(size_t index) noexcept
{
    primary_.erase(std::prev(primary_.end(), add1(index)));
}

template <typename Stack>
inline void program<Stack>::
push_variant(const variant& vary) noexcept
{
    primary_.push_back(vary);
}

template <typename Stack>
inline const typename program<Stack>::variant& program<Stack>::
peek_variant_unsafe() const noexcept
{
    return primary_.back();
}

template <typename Stack>
inline const typename program<Stack>::variant& program<Stack>::
peek_variant_unsafe(size_t peek_index) const noexcept
{
    return *std::prev(primary_.end(), add1(peek_index));
}

template <typename Stack>
inline typename program<Stack>::variant program<Stack>::
pop_variant_unsafe() noexcept
{
    variant temporary = std::move(primary_.back());
    drop_unsafe();
    return temporary;
}

// Primary stack state (untyped).
// ----------------------------------------------------------------------------

template <typename Stack>
inline size_t program<Stack>::
stack_size() const noexcept
{
    return primary_.size();
}

template <typename Stack>
inline bool program<Stack>::
is_stack_empty() const noexcept
{
    return primary_.empty();
}

template <typename Stack>
inline bool program<Stack>::
is_stack_overflow() const noexcept
{
    // bit.ly/2cowHlP
    // Addition is safe due to stack size constraint.
    return (stack_size() + alternate_.size()) > max_stack_size;
}

// private
template <typename Stack>
inline bool program<Stack>::
is_stack_clean() const noexcept
{
    return stack_size() == one;
}

// Alternate stack.
// ----------------------------------------------------------------------------

template <typename Stack>
inline bool program<Stack>::
is_alternate_empty() const noexcept
{
    return alternate_.empty();
}

// Moving a shared pointer to the alternate stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
template <typename Stack>
inline void program<Stack>::
push_alternate(variant&& vary) noexcept
BC_POP_WARNING()
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    alternate_.push_back(std::move(vary));
    BC_POP_WARNING()
}

template <typename Stack>
inline typename program<Stack>::variant program<Stack>::
pop_alternate_unsafe() noexcept
{
    BC_ASSERT(!alternate_.empty());

    variant value{ std::move(alternate_.back()) };
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------

template <typename Stack>
inline void program<Stack>::
begin_if(bool value) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(value || !overflows(negative_condition_count_, one));

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
template <typename Stack>
inline void program<Stack>::
else_if_unsafe() noexcept
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    // Addition is safe due to script size constraint.
    BC_ASSERT(condition_.back() || !overflows(negative_condition_count_, one));

    negative_condition_count_ += (condition_.back() ? 1 : -1);
    condition_.back() = !condition_.back();
}

template <typename Stack>
inline void program<Stack>::
end_if_unsafe() noexcept
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    negative_condition_count_ += (condition_.back() ? 0 : -1);
    condition_.pop_back();
}

template <typename Stack>
inline bool program<Stack>::
is_balanced() const noexcept
{
    return condition_.empty();
}

template <typename Stack>
inline bool program<Stack>::
is_succeess() const noexcept
{
    ////const auto is_true = [](bool value) noexcept { return value; };
    ////return std::all_of(condition_.begin(), condition_.end(), is_true);
    // Optimization changes O(n) search [for every operation] to O(1).
    // bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
    return is_zero(negative_condition_count_);
}

template <typename Stack>
inline bool program<Stack>::
if_(const operation& op) const noexcept
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
constexpr bool operation_count_exceeded(size_t count) noexcept
{
    return count > max_counted_ops;
}

template <typename Stack>
inline bool program<Stack>::
ops_increment(const operation& op) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(!overflows(operation_count_, one));

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return operation_count_ <= max_counted_ops;
}

template <typename Stack>
inline bool program<Stack>::
ops_increment(size_t public_keys) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(!overflows(operation_count_, public_keys));

    operation_count_ += public_keys;
    return !operation_count_exceeded(operation_count_);
}

// Signature validation helpers.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. However, execution of any one input
// script instance is not safe for concurrent execution (unnecessary scenario).
template <typename Stack>
inline bool program<Stack>::
set_subscript(const op_iterator& op) noexcept
{
    // End is not reachable if op is an element of script_.
    if (script_->ops().empty() || op == script_->ops().end())
        return false;

    // Advance the offset to the op following the found code separator.
    // This is non-const because changes script state (despite being mutable).
    script_->offset = std::next(op);
    return true;
}

inline strippers create_strip_ops(const chunk_xptrs& endorsements) noexcept
{
    strippers strip;

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    strip.reserve(add1(endorsements.size()));

    for (const auto& endorsement: endorsements)
        strip.emplace_back(endorsement);

    strip.emplace_back(opcode::codeseparator);
    BC_POP_WARNING()

    return strip;
}

// ****************************************************************************
// CONSENSUS: Endorsement and code separator stripping are always performed in
// conjunction and are limited to non-witness signature hash subscripts.
// The order of operations is inconsequential, as they are all removed.
// Subscripts are not evaluated, they are limited to signature hash creation.
// ****************************************************************************
template <typename Stack>
inline script::cptr program<Stack>::
subscript(const chunk_xptrs& endorsements) const noexcept
{
    // bip141: establishes the version property.
    // bip143: op stripping is not applied to bip141 v0 scripts.
    if (is_enabled(forks::bip143_rule) && version_ == script_version::zero)
        return script_;

    // Transform into a set of endorsement push ops and one op_codeseparator.
    const auto strip = create_strip_ops(endorsements);
    const auto stop = script_->ops().end();
    const op_iterator offset{ script_->offset };

    // If none of the strip ops are found, return the subscript.
    // Prefail is not circumvented as subscript used only for signature hash.
    if (!intersecting<operations>(offset, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    // Prefail is not copied to the subscript, used only for signature hash.
    BC_PUSH_WARNING(NO_NEW_DELETE)
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return to_shared(new script{ difference<operations>(offset, stop, strip) });
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// TODO: use sighash and key to generate signature in sign mode.
template <typename Stack>
inline bool program<Stack>::
prepare(ec_signature& signature, const data_chunk&, hash_digest& hash,
    const chunk_xptr& endorsement) const noexcept
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
template <typename Stack>
inline bool program<Stack>::
prepare(ec_signature& signature, const data_chunk&, hash_cache& cache,
    uint8_t& flags, const data_chunk& endorsement, const script& sub) const noexcept
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

// Signature hashing.
// ----------------------------------------------------------------------------

template <typename Stack>
inline hash_digest program<Stack>::
signature_hash(const script& sub, uint8_t flags) const noexcept
{
    // The bip141 fork establishes witness version, hashing is a distinct fork.
    const auto bip143 = is_enabled(forks::bip143_rule);

    // bip143: the method of signature hashing is changed for v0 scripts.
    return transaction_.signature_hash(input_, sub, value_, flags, version_,
        bip143);
}

// Caches signature hashes in a map against sighash flags.
// Prevents recomputation in the common case where flags are the same.
template <typename Stack>
inline void program<Stack>::
signature_hash(hash_cache& cache, const script& sub,
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

#endif
