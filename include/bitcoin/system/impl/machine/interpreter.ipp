/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include "bitcoin/system/chain/prevout.hpp"
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>

// Push data revalidation (op.is_underclaimed()):
// Incorrectly-sized push data is validated upon op parse, setting an invalid
// opcode, the underflow bit, and the op data. This invalid opcode will result
// in script execution failure if the script is evaluated, but is otherwise
// valid (such as for a coinbase input script). Underflow can only occur at the
// end of a script. All bytes are valid script, but as scripts have finite
// length, the last operation may be an undersized push data. The retention of
// the undersized data within an invalid op allows for the script to be
// serialized and for the proper size and hash of the transaction computed.
// The invalid opcode with underflow data is serialized as data bytes with the
// original opcode stored in the data member, but seen here as op_xor.
// By design it is not possible to populate an op.data size that does not
// correspond to the op.code. Size mismatch is revalidated here as final
// insurance against derived classes that may alter this behavior. This ensures
// that an opcode that does not push correctly-sized data here will fail.

namespace libbitcoin {
namespace system {
namespace machine {

using namespace system::chain;
using namespace system::error;

// Operation handlers.
// ----------------------------------------------------------------------------

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_unevaluated(opcode code) const NOEXCEPT
{
    return operation::is_invalid(code) ? error::op_invalid :
        error::op_reserved;
}

// TODO: nops_rule *must* be enabled in test cases and default config.
// TODO: cats_rule should be enabled in test cases and default config.
// Codes op_nop1..op_nop10 promoted from reserved by [0.3.6] hard fork.
template <typename Stack>
inline op_error_t interpreter<Stack>::
op_nop(opcode) const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return error::op_success;

    ////return op_unevaluated(code);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_push_number(int8_t value) NOEXCEPT
{
    state::push_signed64(value);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_push_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_push_one_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_one_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_push_two_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_two_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_push_four_size(const operation& op) NOEXCEPT
{
    if (op.is_underclaimed())
        return error::op_push_four_size;

    state::push_chunk(op.data_ptr());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_nop() const NOEXCEPT
{
    return error::op_success;
}

// This opcode pushed the version to the stack, a hard fork per release.
template <typename Stack>
inline op_error_t interpreter<Stack>::
op_ver() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_ver);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_if() NOEXCEPT
{
    auto value = false;

    if (state::is_succeess())
    {
        if (state::is_stack_empty())
            return error::op_if;

        value = state::pop_bool_();
    }

    state::begin_if(value);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_notif() NOEXCEPT
{
    auto value = false;

    if (state::is_succeess())
    {
        if (state::is_stack_empty())
            return error::op_notif;

        value = !state::pop_bool_();
    }

    state::begin_if(value);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_verif() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_verif);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_vernotif() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_vernotif);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_else() NOEXCEPT
{
    if (state::is_balanced())
        return error::op_else;

    state::else_if_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_endif() NOEXCEPT
{
    if (state::is_balanced())
        return error::op_endif;

    state::end_if_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_verify() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_verify1;

    if (!state::peek_bool_())
        return error::op_verify2;

    state::drop_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_return() const NOEXCEPT
{
    if (state::is_enabled(flags::nops_rule))
        return op_unevaluated(opcode::op_return);
        
    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_to_alt_stack() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_to_alt_stack;

    state::push_alternate(state::pop_());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_from_alt_stack() NOEXCEPT
{
    if (state::is_alternate_empty())
        return error::op_from_alt_stack;

    state::push_variant(state::pop_alternate_());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_drop2() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_drop2;

    // 0,1,[2,3] => 1,[2,3] => [2,3]
    state::drop_();
    state::drop_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_dup2() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_dup2;

    // [0,1,2,3] => 1,[0,1,2,3] =>  0,1,[0,1,2,3]
    state::push_variant(state::peek_(1));
    state::push_variant(state::peek_(1));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_dup3() NOEXCEPT
{
    if (state::stack_size() < 3)
        return error::op_dup3;

    // [0,1,2,3] => 2,[0,1,2,3] => 1,2,[0,1,2,3] => 0,1,2,[0,1,2,3]
    state::push_variant(state::peek_(2));
    state::push_variant(state::peek_(2));
    state::push_variant(state::peek_(2));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_over2() NOEXCEPT
{
    if (state::stack_size() < 4)
        return error::op_over2;

    // [0,1,2,3] => 3,[0,1,2,3] => 2,3,[0,1,2,3]
    state::push_variant(state::peek_(3));
    state::push_variant(state::peek_(3));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_rot2() NOEXCEPT
{
    if (state::stack_size() < 6)
        return error::op_rot2;

    // [0,1,2,3,4,5] => [4,1,2,3,0,5] => [4,5,2,3,0,1] =>
    // [4,5,0,3,2,1] => [4,5,0,1,2,3]
    state::swap_(0, 4);
    state::swap_(1, 5);
    state::swap_(2, 4);
    state::swap_(3, 5);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_swap2() NOEXCEPT
{
    if (state::stack_size() < 4)
        return error::op_swap2;

    // [0,1,2,3] => [0,3,2,1] => [2,3,0,1]
    state::swap_(1,3);
    state::swap_(0,2);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_if_dup() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_if_dup;

    // [0,1,2] => 0,[0,1,2]
    if (state::peek_bool_())
        state::push_variant(state::peek_());

    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_depth() NOEXCEPT
{
    // [0,1,2] => 3,[0,1,2]
    state::push_length(state::stack_size());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_drop() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_drop;

    // 0,[1,2] => [1,2]
    state::drop_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_dup() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_dup;

    // [0,1,2] => 0,[0,1 2]
    state::push_variant(state::peek_());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_nip() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_nip;

    // [0,1,2] => 1,[0,2] => [0,2]
    state::swap_(0, 1);
    state::drop_();
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_over() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_over;

    // [0,1] => 1,[0,1]
    state::push_variant(state::peek_(1));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
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
// This remains the current satoshi implementation (std::vector).
// ****************************************************************************
template <typename Stack>
inline op_error_t interpreter<Stack>::
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

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_rot() NOEXCEPT
{
    if (state::stack_size() < 3)
        return error::op_rot;

    // [0,1,2,3] = > [0,2,1,3] => [2,0,1,3]
    state::swap_(1,2);
    state::swap_(0,1);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_swap() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_swap;

    // [0,1,2] = > [1,0,2]
    state::swap_(0,1);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_tuck() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_tuck;

    // [0,1,2] = > [1,0,2]  => 0,[1,0,2]
    state::swap_(0, 1);
    state::push_variant(state::peek_(1));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_cat() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_cat);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_substr() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_substr);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_left() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_left);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_right() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_right);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_size() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_size;

    state::push_length(state::peek_size());
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_invert() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_invert);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_and() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_and);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_or() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_or);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_xor() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_xor);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_equal() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_equal;

    state::push_bool(state::equal_chunks(state::pop_(), state::pop_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_equal_verify() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_equal_verify1;

    return state::equal_chunks(state::pop_(), state::pop_()) ?
        error::op_success : error::op_equal_verify2;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_add1() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_add1;

    state::push_signed64(add1<int64_t>(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_sub1() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_sub1;

    state::push_signed64(sub1<int64_t>(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_mul2() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mul2);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_div2() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_div2);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_negate() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_negate;

    // negate(minimum) overflow precluded by domain increase.
    state::push_signed64(negate<int64_t>(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_abs() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_abs;

    // absolute(minimum) overflow precluded by domain increase.
    state::push_signed64(absolute<int64_t>(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_not() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_not;

    // Do not pop_bool() above, as number must be validated.
    state::push_bool(!to_bool(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_nonzero() NOEXCEPT
{
    int32_t number;
    if (!state::pop_signed32(number))
        return error::op_nonzero;

    state::push_bool(is_nonzero(number));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_add() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_add;

    // addition overflow precluded by domain increase.
    state::push_signed64(add<int64_t>(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_sub() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_sub;

    // subtraction overflow precluded by domain increase.
    state::push_signed64(subtract<int64_t>(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_mul() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mul);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_div() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_div);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_mod() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_mod);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_lshift() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_lshift);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_rshift() const NOEXCEPT
{
    if (state::is_enabled(flags::cats_rule))
        return op_unevaluated(opcode::op_rshift);

    return error::op_not_implemented;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_bool_and() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_bool_and;

    state::push_bool(to_bool(left) && to_bool(right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_bool_or() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_bool_or;

    state::push_bool(to_bool(left) || to_bool(right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_num_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_equal;

    state::push_bool(left == right);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_num_equal_verify() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_equal_verify1;

    return (left == right) ? error::op_success : 
        error::op_num_equal_verify2;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_num_not_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_num_not_equal;

    state::push_bool(left != right);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_less_than() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_less_than;

    state::push_bool(is_lesser(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_greater_than() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_greater_than;

    state::push_bool(is_greater(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_less_than_or_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_less_than_or_equal;

    state::push_bool(!is_greater(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_greater_than_or_equal() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_greater_than_or_equal;

    state::push_bool(!is_lesser(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_min() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_min;

    state::push_signed64(lesser(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_max() NOEXCEPT
{
    int32_t right, left;
    if (!state::pop_binary32(left, right))
        return error::op_max;

    state::push_signed64(greater(left, right));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_within() NOEXCEPT
{
    int32_t upper, lower, value;
    if (!state::pop_ternary32(upper, lower, value))
        return error::op_within;

    state::push_bool(!is_lesser(value, lower) && is_lesser(value, upper));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_ripemd160() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_ripemd160;

    state::push_chunk(rmd160_chunk(*state::pop_chunk_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_sha1() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_sha1;

    state::push_chunk(sha1_chunk(*state::pop_chunk_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_sha256() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_sha256;

    state::push_chunk(sha256_chunk(*state::pop_chunk_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_hash160() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_hash160;

    state::push_chunk(bitcoin_short_chunk(*state::pop_chunk_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_hash256() NOEXCEPT
{
    if (state::is_stack_empty())
        return error::op_hash256;

    state::push_chunk(bitcoin_chunk(*state::pop_chunk_()));
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_codeseparator(const op_iterator& op) NOEXCEPT
{
    // Not thread safe for the script (changes script object metadata).
    return state::set_subscript(op) ? error::op_success :
        error::op_code_separator;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_sig() NOEXCEPT
{
    const auto verify = op_check_sig_verify();
    const auto bip66 = state::is_enabled(flags::bip66_rule);

    // BIP66: invalid signature encoding fails the operation.
    if (bip66 && verify == error::op_check_sig_verify_parse)
        return error::op_check_sig;

    state::push_bool(verify == error::op_success);
    return error::op_success;
}

// In signing mode, prepare_signature converts key from a private key to
// a public key and generates the signature from key and hash. The signature is
// then verified against the key and hash as if obtained from the script.
template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_sig_verify() NOEXCEPT
{
    if (state::stack_size() < 2)
        return error::op_check_sig_verify1;

    const auto key = state::pop_chunk_();

    if (key->empty())
        return error::op_check_sig_verify2;

    const auto endorsement = state::pop_chunk_();

    // error::op_check_sig_verify_parse causes op_check_sig fail.
    if (endorsement->empty())
        return error::op_check_sig_verify3;

    hash_digest hash;
    ec_signature sig;

    // Parse endorsement into DER signature into an EC signature.
    // Also generates signature hash from endorsement sighash flags.
    // Under bip66 op_check_sig fails if parsed endorsement is not strict DER.
    if (!state::prepare(sig, *key, hash, endorsement))
        return error::op_check_sig_verify_parse;

    // TODO: for signing mode - make key mutable and return above.
    return system::verify_signature(*key, hash, sig) ?
        error::op_success : error::op_check_sig_verify4;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_multisig() NOEXCEPT
{
    const auto verify = op_check_multisig_verify();
    const auto bip66 = state::is_enabled(flags::bip66_rule);

    // BIP66: invalid signature encoding fails the operation.
    if (bip66 && verify == error::op_check_multisig_verify_parse)
        return error::op_check_multisig;

    state::push_bool(verify == error::op_success);
    return error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_multisig_verify() NOEXCEPT
{
    const auto bip147 = state::is_enabled(flags::bip147_rule);

    size_t count;
    if (!state::pop_index32(count))
        return error::op_check_multisig_verify1;

    if (count > max_script_public_keys)
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
    // This check is unique in that it requires a single "zero" on the stack.
    // True here implies variant non-zero ('true', '!= 0', or other than '[]').
    if (state::pop_strict_bool_() && bip147)
        return error::op_check_multisig_verify9;

    ec_signature sig;
    uint8_t sighash_flags;
    typename state::hash_cache cache;

    // Subscript is the same for all signatures.
    const auto sub = state::subscript(endorsements);
    auto endorsement = endorsements.begin();

    // Keys may be empty, endorsements is an ordered subset of corresponding
    // keys, all endorsements must be verified against a key. Under bip66,
    // op_check_multisig fails if any parsed endorsement is not strict DER.
    for (const auto& key: keys)
    {
        // All signatures are valid (empty does not increment the iterator).
        if (endorsement == endorsements.end())
            break;

        // error::op_check_multisig_verify_parse causes op_check_multisig fail.
        if (!(*endorsement)->empty())
        {
            // Parse endorsement into DER signature into an EC signature.
            // Also generates signature hash from endorsement sighash flags.
            if (!state::prepare(sig, *key, cache, sighash_flags,
                **endorsement, *sub))
                return error::op_check_multisig_verify_parse;

            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            const auto& hash = cache.at(sighash_flags);
            BC_POP_WARNING()

            // TODO: for signing mode - make key mutable and return above.
            if (system::verify_signature(*key, hash, sig))
                ++endorsement;
        }
    }

    return endorsement != endorsements.end() ?
        error::op_check_multisig_verify10 : error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_locktime_verify() const NOEXCEPT
{
    // BIP65: nop2 subsumed by checklocktimeverify when bip65 fork is active.
    if (!state::is_enabled(flags::bip65_rule))
        return op_nop(opcode::nop2);

    // BIP65: the tx sequence is 0xffffffff.
    if (state::input().is_final())
        return error::op_check_locktime_verify1;

    // BIP65: the stack is empty.
    // BIP65: the top stack item is negative.
    // BIP65: extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 40 bits are usable.
    uint64_t stack_locktime40;
    if (!state::peek_unsigned40(stack_locktime40))
        return error::op_check_locktime_verify2;

    const auto trans_locktime32 = state::transaction().locktime();

    // BIP65: the stack locktime type differs from that of tx.
    if ((stack_locktime40 < locktime_threshold) !=
        (trans_locktime32 < locktime_threshold))
        return error::op_check_locktime_verify3;

    // BIP65: the stack locktime is greater than the tx locktime.
    return (stack_locktime40 > trans_locktime32) ?
        error::op_check_locktime_verify4 : error::op_success;
}

template <typename Stack>
inline op_error_t interpreter<Stack>::
op_check_sequence_verify() const NOEXCEPT
{
    // BIP112: nop3 subsumed by checksequenceverify when bip112 fork is active.
    if (!state::is_enabled(flags::bip112_rule))
        return op_nop(opcode::nop3);

    // BIP112: the stack is empty.
    // BIP112: the top stack item is negative.
    // BIP112: extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 32 bits are used (33rd-40th discarded).
    uint32_t stack_sequence32;
    if (!state::peek_unsigned32(stack_sequence32))
        return error::op_check_sequence_verify1;

    // Only 32 bits are tested.
    const auto input_sequence32 = state::input().sequence();

    // BIP112: the stack sequence is disabled, treat as nop3.
    if (get_right(stack_sequence32, relative_locktime_disabled_bit))
        return op_nop(opcode::nop3);

    // BIP112: the stack sequence is enabled and tx version less than 2.
    if (state::transaction().version() < relative_locktime_min_version)
        return error::op_check_sequence_verify2;

    // BIP112: the transaction sequence is disabled.
    if (get_right(input_sequence32, relative_locktime_disabled_bit))
        return error::op_check_sequence_verify3;

    // BIP112: the stack sequence type differs from that of tx input.
    if (get_right(stack_sequence32, relative_locktime_time_locked_bit) !=
        get_right(input_sequence32, relative_locktime_time_locked_bit))
        return error::op_check_sequence_verify4;

    // BIP112: the unmasked stack sequence is greater than that of tx sequence.
    return
        (mask_left(stack_sequence32, relative_locktime_mask_left)) >
        (mask_left(input_sequence32, relative_locktime_mask_left)) ?
        error::op_check_sequence_verify5 : error::op_success;
}

// Operation disatch.
// ----------------------------------------------------------------------------
// It is expected that the compiler will produce a very efficient jump table.

// private:
template <typename Stack>
op_error_t interpreter<Stack>::
run_op(const op_iterator& op) NOEXCEPT
{
    const auto code = op->code();

    switch (code)
    {
        case opcode::push_size_0:
            return op_push_number(0);
        case opcode::push_size_1:
        case opcode::push_size_2:
        case opcode::push_size_3:
        case opcode::push_size_4:
        case opcode::push_size_5:
        case opcode::push_size_6:
        case opcode::push_size_7:
        case opcode::push_size_8:
        case opcode::push_size_9:
        case opcode::push_size_10:
        case opcode::push_size_11:
        case opcode::push_size_12:
        case opcode::push_size_13:
        case opcode::push_size_14:
        case opcode::push_size_15:
        case opcode::push_size_16:
        case opcode::push_size_17:
        case opcode::push_size_18:
        case opcode::push_size_19:
        case opcode::push_size_20:
        case opcode::push_size_21:
        case opcode::push_size_22:
        case opcode::push_size_23:
        case opcode::push_size_24:
        case opcode::push_size_25:
        case opcode::push_size_26:
        case opcode::push_size_27:
        case opcode::push_size_28:
        case opcode::push_size_29:
        case opcode::push_size_30:
        case opcode::push_size_31:
        case opcode::push_size_32:
        case opcode::push_size_33:
        case opcode::push_size_34:
        case opcode::push_size_35:
        case opcode::push_size_36:
        case opcode::push_size_37:
        case opcode::push_size_38:
        case opcode::push_size_39:
        case opcode::push_size_40:
        case opcode::push_size_41:
        case opcode::push_size_42:
        case opcode::push_size_43:
        case opcode::push_size_44:
        case opcode::push_size_45:
        case opcode::push_size_46:
        case opcode::push_size_47:
        case opcode::push_size_48:
        case opcode::push_size_49:
        case opcode::push_size_50:
        case opcode::push_size_51:
        case opcode::push_size_52:
        case opcode::push_size_53:
        case opcode::push_size_54:
        case opcode::push_size_55:
        case opcode::push_size_56:
        case opcode::push_size_57:
        case opcode::push_size_58:
        case opcode::push_size_59:
        case opcode::push_size_60:
        case opcode::push_size_61:
        case opcode::push_size_62:
        case opcode::push_size_63:
        case opcode::push_size_64:
        case opcode::push_size_65:
        case opcode::push_size_66:
        case opcode::push_size_67:
        case opcode::push_size_68:
        case opcode::push_size_69:
        case opcode::push_size_70:
        case opcode::push_size_71:
        case opcode::push_size_72:
        case opcode::push_size_73:
        case opcode::push_size_74:
        case opcode::push_size_75:
            return op_push_size(*op);
        case opcode::push_one_size:
            return op_push_one_size(*op);
        case opcode::push_two_size:
            return op_push_two_size(*op);
        case opcode::push_four_size:
            return op_push_four_size(*op);
        case opcode::push_negative_1:
            return op_push_number(-1);
        case opcode::reserved_80:
            return op_unevaluated(code);
        case opcode::push_positive_1:
            return op_push_number(1);
        case opcode::push_positive_2:
            return op_push_number(2);
        case opcode::push_positive_3:
            return op_push_number(3);
        case opcode::push_positive_4:
            return op_push_number(4);
        case opcode::push_positive_5:
            return op_push_number(5);
        case opcode::push_positive_6:
            return op_push_number(6);
        case opcode::push_positive_7:
            return op_push_number(7);
        case opcode::push_positive_8:
            return op_push_number(8);
        case opcode::push_positive_9:
            return op_push_number(9);
        case opcode::push_positive_10:
            return op_push_number(10);
        case opcode::push_positive_11:
            return op_push_number(11);
        case opcode::push_positive_12:
            return op_push_number(12);
        case opcode::push_positive_13:
            return op_push_number(13);
        case opcode::push_positive_14:
            return op_push_number(14);
        case opcode::push_positive_15:
            return op_push_number(15);
        case opcode::push_positive_16:
            return op_push_number(16);
        case opcode::nop:
            return op_nop();
        case opcode::op_ver:
            return op_ver();
        case opcode::if_:
            return op_if();
        case opcode::notif:
            return op_notif();
        case opcode::op_verif:
            return op_verif();
        case opcode::op_vernotif:
            return op_vernotif();
        case opcode::else_:
            return op_else();
        case opcode::endif:
            return op_endif();
        case opcode::verify:
            return op_verify();
        case opcode::op_return:
            return op_return();
        case opcode::toaltstack:
            return op_to_alt_stack();
        case opcode::fromaltstack:
            return op_from_alt_stack();
        case opcode::drop2:
            return op_drop2();
        case opcode::dup2:
            return op_dup2();
        case opcode::dup3:
            return op_dup3();
        case opcode::over2:
            return op_over2();
        case opcode::rot2:
            return op_rot2();
        case opcode::swap2:
            return op_swap2();
        case opcode::ifdup:
            return op_if_dup();
        case opcode::depth:
            return op_depth();
        case opcode::drop:
            return op_drop();
        case opcode::dup:
            return op_dup();
        case opcode::nip:
            return op_nip();
        case opcode::over:
            return op_over();
        case opcode::pick:
            return op_pick();
        case opcode::roll:
            return op_roll();
        case opcode::rot:
            return op_rot();
        case opcode::swap:
            return op_swap();
        case opcode::tuck:
            return op_tuck();
        case opcode::op_cat:
            return op_cat();
        case opcode::op_substr:
            return op_substr();
        case opcode::op_left:
            return op_left();
        case opcode::op_right:
            return op_right();
        case opcode::size:
            return op_size();
        case opcode::op_invert:
            return op_invert();
        case opcode::op_and:
            return op_and();
        case opcode::op_or:
            return op_or();
        case opcode::op_xor:
            return op_xor();
        case opcode::equal:
            return op_equal();
        case opcode::equalverify:
            return op_equal_verify();
        case opcode::reserved_137:
            return op_unevaluated(code);
        case opcode::reserved_138:
            return op_unevaluated(code);
        case opcode::add1:
            return op_add1();
        case opcode::sub1:
            return op_sub1();
        case opcode::op_mul2:
            return op_mul2();
        case opcode::op_div2:
            return op_div2();
        case opcode::negate:
            return op_negate();
        case opcode::abs:
            return op_abs();
        case opcode::not_:
            return op_not();
        case opcode::nonzero:
            return op_nonzero();
        case opcode::add:
            return op_add();
        case opcode::sub:
            return op_sub();
        case opcode::op_mul:
            return op_mul();
        case opcode::op_div:
            return op_div();
        case opcode::op_mod:
            return op_mod();
        case opcode::op_lshift:
            return op_lshift();
        case opcode::op_rshift:
            return op_rshift();
        case opcode::booland:
            return op_bool_and();
        case opcode::boolor:
            return op_bool_or();
        case opcode::numequal:
            return op_num_equal();
        case opcode::numequalverify:
            return op_num_equal_verify();
        case opcode::numnotequal:
            return op_num_not_equal();
        case opcode::lessthan:
            return op_less_than();
        case opcode::greaterthan:
            return op_greater_than();
        case opcode::lessthanorequal:
            return op_less_than_or_equal();
        case opcode::greaterthanorequal:
            return op_greater_than_or_equal();
        case opcode::min:
            return op_min();
        case opcode::max:
            return op_max();
        case opcode::within:
            return op_within();
        case opcode::ripemd160:
            return op_ripemd160();
        case opcode::sha1:
            return op_sha1();
        case opcode::sha256:
            return op_sha256();
        case opcode::hash160:
            return op_hash160();
        case opcode::hash256:
            return op_hash256();
        case opcode::codeseparator:
            return op_codeseparator(op);
        case opcode::checksig:
            return op_check_sig();
        case opcode::checksigverify:
            return op_check_sig_verify();
        case opcode::checkmultisig:
            return op_check_multisig();
        case opcode::checkmultisigverify:
            return op_check_multisig_verify();
        case opcode::nop1:
            return op_nop(code);
        case opcode::checklocktimeverify:
            return op_check_locktime_verify();
        case opcode::checksequenceverify:
            return op_check_sequence_verify();
        case opcode::nop4:
        case opcode::nop5:
        case opcode::nop6:
        case opcode::nop7:
        case opcode::nop8:
        case opcode::nop9:
        case opcode::nop10:
            return op_nop(code);
        default:
            return op_unevaluated(code);
    }
}

// Run the program.
// ----------------------------------------------------------------------------

template <typename Stack>
code interpreter<Stack>::
run() NOEXCEPT
{
    error::op_error_t operation_ec;
    error::script_error_t script_ec;

    // Enforce script size limit (10,000) [0.3.7+].
    // Enforce initial primary stack size limit (520) [bip141].
    // Enforce first op not reserved (not skippable by condition).
    if ((script_ec = state::validate()))
        return script_ec;

    for (auto it = state::begin(); it != state::end(); ++it)
    {
        // An iterator is required only for run_op:op_codeseparator.
        const auto& op = *it;

        // Enforce unconditionally invalid opcodes ("disabled").
        if (op.is_invalid())
            return error::op_invalid;

        // Rule imposed by [0.3.6] soft fork.
        if (op.is_oversized())
            return error::invalid_push_data_size;

        // Enforce opcode count limit (201).
        if (!state::ops_increment(op))
            return error::invalid_operation_count;

        // Conditional evaluation scope.
        if (state::if_(op))
        {
            // Evaluate opcode (switch).
            if ((operation_ec = run_op(it)))
                return operation_ec;

            // Enforce combined stacks size limit (1,000).
            if (state::is_stack_overflow())
                return error::invalid_stack_size;
        }
    }

    // Guard against unbalanced evaluation scope.
    return state::is_balanced() ? error::script_success :
        error::invalid_stack_scope;
}

template <typename Stack>
code interpreter<Stack>::
connect(const context& state, const transaction& tx, uint32_t index) NOEXCEPT
{
    if (index >= tx.inputs())
        return error::inputs_overflow;

    return connect(state, tx, std::next(tx.inputs_ptr()->begin(), index));
}

// TODO: Implement original op_codeseparator concatenation [< 0.3.6].
// TODO: Implement combined script size limit soft fork (20,000) [0.3.6+].
template <typename Stack>
code interpreter<Stack>::
connect(const context& state, const transaction& tx,
    const input_iterator& it) NOEXCEPT
{
    code ec;
    const auto& input = **it;
    if (!input.prevout)
        return error::missing_previous_output;

    // Evaluate input script.
    interpreter in_program(tx, it, state.flags);
    if ((ec = in_program.run()))
        return ec;

    // Evaluate output script using stack copied from input script evaluation.
    const auto& prevout = input.prevout->script_ptr();
    interpreter out_program(in_program, prevout);
    if ((ec = out_program.run()))
    {
        return ec;
    }
    else if (!out_program.is_true(false))
    {
        return error::stack_false;
    }
    else if (prevout->is_pay_to_script_hash(state.flags))
    {
        // Because output script pushed script hash program (bip16).
        if ((ec = connect_embedded(state, tx, it, in_program)))
            return ec;
    }
    else if (prevout->is_pay_to_witness(state.flags))
    {
        // The input script must be empty (bip141).
        if (!input.script().ops().empty())
            return error::dirty_witness;

        // Because output script pushed version and witness program (bip141).
        if ((ec = connect_witness(state, tx, it, *prevout)))
            return ec;
    }
    else if (!input.witness().stack().empty())
    {
        // A non-witness program must have empty witness field (bip141).
        return error::unexpected_witness;
    }

    return error::script_success;
}

BC_PUSH_WARNING(NO_NEW_OR_DELETE)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

template <typename Stack>
code interpreter<Stack>::connect_embedded(const context& state,
    const transaction& tx, const input_iterator& it,
    interpreter& in_program) NOEXCEPT
{
    code ec;
    const auto& input = **it;

    // Input script is limited to relaxed push data operations (bip16).
    if (!script::is_relaxed_push_pattern(input.script().ops()))
        return error::invalid_script_embed;

    // Embedded script must be at the top of the stack (bip16).
    // Evaluate embedded script using stack moved from input script.
    const auto prevout = to_shared<script>(in_program.pop(), false);
    interpreter out_program(std::move(in_program), prevout);
    if ((ec = out_program.run()))
    {
        return ec;
    }
    else if (!out_program.is_true(false))
    {
        return error::stack_false;
    }
    else if (prevout->is_pay_to_witness(state.flags))
    {
        // The input script must be a push of the embedded_script (bip141).
        if (input.script().ops().size() != one)
            return error::dirty_witness;

        // Because output script pushed version/witness program (bip141).
        if ((ec = connect_witness(state, tx, it, *prevout)))
            return ec;
    }
    else if (!input.witness().stack().empty())
    {
        // A non-witness program must have empty witness field (bip141).
        return error::unexpected_witness;
    }

    return error::script_success;
}

BC_POP_WARNING()
BC_POP_WARNING()

template <typename Stack>
code interpreter<Stack>::connect_witness(const context& state,
    const transaction& tx, const input_iterator& it,
    const script& prevout) NOEXCEPT
{
    const auto& input = **it;
    const auto version = prevout.version();

    switch (version)
    {
        case script_version::zero:
        {
            code ec;
            script::cptr script;
            chunk_cptrs_ptr stack;

            if (!input.witness().extract_script(script, stack, prevout))
                return error::invalid_witness;

            // A defined version indicates bip141 is active.
            interpreter program(tx, it, script, state.flags, version, stack);
            if ((ec = program.run()))
                return ec;

            // A v0 script must succeed with a clean true stack (bip141).
            return program.is_true(true) ? error::script_success :
                error::stack_false;
        }

        // These versions are reserved for future extensions (bip141).
        case script_version::reserved:
            return error::script_success;

        case script_version::unversioned:
        default:
            return error::unversioned_script;
    }
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
