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
#include <bitcoin/bitcoin/chain/script/script.hpp>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/variable_uint_size.hpp>
#include "conditional_stack.hpp"
#include "evaluation_context.hpp"

namespace libbitcoin {
namespace chain {

static constexpr size_t op_counter_limit = 201;
static constexpr size_t max_number_size = 4;
static constexpr size_t max_cltv_number_size = 5;
static constexpr size_t max_stack_size = 1000;
static constexpr size_t max_script_size = 10000;
static constexpr size_t max_data_script_size = 520;
static constexpr size_t max_script_public_key_count = 20;
static constexpr size_t multisig_default_signature_ops = 20;
static constexpr uint8_t negative_mask = 0x80;
static constexpr uint8_t negative_1 = 0x81;

// bit.ly/2cPazSa
static const hash_digest one_hash
{
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

enum class signature_parse_result
{
    valid,
    invalid,
    lax_encoding
};

script script::factory_from_data(const data_chunk& data, bool length_prefix,
    parse_mode mode)
{
    script instance;
    instance.from_data(data, length_prefix, mode);
    return instance;
}

script script::factory_from_data(std::istream& stream, bool length_prefix,
    parse_mode mode)
{
    script instance;
    instance.from_data(stream, length_prefix, mode);
    return instance;
}

script script::factory_from_data(reader& source, bool length_prefix,
    parse_mode mode)
{
    script instance;
    instance.from_data(source, length_prefix, mode);
    return instance;
}

script::script()
  : operations_(), is_raw_(false)
{
}

script::script(const operation::stack& operations)
  : operations_(operations), is_raw_(false)
{
}

script::script(operation::stack&& operations)
  : operations_(std::move(operations)), is_raw_(false)
{
}

script::script(const script& other)
  : operations_(other.operations_), is_raw_(other.is_raw_)
{
}

script::script(script&& other)
  : operations_(std::move(other.operations_)),
    is_raw_(other.is_raw_)
{
}

// TODO: cache.
script_pattern script::pattern() const
{
    if (operation::is_null_data_pattern(operations_))
        return script_pattern::null_data;

    if (operation::is_pay_multisig_pattern(operations_))
        return script_pattern::pay_multisig;

    if (operation::is_pay_public_key_pattern(operations_))
        return script_pattern::pay_public_key;

    if (operation::is_pay_key_hash_pattern(operations_))
        return script_pattern::pay_key_hash;

    if (operation::is_pay_script_hash_pattern(operations_))
        return script_pattern::pay_script_hash;

    if (operation::is_sign_multisig_pattern(operations_))
        return script_pattern::sign_multisig;

    if (operation::is_sign_public_key_pattern(operations_))
        return script_pattern::sign_public_key;

    if (operation::is_sign_key_hash_pattern(operations_))
        return script_pattern::sign_key_hash;

    if (operation::is_sign_script_hash_pattern(operations_))
        return script_pattern::sign_script_hash;

    return script_pattern::non_standard;
}

bool script::is_raw_data() const
{
    return (operations_.size() == 1) && is_raw_;
}

// BUGBUG: An empty script is valid.
bool script::is_valid() const
{
    return !operations_.empty();
}

void script::reset()
{
    operations_.clear();
    operations_.shrink_to_fit();
    is_raw_ = false;
}

bool script::from_data(const data_chunk& data, bool length_prefix,
    parse_mode mode)
{
    auto result = true;

    if (length_prefix)
    {
        data_source istream(data);
        result = from_data(istream, true, mode);
    }
    else
    {
        reset();
        result = deserialize(data, mode);

        if (!result)
            reset();
    }

    return result;
}

bool script::from_data(std::istream& stream, bool length_prefix,
    parse_mode mode)
{
    istream_reader source(stream);
    return from_data(source, length_prefix, mode);
}

bool script::from_data(reader& source, bool length_prefix, parse_mode mode)
{
    reset();

    const auto bytes = length_prefix ?
        source.read_bytes(source.read_size_little_endian()) :
        source.read_bytes();

    if (source && !deserialize(bytes, mode))
        source.invalidate();

    if (!source)
        reset();

    return source;
}

data_chunk script::to_data(bool prefix) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream, prefix);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(prefix));
    return data;
}

void script::to_data(std::ostream& stream, bool prefix) const
{
    ostream_writer sink(stream);
    to_data(sink, prefix);
}

void script::to_data(writer& sink, bool prefix) const
{
    if (prefix)
        sink.write_variable_little_endian(satoshi_content_size());

    if (is_raw_data())
    {
        sink.write_bytes(operations_[0].data());
        return;
    }

    for (const auto& op: operations_)
        op.to_data(sink);
}

// TODO: cache.
uint64_t script::satoshi_content_size() const
{
    if (is_raw_data())
        return operations_[0].data().size();

    const auto value = [](uint64_t total, const operation& op)
    {
        return safe_add(total, op.serialized_size());
    };

    return std::accumulate(operations_.begin(), operations_.end(), uint64_t{0},
        value);
}

uint64_t script::serialized_size(bool prefix) const
{
    auto size = satoshi_content_size();

    if (prefix)
        size += variable_uint_size(size);

    return size;
}

bool script::from_string(const std::string& human_readable)
{
    // clear current contents
    operations_.clear();
    const auto tokens = split(human_readable);
    auto clear = false;

    for (auto token = tokens.begin(); token != tokens.end(); ++token)
    {
        opcode code;
        data_chunk data;

        if (*token == "[")
        {
            data_chunk raw_data;
            if (!decode_base16(raw_data, *++token))
            {
                clear = true;
                break;
            }

            if (raw_data.empty() || *++token != "]")
            {
                clear = true;
                break;
            }

            code = data_to_opcode(raw_data);
            data = raw_data;
        }
        else
        {
            code = string_to_opcode(*token);
        }

        if (code == opcode::bad_operation)
        {
            clear = true;
            break;
        }

        operations_.push_back({ code, data });
    }

    // empty invalid/failed parse content
    if (clear)
        operations_.clear();

    return !clear;
}

std::string script::to_string(uint32_t flags) const
{
    std::ostringstream text;

    for (auto it = operations_.begin(); it != operations_.end(); ++it)
    {
        if (it != operations_.begin())
            text << " ";

        text << it->to_string(flags);
    }

    return text.str();
}

// See BIP16.
// TODO: distinct cache property for serialized_script total.
size_t script::sigops(bool serialized_script) const
{
    size_t total = 0;
    opcode last_opcode = opcode::bad_operation;

    for (const auto& op: operations_)
    {
        if (op.code() == opcode::checksig ||
            op.code() == opcode::checksigverify)
        {
            total++;
        }
        else if (
            op.code() == opcode::checkmultisig ||
            op.code() == opcode::checkmultisigverify)
        {
            total += serialized_script && within_op_n(last_opcode) ?
                decode_op_n(last_opcode) : multisig_default_signature_ops;
        }

        last_opcode = op.code();
    }

    return total;
}

// See BIP16.
// TODO: cache (default to max_size_t sentinel).
size_t script::pay_script_hash_sigops(const script& prevout) const
{
    // The prevout script is not p2sh, so no signature increment.
    if (prevout.pattern() != script_pattern::pay_script_hash)
        return 0;

    // Conditions added by EKV on 2016.09.15 for safety and BIP16 consistency.
    // Only push data operations allowed in script, so no signature increment.
    if (operations_.empty() || !operation::is_push_only(operations_))
        return 0;

    script eval;

    // We can be strict here and treat failure as zero signatures (data).
    if (!eval.from_data(operations_.back().data(), false, parse_mode::strict))
        return 0;

    // Count the sigops in the serialized script using BIP16 rules.
    return eval.sigops(true);
}

bool script::deserialize(const data_chunk& raw_script, parse_mode mode)
{
    if (mode != parse_mode::raw_data && parse(raw_script))
        return true;

    if (mode != parse_mode::strict && emplace(raw_script))
        return true;

    return false;
}

bool script::emplace(const data_chunk& raw_script)
{
    is_raw_ = true;
    operations_.clear();

    // The opcode is ignored thanks to is_raw_ in the script.
    operations_.emplace_back(opcode::raw_data, to_chunk(raw_script));
    return true;
}

bool script::parse(const data_chunk& raw_script)
{
    data_source istream(raw_script);
    istream_reader source(istream);

    while (!source.is_exhausted())
    {
        operations_.emplace_back();

        if (!operations_.back().from_data(source))
            return false;
    }

    return true;
}

inline signature_hash_algorithm to_sighash_enum(uint8_t sighash_type)
{
    return static_cast<signature_hash_algorithm>(
        sighash_type & signature_hash_algorithm::mask);
}

inline uint8_t is_sighash_enum(uint8_t sighash_type,
    signature_hash_algorithm value)
{
    return to_sighash_enum(sighash_type) == value;
}

inline bool is_sighash_flag(uint8_t sighash_type,
    signature_hash_algorithm value)
{
    return (sighash_type & value) != 0;
}

static transaction sign_none(const transaction& tx, uint32_t input_index,
    const script& script_code, bool anyone)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    ins.reserve(anyone ? 1 : inputs.size());

    BITCOIN_ASSERT(input_index < inputs.size());
    const auto& self = inputs[input_index];

    if (anyone)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    }
    else
    {
        // Erase all input scripts and sequences.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{}, 0);

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ins[input_index].set_sequence(self.sequence());
        ////ins[input_index].set_script(self.previous_output());
    }

    // Move new inputs to new transaction and drop outputs.
    return transaction(tx.version(), tx.locktime(), std::move(ins),
        output::list{});
}

static transaction sign_single(const transaction& tx, uint32_t input_index,
    const script& script_code, bool anyone)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    ins.reserve(anyone ? 1 : inputs.size());

    BITCOIN_ASSERT(input_index < inputs.size());
    const auto& self = inputs[input_index];

    if (anyone)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    }
    else
    {
        // Erase all input scripts and sequences.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{}, 0);

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ins[input_index].set_sequence(self.sequence());
        ////ins[input_index].set_script(self.previous_output());
    }

    // Trim and clear outputs except that of specified input index.
    const auto& outputs = tx.outputs();
    output::list outs(input_index + 1);

    BITCOIN_ASSERT(input_index < outputs.size());
    outs.back() = outputs[input_index];

    // Move new inputs and new outputs to new transaction.
    return transaction(tx.version(), tx.locktime(), std::move(ins),
        std::move(outs));
}

static transaction sign_all(const transaction& tx, uint32_t input_index,
    const script& script_code, bool anyone)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    ins.reserve(anyone ? 1 : inputs.size());

    BITCOIN_ASSERT(input_index < inputs.size());
    const auto& self = inputs[input_index];

    if (anyone)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    }
    else
    {
        // Erase all input scripts.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{},
                input.sequence());

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ////ins[input_index].set_sequence(self.sequence());
        ////ins[input_index].set_script(self.previous_output());
    }

    // Move new inputs and copy outputs to new transaction.
    transaction out(tx.version(), tx.locktime(), input::list{}, tx.outputs());
    out.set_inputs(std::move(ins));
    return out;
}

// FindAndDelete(OP_CODESEPARATOR) done in op_checksigverify(...)
hash_digest script::generate_signature_hash(const transaction& tx,
    uint32_t input_index, const script& script_code, uint8_t sighash_type)
{
    const auto sighash_all = signature_hash_algorithm::all;
    const auto sighash_none = signature_hash_algorithm::none;
    const auto sighash_single = signature_hash_algorithm::single;
    const auto anyone_flag = signature_hash_algorithm::anyone_can_pay;

    const auto single = is_sighash_enum(sighash_type, sighash_single);
    const auto anyone = is_sighash_flag(sighash_type, anyone_flag);

    // These are verified here and therefore only asserted in the helpers.
    if (input_index >= tx.inputs().size() || 
        (input_index >= tx.outputs().size() && single))
    {
        // This is a bitcoind behavior we necessarily perpetuate.
        return one_hash;
    }

    switch (to_sighash_enum(sighash_type))
    {
        case sighash_none:
            return sign_none(tx, input_index, script_code, anyone)
                .hash(sighash_type);

        case sighash_single:
            return sign_single(tx, input_index, script_code, anyone)
                .hash(sighash_type);

        default:
        case sighash_all:
            return sign_all(tx, input_index, script_code, anyone)
                .hash(sighash_type);
    }
}

inline void stack_swap(evaluation_context& context, size_t left, size_t right)
{
    std::swap(*(context.stack.end() - left), *(context.stack.end() - right));
}

inline bool pop_unary(evaluation_context& context, script_number& out_number,
    size_t maxiumum_size=max_number_size)
{
    return !context.stack.empty() &&
        out_number.set_data(context.pop_stack(), maxiumum_size);
}

static bool pop_unary(evaluation_context& context, int32_t& out_value)
{
    script_number middle;
    if (!pop_unary(context, middle))
        return false;

    out_value = middle.int32();
    return true;
}

static bool pop_binary(evaluation_context& context,
    script_number& left, script_number& right)
{
    // The right hand side number is at the top of the stack.
    return pop_unary(context, right) && pop_unary(context, left);
}

static bool pop_ternary(evaluation_context& context,
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

inline void copy_item_over_stack(evaluation_context& context, size_t index)
{
    const auto dup = *(context.stack.end() - index);
    context.stack.emplace_back(std::move(dup));
}

inline data_chunk bool_to_stack(bool value)
{
    return value ? data_chunk{1} : data_chunk{};
}

inline bool stack_to_bool(const data_chunk& values)
{
    if (values.empty())
        return false;

    const auto last_position = values.end() - 1;
    for (auto it = values.begin(); it != values.end(); ++it)
        if (*it != 0)
            return !(it == last_position && *it == negative_mask);

    return false;
}

inline bool is_condition_opcode(opcode code)
{
    return code == opcode::if_
        || code == opcode::notif
        || code == opcode::else_
        || code == opcode::endif;
}

inline uint8_t to_byte_code(opcode code)
{
    return static_cast<uint8_t>(code);
}

inline bool greater_op_16(opcode code)
{
    return to_byte_code(code) > to_byte_code(opcode::op_16);
}

inline bool op_negative_1(evaluation_context& context)
{
    context.stack.push_back({ negative_1 });
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

static bool op_toaltstack(evaluation_context& context)
{
    if (context.stack.empty())
        return false;

    context.alternate.push_back(context.pop_stack());
    return true;
}

static bool op_fromaltstack(evaluation_context& context)
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

    copy_item_over_stack(context, 4);

    // Item -3 now becomes -4 because of last push
    copy_item_over_stack(context, 4);
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
    stack_swap(context, 4, 2);
    stack_swap(context, 3, 1);
    return true;
}

static bool op_ifdup(evaluation_context& context)
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

    copy_item_over_stack(context, 2);
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

    stack_swap(context, 3, 2);
    stack_swap(context, 2, 1);
    return true;
}

static bool op_swap(evaluation_context& context)
{
    if (context.stack.size() < 2)
        return false;

    stack_swap(context, 2, 1);
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

static bool op_equalverify(evaluation_context& context)
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

static bool op_0notequal(evaluation_context& context)
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

static bool op_booland(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != 0 && right != 0));
    return true;
}

static bool op_boolor(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != 0 || right != 0));
    return true;
}

static bool op_numequal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left == right));
    return true;
}

static bool op_numequalverify(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    return left == right;
}

static bool op_numnotequal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left != right));
    return true;
}

static bool op_lessthan(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left < right));
    return true;
}

static bool op_greaterthan(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left > right));
    return true;
}

static bool op_lessthanorequal(evaluation_context& context)
{
    script_number left, right;
    if (!pop_binary(context, left, right))
        return false;

    context.stack.push_back(bool_to_stack(left <= right));
    return true;
}

static bool op_greaterthanorequal(evaluation_context& context)
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

bool script::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& prevout_script, const transaction& tx, uint32_t input_index,
    uint8_t sighash_type)
{
    // This always produces a valid signature hash.
    const auto sighash = script::generate_signature_hash(tx, input_index,
        prevout_script, sighash_type);

    // Create the EC signature and encode as DER.
    ec_signature signature;
    if (!sign(signature, secret, sighash) || !encode_signature(out, signature))
        return false;

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(sighash_type);
    return true;
}

bool script::check_signature(const ec_signature& signature,
    uint8_t sighash_type, const data_chunk& public_key,
    const script& script_code, const transaction& tx, uint32_t input_index)
{
    if (public_key.empty())
        return false;

    // This always produces a valid signature hash.
    const auto sighash = script::generate_signature_hash(tx, input_index,
        script_code, sighash_type);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

static signature_parse_result op_checksigverify(evaluation_context& context,
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

static bool op_checksig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index, bool strict)
{
    switch (op_checksigverify(context, script, tx, input_index, strict))
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

static bool read_section(evaluation_context& context, data_stack& section,
    size_t count)
{
    if (context.stack.size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
        section.push_back(context.pop_stack());

    return true;
}

static signature_parse_result op_checkmultisigverify(
    evaluation_context& context, const script& script, const transaction& tx,
    uint32_t input_index, bool strict)
{
    int32_t pubkeys_count;
    if (!pop_unary(context, pubkeys_count))
        return signature_parse_result::invalid;

    // bit.ly/2d1bsdB
    if (pubkeys_count < 0 || pubkeys_count > max_script_public_key_count)
        return signature_parse_result::invalid;

    context.operation_counter += pubkeys_count;

    if (context.operation_counter > op_counter_limit)
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

static bool op_checkmultisig(evaluation_context& context, const script& script,
    const transaction& tx, uint32_t input_index, bool strict)
{
    switch (op_checkmultisigverify(context, script, tx, input_index, strict))
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

static bool is_locktime_type_match(uint64_t left, uint64_t right)
{
    return (left < locktime_threshold) == (right < locktime_threshold);
}

static bool op_checklocktimeverify(evaluation_context& context,
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
    if (!is_locktime_type_match(stack, tx.locktime()))
        return false;

    // BIP65: the top stack item is greater than the tx's nLockTime.
    return stack <= tx.locktime();
}

// Test rule_fork flag for a given context.
bool script::is_enabled(uint32_t flags, rule_fork flag)
{
    return (flag & flags) != 0;
}

inline void report(const char*)
{
    // Don't assert here since invalid script values are possible due to public
    // access to the script stack. Our test cases populate these values even in
    // the cases where the script parser may not.
    ////BITCOIN_ASSERT_MSG(false, message);
}

static bool run_operation(const operation& op, const transaction& tx,
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
            return op_toaltstack(context);

        case opcode::fromaltstack:
            return op_fromaltstack(context);

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
            return op_ifdup(context);

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
            return op_equalverify(context);

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
            return op_0notequal(context);

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
            return op_booland(context);

        case opcode::boolor:
            return op_boolor(context);

        case opcode::numequal:
            return op_numequal(context);

        case opcode::numequalverify:
            return op_numequalverify(context);

        case opcode::numnotequal:
            return op_numnotequal(context);

        case opcode::lessthan:
            return op_lessthan(context);

        case opcode::greaterthan:
            return op_greaterthan(context);

        case opcode::lessthanorequal:
            return op_lessthanorequal(context);

        case opcode::greaterthanorequal:
            return op_greaterthanorequal(context);

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
            return op_checksig(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule));

        case opcode::checksigverify:
            return op_checksigverify(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule)) ==
                    signature_parse_result::valid;

        case opcode::checkmultisig:
            return op_checkmultisig(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule));

        case opcode::checkmultisigverify:
            return op_checkmultisigverify(context, script, tx, input_index,
                script::is_enabled(flags, rule_fork::bip66_rule)) ==
                    signature_parse_result::valid;

        case opcode::checklocktimeverify:
            return script::is_enabled(context.flags, rule_fork::bip65_rule) ?
                op_checklocktimeverify(context, script, tx,
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

static bool increment_op_counter(opcode code, evaluation_context& context)
{
    if (greater_op_16(code))
        ++context.operation_counter;

    return context.operation_counter <= op_counter_limit;
}

static bool opcode_is_disabled(opcode code)
{
    switch (code)
    {
        case opcode::cat:
        case opcode::substr:
        case opcode::left:
        case opcode::right:
        case opcode::invert:
        case opcode::and_:
        case opcode::or_:
        case opcode::xor_:
        case opcode::op_2mul:
        case opcode::op_2div:
        case opcode::mul:
        case opcode::div:
        case opcode::mod:
        case opcode::lshift:
        case opcode::rshift:
            return true;

        // These opcodes aren't in the main Satoshi EvalScript
        // switch-case so the script loop always fails regardless of
        // whether these are executed or not.
        case opcode::verif:
        case opcode::vernotif:
            return true;

        default:
            return false;
    }
}

static bool opcode_is_empty_pusher(opcode code)
{
    switch (code)
    {
        // These operations may push empty data (opcode zero).
        case opcode::special:
        case opcode::pushdata1:
        case opcode::pushdata2:
        case opcode::pushdata4:
            return true;

        default:
            return false;
    }
}

static bool next_step(const transaction& tx, uint32_t input_index,
    operation::stack::const_iterator it, const script& script,
    evaluation_context& context, uint32_t flags)
{
    const auto& op = *it;

    // See BIP16
    if (op.data().size() > max_data_script_size)
        return false;

    if (!increment_op_counter(op.code(), context))
        return false;

    if (opcode_is_disabled(op.code()))
        return false;

    if (!context.conditional.succeeded() && !is_condition_opcode(op.code()))
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
        // opcodes above should assert inside run_operation
        return false;
    }

    // bit.ly/2cowHlP
    return context.stack.size() + context.alternate.size() <= max_stack_size;
}

// The script paramter is NOT always tx.indexes[input_index].script.
static bool evaluate(const transaction& tx, uint32_t input_index,
    const script& script, evaluation_context& context, uint32_t flags)
{
    // bit.ly/2c9HzmN
    if (script.satoshi_content_size() > max_script_size)
        return false;

    auto& ops = script.operations();
    context.operation_counter = 0;
    context.code_begin = ops.begin();

    // If any op returns false the script terminates and is false.
    for (auto it = ops.begin(); it != ops.end(); ++it)
        if (!next_step(tx, input_index, it, script, context, flags))
            return false;

    return context.conditional.closed();
}

// TODO: return detailed result code indicating failure condition.
code script::verify(const transaction& tx, uint32_t input_index,
    uint32_t flags)
{
    if (input_index >= tx.inputs().size())
        return error::operation_failed;

    // Obtain the previous output script from the cached previous output.
    auto& prevout = tx.inputs()[input_index].previous_output().validation;
    return verify(tx, input_index, prevout.cache.script(), flags);
}

inline bool stack_result(const evaluation_context& context)
{
    return !context.stack.empty() && stack_to_bool(context.stack.back());
}

// TODO: return detailed result code indicating failure condition.
code script::verify(const transaction& tx, uint32_t input_index,
    const script& prevout_script, uint32_t flags)
{
    if (input_index >= tx.inputs().size())
        return error::operation_failed;

    const auto& input_script = tx.inputs()[input_index].script();
    evaluation_context in_context(flags);

    // Evaluate the input script.
    if (!evaluate(tx, input_index, input_script, in_context, flags))
        return error::validate_inputs_failed;

    evaluation_context out_context(flags, in_context.stack);

    // Evaluate the output script.
    if (!evaluate(tx, input_index, prevout_script, out_context, flags))
        return error::validate_inputs_failed;

    // Return if stack is false.
    if (!stack_result(out_context))
        return error::validate_inputs_failed;

    // BIP16: Additional validation for pay-to-script-hash transactions.
    if (is_enabled(flags, rule_fork::bip16_rule) &&
        (prevout_script.pattern() == script_pattern::pay_script_hash))
    {
        // Only push data operations allowed in script.
        if (!operation::is_push_only(input_script.operations()))
            return error::validate_inputs_failed;

        // Use the last stack item as the serialized script.
        script eval;

        // in_context.stack cannot be empty here because out_context is true.
        // Always process a serialized script as fallback since it can be data.
        if (!eval.from_data(in_context.stack.back(), false, 
            parse_mode::raw_data_fallback))
            return error::validate_inputs_failed;

        // Pop last item and use popped stack for eval script.
        in_context.stack.pop_back();
        evaluation_context eval_context(flags, in_context.stack);

        // Evaluate the eval (serialized) script.
        if (!evaluate(tx, input_index, eval, eval_context, flags))
            return error::validate_inputs_failed;

        // Return the stack state.
        if (!stack_result(eval_context))
            return error::validate_inputs_failed;
    }

    return error::success;
}

operation::stack& script::operations()
{
    return operations_;
}

const operation::stack& script::operations() const
{
    return operations_;
}

void script::set_operations(const operation::stack& value)
{
    operations_ = value;
}

void script::set_operations(operation::stack&& value)
{
    operations_ = std::move(value);
}

script& script::operator=(script&& other)
{
    operations_ = std::move(other.operations_);
    is_raw_ = other.is_raw_;
    return *this;
}

script& script::operator=(const script& other)
{
    operations_ = other.operations_;
    is_raw_ = other.is_raw_;
    return *this;
}

bool script::operator==(const script& other) const
{
    if (operations_.size() != other.operations_.size())
        return false;

    for (size_t op = 0; op < operations_.size(); ++op)
        if (operations_[op] != other.operations_[op])
            return false;

    return true;
}

bool script::operator!=(const script& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
