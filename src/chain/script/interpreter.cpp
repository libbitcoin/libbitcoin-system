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
#include <bitcoin/bitcoin/chain/script/interpreter.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/evaluation_context.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/rule_fork.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/chain/script/sighash_algorithm.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>

namespace libbitcoin {
namespace chain {
    
enum class signature_parse_result
{
    valid,
    invalid,
    lax_encoding
};

// Operations.
//-----------------------------------------------------------------------------
// shared handler
static bool op_disabled(opcode)
{
    BITCOIN_ASSERT_MSG(false, "disabled opcode in run");
    return false;
}

// shared handler
static bool op_reserved(opcode)
{
    return false;
}

// shared handler
static bool op_nop(opcode)
{
    return true;
}

// shared handler
static bool op_push_size(evaluation_context& context, const operation& op)
{
    static constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);
    const auto size = op.code();
    BITCOIN_ASSERT(op.data().size() <= op_75);
    context.push_copy(op.data());
    return true;
}

// shared handler
static bool op_push_size(evaluation_context& context, const data_chunk& data,
    uint32_t DEBUG_ONLY(size_limit))
{
    BITCOIN_ASSERT(data.size() <= size_limit);
    context.push_copy(data);
    return true;
}

// shared handler
static bool op_push_number(evaluation_context& context, uint8_t value)
{
    // This handles positive_0 identically to op_push_size with empty data.
    BITCOIN_ASSERT(value == script_number::negative_1 || 
        value <= script_number::positive_16);
    context.push_move({ value });
    return true;
}

static bool op_if(evaluation_context& context)
{
    auto value = false;

    if (context.succeeded())
    {
        if (context.empty())
            return false;

        value = context.stack_state();
        context.pop();
    }

    context.open(value);
    return true;
}

static bool op_notif(evaluation_context& context)
{
    auto value = false;

    if (context.succeeded())
    {
        if (context.empty())
            return false;

        value = !context.stack_state();
        context.pop();
    }

    context.open(value);
    return true;
}

static bool op_else(evaluation_context& context)
{
    if (context.closed())
        return false;

    context.negate();
    return true;
}

static bool op_endif(evaluation_context& context)
{
    if (context.closed())
        return false;

    context.close();
    return true;
}

static bool op_verify(evaluation_context& context)
{
    if (context.empty())
        return false;

    if (!context.stack_state())
        return false;

    context.pop();
    return true;
}

static bool op_return(evaluation_context& context)
{
    // In terms of validation this behaves identically to reserved opcodes.
    return false;
}

static bool op_to_alt_stack(evaluation_context& context)
{
    if (context.empty())
        return false;

    context.push_alternate(context.pop());
    return true;
}

static bool op_from_alt_stack(evaluation_context& context)
{
    if (context.empty_alternate())
        return false;

    context.push_move(context.pop_alternate());
    return true;
}

static bool op_drop2(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    context.pop();
    context.pop();
    return true;
}

static bool op_dup2(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    auto item1 = context.item(1);
    auto item0 = context.item(0);

    context.push_move(std::move(item1));
    context.push_move(std::move(item0));
    return true;
}

static bool op_dup3(evaluation_context& context)
{
    if (context.size() < 3)
        return false;

    auto item2 = context.item(2);
    auto item1 = context.item(1);
    auto item0 = context.item(0);

    context.push_move(std::move(item2));
    context.push_move(std::move(item1));
    context.push_move(std::move(item0));
    return true;
}

static bool op_over2(evaluation_context& context)
{
    if (context.size() < 4)
        return false;

    auto item3 = context.item(3);
    auto item2 = context.item(2);

    context.push_move(std::move(item3));
    context.push_move(std::move(item2));
    return true;
}

static bool op_rot2(evaluation_context& context)
{
    if (context.size() < 6)
        return false;

    const auto position_5 = context.position(5);
    const auto position_4 = context.position(4);

    auto copy_5 = *position_5;
    auto copy_4 = *position_4;

    context.erase(position_5, position_4 + 1);
    context.push_move(std::move(copy_5));
    context.push_move(std::move(copy_4));
    return true;
}

static bool op_swap2(evaluation_context& context)
{
    if (context.size() < 4)
        return false;

    context.swap(3, 1);
    context.swap(2, 0);
    return true;
}

static bool op_if_dup(evaluation_context& context)
{
    if (context.empty())
        return false;

    if (context.stack_state())
        context.duplicate(0);

    return true;
}

static bool op_depth(evaluation_context& context)
{
    const script_number stack_size(context.size());
    context.push_move(stack_size.data());
    return true;
}

static bool op_drop(evaluation_context& context)
{
    if (context.empty())
        return false;

    context.pop();
    return true;
}

static bool op_dup(evaluation_context& context)
{
    if (context.empty())
        return false;

    context.duplicate(0);
    return true;
}

static bool op_nip(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    context.erase(context.position(1));
    return true;
}

static bool op_over(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    context.duplicate(1);
    return true;
}

static bool op_pick(evaluation_context& context)
{
    data_stack::iterator position;
    if (!context.pop_position(position))
        return false;

    context.push_copy(*position);
    return true;
}

static bool op_roll(evaluation_context& context)
{
    data_stack::iterator position;
    if (!context.pop_position(position))
        return false;

    auto copy = *position;
    context.erase(position);
    context.push_move(std::move(copy));
    return true;
}

static bool op_rot(evaluation_context& context)
{
    if (context.size() < 3)
        return false;

    context.swap(2, 1);
    context.swap(1, 0);
    return true;
}

static bool op_swap(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    context.swap(1, 0);
    return true;
}

static bool op_tuck(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    auto first = context.pop();
    auto second = context.pop();
    context.push_copy(first);
    context.push_move(std::move(second));
    context.push_move(std::move(first));
    return true;
}

static bool op_size(evaluation_context& context)
{
    if (context.empty())
        return false;

    auto top = context.pop();
    const auto size = top.size();
    context.push_move(std::move(top));
    context.push_move(script_number(size).data());
    return true;
}

static bool op_equal(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    context.push(context.pop() == context.pop());
    return true;
}

static bool op_equal_verify(evaluation_context& context)
{
    if (context.size() < 2)
        return false;

    return context.pop() == context.pop();
}

static bool op_add1(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    //*************************************************************************
    // CONSENSUS: overflow potential.
    //*************************************************************************
    number += 1;
    context.push_move(number.data());
    return true;
}

static bool op_sub1(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    //*************************************************************************
    // CONSENSUS: underflow potential.
    //*************************************************************************
    number -= 1;
    context.push_move(number.data());
    return true;
}

static bool op_negate(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    //*************************************************************************
    // CONSENSUS: overflow potential.
    //*************************************************************************
    number = -number;
    context.push_move(number.data());
    return true;
}

static bool op_abs(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    //*************************************************************************
    // CONSENSUS: overflow potential.
    //*************************************************************************
    if (number < 0)
        number = -number;

    context.push_move(number.data());
    return true;
}

static bool op_not(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    context.push(number == 0);
    return true;
}

static bool op_nonzero(evaluation_context& context)
{
    script_number number;
    if (!context.pop(number))
        return false;

    context.push(number != 0);
    return true;
}

static bool op_add(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    //*************************************************************************
    // CONSENSUS: overflow potential.
    //*************************************************************************
    const auto result = first + second;
    context.push_move(result.data());
    return true;
}

static bool op_sub(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    //*************************************************************************
    // CONSENSUS: underflow potential.
    //*************************************************************************
    const auto result = second - first;
    context.push_move(result.data());
    return true;
}

static bool op_bool_and(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(first != 0 && second != 0);
    return true;
}

static bool op_bool_or(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(first != 0 || second != 0);
    return true;
}

static bool op_num_equal(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(first == second);
    return true;
}

static bool op_num_equal_verify(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    return first == second;
}

static bool op_num_not_equal(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(first != second);
    return true;
}

static bool op_less_than(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(second < first);
    return true;
}

static bool op_greater_than(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(second > first);
    return true;
}

static bool op_less_than_or_equal(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(second <= first);
    return true;
}

static bool op_greater_than_or_equal(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    context.push(second >= first);
    return true;
}

static bool op_min(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    if (second < first)
        context.push_move(second.data());
    else
        context.push_move(first.data());

    return true;
}

static bool op_max(evaluation_context& context)
{
    script_number first, second;
    if (!context.pop_binary(first, second))
        return false;

    auto greater = second > first ? second.data() : first.data();
    context.push_move(std::move(greater));
    return true;
}

static bool op_within(evaluation_context& context)
{
    script_number first, second, third;
    if (!context.pop_ternary(first, second, third))
        return false;

    context.push(second <= third && third < first);
    return true;
}

static bool op_ripemd160(evaluation_context& context)
{
    if (context.empty())
        return false;

    // TODO: move array buffer into vector.
    const auto hash = ripemd160_hash(context.pop());
    context.push_move(to_chunk(hash));
    return true;
}

static bool op_sha1(evaluation_context& context)
{
    if (context.empty())
        return false;

    // TODO: create sha1_hash overload that emits to data_chunk.
    context.push_move(to_chunk(sha1_hash(context.pop())));
    return true;
}

static bool op_sha256(evaluation_context& context)
{
    if (context.empty())
        return false;

    // TODO: create sha256_hash overload that emits to data_chunk.
    context.push_move(to_chunk(sha256_hash(context.pop())));
    return true;
}

static bool op_hash160(evaluation_context& context)
{
    if (context.empty())
        return false;

    // TODO: create bitcoin_short_hash overload that emits to data_chunk.
    context.push_move(to_chunk(bitcoin_short_hash(context.pop())));
    return true;
}

static bool op_hash256(evaluation_context& context)
{
    if (context.empty())
        return false;

    // TODO: create bitcoin_hash overload that emits to data_chunk.
    context.push_move(to_chunk(bitcoin_hash(context.pop())));
    return true;
}

static bool op_code_seperator(evaluation_context& context,
    const operation::const_iterator program_counter)
{
    context.set_jump(program_counter + 1);
    return true;
}

static signature_parse_result op_check_sig_verify(evaluation_context& context,
    const script& script, const transaction& tx, uint32_t input_index)
{
    if (context.size() < 2)
        return signature_parse_result::invalid;

    const auto pubkey = context.pop();
    const auto endorsement = context.pop();

    uint8_t sighash_type;
    ec_signature signature;
    der_signature distinguished;
    auto strict = script::is_enabled(context.flags(), rule_fork::bip66_rule);

    // Parse endorsement into sighash_type and distinguished.
    if (!parse_endorsement(sighash_type, distinguished, endorsement))
        return signature_parse_result::invalid;

    // Parse DER signature into an EC signature.
    if (!parse_signature(signature, distinguished, strict))
        return strict ? signature_parse_result::lax_encoding :
            signature_parse_result::invalid;

    // Create a subscript with endorsements stripped.
    auto script_code = context.subscript();
    script_code.purge({ endorsement });

    return script::check_signature(signature, sighash_type, pubkey,
        script_code, tx, input_index) ? signature_parse_result::valid :
        signature_parse_result::invalid;
}

static bool op_check_sig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index)
{
    switch (op_check_sig_verify(context, script, tx, input_index))
    {
        case signature_parse_result::valid:
            context.push(true);
            break;
        case signature_parse_result::invalid:
            context.push(false);
            break;
        case signature_parse_result::lax_encoding:
            return false;
    }

    return true;
}

static signature_parse_result op_check_multisig_verify(
    evaluation_context& context, const script& script, const transaction& tx,
    uint32_t input_index)
{
    int32_t pubkeys_count;
    if (!context.pop(pubkeys_count))
        return signature_parse_result::invalid;

    if (!context.update_pubkey_count(pubkeys_count))
        return signature_parse_result::invalid;

    data_stack public_keys;
    if (!context.pop(public_keys, pubkeys_count))
        return signature_parse_result::invalid;

    int32_t sigs_count;
    if (!context.pop(sigs_count))
        return signature_parse_result::invalid;

    if (sigs_count < 0 || sigs_count > pubkeys_count)
        return signature_parse_result::invalid;

    data_stack endorsements;
    if (!context.pop(endorsements, sigs_count))
        return signature_parse_result::invalid;

    if (context.empty())
        return signature_parse_result::invalid;

    //*****************************************************************************
    // CONSENSUS: Due to a satoshi bug, read and discard an extra op/byte.
    // This is a source of malleability since the value is ignored.
    //*****************************************************************************
    context.pop();

    uint8_t sighash_type;
    ec_signature signature;
    der_signature distinguished;
    auto public_key = public_keys.begin();
    auto strict = script::is_enabled(context.flags(), rule_fork::bip66_rule);

    // Before entering the loop create a subscript with all endorsements stripped.
    auto script_code = context.subscript();
    script_code.purge(endorsements);

    // The exact number of signatures are required and must be in order.
    // One key can validate more than one script. So we always advance 
    // until we exhaust either pubkeys (fail) or signatures (pass).
    for (const auto& endorsement: endorsements)
    {
        // Parse endorsement into sighash_type and distinguished.
        if (!parse_endorsement(sighash_type, distinguished, endorsement))
            return signature_parse_result::invalid;

        // Parse DER signature into an EC signature.
        if (!parse_signature(signature, distinguished, strict))
            return strict ? signature_parse_result::lax_encoding :
                signature_parse_result::invalid;

        while (true)
        {
            if (script::check_signature(signature, sighash_type, *public_key,
                script_code, tx, input_index))
                break;

            if (++public_key == public_keys.end())
                return signature_parse_result::invalid;
        }
    }

    return signature_parse_result::valid;
}

static bool op_check_multisig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index)
{
    switch (op_check_multisig_verify(context, script, tx, input_index))
    {
        case signature_parse_result::valid:
            context.push(true);
            break;
        case signature_parse_result::invalid:
            context.push(false);
            break;
        case signature_parse_result::lax_encoding:
            return false;
    }

    return true;
}

static bool op_check_locktime_verify(evaluation_context& context,
    const script& script, const transaction& tx, uint32_t input_index)
{
    // nop2 is subsumed by checklocktimeverify when bip65 fork is active.
    if (!script::is_enabled(context.flags(), rule_fork::bip65_rule))
        return op_nop(opcode::nop2);

    if (input_index >= tx.inputs().size())
        return false;

    // BIP65: the nSequence field of the txin is 0xffffffff.
    if (tx.inputs()[input_index].is_final())
        return false;

    // BIP65: the stack is empty.
    // BIP65: extend the (signed) CLTV script number range to 5 bytes.
    script_number number;
    if (!context.pop(number, max_cltv_number_size))
        return false;

    // BIP65: the top item on the stack is less than 0.
    if (number < 0)
        return false;

    // TODO: confirm the domain of context.pop_stack() above is uint32_t.
    // If so there is no reason to cast into 64 bit here, just use uint32_t.
    // The value is positive, so it is safe to cast to uint64_t.
    const auto stack = static_cast<uint64_t>(number.int64());

    // BIP65: the stack lock-time type differs from that of tx nLockTime.
    if ((stack < locktime_threshold) != (tx.locktime() < locktime_threshold))
        return false;

    // BIP65: the top stack item is greater than the tx's nLockTime.
    return stack <= tx.locktime();
}

// Validation - run.
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
//
// TODO:
// Refactor context to allow simple: script.run(context)
// * remove script from context and directly use script iterator for ops.
// * keep jump on context.
// * optionally capture tx/input_index on context (only required for sigs).
// * expose context signature checker that invokes jump script on tx/input
//   given inputs parsed from the current stack.
// * fail signature check if there is no context.transaction/input_index.
//_____________________________________________________________________________

// The script paramter is NOT always tx.indexes[input_index].script.
bool interpreter::run(const transaction& tx, uint32_t input_index,
    const script& script, evaluation_context& context)
{
    // SCRIPT_VERIFY_SIGPUSHONLY
    ////if (!script.is_relaxed_push_data_only())
    ////    return false;

    if (!context.set_script(script))
        return false;

    for (auto pc = context.begin(); pc != context.end(); ++pc)
    {
        if (pc->is_oversized() || pc->is_disabled())
            return false;

        if (!context.update_operation_count(*pc))
            return false;

        // Reserved codes may be skipped (allowed) so can't handle prior.
        // Disabled codes can't be skipped so they must be handled prior.
        if (context.is_short_circuited(*pc))
            continue;

        if (!run_op(pc, tx, input_index, script, context))
            return false;

        if (context.is_stack_overflow())
            return false;
    }

    // SCRIPT_VERIFY_CLEANSTACK
    ////if (!context.empty())
    ////    return false;

    // Confirm that scopes are paired.
    return context.closed();
}

bool interpreter::run_op(operation::const_iterator pc, const transaction& tx,
    uint32_t input_index, const script& script, evaluation_context& context)
{
    const auto code = pc->code();
    const auto data = pc->data();
    BITCOIN_ASSERT(data.empty() || operation::is_push(code));

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
            return op_push_size(context, *pc);
        case opcode::push_one_size:
            return op_push_size(context, data, max_uint8);
        case opcode::push_two_size:
            return op_push_size(context, data, max_uint16);
        case opcode::push_four_size:
            return op_push_size(context, data, max_uint32);
        case opcode::push_negative_1:
            return op_push_number(context, script_number::negative_1);
        case opcode::reserved_80:
            return op_reserved(code);
        case opcode::push_positive_1:
            return op_push_number(context, script_number::positive_1);
        case opcode::push_positive_2:
            return op_push_number(context, script_number::positive_2);
        case opcode::push_positive_3:
            return op_push_number(context, script_number::positive_3);
        case opcode::push_positive_4:
            return op_push_number(context, script_number::positive_4);
        case opcode::push_positive_5:
            return op_push_number(context, script_number::positive_5);
        case opcode::push_positive_6:
            return op_push_number(context, script_number::positive_6);
        case opcode::push_positive_7:
            return op_push_number(context, script_number::positive_7);
        case opcode::push_positive_8:
            return op_push_number(context, script_number::positive_8);
        case opcode::push_positive_9:
            return op_push_number(context, script_number::positive_9);
        case opcode::push_positive_10:
            return op_push_number(context, script_number::positive_10);
        case opcode::push_positive_11:
            return op_push_number(context, script_number::positive_11);
        case opcode::push_positive_12:
            return op_push_number(context, script_number::positive_12);
        case opcode::push_positive_13:
            return op_push_number(context, script_number::positive_13);
        case opcode::push_positive_14:
            return op_push_number(context, script_number::positive_14);
        case opcode::push_positive_15:
            return op_push_number(context, script_number::positive_15);
        case opcode::push_positive_16:
            return op_push_number(context, script_number::positive_16);
        case opcode::nop:
            return op_nop(code);
        case opcode::reserved_98:
            return op_reserved(code);
        case opcode::if_:
            return op_if(context);
        case opcode::notif:
            return op_notif(context);
        case opcode::disabled_verif:
            return op_disabled(code);
        case opcode::disabled_vernotif:
            return op_disabled(code);
        case opcode::else_:
            return op_else(context);
        case opcode::endif:
            return op_endif(context);
        case opcode::verify:
            return op_verify(context);
        case opcode::return_:
            return op_return(context);
        case opcode::toaltstack:
            return op_to_alt_stack(context);
        case opcode::fromaltstack:
            return op_from_alt_stack(context);
        case opcode::drop2:
            return op_drop2(context);
        case opcode::dup2:
            return op_dup2(context);
        case opcode::dup3:
            return op_dup3(context);
        case opcode::over2:
            return op_over2(context);
        case opcode::rot2:
            return op_rot2(context);
        case opcode::swap2:
            return op_swap2(context);
        case opcode::ifdup:
            return op_if_dup(context);
        case opcode::depth:
            return op_depth(context);
        case opcode::drop:
            return op_drop(context);
        case opcode::dup:
            return op_dup(context);
        case opcode::nip:
            return op_nip(context);
        case opcode::over:
            return op_over(context);
        case opcode::pick:
            return op_pick(context);
        case opcode::roll:
            return op_roll(context);
        case opcode::rot:
            return op_rot(context);
        case opcode::swap:
            return op_swap(context);
        case opcode::tuck:
            return op_tuck(context);
        case opcode::disabled_cat:
            return op_disabled(code);
        case opcode::disabled_substr:
            return op_disabled(code);
        case opcode::disabled_left:
            return op_disabled(code);
        case opcode::disabled_right:
            return op_disabled(code);
        case opcode::size:
            return op_size(context);
        case opcode::disabled_invert:
            return op_disabled(code);
        case opcode::disabled_and:
            return op_disabled(code);
        case opcode::disabled_or:
            return op_disabled(code);
        case opcode::disabled_xor:
            return op_disabled(code);
        case opcode::equal:
            return op_equal(context);
        case opcode::equalverify:
            return op_equal_verify(context);
        case opcode::reserved_137:
            return op_reserved(code);
        case opcode::reserved_138:
            return op_reserved(code);
        case opcode::add1:
            return op_add1(context);
        case opcode::sub1:
            return op_sub1(context);
        case opcode::disabled_mul2:
            return op_disabled(code);
        case opcode::disabled_div2:
            return op_disabled(code);
        case opcode::negate:
            return op_negate(context);
        case opcode::abs:
            return op_abs(context);
        case opcode::not_:
            return op_not(context);
        case opcode::nonzero:
            return op_nonzero(context);
        case opcode::add:
            return op_add(context);
        case opcode::sub:
            return op_sub(context);
        case opcode::disabled_mul:
            return op_disabled(code);
        case opcode::disabled_div:
            return op_disabled(code);
        case opcode::disabled_mod:
            return op_disabled(code);
        case opcode::disabled_lshift:
            return op_disabled(code);
        case opcode::disabled_rshift:
            return op_disabled(code);
        case opcode::booland:
            return op_bool_and(context);
        case opcode::boolor:
            return op_bool_or(context);
        case opcode::numequal:
            return op_num_equal(context);
        case opcode::numequalverify:
            return op_num_equal_verify(context);
        case opcode::numnotequal:
            return op_num_not_equal(context);
        case opcode::lessthan:
            return op_less_than(context);
        case opcode::greaterthan:
            return op_greater_than(context);
        case opcode::lessthanorequal:
            return op_less_than_or_equal(context);
        case opcode::greaterthanorequal:
            return op_greater_than_or_equal(context);
        case opcode::min:
            return op_min(context);
        case opcode::max:
            return op_max(context);
        case opcode::within:
            return op_within(context);
        case opcode::ripemd160:
            return op_ripemd160(context);
        case opcode::sha1:
            return op_sha1(context);
        case opcode::sha256:
            return op_sha256(context);
        case opcode::hash160:
            return op_hash160(context);
        case opcode::hash256:
            return op_hash256(context);
        case opcode::codeseparator:
            return op_code_seperator(context, pc);
        case opcode::checksig:
            return op_check_sig(context, script, tx, input_index);
        case opcode::checksigverify:
            return op_check_sig_verify(context, script, tx, input_index) ==
                signature_parse_result::valid;
        case opcode::checkmultisig:
            return op_check_multisig(context, script, tx, input_index);
        case opcode::checkmultisigverify:
            return op_check_multisig_verify(context, script, tx, input_index) ==
                signature_parse_result::valid;
        case opcode::nop1:
            return op_nop(code);
        case opcode::checklocktimeverify:
            return op_check_locktime_verify(context, script, tx, input_index);
        case opcode::nop3:
        case opcode::nop4:
        case opcode::nop5:
        case opcode::nop6:
        case opcode::nop7:
        case opcode::nop8:
        case opcode::nop9:
        case opcode::nop10:
            return op_nop(code);
        case opcode::reserved_186:
        case opcode::reserved_187:
        case opcode::reserved_188:
        case opcode::reserved_189:
        case opcode::reserved_190:
        case opcode::reserved_191:
        case opcode::reserved_192:
        case opcode::reserved_193:
        case opcode::reserved_194:
        case opcode::reserved_195:
        case opcode::reserved_196:
        case opcode::reserved_197:
        case opcode::reserved_198:
        case opcode::reserved_199:
        case opcode::reserved_200:
        case opcode::reserved_201:
        case opcode::reserved_202:
        case opcode::reserved_203:
        case opcode::reserved_204:
        case opcode::reserved_205:
        case opcode::reserved_206:
        case opcode::reserved_207:
        case opcode::reserved_208:
        case opcode::reserved_209:
        case opcode::reserved_210:
        case opcode::reserved_211:
        case opcode::reserved_212:
        case opcode::reserved_213:
        case opcode::reserved_214:
        case opcode::reserved_215:
        case opcode::reserved_216:
        case opcode::reserved_217:
        case opcode::reserved_218:
        case opcode::reserved_219:
        case opcode::reserved_220:
        case opcode::reserved_221:
        case opcode::reserved_222:
        case opcode::reserved_223:
        case opcode::reserved_224:
        case opcode::reserved_225:
        case opcode::reserved_226:
        case opcode::reserved_227:
        case opcode::reserved_228:
        case opcode::reserved_229:
        case opcode::reserved_230:
        case opcode::reserved_231:
        case opcode::reserved_232:
        case opcode::reserved_233:
        case opcode::reserved_234:
        case opcode::reserved_235:
        case opcode::reserved_236:
        case opcode::reserved_237:
        case opcode::reserved_238:
        case opcode::reserved_239:
        case opcode::reserved_240:
        case opcode::reserved_241:
        case opcode::reserved_242:
        case opcode::reserved_243:
        case opcode::reserved_244:
        case opcode::reserved_245:
        case opcode::reserved_246:
        case opcode::reserved_247:
        case opcode::reserved_248:
        case opcode::reserved_249:
        case opcode::reserved_250:
        case opcode::reserved_251:
        case opcode::reserved_252:
        case opcode::reserved_253:
        case opcode::reserved_254:
        case opcode::reserved_255:
        default:
            return op_reserved(code);
    }
}

} // namespace chain
} // namespace libbitcoin
