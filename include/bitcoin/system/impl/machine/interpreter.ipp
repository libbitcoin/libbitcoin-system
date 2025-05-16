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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Operation handlers.
// ----------------------------------------------------------------------------

TEMPLATE
inline error::op_error_t CLASS::
op_unevaluated(opcode code) const NOEXCEPT
{
    return operation::is_invalid(code) ? error::op_invalid :
        error::op_reserved;
}

// TODO: nops_rule *must* be enabled in test cases and default config.
// TODO: cats_rule should be enabled in test cases and default config.
// Codes op_nop1..op_nop10 promoted from reserved by [0.3.6] hard fork.
TEMPLATE
inline error::op_error_t CLASS::
op_nop(opcode) const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return error::op_success;

    ////return op_unevaluated(code);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_push_number(int8_t value) NOEXCEPT
{
    state::push_signed64(value);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_push_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_push_one_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_one_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_push_two_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_two_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_push_four_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_four_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_nop() const NOEXCEPT
{
    return error::op_success;
}

// This opcode pushed the version to the stack, a hard fork per release.
TEMPLATE
inline error::op_error_t CLASS::
op_ver() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_ver);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_if() NOEXCEPT
{
    auto value = false;

    if (state::is_succeess())
    {
        if (state::is_stack_empty())
            return error::op_if1;

        if (!state::pop_bool_(value, state::is_enabled(flags::bip342_rule)))
            return error::op_if2;
    }

    state::begin_if(value);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_notif() NOEXCEPT
{
    auto value = false;

    if (state::is_succeess())
    {
        if (state::is_stack_empty())
            return error::op_notif1;

        if (!state::pop_bool_(value, state::is_enabled(flags::bip342_rule)))
            return error::op_notif2;
    }

    state::begin_if(!value);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_verif() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_verif);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_vernotif() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_vernotif);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_else() NOEXCEPT
{
    if (state::is_balanced())
        return error::op_else;

    state::else_if_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_endif() NOEXCEPT
{
    if (state::is_balanced())
        return error::op_endif;

    state::end_if_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_verify() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_verify1;

    if (!state::peek_bool_())
        return error::op_verify2;

    state::drop_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_return() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_return);
        
    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_to_alt_stack() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_to_alt_stack;

    state::push_alternate(state::pop_());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_from_alt_stack() NOEXCEPT
{
    if (state::is_alternate_empty())
        return error::op_from_alt_stack;

    state::push_variant(state::pop_alternate_());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_drop2() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_drop2;

    // 0,1,[2,3] => 1,[2,3] => [2,3]
    state::drop_();
    state::drop_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_dup2() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_dup2;

    // [0,1,2,3] => 1,[0,1,2,3] =>  0,1,[0,1,2,3]
    state::push_variant(state::peek_(1));
    state::push_variant(state::peek_(1));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_dup3() NOEXCEPT
{
    if (state::stack_size() < 3u)
        return error::op_dup3;

    // [0,1,2,3] => 2,[0,1,2,3] => 1,2,[0,1,2,3] => 0,1,2,[0,1,2,3]
    state::push_variant(state::peek_(2));
    state::push_variant(state::peek_(2));
    state::push_variant(state::peek_(2));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_over2() NOEXCEPT
{
    if (state::stack_size() < 4u)
        return error::op_over2;

    // [0,1,2,3] => 3,[0,1,2,3] => 2,3,[0,1,2,3]
    state::push_variant(state::peek_(3));
    state::push_variant(state::peek_(3));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_rot2() NOEXCEPT
{
    if (state::stack_size() < 6u)
        return error::op_rot2;

    // [0,1,2,3,4,5] => [4,1,2,3,0,5] => [4,5,2,3,0,1] =>
    // [4,5,0,3,2,1] => [4,5,0,1,2,3]
    state::swap_(0, 4);
    state::swap_(1, 5);
    state::swap_(2, 4);
    state::swap_(3, 5);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_swap2() NOEXCEPT
{
    if (state::stack_size() < 4u)
        return error::op_swap2;

    // [0,1,2,3] => [0,3,2,1] => [2,3,0,1]
    state::swap_(1, 3);
    state::swap_(0, 2);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_if_dup() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_if_dup;

    // [0,1,2] => 0,[0,1,2]
    if (state::peek_bool_())
        state::push_variant(state::peek_());

    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_depth() NOEXCEPT
{
    // [0,1,2] => 3,[0,1,2]
    state::push_length(state::stack_size());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_drop() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_drop;

    // 0,[1,2] => [1,2]
    state::drop_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_dup() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_dup;

    // [0,1,2] => 0,[0,1 2]
    state::push_variant(state::peek_());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_nip() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_nip;

    // [0,1,2] => 1,[0,2] => [0,2]
    state::swap_(0, 1);
    state::drop_();
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_over() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_over;

    // [0,1] => 1,[0,1]
    state::push_variant(state::peek_(1));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_pick() NOEXCEPT
{
    size_t index;

    // 2,[0,1,2,3] => {2} [0,1,2,3]
    if (!state::pop_index32(index))
        return error::op_pick;

    // [0,1,2,3] => 2,[0,1,2,3]
    state::push_variant(state::peek_(index));
    return error::op_success;
}

// ****************************************************************************
// Rock-and-ROLL:
// "The value stack can store a maximum of 1000 elements. The following script
// fills the stack and then moves each stack element 200 times, so the number
// of moved elements is 200K. This took almost 5.6 seconds in my test VM (for
// a block filled with these scriptSigs):
// 1 {999 times}, 998 OP_ROLL{ 200 times }"
// bitslog.com/2017/04/17/new-quadratic-delays-in-bitcoin-scripts
// Shifting larger chunks does not change time, as vector stores references.
// This remains in the current satoshi implementation (std_vector).
// ****************************************************************************
TEMPLATE
inline error::op_error_t CLASS::
op_roll() NOEXCEPT
{
    size_t index;

    // 998,[0,1,2,...,997,998,999] => {998} [0,1,2,...,997,998,999] 
    if (!state::pop_index32(index))
        return error::op_roll;

    // Copy variant because should be deleted before push (no stack alloc).
    stack_variant temporary{ state::peek_(index) };

    // Shifts maximum of n-1 references within vector of n.
    // [0,1,2,...,997,xxxx,999] => [0,1,2,...,997,999]
    state::erase_(index);

    // [0,1,2,...,997,999] => 998,[0,1,2,...,997,999]
    state::push_variant(std::move(temporary));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_rot() NOEXCEPT
{
    if (state::stack_size() < 3u)
        return error::op_rot;

    // [0,1,2,3] = > [0,2,1,3] => [2,0,1,3]
    state::swap_(1, 2);
    state::swap_(0, 1);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_swap() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_swap;

    // [0,1,2] = > [1,0,2]
    state::swap_(0, 1);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_tuck() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_tuck;

    // [0,1,2] = > [1,0,2]  => 0,[1,0,2]
    state::swap_(0, 1);
    state::push_variant(state::peek_(1));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_cat() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_cat);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_substr() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_substr);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_left() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_left);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_right() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_right);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_size() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_size;

    state::push_length(state::peek_size());
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_invert() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_invert);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_and() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_and);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_or() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_or);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_xor() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_xor);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_equal() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_equal;

    state::push_bool(state::equal_chunks(state::pop_(), state::pop_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_equal_verify() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_equal_verify1;

    return state::equal_chunks(state::pop_(), state::pop_()) ?
        error::op_success : error::op_equal_verify2;
}

TEMPLATE
inline error::op_error_t CLASS::
op_add1() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_add1;

    state::push_signed64(add1<int64_t>(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_sub1() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_sub1;

    state::push_signed64(sub1<int64_t>(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_mul2() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mul2);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_div2() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_div2);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_negate() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_negate;

    // negate(minimum) overflow precluded by domain increase.
    state::push_signed64(negate<int64_t>(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_abs() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_abs;

    // absolute(minimum) overflow precluded by domain increase.
    state::push_signed64(absolute<int64_t>(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_not() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_not;

    // Do not pop_bool() above, as number must be validated.
    state::push_bool(!to_bool(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_nonzero() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_nonzero;

    state::push_bool(is_nonzero(number));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_add() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_add;

    // addition overflow precluded by domain increase.
    state::push_signed64(add<int64_t>(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_sub() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_sub;

    // subtraction overflow precluded by domain increase.
    state::push_signed64(subtract<int64_t>(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_mul() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mul);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_div() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_div);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_mod() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mod);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_lshift() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_lshift);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_rshift() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_rshift);

    return error::op_not_implemented;
}

TEMPLATE
inline error::op_error_t CLASS::
op_bool_and() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_bool_and;

    state::push_bool(to_bool(left) && to_bool(right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_bool_or() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_bool_or;

    state::push_bool(to_bool(left) || to_bool(right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_num_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_equal;

    state::push_bool(left == right);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_num_equal_verify() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_equal_verify1;

    return (left == right) ? error::op_success : 
        error::op_num_equal_verify2;
}

TEMPLATE
inline error::op_error_t CLASS::
op_num_not_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_not_equal;

    state::push_bool(left != right);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_less_than() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_less_than;

    state::push_bool(is_lesser(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_greater_than() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_greater_than;

    state::push_bool(is_greater(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_less_than_or_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_less_than_or_equal;

    state::push_bool(!is_greater(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_greater_than_or_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_greater_than_or_equal;

    state::push_bool(!is_lesser(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_min() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_min;

    state::push_signed64(lesser(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_max() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_max;

    state::push_signed64(greater(left, right));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_within() NOEXCEPT
{
    int32_t upper, lower, value;
    if (!state::pop_ternary32(upper, lower, value))
        return error::op_within;

    state::push_bool(!is_lesser(value, lower) && is_lesser(value, upper));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_ripemd160() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_ripemd160;

    state::push_chunk(rmd160_chunk(*state::pop_chunk_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_sha1() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_sha1;

    state::push_chunk(sha1_chunk(*state::pop_chunk_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_sha256() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_sha256;

    state::push_chunk(sha256_chunk(*state::pop_chunk_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_hash160() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_hash160;

    state::push_chunk(bitcoin_short_chunk(*state::pop_chunk_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_hash256() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_hash256;

    state::push_chunk(bitcoin_chunk(*state::pop_chunk_()));
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_codeseparator(const op_iterator& op) NOEXCEPT
{
    // Not thread safe for the script (changes script object metadata).
    state::set_subscript(op);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_sig() NOEXCEPT
{
    const auto ec = op_check_sig_verify();
    if (ec == error::op_check_sig_empty_key)
        return ec;

    // BIP66: if DER encoding invalid script MUST fail and end.
    const auto bip66 = state::is_enabled(flags::bip66_rule);
    if (bip66 && ec == error::op_check_sig_parse_signature)
        return ec;

    state::push_bool(ec == error::op_success);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_sig_verify() NOEXCEPT
{
    if (state::stack_size() < 2u)
        return error::op_check_sig_verify1;

    const auto key = state::pop_chunk_();
    const auto endorsement = state::pop_chunk_();
    if (key->empty())
        return error::op_check_sig_empty_key;

    // BIP342:
    if (state::is_enabled(flags::bip342_rule))
    {
        // If signature is empty, script MUST fail and end (or push false).
        if (endorsement->empty())
            return error::op_check_sig_verify2;

        // If public key is 32 bytes it is a bip340 schnorr key.
        // If signature is not empty, it is validated against public key.
        if (key->size() == schnorr::public_key_size)
        {
            // Split signature and signature hash flags.
            uint8_t sighash_flags;
            const auto& sig = state::schnorr_split(sighash_flags, *endorsement);
            if (sighash_flags == chain::coverage::invalid)
                return error::op_check_sig_verify3;

            // Generate signature hash.
            hash_digest hash{};
            if (!state::signature_hash(hash, sighash_flags))
                return error::op_check_sig_verify4;

            // Verify schnorr signature against public key and signature hash.
            if (!schnorr::verify_signature(*key, hash, sig))
                return error::op_check_sig_verify5;

            // If signature not empty, opcode counted toward sigops budget.
            if (!state::sigops_increment())
                return error::op_check_sig_verify6;
        }

        // If signature not empty, opcode counted toward sigops budget.
        if (!state::sigops_increment())
            return error::op_check_sig_verify7;

        // If public key size is neither 0 nor 32 bytes, it is an unknown type.
        // During script execution of signature opcodes these behave exactly as
        // known types except that signature validation considered successful.
        return error::op_success;
    }

    if (endorsement->empty())
        return error::op_check_sig_verify8;

    // Split endorsement into DER signature and signature hash flags.
    uint8_t sighash_flags;
    const auto& der = state::ecdsa_split(sighash_flags, *endorsement);
    const auto bip66 = state::is_enabled(flags::bip66_rule);

    // BIP66: if DER encoding invalid script MUST fail and end.
    ec_signature sig;
    if (!ecdsa::parse_signature(sig, der, bip66))
        return error::op_check_sig_parse_signature;

    // Generate signature hash.
    hash_digest hash{};
    const auto subscript = state::subscript(endorsement);
    if (!state::signature_hash(hash, *subscript, sighash_flags))
        return error::op_check_sig_verify9;

    // Verify ECDSA signature against public key and signature hash.
    if (!ecdsa::verify_signature(*key, hash, sig))
        return error::op_check_sig_verify10;

    // TODO: use sighash and key to generate signature in sign mode.
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_multisig() NOEXCEPT
{
    if (state::is_enabled(flags::bip342_rule))
        return op_unevaluated(opcode::checkmultisig);

    const auto ec = op_check_multisig_verify();

    // BIP66: if DER encoding invalid, script MUST fail and end.
    const auto bip66 = state::is_enabled(flags::bip66_rule);
    if (bip66 && ec == error::op_check_multisig_parse_signature)
        return ec;

    state::push_bool(ec == error::op_success);
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_multisig_verify() NOEXCEPT
{
    if (state::is_enabled(flags::bip342_rule))
        return op_unevaluated(opcode::checkmultisigverify);

    size_t count{};
    if (!state::pop_index32(count))
        return error::op_check_multisig_verify1;

    if (count > chain::max_script_public_keys)
        return error::op_check_multisig_verify2;

    if (!state::ops_increment(count))
        return error::op_check_multisig_verify3;

    chunk_xptrs keys;
    if (!state::pop_chunks(keys, count))
        return error::op_check_multisig_verify4;

    if (!state::pop_index32(count))
        return error::op_check_multisig_verify5;

    if (count > keys.size())
        return error::op_check_multisig_verify6;

    chunk_xptrs endorsements;
    if (!state::pop_chunks(endorsements, count))
        return error::op_check_multisig_verify7;

    if (state::is_stack_empty())
        return error::op_check_multisig_verify8;

    //*************************************************************************
    // CONSENSUS: Satoshi bug, discard stack element, malleable until bip147.
    //*************************************************************************
    const auto bip147 = state::is_enabled(flags::bip147_rule);

    // This check is unique in that a chunk must be empty to be false.
    if (state::pop_strict_bool_() && bip147)
        return error::op_check_multisig_verify9;

    state::initialize_cache();
    auto it = endorsements.begin();
    const auto subscript = state::subscript(endorsements);
    const auto bip66 = state::is_enabled(flags::bip66_rule);

    // Keys may be empty.
    for (const auto& key: keys)
    {
        // Implies that all signatures are valid.
        if (it == endorsements.end())
            break;

        // Empty endorsement does not increment iterator.
        const auto endorsement = *it;
        if (endorsement->empty())
            continue;

        // Split endorsement into DER signature signature hash flags.
        uint8_t sighash_flags;
        const auto& der = state::ecdsa_split(sighash_flags, *endorsement);

        // BIP66: if DER encoding invalid script MUST fail and end.
        ec_signature sig;
        if (!ecdsa::parse_signature(sig, der, bip66))
            return error::op_check_sig_parse_signature;

        // Signature hash caching (bypass signature hash if same as previous).
        if (state::uncached(sighash_flags))
            if (!state::set_hash(*subscript, sighash_flags))
                return error::op_check_multisig_verify10;

        // Verify ECDSA signature against public key and cache signature hash.
        if (ecdsa::verify_signature(*key, state::cached_hash(), sig))
            ++it;
    }

    // All endorsements must be verified against a key.
    if (it != endorsements.end())
        return error::op_check_multisig_verify11;

    // TODO: use sighash and key to generate signature in sign mode.
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_locktime_verify() const NOEXCEPT
{
    // BIP65: nop2 subsumed by checklocktimeverify when bip65 fork is active.
    if (!state::is_enabled(flags::bip65_rule))
        return op_nop(opcode::nop2);

    // The tx sequence is 0xffffffff.
    if (state::input().is_final())
        return error::op_check_locktime_verify1;

    // The stack is empty.
    // The top stack item is negative.
    // Extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 40 bits are usable.
    uint64_t stack_locktime40;
    if (!state::peek_unsigned40(stack_locktime40))
        return error::op_check_locktime_verify2;

    const auto trans_locktime32 = state::tx().locktime();
    using namespace chain;

    // The stack locktime type differs from that of tx.
    if ((stack_locktime40 < locktime_threshold) !=
        (trans_locktime32 < locktime_threshold))
        return error::op_check_locktime_verify3;

    // The stack locktime is greater than the tx locktime.
    if (stack_locktime40 > trans_locktime32)
        return error::op_check_locktime_verify4;

    // TODO: use sighash and key to generate signature in sign mode?
    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_sequence_verify() const NOEXCEPT
{
    // BIP112: nop3 subsumed by checksequenceverify when bip112 fork is active.
    if (!state::is_enabled(flags::bip112_rule))
        return op_nop(opcode::nop3);

    // The stack is empty.
    // The top stack item is negative.
    // Extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 32 bits are used (33rd-40th discarded).
    uint32_t stack_sequence32;
    if (!state::peek_unsigned32(stack_sequence32))
        return error::op_check_sequence_verify1;

    // Only 32 bits are tested.
    const auto input_sequence32 = state::input().sequence();
    using namespace chain;

    // The stack sequence is disabled, treat as nop3.
    if (get_right(stack_sequence32, relative_locktime_disabled_bit))
        return op_nop(opcode::nop3);

    // The stack sequence is enabled and tx version less than 2.
    if (state::tx().version() < relative_locktime_min_version)
        return error::op_check_sequence_verify2;

    // The transaction sequence is disabled.
    if (get_right(input_sequence32, relative_locktime_disabled_bit))
        return error::op_check_sequence_verify3;

    // The stack sequence type differs from that of tx input.
    if (get_right(stack_sequence32, relative_locktime_time_locked_bit) !=
        get_right(input_sequence32, relative_locktime_time_locked_bit))
        return error::op_check_sequence_verify4;

    // The unmasked stack sequence is greater than that of tx sequence.
    if (mask_left(stack_sequence32, relative_locktime_mask_left) >
        mask_left(input_sequence32, relative_locktime_mask_left))
        return error::op_check_sequence_verify5;

    return error::op_success;
}

TEMPLATE
inline error::op_error_t CLASS::
op_check_sig_add() NOEXCEPT
{
    // BIP342: reserved_186 subsumed by op_checksigadd when tapscript active.
    if (!state::is_enabled(flags::bip342_rule))
        return op_unevaluated(opcode::reserved_186);

    // If fewer than 3 elements on stack, script MUST fail and end.
    if (state::stack_size() < 3u)
        return error::op_check_schnorr_sig1;

    // Public key (top) is popped.
    const auto key = state::pop_chunk_();

    // If public key is empty, script MUST fail and end.
    if (key->empty())
        return error::op_check_schnorr_sig2;

    // Number (second to top) is popped.
    // If number is larger than 4 bytes, script MUST fail and end.
    int32_t number;
    if (!state::pop_signed32_(number))
        return error::op_check_schnorr_sig3;

    // Signature (third to top) is popped.
    const auto endorsement = state::pop_chunk_();

    // If signature is empty, [number] pushed, execution continues.
    if (endorsement->empty())
    {
        state::push_signed64(number);
        return error::op_success;
    }

    // Split signature and signature hash flags.
    uint8_t sighash_flags;
    const auto& sig = state::schnorr_split(sighash_flags, *endorsement);
    if (sighash_flags == chain::coverage::invalid)
        return error::op_check_schnorr_sig4;

    // Generate signature hash.
    hash_digest hash{};
    if (!state::signature_hash(hash, sighash_flags))
        return error::op_check_schnorr_sig5;

    // Verify schnorr signature against public key and signature hash.
    if (!schnorr::verify_signature(*key, hash, sig))
        return error::op_check_schnorr_sig6;

    // If signature not empty, opcode counted toward sigops budget.
    if (!state::sigops_increment())
        return error::op_check_schnorr_sig7;

    // If signature not empty (and successful), [number+1] pushed.
    state::push_signed64(add1<int64_t>(number));
    return error::op_success;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
