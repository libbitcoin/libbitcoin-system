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
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/evaluation_context.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/sighash_algorithm.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace chain {

enum class signature_parse_result
{
    valid,
    invalid,
    lax_encoding
};

inline void report(const char*)
{
    // Don't assert here since invalid script values are possible due to public
    // access to the script stack. Our test cases populate these values even in
    // the cases where the script parser may not.
    ////BITCOIN_ASSERT_MSG(false, message);
}

// Operation count.
//-----------------------------------------------------------------------------

inline bool overflow_op_count(size_t op_count)
{
    return op_count > op_counter_limit;
}

static bool update_op_counter(const operation& op, evaluation_context& context)
{
    if (opcode_is_operation(op.code()))
        ++context.operation_counter;

    return !overflow_op_count(context.operation_counter);
}

static bool update_op_counter(int32_t multisig_pubkeys,
    evaluation_context& context)
{
    // bit.ly/2d1bsdB
    if (multisig_pubkeys < 0 || multisig_pubkeys > max_script_public_key_count)
        return false;

    context.operation_counter += multisig_pubkeys;
    return !overflow_op_count(context.operation_counter);
}

// Stack condition.
//-----------------------------------------------------------------------------

// static
data_chunk bool_to_stack(bool value)
{
    return value ? data_chunk{ 1 } : data_chunk{};
}

// static
bool stack_to_bool(const data_chunk& values)
{
    if (values.empty())
        return false;

    const auto last_position = values.end() - 1;

    for (auto it = values.begin(); it != values.end(); ++it)
        if (*it != 0)
            return !(it == last_position && *it == script_number::negative_mask);

    return false;
}

// static
bool stack_result(const evaluation_context& context)
{
    return !context.stack.empty() && stack_to_bool(context.stack.back());
}

// Stack manipulation.
//-----------------------------------------------------------------------------

inline data_chunk& item(evaluation_context& context, size_t back_index)
{
    const auto index = context.stack.size() - back_index;
    return context.stack[index];
}

inline void swap_items(evaluation_context& context, size_t back_index_left,
    size_t back_index_right)
{
    std::swap(item(context, back_index_left), item(context, back_index_right));
}

inline void copy_item(evaluation_context& context, size_t back_index)
{
    context.stack.push_back(item(context, back_index));
}

inline bool pop_unary(evaluation_context& context, script_number& out_number,
    size_t maxiumum_size=max_number_size)
{
    return !context.stack.empty() &&
        out_number.set_data(context.pop_stack(), maxiumum_size);
}

inline bool pop_unary(evaluation_context& context, int32_t& out_value)
{
    script_number middle;
    if (!pop_unary(context, middle))
        return false;

    out_value = middle.int32();
    return true;
}

inline bool pop_binary(evaluation_context& context,
    script_number& left, script_number& right)
{
    // The right hand side number is at the top of the stack.
    return pop_unary(context, right) && pop_unary(context, left);
}

inline bool pop_ternary(evaluation_context& context,
    script_number& upper, script_number& lower, script_number& value)
{
    // The upper bound is at the top of the stack and the lower bound next.
    return pop_unary(context, upper) && pop_unary(context, lower) &&
        pop_unary(context, value);
}

// Determines if the value is a valid stack index and returns the index.
inline bool pop_stack_index(evaluation_context& context, size_t& out_index)
{
    int32_t reverse_index;
    if (!pop_unary(context, reverse_index))
        return false;

    // Ensure the index is within bounds.
    const auto size = context.stack.size();
    if (reverse_index < 0 || reverse_index >= size)
        return false;

    // Convert to a forward index.
    out_index = (size - 1) - static_cast<size_t>(reverse_index);
    return out_index < size;
}

static bool read_section(evaluation_context& context, data_stack& section,
    size_t count)
{
    if (context.stack.size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
        section.push_back(context.pop_stack());

    return true;
}

// Operations.
//-----------------------------------------------------------------------------

static bool op_negative_1(evaluation_context& context)
{
    context.stack.push_back({ script_number::negative_1 });
    return true;
}

static bool op_x(evaluation_context& context, opcode code)
{
    static const auto byte_code_0 = to_byte_code(opcode::op_1) - 1;
    const auto span = to_byte_code(code) - byte_code_0;
    context.stack.push_back(script_number(span).data());
    return true;
}

static bool op_if(evaluation_context& context)
{
    auto value = false;

    if (context.conditional.succeeded())
    {
        if (context.stack.empty())
            return false;

        value = stack_to_bool(context.pop_stack());
    }

    context.conditional.open(value);
    return true;
}

static bool op_notif(evaluation_context& context)
{
    // A bit hackish...
    // Open IF statement but then invert it to get NOTIF
    if (!op_if(context))
        return false;

    context.conditional.negate();
    return true;
}

static bool op_else(evaluation_context& context)
{
    if (context.conditional.closed())
        return false;

    context.conditional.negate();
    return true;
}

static bool op_endif(evaluation_context& context)
{
    if (context.conditional.closed())
        return false;

    context.conditional.close();
    return true;
}

static bool op_verify(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    if (!stack_to_bool(context.stack.back()))
        return false;

    context.pop_stack();
    return true;
}

static bool op_to_alt_stack(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    context.alternate.push_back(context.pop_stack());
    return true;
}

static bool op_from_alt_stack(evaluation_context& context)
{
    if (context.alternate.empty())
        return false;

    context.stack.push_back(context.alternate.back());
    context.alternate.pop_back();
    return true;
}

static bool op_2drop(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    context.stack.pop_back();
    context.stack.pop_back();
    return true;
}

static bool op_2dup(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    const auto dup_first = *(context.stack.end() - 2);
    const auto dup_second = *(context.stack.end() - 1);

    context.stack.emplace_back(std::move(dup_first));
    context.stack.emplace_back(std::move(dup_second));
    return true;
}

static bool op_3dup(evaluation_context& context)
{
    if (context.stack.size() < 3)
        return false;

    const auto dup_first = *(context.stack.end() - 3);
    const auto dup_second = *(context.stack.end() - 2);
    const auto dup_third = *(context.stack.end() - 1);

    context.stack.emplace_back(std::move(dup_first));
    context.stack.emplace_back(std::move(dup_second));
    context.stack.emplace_back(std::move(dup_third));
    return true;
}

static bool op_2over(evaluation_context& context)
{
    if (context.stack.size() < 4)
        return false;

    copy_item(context, 4);

    // Item -3 now becomes -4 because of last push
    copy_item(context, 4);
    return true;
}

static bool op_2rot(evaluation_context& context)
{
    if (context.stack.size() < 6)
        return false;

    const auto first_position = context.stack.end() - 6;
    const auto second_position = context.stack.end() - 5;
    const auto third_position = context.stack.end() - 4;

    const auto dup_first = *(first_position);
    const auto dup_second = *(second_position);

    context.stack.erase(first_position, third_position);
    context.stack.emplace_back(std::move(dup_first));
    context.stack.emplace_back(std::move(dup_second));
    return true;
}

static bool op_2swap(evaluation_context& context)
{
    if (context.stack.size() < 4)
        return false;

    // Before: x1 x2 x3 x4
    // After:  x3 x4 x1 x2
    swap_items(context, 4, 2);
    swap_items(context, 3, 1);
    return true;
}

static bool op_if_dup(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    if (stack_to_bool(context.stack.back()))
        context.stack.push_back(context.stack.back());

    return true;
}

static bool op_depth(evaluation_context& context)
{
    const auto size = context.stack.size();

    // Condition added by EKV on 2016.10.06.
    if (size > max_int64)
        return false;

    const script_number stack_size(size);
    context.stack.push_back(stack_size.data());
    return true;
}

static bool op_drop(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    context.stack.pop_back();
    return true;
}

static bool op_dup(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    context.stack.push_back(context.stack.back());
    return true;
}

static bool op_nip(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    context.stack.erase(context.stack.end() - 2);
    return true;
}

static bool op_over(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    copy_item(context, 2);
    return true;
}

static bool op_pick(evaluation_context& context)
{
    size_t index;
    if (!pop_stack_index(context, index))
        return false;

    context.stack.push_back(context.stack[index]);
    return true;
}

static bool op_roll(evaluation_context& context)
{
    size_t index;
    if (!pop_stack_index(context, index))
        return false;

    const auto it = context.stack.begin() + index;
    const auto dup = *it;
    context.stack.erase(it);
    context.stack.emplace_back(std::move(dup));
    return true;
}

static bool op_rot(evaluation_context& context)
{
    // Top 3 stack items are rotated to the left.
    // Before: x1 x2 x3
    // After:  x2 x3 x1
    if (context.stack.size() < 3)
        return false;

    swap_items(context, 3, 2);
    swap_items(context, 2, 1);
    return true;
}

static bool op_swap(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    swap_items(context, 2, 1);
    return true;
}

static bool op_tuck(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    context.stack.insert(context.stack.end() - 2, context.stack.back());
    return true;
}

static bool op_size(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto size = context.stack.back().size();

    // Condition added by EKV on 2016.09.06.
    if (size > max_int64)
        return false;

    const script_number top_item_size(size);
    context.stack.push_back(top_item_size.data());
    return true;
}

static bool op_equal(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    const auto value = context.pop_stack() == context.pop_stack();
    context.stack.push_back(bool_to_stack(value));
    return true;
}

static bool op_equal_verify(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    return context.pop_stack() == context.pop_stack();
}

static bool op_1add(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    number += 1;
    context.stack.push_back(number.data());
    return true;
}

static bool op_1sub(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    number -= 1;
    context.stack.push_back(number.data());
    return true;
}

static bool op_negate(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    number = -number;
    context.stack.push_back(number.data());
    return true;
}

static bool op_abs(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    if (number < 0)
        number = -number;

    context.stack.push_back(number.data());
    return true;
}

static bool op_not(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    context.stack.push_back(bool_to_stack(number == 0));
    return true;
}

static bool op_0not_equal(evaluation_context& context)
{
    script_number number;
    if (!pop_unary(context, number))
        return false;

    context.stack.push_back(bool_to_stack(number != 0));
    return true;
}

static bool op_add(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    const auto result = left + right;
    context.stack.push_back(result.data());
    return true;
}

static bool op_sub(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    const auto result = left - right;
    context.stack.push_back(result.data());
    return true;
}

static bool op_bool_and(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != 0 && right != 0));
    return true;
}

static bool op_bool_or(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != 0 || right != 0));
    return true;
}

static bool op_num_equal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left == right));
    return true;
}

static bool op_num_equal_verify(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    return left == right;
}

static bool op_num_not_equal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != right));
    return true;
}

static bool op_less_than(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left < right));
    return true;
}

static bool op_greater_than(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left > right));
    return true;
}

static bool op_less_than_or_equal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left <= right));
    return true;
}

static bool op_greater_than_or_equal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left >= right));
    return true;
}

static bool op_min(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    if (left < right)
        context.stack.push_back(left.data());
    else
        context.stack.push_back(right.data());

    return true;
}

static bool op_max(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    const auto greater = left > right ? left.data() : right.data();
    context.stack.emplace_back(std::move(greater));
    return true;
}

static bool op_within(evaluation_context& context)
{
    script_number upper, lower, value;
    if (!pop_ternary(context, upper, lower, value))
        return false;

    context.stack.push_back(bool_to_stack(lower <= value && value < upper));
    return true;
}

static bool op_ripemd160(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto hash = ripemd160_hash(context.pop_stack());
    context.stack.push_back(to_chunk(hash));
    return true;
}

static bool op_sha1(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto hash = sha1_hash(context.pop_stack());
    context.stack.push_back(to_chunk(hash));
    return true;
}

static bool op_sha256(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto hash = sha256_hash(context.pop_stack());
    context.stack.push_back(to_chunk(hash));
    return true;
}

static bool op_hash160(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto hash = bitcoin_short_hash(context.pop_stack());
    context.stack.push_back(to_chunk(hash));
    return true;
}

static bool op_hash256(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    const auto hash = bitcoin_hash(context.pop_stack());
    context.stack.push_back(to_chunk(hash));
    return true;
}

static signature_parse_result op_check_sig_verify(evaluation_context& context,
    const script& script, const transaction& tx, uint32_t input_index,
    bool strict)
{
    if (context.stack.size() < 2)
        return signature_parse_result::invalid;

    const auto pubkey = context.pop_stack();
    auto endorsement = context.pop_stack();

    if (endorsement.empty())
        return signature_parse_result::invalid;

    const auto sighash_type = endorsement.back();
    auto& distinguished = endorsement;
    distinguished.pop_back();
    ec_signature signature;

    if (strict && !parse_signature(signature, distinguished, true))
        return signature_parse_result::lax_encoding;

    chain::script script_code;

    for (auto it = context.code_begin; it != script.operations().end(); ++it)
        if (it->data() != endorsement && it->code() != opcode::codeseparator)
            script_code.operations().push_back(*it);

    if (!strict && !parse_signature(signature, distinguished, false))
        return signature_parse_result::invalid;

    return script::check_signature(signature, sighash_type, pubkey,
        script_code, tx, input_index) ? signature_parse_result::valid :
        signature_parse_result::invalid;
}

static bool op_check_sig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index, bool strict)
{
    switch (op_check_sig_verify(context, script, tx, input_index, strict))
    {
        case signature_parse_result::valid:
            context.stack.push_back(bool_to_stack(true));
            break;
        case signature_parse_result::invalid:
            context.stack.push_back(bool_to_stack(false));
            break;
        case signature_parse_result::lax_encoding:
            return false;
    }

    return true;
}

static signature_parse_result op_check_multisig_verify(
    evaluation_context& context, const script& script, const transaction& tx,
    uint32_t input_index, bool strict)
{
    int32_t pubkeys_count;
    if (!pop_unary(context, pubkeys_count))
        return signature_parse_result::invalid;

    if (!update_op_counter(pubkeys_count, context))
        return signature_parse_result::invalid;

    data_stack pubkeys;
    if (!read_section(context, pubkeys, pubkeys_count))
        return signature_parse_result::invalid;

    int32_t sigs_count;
    if (!pop_unary(context, sigs_count))
        return signature_parse_result::invalid;

    if (sigs_count < 0 || sigs_count > pubkeys_count)
        return signature_parse_result::invalid;

    data_stack endorsements;
    if (!read_section(context, endorsements, sigs_count))
        return signature_parse_result::invalid;

    // Due to a bug in bitcoind, we need to read an extra null value which we
    // discard later.
    if (context.stack.empty())
        return signature_parse_result::invalid;

    context.stack.pop_back();
    const auto is_endorsement = [&endorsements](const data_chunk& data)
    {
        return std::find(endorsements.begin(), endorsements.end(), data) !=
            endorsements.end();
    };

    chain::script script_code;

    for (auto it = context.code_begin; it != script.operations().end(); ++it)
        if (it->code() != opcode::codeseparator && !is_endorsement(it->data()))
            script_code.operations().push_back(*it);

    // The exact number of signatures are required and must be in order.
    // One key can validate more than one script. So we always advance 
    // until we exhaust either pubkeys (fail) or signatures (pass).
    auto pubkey_iterator = pubkeys.begin();

    for (const auto& endorsement: endorsements)
    {
        if (endorsement.empty())
            return signature_parse_result::invalid;

        const auto sighash_type = endorsement.back();
        auto distinguished = endorsement;
        distinguished.pop_back();

        ec_signature signature;

        if (!parse_signature(signature, distinguished, strict))
            return strict ?
                signature_parse_result::lax_encoding :
                signature_parse_result::invalid;

        while (true)
        {
            const auto& point = *pubkey_iterator;

            if (script::check_signature(signature, sighash_type, point,
                script_code, tx, input_index))
                break;

            ++pubkey_iterator;

            if (pubkey_iterator == pubkeys.end())
                return signature_parse_result::invalid;
        }
    }

    return signature_parse_result::valid;
}

static bool op_check_multisig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index, bool strict)
{
    switch (op_check_multisig_verify(context, script, tx, input_index, strict))
    {
        case signature_parse_result::valid:
            context.stack.push_back(bool_to_stack(true));
            break;
        case signature_parse_result::invalid:
            context.stack.push_back(bool_to_stack(false));
            break;
        case signature_parse_result::lax_encoding:
            return false;
    }

    return true;
}

static bool op_check_locktime_verify(evaluation_context& context,
    const script& script, const transaction& tx, uint32_t input_index)
{
    if (input_index >= tx.inputs().size())
        return false;

    // BIP65: the nSequence field of the txin is 0xffffffff.
    if (tx.inputs()[input_index].is_final())
        return false;

    // BIP65: the stack is empty.
    // BIP65: We extend the (signed) CLTV script number range to 5 bytes in
    // order to reach the domain of the (unsigned) tx.locktime field.
    script_number number;
    if (!pop_unary(context, number, max_cltv_number_size))
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

// The script paramter is NOT always tx.indexes[input_index].script.
bool interpreter::run(const transaction& tx, uint32_t input_index,
    const script& script, evaluation_context& context, uint32_t flags)
{
    // bit.ly/2c9HzmN
    if (script.satoshi_content_size() > max_script_size)
        return false;

    auto& ops = script.operations();
    context.operation_counter = 0;
    context.code_begin = ops.begin();

    // If any op returns false the execution terminates and is false.
    for (auto it = ops.begin(); it != ops.end(); ++it)
        if (!next_operation(tx, input_index, it, script, context, flags))
            return false;

    return context.conditional.closed();
}

bool interpreter::next_operation(const transaction& tx, uint32_t input_index,
    operation::stack::const_iterator it, const script& script,
    evaluation_context& context, uint32_t flags)
{
    const auto& op = *it;

    // See BIP16
    if (op.data().size() > max_data_script_size)
        return false;

    if (!update_op_counter(op, context))
        return false;

    if (opcode_is_disabled(op.code()))
        return false;

    if (!context.conditional.succeeded() && !opcode_is_condition(op.code()))
        return true;

    // push data to the stack
    if (op.code() == opcode::zero)
    {
        context.stack.push_back(data_chunk{});
    }
    else if (op.code() == opcode::codeseparator)
    {
        context.code_begin = it;
    }
    else if (opcode_is_empty_pusher(op.code()))
    {
        context.stack.push_back(op.data());
    }
    else if (!run_operation(op, tx, input_index, script, context, flags))
    {
        // opcodes above should report inside of run_operation.
        return false;
    }

    // bit.ly/2cowHlP
    return context.stack.size() + context.alternate.size() <= max_stack_size;
}

bool interpreter::run_operation(const operation& op, const transaction& tx,
    uint32_t input_index, const script& script, evaluation_context& context,
    uint32_t flags)
{
    switch (op.code())
    {
        case opcode::zero:
        case opcode::special:
        case opcode::pushdata1:
        case opcode::pushdata2:
        case opcode::pushdata4:
            report("Invalid push operation in script.");
            return true;

        case opcode::negative_1:
            return op_negative_1(context);

        case opcode::reserved:
            return false;

        case opcode::op_1:
        case opcode::op_2:
        case opcode::op_3:
        case opcode::op_4:
        case opcode::op_5:
        case opcode::op_6:
        case opcode::op_7:
        case opcode::op_8:
        case opcode::op_9:
        case opcode::op_10:
        case opcode::op_11:
        case opcode::op_12:
        case opcode::op_13:
        case opcode::op_14:
        case opcode::op_15:
        case opcode::op_16:
            return op_x(context, op.code());

        case opcode::nop:
            return true;

        case opcode::ver:
            return false;

        case opcode::if_:
            return op_if(context);

        case opcode::notif:
            return op_notif(context);

        case opcode::verif:
        case opcode::vernotif:
            report("Disabled opcodes (verif/vernotif) in script.");
            return false;

        case opcode::else_:
            return op_else(context);

        case opcode::endif:
            return op_endif(context);

        case opcode::verify:
            return op_verify(context);

        case opcode::return_:
            return false;

        case opcode::toaltstack:
            return op_to_alt_stack(context);

        case opcode::fromaltstack:
            return op_from_alt_stack(context);

        case opcode::op_2drop:
            return op_2drop(context);

        case opcode::op_2dup:
            return op_2dup(context);

        case opcode::op_3dup:
            return op_3dup(context);

        case opcode::op_2over:
            return op_2over(context);

        case opcode::op_2rot:
            return op_2rot(context);

        case opcode::op_2swap:
            return op_2swap(context);

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

        case opcode::cat:
        case opcode::substr:
        case opcode::left:
        case opcode::right:
            report("Disabled splice operations in script.");
            return false;

        case opcode::tuck:
            return op_tuck(context);

        case opcode::size:
            return op_size(context);

        case opcode::invert:
        case opcode::and_:
        case opcode::or_:
        case opcode::xor_:
            report("Disabled bit logic operations in script.");
            return false;

        case opcode::equal:
            return op_equal(context);

        case opcode::equalverify:
            return op_equal_verify(context);

        case opcode::reserved1:
        case opcode::reserved2:
            return false;

        case opcode::op_1add:
            return op_1add(context);

        case opcode::op_1sub:
            return op_1sub(context);

        case opcode::op_2mul:
        case opcode::op_2div:
            report("Disabled opcodes (2mul/2div) in script.");
            return false;

        case opcode::negate:
            return op_negate(context);

        case opcode::abs:
            return op_abs(context);

        case opcode::not_:
            return op_not(context);

        case opcode::op_0notequal:
            return op_0not_equal(context);

        case opcode::add:
            return op_add(context);

        case opcode::sub:
            return op_sub(context);

        case opcode::mul:
        case opcode::div:
        case opcode::mod:
        case opcode::lshift:
        case opcode::rshift:
            report("Disabled numeric operations in script.");
            return false;

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
            report("Invalid operation (codeseparator) in script.");
            return true;

        case opcode::checksig:
            return op_check_sig(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule));

        case opcode::checksigverify:
            return op_check_sig_verify(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule)) ==
                    signature_parse_result::valid;

        case opcode::checkmultisig:
            return op_check_multisig(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule));

        case opcode::checkmultisigverify:
            return op_check_multisig_verify(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule)) ==
                    signature_parse_result::valid;

        case opcode::checklocktimeverify:
            return script::is_enabled(context.flags, rule_fork::bip65_rule) ?
                op_check_locktime_verify(context, script, tx,
                    input_index) : true;

        case opcode::op_nop1:

        // op_nop2 has been consumed by checklocktimeverify
        ////case opcode::op_nop2:

        case opcode::op_nop3:
        case opcode::op_nop4:
        case opcode::op_nop5:
        case opcode::op_nop6:
        case opcode::op_nop7:
        case opcode::op_nop8:
        case opcode::op_nop9:
        case opcode::op_nop10:
            return true;

        case opcode::bad_operation:
            // Our test cases pass bad_operation into scripts, for example:
            // [if bad_operation else op_1 endif]
            report("Invalid operation (bad_operation sentinel) in script.");
            return false;

        case opcode::raw_data:
            // Our test cases pass raw_data into scripts, for example:
            // [if raw_data else op_1 endif]
            report("Invalid operation (raw_data sentinel) in script.");
            return false;

        default:
            // Our test cases pass these values into scripts, for example:
            // [if 188 else op_1 endif]
            report("Invalid operation (unnamed) in script.");
            return false;
    }

    return false;
}

} // namespace chain
} // namespace libbitcoin
