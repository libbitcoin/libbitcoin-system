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

#include <iterator>
#include <utility>
#include <variant>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/interpreter.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace system::chain;
using namespace system::error;

// Constructors.
// ----------------------------------------------------------------------------

// Input script run (default/empty stack).
// 'tx' must remain in scope, this holds state referenced by weak pointers.
// This expectation is guaranteed by the retained tx reference.
template <typename Stack>
inline program<Stack>::
program(const chain::transaction& tx, const input_iterator& input,
     uint32_t forks) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_((*input)->script_ptr()),
    forks_(forks),
    value_(max_uint64),
    version_(script_version::unversioned),
    witness_(),
    primary_()
{
}

// Legacy p2sh or prevout script run (copied input stack - use first).
// 'other' must remain in scope, this holds state referenced by weak pointers.
// This expectation is guaranteed by the retained transaction_ member reference
// and copied program tether (which is not tx state).
template <typename Stack>
inline program<Stack>::
program(const program& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    witness_(),
    primary_(other.primary_)
{
}

// Legacy p2sh or prevout script run (moved input stack/tether - use last).
template <typename Stack>
inline program<Stack>::
program(program&& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    forks_(other.forks_),
    value_(other.value_),
    version_(other.version_),
    witness_(),
    primary_(std::move(other.primary_))
{
}

// Witness script run (witness-initialized stack).
// 'tx', 'input' (and iterated chain::input) must remain in scope, as these
// hold chunk state weak references. A witness pointer is explicitly retained
// to guarantee the lifetime of its elements.
template <typename Stack>
inline program<Stack>::
program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, script_version version,
    const chunk_cptrs_ptr& witness) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_(script),
    forks_(forks),
    value_((*input)->prevout->value()),
    version_(version),
    witness_(witness),
    primary_(projection<Stack>(*witness))
{
}

// Public.
// ----------------------------------------------------------------------------

template <typename Stack>
inline bool program<Stack>::
is_true(bool clean_stack) const NOEXCEPT
{
    return (!clean_stack || is_stack_clean()) && !is_stack_empty() &&
        peek_bool_();
}

template <typename Stack>
inline const data_chunk& program<Stack>::
pop() NOEXCEPT
{
    BC_ASSERT_MSG(!is_stack_empty(), "pop from empty stack");

    return *pop_chunk_();
}

// Non-public.
// ============================================================================

template <typename Stack>
INLINE bool program<Stack>::
is_prefail() const NOEXCEPT
{
    return script_->is_prefail();
}

template <typename Stack>
INLINE typename program<Stack>::op_iterator program<Stack>::
begin() const NOEXCEPT
{
    return script_->ops().begin();
}

template <typename Stack>
INLINE typename program<Stack>::op_iterator program<Stack>::
end() const NOEXCEPT
{
    return script_->ops().end();
}

template <typename Stack>
INLINE const chain::input& program<Stack>::
input() const NOEXCEPT
{
    return **input_;
}

template <typename Stack>
INLINE const chain::transaction& program<Stack>::
transaction() const NOEXCEPT
{
    return transaction_;
}

template <typename Stack>
INLINE bool program<Stack>::
is_enabled(chain::forks rule) const NOEXCEPT
{
    return to_bool(forks_ & rule);
}

// TODO: only perform is_push_size check on witness initialized stack.
// TODO: others are either empty or presumed push_size from prevout script run.
template <typename Stack>
INLINE script_error_t program<Stack>::
validate() const NOEXCEPT
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

// Primary stack (conversions).
// ----------------------------------------------------------------------------

// static
template <typename Stack>
INLINE bool program<Stack>::
equal_chunks(const stack_variant& left, const stack_variant& right) NOEXCEPT
{
    return primary_stack::equal_chunks(left, right);
}

template <typename Stack>
INLINE bool program<Stack>::
peek_bool_() const NOEXCEPT
{
    return primary_.peek_bool();
}

template <typename Stack>
INLINE chunk_xptr program<Stack>::
peek_chunk_() const NOEXCEPT
{
    return primary_.peek_chunk();
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// This is the only source of push (write) tethering.
template <typename Stack>
INLINE void program<Stack>::
push_chunk(data_chunk&& datum) NOEXCEPT
{
    primary_.push(std::move(datum));
}

// Passing data_chunk& would be poor interface design, as it would allow
// derived callers to (unsafely) store raw pointers to unshared data_chunk.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)
template <typename Stack>
INLINE void program<Stack>::
push_chunk(const chunk_cptr& datum) NOEXCEPT
BC_POP_WARNING()
BC_POP_WARNING()
{
    primary_.emplace_chunk(datum.get());
}

// private
template <typename Stack>
INLINE void program<Stack>::
push_chunk(const chunk_xptr& datum) NOEXCEPT
{
    primary_.emplace_chunk(datum);
}

template <typename Stack>
INLINE void program<Stack>::
push_bool(bool value) NOEXCEPT
{
    primary_.emplace_boolean(value);
}

template <typename Stack>
INLINE void program<Stack>::
push_signed64(int64_t value) NOEXCEPT
{
    primary_.emplace_integer(value);
}

template <typename Stack>
INLINE void program<Stack>::
push_length(size_t value) NOEXCEPT
{
    // This is guarded by stack size and push data limits.
    BC_ASSERT_MSG(value <= max_int64, "integer overflow");

    push_signed64(possible_narrow_sign_cast<int64_t>(value));
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

// This tethers a chunk if the stack value is not chunk.
template <typename Stack>
INLINE chunk_xptr program<Stack>::
pop_chunk_() NOEXCEPT
{
    const auto value = peek_chunk_();
    drop_();
    return value;
}

// This tethers chunks if the stack values are not chunk.
template <typename Stack>
INLINE bool program<Stack>::
pop_chunks(chunk_xptrs& data, size_t count) NOEXCEPT
{
    if (stack_size() < count)
        return false;

    data.reserve(count);
    for (size_t index = 0; index < count; ++index)
        data.push_back(pop_chunk_());

    return true;
}

template <typename Stack>
INLINE bool program<Stack>::
pop_bool_() NOEXCEPT
{
    const auto value = peek_bool_();
    drop_();
    return value;
}

template <typename Stack>
INLINE bool program<Stack>::
pop_strict_bool_() NOEXCEPT
{
    const auto value = primary_.peek_strict_bool();
    drop_();
    return value;
}

// private
template <typename Stack>
INLINE bool program<Stack>::
pop_signed32_(int32_t& value) NOEXCEPT
{
    const auto result = peek_signed32_(value);
    drop_();
    return result;
}

template <typename Stack>
INLINE bool program<Stack>::
pop_signed32(int32_t& value) NOEXCEPT
{
    if (is_stack_empty())
        return false;

    return pop_signed32_(value);
}

template <typename Stack>
INLINE bool program<Stack>::
pop_binary32(int32_t& left, int32_t& right) NOEXCEPT
{
    if (stack_size() < 2)
        return false;

    // The right hand side operand is at the top of the stack.
    return pop_signed32_(right) && pop_signed32_(left);
}

template <typename Stack>
INLINE bool program<Stack>::
pop_ternary32(int32_t& upper, int32_t& lower,
    int32_t& value) NOEXCEPT
{
    if (stack_size() < 3)
        return false;

    // The upper bound is at stack top, lower bound next, value next.
    return pop_signed32_(upper) && pop_signed32_(lower) &&
        pop_signed32_(value);
}

// ************************************************************************
// CONSENSUS: Satoshi limits this value to the int32_t domain (getint()).
// This value is only used for stack indexing (key/sig counts & pick/roll).
// The upper bound of int32_t always exceeds the possible stack size, which
// is checked downstream. Similarly, a negative causes a downstream script
// failure. As such it is sufficient to fail on non-idexability here,
// allowing the value to be returned as a valid and unsigned stack index.
// ************************************************************************
template <typename Stack>
INLINE bool program<Stack>::
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
template <typename Stack>
INLINE bool program<Stack>::
peek_signed32_(int32_t& value) const NOEXCEPT
{
    return primary_.peek_signed4(value);
}

// private
template <typename Stack>
INLINE bool program<Stack>::
peek_signed40_(int64_t& value) const NOEXCEPT
{
    return primary_.peek_signed5(value);
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.sequence allows use of the full unsigned 32 bit domain, without use of
// the negative range.
// ****************************************************************************
template <typename Stack>
INLINE bool program<Stack>::
peek_unsigned32(uint32_t& value) const NOEXCEPT
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_(signed64) || is_negative(value))
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
INLINE bool program<Stack>::
peek_unsigned40(uint64_t& value) const NOEXCEPT
{
    if (is_stack_empty())
        return false;

    int64_t signed64;
    if (!peek_signed40_(signed64) || is_negative(value))
        return false;

    // 40 bits are usable in unsigned tx.locktime compare.
    value = sign_cast<uint64_t>(signed64);
    return true;
}

// Primary stack (variant - index).
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

// This swaps the variant elements of the stack vector.
template <typename Stack>
INLINE void program<Stack>::
swap_(size_t left_index, size_t right_index) NOEXCEPT
{
    primary_.swap(left_index, right_index);
}

template <typename Stack>
INLINE void program<Stack>::
erase_(size_t index) NOEXCEPT
{
    primary_.erase(index);
}

template <typename Stack>
INLINE const stack_variant& program<Stack>::
peek_(size_t index) const NOEXCEPT
{
    return primary_.peek(index);
}

// Primary stack (variant - top).
// ----------------------------------------------------------------------------

template <typename Stack>
INLINE void program<Stack>::
drop_() NOEXCEPT
{
    primary_.drop();
}

template <typename Stack>
INLINE void program<Stack>::
push_variant(const stack_variant& vary) NOEXCEPT
{
    primary_.push(vary);
}

template <typename Stack>
INLINE const stack_variant& program<Stack>::
peek_() const NOEXCEPT
{
    return primary_.top();
}

template <typename Stack>
INLINE stack_variant program<Stack>::
pop_() NOEXCEPT
{
    return primary_.pop();
}

// Primary stack state (untyped).
// ----------------------------------------------------------------------------

template <typename Stack>
INLINE size_t program<Stack>::
stack_size() const NOEXCEPT
{
    return primary_.size();
}

template <typename Stack>
INLINE bool program<Stack>::
is_stack_empty() const NOEXCEPT
{
    return primary_.empty();
}

template <typename Stack>
INLINE bool program<Stack>::
is_stack_overflow() const NOEXCEPT
{
    // Addition is safe due to stack size constraint.
    return (stack_size() + alternate_.size()) > max_stack_size;
}

// private
template <typename Stack>
INLINE bool program<Stack>::
is_stack_clean() const NOEXCEPT
{
    return stack_size() == one;
}

// Alternate stack.
// ----------------------------------------------------------------------------

template <typename Stack>
INLINE bool program<Stack>::
is_alternate_empty() const NOEXCEPT
{
    return alternate_.empty();
}

// Moving a shared pointer to the alternate stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
template <typename Stack>
INLINE void program<Stack>::
push_alternate(stack_variant&& vary) NOEXCEPT
BC_POP_WARNING()
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    alternate_.push_back(std::move(vary));
    BC_POP_WARNING()
}

template <typename Stack>
INLINE stack_variant program<Stack>::
pop_alternate_() NOEXCEPT
{
    BC_ASSERT(!alternate_.empty());

    stack_variant value{ std::move(alternate_.back()) };
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------

template <typename Stack>
INLINE void program<Stack>::
begin_if(bool value) NOEXCEPT
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(value || !is_add_overflow(negative_condition_count_, one));

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
INLINE void program<Stack>::
else_if_() NOEXCEPT
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    // Addition is safe due to script size constraint.
    BC_ASSERT(condition_.back() ||
        !is_add_overflow(negative_condition_count_, one));

    negative_condition_count_ += (condition_.back() ? 1 : -1);
    condition_.back() = !condition_.back();
}

template <typename Stack>
INLINE void program<Stack>::
end_if_() NOEXCEPT
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    negative_condition_count_ += (condition_.back() ? 0 : -1);
    condition_.pop_back();
}

template <typename Stack>
INLINE bool program<Stack>::
is_balanced() const NOEXCEPT
{
    return condition_.empty();
}

template <typename Stack>
INLINE bool program<Stack>::
is_succeess() const NOEXCEPT
{
    // Optimization changes O(n) search [for every operation] to O(1).
    // bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
    return is_zero(negative_condition_count_);
}

template <typename Stack>
INLINE bool program<Stack>::
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
    return count > max_counted_ops;
}

template <typename Stack>
INLINE bool program<Stack>::
ops_increment(const operation& op) NOEXCEPT
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operation_count_, one));

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return operation_count_ <= max_counted_ops;
}

template <typename Stack>
INLINE bool program<Stack>::
ops_increment(size_t public_keys) NOEXCEPT
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operation_count_, public_keys));

    operation_count_ += public_keys;
    return !operation_count_exceeded(operation_count_);
}

// Signature validation helpers.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. However, concurrent execution of any
// one input script instance is not thread safe (unnecessary scenario).
template <typename Stack>
inline bool program<Stack>::
set_subscript(const op_iterator& op) NOEXCEPT
{
    // End is not reachable if op is an element of script_.
    if (script_->ops().empty() || op == script_->ops().end())
        return false;

    // Advance the offset to the op following the found code separator.
    // This is non-const because changes script state (despite being mutable).
    script_->offset = std::next(op);
    return true;
}

using strippers = std::vector<stripper, no_fill_allocator<stripper>>;
inline strippers create_strip_ops(const chunk_xptrs& endorsements) NOEXCEPT
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
subscript(const chunk_xptrs& endorsements) const NOEXCEPT
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
    if (!is_intersecting<operations>(offset, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    // Prefail is not copied to the subscript, used only for signature hash.
    return to_shared<script>(difference<operations>(offset, stop, strip));
}

// TODO: use sighash and key to generate signature in sign mode.
template <typename Stack>
inline bool program<Stack>::
prepare(ec_signature& signature, const data_chunk&, hash_digest& hash,
    const chunk_xptr& endorsement) const NOEXCEPT
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
    uint8_t& flags, const data_chunk& endorsement, const script& sub) const NOEXCEPT
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
INLINE hash_digest program<Stack>::
signature_hash(const script& sub, uint8_t flags) const NOEXCEPT
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
INLINE void program<Stack>::
signature_hash(hash_cache& cache, const script& sub,
    uint8_t flags) const NOEXCEPT
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
