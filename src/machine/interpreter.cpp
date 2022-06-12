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
#include <bitcoin/system/machine/interpreter.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace system::chain;
using namespace system::error;

// Operation handlers.
// ----------------------------------------------------------------------------

op_error_t interpreter::op_unevaluated(opcode code) const noexcept
{
    return operation::is_invalid(code) ? error::op_invalid :
        error::op_reserved;
}

// Codes op_nop1..op_nop10 promoted from reserved by [0.3.6] hard fork.
op_error_t interpreter::op_nop(opcode) const noexcept
{
    if (is_enabled(forks::nops_rule))
        return error::op_success;

    // TODO: nops_rule *must* be enabled in test cases and default config.
    // TODO: cats_rule should be enabled in test cases and default config.
    ////return op_unevaluated(code);
    return error::op_success;
}

op_error_t interpreter::op_push_no_size(const operation& op) noexcept
{
    BC_ASSERT_MSG(op.data().size() <= static_cast<uint8_t>(
        opcode::push_size_75), "invalid op_push_size");

    push(op.data_ptr());
    return error::op_success;
}

op_error_t interpreter::push_negative_1() noexcept
{
    push_signed64(-1);
    return error::op_success;
}

op_error_t interpreter::op_push_positive(uint8_t value) noexcept
{
    BC_ASSERT_MSG(value >= 1 && value <= 16, "invalid op_push_positive");

    push_signed64(value);
    return error::op_success;
}

op_error_t interpreter::op_push_one_size(const chunk_cptr& data) noexcept
{
    if (data->size() > max_uint8)
        return error::op_push_data;

    push(data);
    return error::op_success;
}

op_error_t interpreter::op_push_two_size(const chunk_cptr& data) noexcept
{
    if (data->size() > max_uint16)
        return error::op_push_data;

    push(data);
    return error::op_success;
}

op_error_t interpreter::op_push_four_size(const chunk_cptr& data) noexcept
{
    if (data->size() > max_uint32)
        return error::op_push_data;

    push(data);
    return error::op_success;
}

op_error_t interpreter::op_nop() const noexcept
{
    return error::op_success;
}

op_error_t interpreter::op_ver() const noexcept
{
    if (is_enabled(forks::nops_rule))
        return op_unevaluated(opcode::op_ver);

    return error::op_not_implemented;
}

op_error_t interpreter::op_if() noexcept
{
    auto value = false;

    if (is_succeess())
    {
        if (is_empty())
            return error::op_if;

        value = pop_bool_unsafe();
    }

    begin_if(value);
    return error::op_success;
}

op_error_t interpreter::op_notif() noexcept
{
    auto value = false;

    if (is_succeess())
    {
        if (is_empty())
            return error::op_notif;

        value = !pop_bool_unsafe();
    }

    begin_if(value);
    return error::op_success;
}

op_error_t interpreter::op_verif() const noexcept
{
    if (is_enabled(forks::nops_rule))
        return op_unevaluated(opcode::op_verif);

    return error::op_not_implemented;
}

op_error_t interpreter::op_vernotif() const noexcept
{
    if (is_enabled(forks::nops_rule))
        return op_unevaluated(opcode::op_vernotif);

    return error::op_not_implemented;
}

op_error_t interpreter::op_else() noexcept
{
    if (is_balanced())
        return error::op_else;

    else_if_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_endif() noexcept
{
    if (is_balanced())
        return error::op_endif;

    end_if_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_verify() noexcept
{
    if (is_empty())
        return error::op_verify1;

    if (!peek_bool_unsafe())
        return error::op_verify2;

    drop_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_return() const noexcept
{
    if (is_enabled(forks::nops_rule))
        return op_unevaluated(opcode::op_return);
        
    return error::op_not_implemented;
}

op_error_t interpreter::op_to_alt_stack() noexcept
{
    if (is_empty())
        return error::op_to_alt_stack;

    push_alternate(pop_unsafe());
    return error::op_success;
}

op_error_t interpreter::op_from_alt_stack() noexcept
{
    if (is_alternate_empty())
        return error::op_from_alt_stack;

    push(pop_alternate_unsafe());
    return error::op_success;
}

op_error_t interpreter::op_drop2() noexcept
{
    if (size() < 2)
        return error::op_drop2;

    // 0,1,[2,3] => 1,[2,3] => [2,3]
    drop_unsafe();
    drop_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_dup2() noexcept
{
    if (size() < 2)
        return error::op_dup2;

    // [0,1,2,3] => 1, [0,1,2,3] =>  0,1,[0,1,2,3]
    push(peek_unsafe(1));
    push(peek_unsafe(1));
    return error::op_success;
}

op_error_t interpreter::op_dup3() noexcept
{
    if (size() < 3)
        return error::op_dup3;

    // [0,1,2,3] => 2,[0,1,2,3] => 1,2,[0,1,2,3] => 0,1,2,[0,1,2,3]
    push(peek_unsafe(2));
    push(peek_unsafe(2));
    push(peek_unsafe(2));
    return error::op_success;
}

op_error_t interpreter::op_over2() noexcept
{
    if (size() < 4)
        return error::op_over2;

    // [0,1,2,3] => 3,[0,1,2,3] => 2,3,[0,1,2,3]
    push(peek_unsafe(3));
    push(peek_unsafe(3));
    return error::op_success;
}

op_error_t interpreter::op_rot2() noexcept
{
    if (size() < 6)
        return error::op_rot2;

    // [0,1,2,3,4,5] => [4,1,2,3,0,5] => [4,5,2,3,0,1] =>
    // [4,5,0,3,2,1] => [4,5,0,1,2,3]
    swap_unsafe(0, 4);
    swap_unsafe(1, 5);
    swap_unsafe(2, 4);
    swap_unsafe(3, 5);
    return error::op_success;
}

op_error_t interpreter::op_swap2() noexcept
{
    if (size() < 4)
        return error::op_swap2;

    // [0,1,2,3] => [0,3,2,1] => [2,3,0,1]
    swap_unsafe(1,3);
    swap_unsafe(0,2);
    return error::op_success;
}

op_error_t interpreter::op_if_dup() noexcept
{
    if (is_empty())
        return error::op_if_dup;

    // [0,1,2] => 0,[0,1,2]
    if (peek_bool_unsafe())
        push(peek_unsafe(0));

    return error::op_success;
}

op_error_t interpreter::op_depth() noexcept
{
    // [0,1,2] => 3,[0,1,2]
    push_length(size());
    return error::op_success;
}

op_error_t interpreter::op_drop() noexcept
{
    if (is_empty())
        return error::op_drop;

    // 0,[1,2] => [1,2]
    drop_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_dup() noexcept
{
    if (is_empty())
        return error::op_dup;

    // [0,1,2] => 0,[0,1 2]
    push(peek_unsafe(0));
    return error::op_success;
}

op_error_t interpreter::op_nip() noexcept
{
    if (size() < 2)
        return error::op_nip;

    // [0,1,2] => 1,[0,2] => [0,2]
    swap_unsafe(0, 1);
    drop_unsafe();
    return error::op_success;
}

op_error_t interpreter::op_over() noexcept
{
    if (size() < 2)
        return error::op_over;

    // [0,1] => 1,[0,1]
    push(peek_unsafe(1));
    return error::op_success;
}

op_error_t interpreter::op_pick() noexcept
{
    size_t index;

    // 2,[0,1,2,3] => {2} [0,1,2,3]
    if (!pop_index32(index))
        return error::op_pick;

    // [0,1,2,3] => 2,[0,1,2,3]
    push(peek_unsafe(index));
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
// This remains the current satoshi implementation (std::vector):
//case OP_ROLL:
//{
//    valtype vch = stacktop(-n-1);
//    stack.erase(stack.end()-n-1);
//    stack.push_back(vch);
//}
// ****************************************************************************
op_error_t interpreter::op_roll() noexcept
{
    size_t index;

    // 998,[0,1,2,...,997,998,999] => {998} [0,1,2,...,997,998,999] 
    if (!pop_index32(index))
        return error::op_roll;

    auto temporary = peek_unsafe(index);

    // Shifts maximum of n-1 references within vector of n.
    // [0,1,2,...,997,xxxx,999] => [0,1,2,...,997,999]
    erase_unsafe(index);

    // [0,1,2,...,997,999] => 998,[0,1,2,...,997,999]
    push(std::move(temporary));
    return error::op_success;
}

op_error_t interpreter::op_rot() noexcept
{
    if (size() < 3)
        return error::op_rot;

    // [0,1,2,3] = > [0,2,1,3] => [2,0,1,3]
    swap_unsafe(1,2);
    swap_unsafe(0,1);
    return error::op_success;
}

op_error_t interpreter::op_swap() noexcept
{
    if (size() < 2)
        return error::op_swap;

    // [0,1,2] = > [1,0,2]
    swap_unsafe(0,1);
    return error::op_success;
}

op_error_t interpreter::op_tuck() noexcept
{
    if (size() < 2)
        return error::op_tuck;

    // [0,1,2] = > [1,0,2]  => 0,[1,0,2]
    swap_unsafe(0, 1);
    push(peek_unsafe(1));
    return error::op_success;
}

op_error_t interpreter::op_cat() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_cat);

    return error::op_not_implemented;
}

op_error_t interpreter::op_substr() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_substr);

    return error::op_not_implemented;
}

op_error_t interpreter::op_left() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_left);

    return error::op_not_implemented;
}

op_error_t interpreter::op_right() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_right);

    return error::op_not_implemented;
}

op_error_t interpreter::op_size() noexcept
{
    if (is_empty())
        return error::op_size;

    push_length(peek_unsafe(0)->size());
    return error::op_success;
}

op_error_t interpreter::op_invert() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_invert);

    return error::op_not_implemented;
}

op_error_t interpreter::op_and() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_and);

    return error::op_not_implemented;
}

op_error_t interpreter::op_or() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_or);

    return error::op_not_implemented;
}

op_error_t interpreter::op_xor() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_xor);

    return error::op_not_implemented;
}

op_error_t interpreter::op_equal() noexcept
{
    if (size() < 2)
        return error::op_equal;

    push_bool(*pop_unsafe() == *pop_unsafe());
    return error::op_success;
}

op_error_t interpreter::op_equal_verify() noexcept
{
    if (size() < 2)
        return error::op_equal_verify1;

    return (*pop_unsafe() == *pop_unsafe()) ? error::op_success :
        error::op_equal_verify2;
}

op_error_t interpreter::op_add1() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_add1;

    push_signed64(increment<int64_t>(number));
    return error::op_success;
}

op_error_t interpreter::op_sub1() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_sub1;

    push_signed64(decrement<int64_t>(number));
    return error::op_success;
}

op_error_t interpreter::op_mul2() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_mul2);

    return error::op_not_implemented;
}

op_error_t interpreter::op_div2() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_div2);

    return error::op_not_implemented;
}

op_error_t interpreter::op_negate() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_negate;

    push_signed64(negate<int64_t>(number));
    return error::op_success;
}

op_error_t interpreter::op_abs() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_abs;

    push_signed64(absolute(number));
    return error::op_success;
}

op_error_t interpreter::op_not() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_not;

    push_bool(!to_bool(number));
    return error::op_success;
}

op_error_t interpreter::op_nonzero() noexcept
{
    int32_t number;
    if (!pop_signed32(number))
        return error::op_nonzero;

    push_bool(is_nonzero(number));
    return error::op_success;
}

op_error_t interpreter::op_add() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_add;

    push_signed64(add<int64_t>(left, right));
    return error::op_success;
}

op_error_t interpreter::op_sub() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_sub;

    push_signed64(subtract<int64_t>(left, right));
    return error::op_success;
}

op_error_t interpreter::op_mul() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_mul);

    return error::op_not_implemented;
}

op_error_t interpreter::op_div() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_div);

    return error::op_not_implemented;
}

op_error_t interpreter::op_mod() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_mod);

    return error::op_not_implemented;
}

op_error_t interpreter::op_lshift() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_lshift);

    return error::op_not_implemented;
}

op_error_t interpreter::op_rshift() const noexcept
{
    if (is_enabled(forks::cats_rule))
        return op_unevaluated(opcode::op_rshift);

    return error::op_not_implemented;
}

op_error_t interpreter::op_bool_and() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_bool_and;

    push_bool(to_bool(left) && to_bool(right));
    return error::op_success;
}

op_error_t interpreter::op_bool_or() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_bool_or;

    push_bool(to_bool(left) || to_bool(right));
    return error::op_success;
}

op_error_t interpreter::op_num_equal() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_num_equal;

    push_bool(left == right);
    return error::op_success;
}

op_error_t interpreter::op_num_equal_verify() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_num_equal_verify1;

    return (left == right) ? error::op_success : error::op_num_equal_verify2;
}

op_error_t interpreter::op_num_not_equal() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_num_not_equal;

    push_bool(left != right);
    return error::op_success;
}

op_error_t interpreter::op_less_than() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_less_than;

    push_bool(left < right);
    return error::op_success;
}

op_error_t interpreter::op_greater_than() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_greater_than;

    push_bool(left > right);
    return error::op_success;
}

op_error_t interpreter::op_less_than_or_equal() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_less_than_or_equal;

    push_bool(left <= right);
    return error::op_success;
}

op_error_t interpreter::op_greater_than_or_equal() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_greater_than_or_equal;

    push_bool(left >= right);
    return error::op_success;
}

op_error_t interpreter::op_min() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_min;

    push_signed64(lesser<int64_t>(left, right));
    return error::op_success;
}

op_error_t interpreter::op_max() noexcept
{
    int32_t right, left;
    if (!pop_binary32(left, right))
        return error::op_max;

    push_signed64(greater<int64_t>(left, right));
    return error::op_success;
}

op_error_t interpreter::op_within() noexcept
{
    int32_t upper, lower, value;
    if (!pop_ternary32(upper, lower, value))
        return error::op_within;

    push_bool((lower <= value) && (value < upper));
    return error::op_success;
}

op_error_t interpreter::op_ripemd160() noexcept
{
    if (is_empty())
        return error::op_ripemd160;

    push_chunk(ripemd160_hash_chunk(*pop_unsafe()));
    return error::op_success;
}

op_error_t interpreter::op_sha1() noexcept
{
    if (is_empty())
        return error::op_sha1;

    push_chunk(sha1_hash_chunk(*pop_unsafe()));
    return error::op_success;
}

op_error_t interpreter::op_sha256() noexcept
{
    if (is_empty())
        return error::op_sha256;

    push_chunk(sha256_hash_chunk(*pop_unsafe()));
    return error::op_success;
}

op_error_t interpreter::op_hash160() noexcept
{
    if (is_empty())
        return error::op_hash160;

    push_chunk(ripemd160_hash_chunk(sha256_hash(*pop_unsafe())));
    return error::op_success;
}

op_error_t interpreter::op_hash256() noexcept
{
    if (is_empty())
        return error::op_hash256;

    push_chunk(sha256_hash_chunk(sha256_hash(*pop_unsafe())));
    return error::op_success;
}

op_error_t interpreter::op_codeseparator(const op_iterator& op) noexcept
{
    // Not thread safe for the script (changes script object metadata).
    return set_subscript(op) ? error::op_success :
        error::op_code_separator;
}

op_error_t interpreter::op_check_sig() noexcept
{
    const auto verify = op_check_sig_verify();
    const auto bip66 = is_enabled(forks::bip66_rule);

    // BIP66: invalid signature encoding fails the operation.
    if (bip66 && verify == error::op_check_sig_verify_parse)
        return error::op_check_sig;

    push_bool(verify == error::op_success);
    return error::op_success;
}

// In signing mode, prepare_signature converts key from a private key to
// a public key and generates the signature from key and hash. The signature is
// then verified against the key and hash as if obtained from the script.
op_error_t interpreter::op_check_sig_verify() noexcept
{
    if (size() < 2)
        return error::op_check_sig_verify1;

    const auto key = pop_unsafe();

    if (key->empty())
        return error::op_check_sig_verify2;

    const auto endorsement = pop_unsafe();

    // error::op_check_sig_verify_parse causes op_check_sig fail.
    if (endorsement->empty())
        return error::op_check_sig_verify3;

    hash_digest hash;
    ec_signature sig;

    // Parse endorsement into DER signature into an EC signature.
    // Also generates signature hash from endorsement sighash flags.
    // Under bip66 op_check_sig fails if parsed endorsement is not strict DER.
    if (!prepare(sig, *key, hash, endorsement))
        return error::op_check_sig_verify_parse;

    // TODO: for signing mode - make key mutable and return above.
    return system::verify_signature(*key, hash, sig) ?
        error::op_success : error::op_check_sig_verify4;
}

op_error_t interpreter::op_check_multisig() noexcept
{
    const auto verify = op_check_multisig_verify();
    const auto bip66 = is_enabled(forks::bip66_rule);

    // BIP66: invalid signature encoding fails the operation.
    if (bip66 && verify == error::op_check_multisig_verify_parse)
        return error::op_check_multisig;

    push_bool(verify == error::op_success);
    return error::op_success;
}

op_error_t interpreter::op_check_multisig_verify() noexcept
{
    const auto bip147 = is_enabled(forks::bip147_rule);

    size_t count;
    if (!pop_index32(count))
        return error::op_check_multisig_verify1;

    if (count > max_script_public_keys)
        return error::op_check_multisig_verify2;

    if (!ops_increment(count))
        return error::op_check_multisig_verify3;

    chunk_cptrs keys;
    if (!pop_count(keys, count))
        return error::op_check_multisig_verify4;

    if (!pop_index32(count))
        return error::op_check_multisig_verify5;

    if (count > keys.size())
        return error::op_check_multisig_verify6;

    chunk_cptrs endorsements;
    if (!pop_count(endorsements, count))
        return error::op_check_multisig_verify7;

    if (is_empty())
        return error::op_check_multisig_verify8;

    //*************************************************************************
    // CONSENSUS: Satoshi bug, discard stack element, malleable until bip147.
    //*************************************************************************
    if (!pop_unsafe()->empty() && bip147)
        return error::op_check_multisig_verify9;

    uint8_t flags;
    ec_signature sig;
    program::hash_cache cache;

    // Subscript is the same for all signatures.
    const auto sub = subscript(endorsements);
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
            if (!prepare(sig, *key, cache, flags, **endorsement, *sub))
                return error::op_check_multisig_verify_parse;

            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            const auto& hash = cache.at(flags);
            BC_POP_WARNING()

            // TODO: for signing mode - make key mutable and return above.
            if (system::verify_signature(*key, hash, sig))
                ++endorsement;
        }
    }

    return endorsement != endorsements.end() ?
        error::op_check_multisig_verify10 : error::op_success;
}

op_error_t interpreter::op_check_locktime_verify() const noexcept
{
    // BIP65: nop2 subsumed by checklocktimeverify when bip65 fork is active.
    if (!is_enabled(forks::bip65_rule))
        return op_nop(opcode::nop2);

    // BIP65: the tx sequence is 0xffffffff.
    if (input().is_final())
        return error::op_check_locktime_verify1;

    // BIP65: the stack is empty.
    // BIP65: the top stack item is negative.
    // BIP65: extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 39 bits are usable (negative discarded).
    uint64_t stack_locktime39;
    if (!peek_unsigned39(stack_locktime39))
        return error::op_check_locktime_verify2;

    const auto trans_locktime32 = transaction().locktime();

    // BIP65: the stack locktime type differs from that of tx.
    if ((stack_locktime39 < locktime_threshold) !=
        (trans_locktime32 < locktime_threshold))
        return error::op_check_locktime_verify3;

    // BIP65: the stack locktime is greater than the tx locktime.
    return (stack_locktime39 > trans_locktime32) ?
        error::op_check_locktime_verify4 : error::op_success;
}

op_error_t interpreter::op_check_sequence_verify() const noexcept
{
    // BIP112: nop3 subsumed by checksequenceverify when bip112 fork is active.
    if (!is_enabled(forks::bip112_rule))
        return op_nop(opcode::nop3);

    // BIP112: the stack is empty.
    // BIP112: the top stack item is negative.
    // BIP112: extend the (signed) script number range to 5 bytes.
    // The stack top is positive and 32 bits are used (33rd-40th discarded).
    uint32_t stack_sequence32;
    if (!peek_unsigned32(stack_sequence32))
        return error::op_check_sequence_verify1;

    // Only 32 bits are tested.
    const auto input_sequence32 = input().sequence();

    // BIP112: the stack sequence is disabled, treat as nop3.
    if (get_right(stack_sequence32, relative_locktime_disabled_bit))
        return op_nop(opcode::nop3);

    // BIP112: the stack sequence is enabled and tx version less than 2.
    if (transaction().version() < relative_locktime_min_version)
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
op_error_t interpreter::run_op(const op_iterator& op) noexcept
{
    const auto code = op->code();

    switch (code)
    {
        case opcode::push_size_0:
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
            return op_push_no_size(*op);
        case opcode::push_one_size:
            return op_push_one_size(op->data_ptr());
        case opcode::push_two_size:
            return op_push_two_size(op->data_ptr());
        case opcode::push_four_size:
            return op_push_four_size(op->data_ptr());
        case opcode::push_negative_1:
            return push_negative_1();
        case opcode::reserved_80:
            return op_unevaluated(code);
        case opcode::push_positive_1:
            return op_push_positive(1);
        case opcode::push_positive_2:
            return op_push_positive(2);
        case opcode::push_positive_3:
            return op_push_positive(3);
        case opcode::push_positive_4:
            return op_push_positive(4);
        case opcode::push_positive_5:
            return op_push_positive(5);
        case opcode::push_positive_6:
            return op_push_positive(6);
        case opcode::push_positive_7:
            return op_push_positive(7);
        case opcode::push_positive_8:
            return op_push_positive(8);
        case opcode::push_positive_9:
            return op_push_positive(9);
        case opcode::push_positive_10:
            return op_push_positive(10);
        case opcode::push_positive_11:
            return op_push_positive(11);
        case opcode::push_positive_12:
            return op_push_positive(12);
        case opcode::push_positive_13:
            return op_push_positive(13);
        case opcode::push_positive_14:
            return op_push_positive(14);
        case opcode::push_positive_15:
            return op_push_positive(15);
        case opcode::push_positive_16:
            return op_push_positive(16);
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

// Operation loop (script execution).
// ----------------------------------------------------------------------------

code interpreter::run() noexcept
{
    error::op_error_t ec;

    // Enforce script size limit (10,000) [0.3.7+].
    // Enforce initial primary stack size limit (520) [bip141].
    // Enforce first op not reserved (not skippable by condition).
    if (!is_valid())
        return error::invalid_script;

    for (auto it = begin(); it != end(); ++it)
    {
        // An iterator is required only for run_op:op_codeseparator.
        const auto& op = *it;

        // Enforce push data limit (520) [0.3.6+].
        if (op.is_oversized())
            return error::invalid_push_data_size;

        // Enforce unconditionally invalid opcodes ("disabled").
        if (op.is_invalid())
            return error::op_invalid;

        // Enforce opcode count limit (201).
        if (!ops_increment(op))
            return error::invalid_operation_count;

        // Conditional evaluation scope.
        if (if_(op))
        {
            // Evaluate opcode (switch).
            if ((ec = run_op(it)))
                return ec;

            // Enforce combined stacks size limit (1,000).
            if (is_overflow())
                return error::invalid_stack_size;
        }
    }

    // Guard against unbalanced evaluation scope.
    return is_balanced() ? error::script_success :
        error::invalid_stack_scope;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
