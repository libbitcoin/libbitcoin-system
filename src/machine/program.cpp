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

constexpr auto unused_value = max_uint64;
constexpr auto unused_version = script_version::unversioned;

constexpr bool is_valid_witness_stack(const chunk_cptrs& stack) noexcept
{
    return witness::is_push_size(stack);
}

inline program::variant_stack_ptr default_stack() noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<program::variant_stack>();
    BC_POP_WARNING()
}

inline program::variant_stack_ptr copy_stack(
    const program::variant_stack& stack) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<program::variant_stack>(
        program::variant_stack{ stack });
    BC_POP_WARNING()
}

// TODO: no_fill_allocator.
// TODO: C++17: Parallel policy for std::transform.
inline program::variant_stack_ptr create_stack(
    const chunk_cptrs& stack) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    auto out = std::make_shared<program::variant_stack>(stack.size());
    BC_POP_WARNING()

    std::transform(stack.begin(), stack.end(), out->begin(),
        [](const chunk_cptr& ptr)
        {
            return program::variant{ ptr };
        });

    return out;
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// Explicit deduction guide (not needed as of C++20).
////template<class... Ts>
////overloaded(Ts...)->overloaded<Ts...>;

////// Only one statement remains, others discarded by compiler.
////const auto visitor = [&](const auto& vary) noexcept
////{
////    using Vary = std::decay_t<decltype(vary)>;
////
////    if constexpr (if_same<Vary, bool>::value)
////    {
////        item = to_shared(number{ to_int(vary) }.data());
////    }
////    else if constexpr (if_same<Vary, int64_t>::value)
////    {
////        item = to_shared(number{ vary }.data());
////    }
////    else if constexpr (if_same<Vary, chunk_cptr>::value)
////    {
////        item = vary;
////    }
////    else
////    {
////        static_assert(false, "non-exhaustive visitor!");
////    }
////};

// Constructors.
// ----------------------------------------------------------------------------

// Input script run (default/empty stack).
program::program(const chain::transaction& tx, const input_iterator& input,
     uint32_t forks) noexcept
  : program(tx, input, (*input)->script_ptr(), forks, unused_value,
      unused_version, default_stack(), true)
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
    valid_stack_(true),
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
    valid_stack_(true),
    primary_(std::move(other.primary_))
{
}

// Witness script run (witness-initialized stack).
program::program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, script_version version,
    const chunk_cptrs_ptr& stack) noexcept
  : program(tx, input, script, forks, (*input)->prevout->value(), version,
      create_stack(*stack), is_valid_witness_stack(*stack))
{
}

// protected
program::program(const chain::transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t forks, uint64_t value,
    script_version version, const variant_stack_ptr& stack,
    bool valid_stack) noexcept
  : transaction_(tx),
    input_(input),
    script_(script),
    forks_(forks),
    value_(value),
    version_(version),
    valid_stack_(valid_stack),
    primary_(stack)
{
}

// Public.
// ----------------------------------------------------------------------------

// TODO: only perform is_push_size check on witness initialized stack.
// TODO: others are either empty or presumed push_size from prevout script run.
bool program::is_valid() const noexcept
{
    // TODO: nops rule must first be enabled in tests and config.
    ////const auto nops_rule = is_enabled(forks::nops_rule);
    constexpr auto nops_rule = true;
    const auto bip141 = is_enabled(forks::bip141_rule);

    // nops_rule establishes script size limit.
    // bip_141 introduces an initialized stack, so must validate.
    return !(nops_rule && script_->is_oversized()) &&
        !(bip141 && !valid_stack_);
}

bool program::is_true(bool clean_stack) const noexcept
{
    return (!clean_stack || is_clean()) && !is_empty() && peek_bool_unsafe();
}

// Primary stack (push).
// ----------------------------------------------------------------------------

// Moving a shared pointer to the stack is optimal and acceptable.
BC_PUSH_WARNING(NO_RVALUE_REF_SHARED_PTR)
void program::push(chunk_cptr&& datum) noexcept
BC_POP_WARNING()
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(variant{ std::move(datum) });
    BC_POP_WARNING()
}

void program::push(const chunk_cptr& datum) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(variant{ datum });
    BC_POP_WARNING()
}

void program::push_chunk(data_chunk&& datum) noexcept
{
    push(to_shared<data_chunk>(std::move(datum)));
}

void program::push_bool(bool value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(variant{ value });
    BC_POP_WARNING()
}

void program::push_signed64(int64_t value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    primary_->push_back(variant{ value });
    BC_POP_WARNING()
}

void program::push_length(size_t value) noexcept
{
    // This is guarded by stack size and push data limits.
    BC_ASSERT_MSG(value <= max_int64, "integer overflow");
    push_signed64(possible_narrow_sign_cast<int64_t>(value));
}

// Primary stack (pop).
// ----------------------------------------------------------------------------

chunk_cptr program::pop_unsafe() noexcept
{
    const auto value = peek_unsafe();
    drop_unsafe();
    return value;
}

bool program::pop_bool_unsafe() noexcept
{
    const auto value = peek_bool_unsafe();
    drop_unsafe();
    return value;
}

bool program::pop_signed32_unsafe(int32_t& out_value) noexcept
{
    const auto result = peek_signed32_unsafe(out_value);
    drop_unsafe();
    return result;
}

bool program::pop_signed32(int32_t& out_value) noexcept
{
    if (is_empty())
        return false;

    return pop_signed32_unsafe(out_value);
}

bool program::pop_binary32(int32_t& left, int32_t& right) noexcept
{
    if (size() < 2)
        return false;

    // The right hand side operand is at the top of the stack.
    return pop_signed32_unsafe(right) && pop_signed32_unsafe(left);
}

bool program::pop_ternary32(int32_t& upper, int32_t& lower,
    int32_t& value) noexcept
{
    if (size() < 3)
        return false;

    // The upper bound is at stack top, lower bound next, value next.
    return pop_signed32_unsafe(upper) && pop_signed32_unsafe(lower) &&
        pop_signed32_unsafe(value);
}

// True if popped value is valid post-pop stack index (precluded if size < 2).
bool program::pop_index32(size_t& out_index) noexcept
{
    int32_t value;
    if (!pop_signed32(value))
        return false;

    if (is_negative(value))
        return false;

    // Cast unsafe when sizeof(size_t) < sizeof(int64_t), guarded by stack size.
    out_index = limit<size_t>(value);
    return out_index < size();
}

bool program::pop_count(chunk_cptrs& data, size_t count) noexcept
{
    if (size() < count)
        return false;

    data.reserve(count);
    for (size_t index = 0; index < count; ++index)
        data.push_back(pop_unsafe());

    return true;
}

// Primary stack (peek).
// ----------------------------------------------------------------------------

// No size limits on push (of any type) or peek chunk.
chunk_cptr program::peek_unsafe(size_t index) const noexcept
{
    chunk_cptr item;
    const overloaded visitor
    {
        [&](bool vary) noexcept
        {
            item = to_shared(number{ to_int(vary) }.data());
        },
        [&](int64_t vary) noexcept
        {
            item = to_shared(number{ vary }.data());
        },
        [&](const chunk_cptr& vary) noexcept
        {
            item = vary;
        }
    };

    std::visit(visitor, *const_it_unsafe(index));
    return item;
}

// No chunk size limits on peek bool.
bool program::peek_bool_unsafe() const noexcept
{
    bool item{};
    const overloaded visitor
    {
        [&](bool vary) noexcept
        {
            item = vary;
        },
        [&](int64_t vary) noexcept
        {
            item = to_bool(vary);
        },
        [&](const chunk_cptr& vary) noexcept
        {
            item = number::is_stack_true(*vary);
        }
    };

    std::visit(visitor, primary_->back());
    return item;
}

// private
// Generalized integer peek for varying bit widths up to 64.
template<size_t bits, typename Out, if_not_greater<bits, width<Out>()>>
bool program::peek_signed_unsafe(Out& out_value, size_t index) const noexcept
{
    // Byte limit overflow guard.
    bool result{};

    const overloaded visitor
    {
        [&](bool vary) noexcept
        {
            result = true;
            out_value = to_int(vary);
        },
        [&](int64_t vary) noexcept
        {
            const auto narrowed = mask_right(vary, bits);
            result = is_zero(narrowed);
            out_value = possible_narrow_cast<Out>(narrowed);
        },
        [&](const chunk_cptr& vary) noexcept
        {
            number value{};
            result = value.set_data(*vary, byte_width(bits));
            out_value = possible_narrow_cast<Out>(value.int64());
        }
    };

    std::visit(visitor, *const_it_unsafe(index));
    return result;
}

bool program::peek_signed32_unsafe(int32_t& out_value) const noexcept
{
    constexpr auto bits = width<int32_t>();
    return peek_signed_unsafe<bits>(out_value);
}

bool program::peek_signed40_unsafe(int64_t& out_value) const noexcept
{
    constexpr auto bits = width<int8_t>() + width<int32_t>();
    return peek_signed_unsafe<bits>(out_value);
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.sequence allows use of the full unsigned 32 bit domain, without use of
// the negative range.
// ****************************************************************************
bool program::peek_unsigned32(uint32_t& out_value) const noexcept
{
    if (is_empty())
        return false;

    // Negative exclusion drops the 40th bit (inconsequential).
    int64_t value{};
    if (!peek_signed40_unsafe(value) || is_negative(value))
        return false;

    // 32 bits are used in unsigned input.sequence compare.
    out_value = narrow_sign_cast<uint32_t>(value);
    return true;
}

// ****************************************************************************
// CONSENSUS: Read of 40 bit (vs. 32 bit) value for comparison against uint32_t
// input.locktime allows use of the full unsigned 32 bit domain, without use of
// the negative range. Otherwise a 2038 limit (beyond the inherent 2106 limit)
// would have been introduced. Since the sign bit is unused, the domain is 39.
// ****************************************************************************
bool program::peek_unsigned39(uint64_t& out_value) const noexcept
{
    if (is_empty())
        return false;

    // Negative exclusion drops the 40th bit (limits comparable domain).
    int64_t value;
    if (!peek_signed40_unsafe(value) || is_negative(value))
        return false;

    // 39 bits are used in unsigned tx.locktime compare.
    out_value = sign_cast<uint64_t>(value);
    return true;
}

// Primary stack push/pop non-const functions (optimizations).
// ----------------------------------------------------------------------------
// Stack index is zero-based, back() is element zero.

void program::drop_unsafe() noexcept
{
    BC_ASSERT(!is_empty());
    primary_->pop_back();
}

void program::swap_unsafe(size_t left_index, size_t right_index) noexcept
{
    // This swaps the value of each pointer, not vector positions.
    // This is safe as all stack pointer instances are owned by program.
    std::swap(*it_unsafe(left_index), *it_unsafe(right_index));
}

void program::erase_unsafe(size_t index) noexcept
{
    primary_->erase(it_unsafe(index));
}

// Primary stack push/pop const functions.
// ----------------------------------------------------------------------------

size_t program::size() const noexcept
{
    return primary_->size();
}

bool program::is_empty() const noexcept
{
    return primary_->empty();
}

bool program::is_overflow() const noexcept
{
    // bit.ly/2cowHlP
    // Addition is safe due to stack size constraint.
    return (size() + alternate_.size()) > max_stack_size;
}

bool program::is_clean() const noexcept
{
    return size() == one;
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

chunk_cptr program::pop_alternate_unsafe() noexcept
{
    BC_ASSERT(!alternate_.empty());

    // This must be a pointer copy, as the pointer is about to be destroyed.
    const chunk_cptr value{ alternate_.back() };
    alternate_.pop_back();
    return value;
}

// Conditional stack.
// ----------------------------------------------------------------------------

void program::begin_if(bool value) noexcept
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
void program::else_if_unsafe() noexcept
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

    // Addition is safe due to script size constraint.
    BC_ASSERT(condition_.back() || !overflows(negative_condition_count_, one));

    negative_condition_count_ += (condition_.back() ? 1 : -1);
    condition_.back() = !condition_.back();
}

void program::end_if_unsafe() noexcept
{
    // Subtraction must be guarded by caller logical constraints.
    BC_ASSERT(!is_balanced());

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
constexpr bool operation_count_exceeded(size_t count) noexcept
{
    return count > max_counted_ops;
}

bool program::ops_increment(const operation& op) noexcept
{
    // Addition is safe due to script size constraint.
    BC_ASSERT(!overflows(operation_count_, one));

    if (operation::is_counted(op.code()))
        ++operation_count_;

    return operation_count_ <= max_counted_ops;
}

bool program::ops_increment(size_t public_keys) noexcept
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
bool program::set_subscript(const op_iterator& op) noexcept
{
    // End is not reachable if op is an element of script_.
    if (script_->ops().empty() || op == script_->ops().end())
        return false;

    // Advance the offset to the op following the found code separator.
    // This is non-const because changes script state (despite being mutable).
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
