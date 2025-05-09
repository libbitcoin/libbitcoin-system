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

#include <iterator>
#include <utility>
#include <variant>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
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
TEMPLATE
inline CLASS::
program(const chain::transaction& tx, const input_iterator& input,
     uint32_t active_flags) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_((*input)->script_ptr()),
    flags_(active_flags),
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
TEMPLATE
inline CLASS::
program(const program& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    flags_(other.flags_),
    value_(other.value_),
    version_(other.version_),
    witness_(),
    primary_(other.primary_)
{
}

// Legacy p2sh or prevout script run (moved input stack/tether - use last).
TEMPLATE
inline CLASS::
program(program&& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    flags_(other.flags_),
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
TEMPLATE
inline CLASS::
program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t active_flags, script_version version,
    const chunk_cptrs_ptr& witness) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_(script),
    flags_(active_flags),
    value_((*input)->prevout->value()),
    version_(version),
    witness_(witness),
    primary_(projection<Stack>(*witness))
{
}

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
INLINE const chain::input& CLASS::
input() const NOEXCEPT
{
    return **input_;
}

TEMPLATE
INLINE const chain::transaction& CLASS::
transaction() const NOEXCEPT
{
    return transaction_;
}

TEMPLATE
INLINE bool CLASS::
is_enabled(chain::flags flag) const NOEXCEPT
{
    return to_bool(flags_ & flag);
}

TEMPLATE
INLINE script_error_t CLASS::
validate() const NOEXCEPT
{
    // TODO: nops rule must first be enabled in tests and config.
    const auto nops = true; ////is_enabled(flags::nops_rule);
    const auto bip141 = is_enabled(flags::bip141_rule);
    const auto bip342 = is_enabled(flags::bip342_rule);

    // Apply stack element limit (520) to initial witness [bip141][tapscript].
    if (bip141 && witness_ && !witness::is_push_size(*witness_))
        return error::invalid_witness_stack;

    // Script size limit (10,000) [0.3.7+], removed [tapscript].
    if (!bip342 && nops && script_->is_oversized())
        return error::invalid_script_size;

    // Stacks element limit (1,000) applied to initial stack [tapscript].
    if (bip342 && is_stack_overflow())
        return error::invalid_stack_size;

    // Succeed if any success code, one overrides all codes [tapscript].
    if (bip342 && script_->is_prevalid())
        return error::prevalid_script;

    // Fail if last op underflow, lower priority than easy [tapscript].
    if (script_->is_underflow())
        return error::invalid_script;

    // Fail if any op invalid (invalid codes reduced in tapscript).
    if (script_->is_prefail())
        return error::prefail_script;

    return error::script_success;
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
    // Limit of 1000 elements in stack and altstack remains [tapscript]. 
    return (stack_size() + alternate_.size()) > max_unified_stack_size;
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
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    alternate_.push_back(std::move(vary));
    BC_POP_WARNING()
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
    return count > max_counted_ops;
}

TEMPLATE
INLINE bool CLASS::
ops_increment(const operation& op) NOEXCEPT
{
    // Non-push opcodes limit of 201 per script does not apply [tapscript].
    if (is_enabled(flags::bip342_rule))
        return true;

    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operations_, one));

    if (operation::is_counted(op.code()))
        ++operations_;

    return operations_ <= max_counted_ops;
}

TEMPLATE
INLINE bool CLASS::
ops_increment(size_t public_keys) NOEXCEPT
{
    // Non-push opcodes limit of 201 per script does not apply [tapscript].
    if (is_enabled(flags::bip342_rule))
        return true;

    // Addition is safe due to script size constraint.
    BC_ASSERT(!is_add_overflow(operations_, public_keys));

    operations_ += public_keys;
    return !operation_count_exceeded(operations_);
}

// Signature validation helpers.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. However, concurrent execution of any
// one input script instance is not thread safe (unnecessary scenario).
TEMPLATE
inline void CLASS::
set_subscript(const op_iterator& op) NOEXCEPT
{
    // Not possible unless op is not an element of script_.
    BC_ASSERT(!script_->ops().empty() && op != script_->ops().end());

    // Advance the offset to the op following the found code separator.
    // This is non-const because changes script state (despite being mutable).
    script_->offset = std::next(op);
}

inline chain::strippers create_strip_ops(
    const chunk_xptrs& endorsements) NOEXCEPT
{
    chain::strippers strip{};

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
TEMPLATE
inline script::cptr CLASS::
subscript(const chunk_xptrs& endorsements) const NOEXCEPT
{
    // bip141: establishes the version property.
    // bip143: op stripping is not applied to bip141 v0 scripts.
    if (is_enabled(flags::bip143_rule) && version_ == script_version::segwit)
        return script_;

    // Transform into a set of endorsement push ops and one op_codeseparator.
    const auto strip = create_strip_ops(endorsements);
    const auto stop = script_->ops().end();
    const op_iterator offset{ script_->offset };

    // If none of the strip ops are found, return the subscript.
    if (!is_intersecting<operations>(offset, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    return to_shared<script>(difference<operations>(offset, stop, strip));
}

// TODO: use sighash and key to generate signature in sign mode.
TEMPLATE
inline bool CLASS::
prepare(ec_signature& signature, const data_chunk&, hash_digest& hash,
    const chunk_xptr& endorsement) const NOEXCEPT
{
    uint8_t sighash_flags;
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(sighash_flags, distinguished, *endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    hash = signature_hash(*subscript({ endorsement }), sighash_flags);

    // Parse DER signature into an ECDSA signature (bip66 sets strict).
    const auto bip66 = is_enabled(flags::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// TODO: use sighash and key to generate signature in sign mode.
TEMPLATE
inline bool CLASS::
prepare(ec_signature& signature, const data_chunk&, hash_cache& cache,
    uint8_t& sighash_flags, const data_chunk& endorsement,
    const script& sub) const NOEXCEPT
{
    data_slice distinguished;

    // Parse Bitcoin endorsement into DER signature and sighash flags.
    if (!parse_endorsement(sighash_flags, distinguished, endorsement))
        return false;

    // Obtain the signature hash from subscript and sighash flags.
    signature_hash(cache, sub, sighash_flags);

    // Parse DER signature into an ECDSA signature (bip66 sets strict).
    const auto bip66 = is_enabled(flags::bip66_rule);
    return parse_signature(signature, distinguished, bip66);
}

// Signature hashing.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE hash_digest CLASS::
signature_hash(const script& sub, uint8_t flags) const NOEXCEPT
{
    // The bip141 fork establishes witness version, hashing is a distinct fork.
    const auto bip143 = is_enabled(flags::bip143_rule);

    // bip143: the method of signature hashing is changed for v0 scripts.
    return transaction_.signature_hash(input_, sub, value_, flags, version_,
        bip143);
}

// Caches signature hashes in a map against sighash flags.
// Prevents recomputation in the common case where flags are the same.
TEMPLATE
INLINE void CLASS::
signature_hash(hash_cache& cache, const script& sub,
    uint8_t sighash_flags) const NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    if (cache.find(sighash_flags) == cache.end())
        cache.emplace(sighash_flags, signature_hash(sub, sighash_flags));
    BC_POP_WARNING()
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
