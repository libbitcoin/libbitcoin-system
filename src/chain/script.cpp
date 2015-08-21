/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/script.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/variable_uint_size.hpp>
#include "../conditional_stack.hpp"
#include "../evaluation_context.hpp"

namespace libbitcoin {
namespace chain {

// False is an empty stack.
static const data_chunk stack_false_value;
static const data_chunk stack_true_value{1};

constexpr uint64_t op_counter_limit = 201;

script script::factory_from_data(const data_chunk& data, bool with_length_prefix,
    script::parse_mode mode)
{
    script instance;
    instance.from_data(data, with_length_prefix, mode);
    return instance;
}

script script::factory_from_data(std::istream& stream, bool with_length_prefix,
    script::parse_mode mode)
{
    script instance;
    instance.from_data(stream, with_length_prefix, mode);
    return instance;
}

script script::factory_from_data(reader& source, bool with_length_prefix,
    script::parse_mode mode)
{
    script instance;
    instance.from_data(source, with_length_prefix, mode);
    return instance;
}

payment_type script::type() const
{
    if (is_pubkey_type(operations))
        return payment_type::pubkey;
    if (is_pubkey_hash_type(operations))
        return payment_type::pubkey_hash;
    if (is_script_hash_type(operations))
        return payment_type::script_hash;
    if (is_stealth_info_type(operations))
        return payment_type::stealth_info;
    if (is_multisig_type(operations))
        return payment_type::multisig;
    if (is_pubkey_sig_type(operations))
        return payment_type::pubkey_sig;
    if (is_pubkey_hash_sig_type(operations))
        return payment_type::pubkey_hash_sig;
    if (is_script_code_sig_type(operations))
        return payment_type::script_code_sig;
    if (is_multi_pubkey_sig_type(operations))
        return payment_type::multi_pubkey_sig;
    return payment_type::non_standard;
}

bool script::is_raw_data() const
{
    return (operations.size() == 1) &&
        (operations[0].code == opcode::raw_data);
}

bool script::is_valid() const
{
    return !operations.empty();
}

void script::reset()
{
    operations.clear();
}

bool script::from_data(const data_chunk& data, bool with_length_prefix,
    script::parse_mode mode)
{
    bool result = true;

    if (with_length_prefix)
    {
        boost::iostreams::stream<byte_source<data_chunk>> istream(data);
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

bool script::from_data(std::istream& stream, bool with_length_prefix,
    script::parse_mode mode)
{
    istream_reader source(stream);
    return from_data(source, with_length_prefix, mode);
}

bool script::from_data(reader& source, bool with_length_prefix,
    script::parse_mode mode)
{
    bool result = true;

    data_chunk raw_script;

    reset();

    if (with_length_prefix)
    {
        auto script_length = source.read_variable_uint_little_endian();
        result = source;

        BITCOIN_ASSERT(script_length <= max_uint32);

        if (result)
        {
            auto script_length32 = static_cast<uint32_t>(script_length);
            raw_script = source.read_data(script_length32);
            result = source;
        }
    }
    else
    {
        raw_script = source.read_data_to_eof();
        result = source;
    }

    if (result)
        result = deserialize(raw_script, mode);

    if (!result)
        reset();

    return result;
}

data_chunk script::to_data(bool with_length_prefix) const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream, with_length_prefix);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size(with_length_prefix));
    return data;
}

void script::to_data(std::ostream& stream, bool with_length_prefix) const
{
    ostream_writer sink(stream);
    to_data(sink, with_length_prefix);
}

void script::to_data(writer& sink, bool with_length_prefix) const
{
    if (with_length_prefix)
        sink.write_variable_uint_little_endian(satoshi_content_size());

    if ((operations.size() > 0) &&
        (operations[0].code == opcode::raw_data))
    {
        operations[0].to_data(sink);
    }
    else
    {
        for (const operation& op: operations)
            op.to_data(sink);
    }
}

uint64_t script::satoshi_content_size() const
{
    uint64_t size = 0;

    if (operations.size() > 0 && (operations[0].code == opcode::raw_data))
    {
        size = operations[0].satoshi_size();
    }
    else
    {
        for (const operation& op: operations)
        {
            size += op.satoshi_size();
        }
    }

    return size;
}

uint64_t script::satoshi_size(bool with_length_prefix) const
{
    uint64_t size = satoshi_content_size();

    if (with_length_prefix)
        size += variable_uint_size(size);

    return size;
}

bool script::from_string(const std::string& human_readable)
{
    // clear current contents
    operations.clear();

    const auto tokens = split(human_readable);
    bool clear = false;

    for (auto token = tokens.begin(); (token != tokens.end()); ++token)
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

        operations.push_back(operation{ code, data });
    }

    // empty invalid/failed parse content
    if (clear)
        operations.clear();

    return !clear;
}

std::string script::to_string() const
{
    std::ostringstream ss;

    for (auto it = operations.begin(); it != operations.end(); ++it)
    {
        if (it != operations.begin())
            ss << " ";

        ss << (*it).to_string();
    }

    return ss.str();
}

bool script::deserialize(const data_chunk& raw_script,
    script::parse_mode mode)
{
    bool successful_parse = false;

    if (mode != script::parse_mode::raw_data)
        successful_parse = parse(raw_script);

    if (!successful_parse && (mode != script::parse_mode::strict))
    {
        successful_parse = true;

        // recognize as raw data
        operations.clear();

        operations.push_back(
            operation{ opcode::raw_data, to_data_chunk(raw_script) });
    }

    return successful_parse;
}

bool script::parse(const data_chunk& raw_script)
{
    bool success = true;

    if (raw_script.begin() != raw_script.end())
    {
        boost::iostreams::stream<byte_source<data_chunk>> istream(raw_script);

        while (success && istream &&
            (istream.peek() != std::istream::traits_type::eof()))
        {
            operations.emplace_back();
            success = operations.back().from_data(istream);
        }
    }

    return success;
}

inline hash_digest one_hash()
{
    return hash_digest{
        {
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };
}

inline void nullify_input_sequences(transaction_input::list& inputs,
    uint32_t except_input)
{
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        if (i != except_input)
            inputs[i].sequence = 0;
    }
}

hash_digest script::generate_signature_hash(transaction parent_tx,
    uint32_t input_index, const script& script_code, uint32_t hash_type)
{
    // This is NOT considered an error result and callers should not test
    // for one_hash. This is a bitcoind bug we perpetuate.
    if (input_index >= parent_tx.inputs.size())
        return one_hash();

    // FindAndDelete(OP_CODESEPARATOR) done in op_checksigverify(...)

    // Blank all other inputs' signatures
    for (transaction_input& input : parent_tx.inputs)
        input.script.reset();

    parent_tx.inputs[input_index].script = script_code;

    // The default sighash::all signs all outputs, and the current input.
    // Transaction cannot be updated without resigning the input.
    // (note the lack of nullify_input_sequences() call)

    // sighash::none signs no outputs so they can be changed.
    if ((hash_type & 0x1f) == signature_hash_algorithm::none)
    {
        parent_tx.outputs.clear();
        nullify_input_sequences(parent_tx.inputs, input_index);
    }
    // Sign the single corresponding output to our index.
    // We don't care about additional inputs or outputs to the tx.
    else if ((hash_type & 0x1f) == signature_hash_algorithm::single)
    {
        transaction_output::list& outputs = parent_tx.outputs;
        uint32_t output_index = input_index;

        // This is NOT considered an error result and callers should not test
        // for one_hash. This is a bitcoind bug we perpetuate.
        if (output_index >= outputs.size())
            return one_hash();

        outputs.resize(output_index + 1);

        // Loop through outputs except the last one
        for (auto it = outputs.begin(); it != outputs.end() - 1; ++it)
        {
            it->value = std::numeric_limits<uint64_t>::max();
            it->script.reset();
        }

        nullify_input_sequences(parent_tx.inputs, input_index);
    }

    // Modifier to ignore the other inputs except our own.
    if (hash_type & signature_hash_algorithm::anyone_can_pay)
    {
        parent_tx.inputs[0] = parent_tx.inputs[input_index];
        parent_tx.inputs.resize(1);
    }

    return parent_tx.hash(hash_type);
}

// This uses the simpler and more secure deterministic nonce technique.
bool script::create_signature(data_chunk& signature,
    const ec_secret& private_key, const script& prevout_script,
    const transaction& new_tx, uint32_t input_index, uint32_t hash_type)
{
    // This always produces a valid signature hash.
    const auto sighash = generate_signature_hash(
        new_tx, input_index, prevout_script, hash_type);

    // Create the EC signature.
    signature = sign(private_key, sighash);

    // Add the sighash type to the end of the signature.
    signature.push_back(hash_type);
    return true;
}

// This uses the legacy non-deterministic nonce technique.
bool script::create_signature(data_chunk& signature,
    const ec_secret& private_key, const script& prevout_script,
    const transaction& new_tx, uint32_t input_index, uint32_t hash_type,
    const ec_secret& nonce)
{
    // This always produces a valid signature hash.
    const auto sighash = generate_signature_hash(
            new_tx, input_index, prevout_script, hash_type);

    // Create the EC signature.
    signature = sign(private_key, sighash, nonce);

    if (signature.empty())
        return false;

    // Add the sighash type to the end of the signature.
    signature.push_back(hash_type);
    return true;
}

bool script::check_signature(data_slice signature, const ec_point& public_key,
    const script& script_code, const transaction& parent_tx,
    uint32_t input_index)
{
    if (public_key.empty())
        return false;

    if (signature.empty())
        return false;

    // Remove the sighash type from the end of the signature.
    data_chunk ec_signature = to_data_chunk(signature);
    const auto hash_type = ec_signature.back();
    ec_signature.pop_back();

    // This always produces a valid signature hash.
    const auto sighash = generate_signature_hash(
            parent_tx, input_index, script_code, hash_type);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, ec_signature);
}

inline bool cast_to_bool(const data_chunk& values)
{
    for (auto it = values.begin(); it != values.end(); ++it)
    {
        if (*it != 0)
        {
            // Can be negative zero
            if (it == values.end() - 1 && *it == 0x80)
                return false;

            return true;
        }
    }

    return false;
}

template <typename DataStack>
void stack_swap(DataStack& stack, size_t index_a, size_t index_b)
{
    std::swap(*(stack.end() - index_a), *(stack.end() - index_b));
}

template <typename DataStack>
data_chunk pop_item(DataStack& stack)
{
    data_chunk value = stack.back();
    stack.pop_back();
    return value;
}

// Used by pick, roll and checkmultisig*
template <typename DataStack>
bool read_value(DataStack& stack, int32_t& value)
{
    if (stack.empty())
        return false;

    script_number mid;

    if (!mid.set_data(pop_item(stack)))
        return false;

    value = mid.int32();
    return true;
}

template <typename DataStack>
bool pick_roll_impl(DataStack& stack, bool is_roll)
{
    if (stack.size() < 2)
        return false;

    int32_t n;

    if (!read_value(stack, n))
        return false;

    int32_t stack_size = static_cast<int32_t>(stack.size());

    if (n < 0 || n >= stack_size)
        return false;

    auto slice_iter = stack.end() - n - 1;
    data_chunk item = *slice_iter;

    if (is_roll)
        stack.erase(slice_iter);

    stack.push_back(item);
    return true;
}

// Used by add, sub, mul, div, mod, lshift, rshift, booland, boolor,
// numequal, numequalverify, numnotequal, lessthan, greaterthan,
// lessthanorequal, greaterthanorequal, min, max
template <typename DataStack>
bool arithmetic_start_new(DataStack& stack, script_number& number_a,
    script_number& number_b)
{
    if (stack.size() < 2)
        return false;

    // The second number is at the top of the stack.
    if (!number_b.set_data(pop_item(stack)))
        return false;

    // The first is at the second position.
    if (!number_a.set_data(pop_item(stack)))
        return false;

    return true;
}

// Convert opcode to its actual numeric value.
template<typename OpCode>
auto base_value(OpCode code)
  -> typename std::underlying_type<OpCode>::type
{
    return static_cast<typename std::underlying_type<OpCode>::type>(code);
}

bool is_condition_opcode(opcode code)
{
    return code == opcode::if_
        || code == opcode::notif
        || code == opcode::else_
        || code == opcode::endif;
}

bool greater_op_16(opcode code)
{
    return base_value(code) > base_value(opcode::op_16);
}

bool op_negative_1(evaluation_context& context)
{
    script_number neg1(-1);
    context.primary.push_back(neg1.data());
    return true;
}

bool op_x(evaluation_context& context, opcode code)
{
    uint8_t difference =
        static_cast<uint8_t>(code) -
            static_cast<uint8_t>(opcode::op_1) + 1;
    script_number value(difference);
    context.primary.push_back(value.data());
    return true;
}

bool op_if(evaluation_context& context)
{
    bool value = false;

    if (!context.conditional.has_failed_branches())
    {
        if (context.primary.size() < 1)
            return false;

        value = cast_to_bool(context.pop_primary());
    }

    context.conditional.open(value);

    return true;
}

bool op_notif(evaluation_context& context)
{
    // A bit hackish...
    // Open IF statement but then invert it to get NOTIF
    if (!op_if(context))
        return false;

    context.conditional.else_();

    return true;
}

bool op_else(evaluation_context& context)
{
    if (context.conditional.closed())
        return false;

    context.conditional.else_();

    return true;
}

bool op_endif(evaluation_context& context)
{
    if (context.conditional.closed())
        return false;

    context.conditional.close();

    return true;
}

bool op_verify(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    if (!cast_to_bool(context.primary.back()))
        return false;

    context.pop_primary();

    return true;
}

bool op_toaltstack(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    data_chunk move_data = context.pop_primary();

    context.secondary.push_back(move_data);

    return true;
}

bool op_fromaltstack(evaluation_context& context)
{
    if (context.secondary.size() < 1)
        return false;

    context.primary.push_back(context.secondary.back());

    context.secondary.pop_back();

    return true;
}

bool op_2drop(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    context.primary.pop_back();
    context.primary.pop_back();

    return true;
}

bool op_2dup(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    auto dup_first = *(context.primary.end() - 2),
        dup_second = *(context.primary.end() - 1);

    context.primary.push_back(dup_first);
    context.primary.push_back(dup_second);

    return true;
}

bool op_3dup(evaluation_context& context)
{
    if (context.primary.size() < 3)
        return false;

    auto dup_first = *(context.primary.end() - 3),
        dup_second = *(context.primary.end() - 2),
        dup_third = *(context.primary.end() - 1);

    context.primary.push_back(dup_first);
    context.primary.push_back(dup_second);
    context.primary.push_back(dup_third);

    return true;
}

template <typename DataStack>
void copy_item_over_stack(DataStack& stack, size_t index)
{
    stack.push_back(*(stack.end() - index));
}

bool op_2over(evaluation_context& context)
{
    if (context.primary.size() < 4)
        return false;

    copy_item_over_stack(context.primary, 4);

    // Item -3 now becomes -4 because of last push
    copy_item_over_stack(context.primary, 4);

    return true;
}

bool op_2rot(evaluation_context& context)
{
    if (context.primary.size() < 6)
        return false;

    data_chunk first = *(context.primary.end() - 6),
        second = *(context.primary.end() - 5);

    context.primary.erase(context.primary.end() - 6, context.primary.end() - 4);
    context.primary.push_back(first);
    context.primary.push_back(second);

    return true;
}

bool op_2swap(evaluation_context& context)
{
    if (context.primary.size() < 4)
        return false;

    // Before: x1 x2 x3 x4
    // After:  x3 x4 x1 x2
    stack_swap(context.primary, 4, 2);
    stack_swap(context.primary, 3, 1);

    return true;
}

bool op_ifdup(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    if (cast_to_bool(context.primary.back()))
        context.primary.push_back(context.primary.back());

    return true;
}

bool op_depth(evaluation_context& context)
{
    script_number stack_size(context.primary.size());
    context.primary.push_back(stack_size.data());
    return true;
}

bool op_drop(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    context.primary.pop_back();

    return true;
}

bool op_dup(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    context.primary.push_back(context.primary.back());

    return true;
}

bool op_nip(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    context.primary.erase(context.primary.end() - 2);

    return true;
}

bool op_over(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    copy_item_over_stack(context.primary, 2);

    return true;
}

bool op_pick(evaluation_context& context)
{
    return pick_roll_impl(context.primary, false);
}

bool op_roll(evaluation_context& context)
{
    return pick_roll_impl(context.primary, true);
}

bool op_rot(evaluation_context& context)
{
    // Top 3 stack items are rotated to the left.
    // Before: x1 x2 x3
    // After:  x2 x3 x1
    if (context.primary.size() < 3)
        return false;

    stack_swap(context.primary, 3, 2);
    stack_swap(context.primary, 2, 1);

    return true;
}

bool op_swap(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    stack_swap(context.primary, 2, 1);

    return true;
}

bool op_tuck(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    data_chunk data = context.primary.back();

    context.primary.insert(context.primary.end() - 2, data);

    return true;
}

bool op_size(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number top_item_size(context.primary.back().size());
    context.primary.push_back(top_item_size.data());

    return true;
}

bool op_equal(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    if (context.pop_primary() == context.pop_primary())
        context.primary.push_back(stack_true_value);
    else
        context.primary.push_back(stack_false_value);

    return true;
}

bool op_equalverify(evaluation_context& context)
{
    if (context.primary.size() < 2)
        return false;

    return context.pop_primary() == context.pop_primary();
}

bool op_1add(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;
    if (!number.set_data(context.pop_primary()))
        return false;

    const script_number one(1);
    number += one;
    context.primary.push_back(number.data());

    return true;
}

bool op_1sub(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;
    if (!number.set_data(context.pop_primary()))
        return false;

    const script_number one(1);
    number -= one;
    context.primary.push_back(number.data());

    return true;
}

bool op_negate(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;
    if (!number.set_data(context.pop_primary()))
        return false;

    number = -number;
    context.primary.push_back(number.data());

    return true;
}

bool op_abs(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;

    if (!number.set_data(context.pop_primary()))
        return false;

    const script_number zero(0);

    if (number < zero)
        number = -number;

    context.primary.push_back(number.data());

    return true;
}

bool op_not(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;

    if (!number.set_data(context.pop_primary()))
        return false;

    const script_number zero(0);

    context.primary.push_back(
        script_number(number == zero).data());

    return true;
}

bool op_0notequal(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    script_number number;

    if (!number.set_data(context.pop_primary()))
        return false;

    const script_number zero(0);

    context.primary.push_back(
        script_number(number != zero).data());

    return true;
}

bool op_add(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result = number_a + number_b;

    context.primary.push_back(result.data());

    return true;
}

bool op_sub(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result = number_a - number_b;

    context.primary.push_back(result.data());

    return true;
}

bool op_booland(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number zero(0);
    const script_number result(number_a != zero && number_b != zero);

    context.primary.push_back(result.data());

    return true;
}

bool op_boolor(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number zero(0);
    const script_number result(number_a != zero || number_b != zero);

    context.primary.push_back(result.data());

    return true;
}

bool op_numequal(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a == number_b);

    context.primary.push_back(result.data());

    return true;
}

bool op_numequalverify(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    return number_a == number_b;
}

bool op_numnotequal(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a != number_b);
    context.primary.push_back(result.data());

    return true;
}

bool op_lessthan(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a < number_b);
    context.primary.push_back(result.data());

    return true;
}

bool op_greaterthan(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a > number_b);
    context.primary.push_back(result.data());

    return true;
}

bool op_lessthanorequal(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a <= number_b);
    context.primary.push_back(result.data());

    return true;
}

bool op_greaterthanorequal(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    const script_number result(number_a >= number_b);
    context.primary.push_back(result.data());

    return true;
}

bool op_min(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    if (number_a < number_b)
        context.primary.push_back(number_a.data());
    else
        context.primary.push_back(number_b.data());

    return true;
}

bool op_max(evaluation_context& context)
{
    script_number number_a, number_b;

    if (!arithmetic_start_new(context.primary, number_a, number_b))
        return false;

    if (number_a < number_b)
        context.primary.push_back(number_b.data());
    else
        context.primary.push_back(number_a.data());

    return true;
}

bool op_within(evaluation_context& context)
{
    if (context.primary.size() < 3)
        return false;

    script_number upper;

    if (!upper.set_data(context.pop_primary()))
        return false;

    script_number lower;

    if (!lower.set_data(context.pop_primary()))
        return false;

    script_number value;

    if (!value.set_data(context.pop_primary()))
        return false;

    if ((lower <= value) && (value < upper))
        context.primary.push_back(stack_true_value);
    else
        context.primary.push_back(stack_false_value);

    return true;
}

bool op_ripemd160(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    short_hash hash = ripemd160_hash(context.pop_primary());
    context.primary.push_back(to_data_chunk(hash));
    return true;
}

bool op_sha1(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    short_hash hash = sha1_hash(context.pop_primary());
    context.primary.push_back(to_data_chunk(hash));
    return true;
}

bool op_sha256(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    hash_digest hash = sha256_hash(context.pop_primary());
    context.primary.push_back(to_data_chunk(hash));
    return true;
}

bool op_hash160(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    short_hash hash = bitcoin_short_hash(context.pop_primary());
    context.primary.push_back(to_data_chunk(hash));
    return true;
}

bool op_hash256(evaluation_context& context)
{
    if (context.primary.size() < 1)
        return false;

    hash_digest hash = bitcoin_hash(context.pop_primary());
    context.primary.push_back(to_data_chunk(hash));
    return true;
}

bool op_checksigverify(evaluation_context& context, const script& script,
    const transaction& parent_tx, uint32_t input_index)
{
    if (context.primary.size() < 2)
        return false;

    data_chunk pubkey = context.pop_primary(),
        signature = context.pop_primary();

    chain::script script_code;

    for (auto it = context.codehash_begin; it != script.operations.end(); ++it)
    {
        const operation op = *it;

        if (op.data == signature || op.code == opcode::codeseparator)
            continue;

        script_code.operations.push_back(op);
    }

    return script::check_signature(
        signature, pubkey, script_code, parent_tx, input_index);
}

bool op_checksig(evaluation_context& context, const script& script,
    const transaction& parent_tx, uint32_t input_index)
{
    if (op_checksigverify(context, script, parent_tx, input_index))
        context.primary.push_back(stack_true_value);
    else
        context.primary.push_back(stack_false_value);

    return true;
}

bool read_section(evaluation_context& context, data_stack& section,
    size_t count)
{
    if (context.primary.size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
        section.push_back(context.pop_primary());

    return true;
}

bool op_checkmultisigverify(evaluation_context& context, const script& script,
    const transaction& parent_tx, uint32_t input_index)
{
    int32_t pubkeys_count;

    if (!read_value(context.primary, pubkeys_count))
        return false;

    if (pubkeys_count < 0 || pubkeys_count > 20)
        return false;

    context.operation_counter += pubkeys_count;

    if (context.operation_counter > op_counter_limit)
        return false;

    data_stack pubkeys;

    if (!read_section(context, pubkeys, pubkeys_count))
        return false;

    int32_t sigs_count;

    if (!read_value(context.primary, sigs_count))
        return false;

    if (sigs_count < 0 || sigs_count > pubkeys_count)
        return false;

    data_stack signatures;

    if (!read_section(context, signatures, sigs_count))
        return false;

    // Due to a bug in bitcoind, we need to read an extra null value
    // which we discard after.
    if (context.primary.empty())
        return false;

    context.primary.pop_back();

    auto is_signature =
        [&signatures](const data_chunk& data)
        {
            return std::find(signatures.begin(), signatures.end(),
                data) != signatures.end();
        };

    chain::script script_code;

    for (auto it = context.codehash_begin; it != script.operations.end(); ++it)
    {
        const operation op = *it;

        if (op.code == opcode::codeseparator)
            continue;

        if (is_signature(op.data))
            continue;

        script_code.operations.push_back(op);
    }

    // When checking the signatures against our public keys,
    // we always advance forwards until we either run out of pubkeys (fail)
    // or finish with our signatures (pass)
    auto pubkey_current = pubkeys.begin();

    for (const data_chunk& signature: signatures)
    {
        for (auto pubkey_iter = pubkey_current; ;)
        {
            if (script::check_signature(signature, *pubkey_iter, script_code,
                parent_tx, input_index))
            {
                pubkey_current = pubkey_iter;
                break;
            }

            // pubkeys are only exhausted when script failed
            ++pubkey_iter;

            if (pubkey_iter == pubkeys.end())
                return false;
        }
    }

    return true;
}

bool op_checkmultisig(evaluation_context& context, const script& script,
    const transaction& parent_tx, uint32_t input_index)
{
    if (op_checkmultisigverify(context, script, parent_tx, input_index))
        context.primary.push_back(stack_true_value);
    else
        context.primary.push_back(stack_false_value);

    return true;
}

bool run_operation(const operation& op, const transaction& parent_tx,
    uint32_t input_index, const script& script, evaluation_context& context)
{
    switch (op.code)
    {
        case opcode::zero:
        case opcode::special:
        case opcode::pushdata1:
        case opcode::pushdata2:
        case opcode::pushdata4:
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Invalid push operation in run_operation");
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
            return op_x(context, op.code);

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
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Disabled opcodes (verif/vernotif) in run_operation");
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
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Disabled splice operations in run_operation");
            return false;

        case opcode::tuck:
            return op_tuck(context);

        case opcode::size:
            return op_size(context);

        case opcode::invert:
        case opcode::and_:
        case opcode::or_:
        case opcode::xor_:
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Disabled bit logic operations in run_operation");
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
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Disabled opcodes (2mul/2div) in run_operation");
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
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Disabled numeric operations in run_operation");
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
            // This is set in the main run(...) loop
            // codehash_begin_ is updated to the current
            // operations_ iterator
            BITCOIN_ASSERT_MSG(op.code == opcode::bad_operation,
                "Invalid operation (codeseparator) in run_operation");
            return true;

        case opcode::checksig:
            return op_checksig(context, script, parent_tx, input_index);

        case opcode::checksigverify:
            return op_checksigverify(context, script, parent_tx, input_index);

        case opcode::checkmultisig:
            return op_checkmultisig(context, script, parent_tx, input_index);

        case opcode::checkmultisigverify:
            return op_checkmultisigverify(context, script, parent_tx, input_index);

        case opcode::op_nop1:
        case opcode::op_nop2:
        case opcode::op_nop3:
        case opcode::op_nop4:
        case opcode::op_nop5:
        case opcode::op_nop6:
        case opcode::op_nop7:
        case opcode::op_nop8:
        case opcode::op_nop9:
        case opcode::op_nop10:
            return true;

        case opcode::raw_data:
            return false;

        default:
            log_fatal(LOG_SCRIPT) << "Unimplemented operation <none "
                << static_cast<int>(op.code) << ">";
            return false;
    }

    return false;
}

bool increment_op_counter(opcode code, evaluation_context& context)
{
    if (greater_op_16(code))
        ++context.operation_counter;

    if (context.operation_counter > op_counter_limit)
        return false;

    return true;
}

bool opcode_is_disabled(opcode code)
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
    return true;
}

bool next_step(const transaction& parent_tx, uint32_t input_index,
    operation::stack::const_iterator it, const script& script,
    evaluation_context& context)
{
    const operation& op = *it;

    if (op.data.size() > 520)
        return false;

    if (!increment_op_counter(op.code, context))
        return false;

    if (opcode_is_disabled(op.code))
        return false;

    bool allow_execution = !context.conditional.has_failed_branches();

    // continue onwards to next command.
    if (!allow_execution && !is_condition_opcode(op.code))
        return true;

    // push data to the stack
    if (op.code == opcode::zero)
        context.primary.push_back(data_chunk());

    // These operations may also push empty data (opcode zero)
    // Hence we check the opcode over the shorter !op.data.empty()
    else if (op.code == opcode::special
        || op.code == opcode::pushdata1
        || op.code == opcode::pushdata2
        || op.code == opcode::pushdata4)
    {
        context.primary.push_back(op.data);
    }
    else if (op.code == opcode::codeseparator)
        context.codehash_begin = it;
    // opcodes above should assert 9;,sinside run_operation
    else if (!run_operation(op, parent_tx, input_index, script, context))
        return false;
    //log_debug() << "--------------------";
    //log_debug() << "Run: " << opcode_to_string(op.code);
    //log_debug() << "Stack:";
    //for (auto s: stack_)
    //    log_debug() << "[" << encode_base16(s) << "]";
    if (context.primary.size() + context.secondary.size() > 1000)
        return false;

    return true;
}

bool evaluate(const transaction& parent_tx, uint32_t input_index,
    const script& script, evaluation_context& context)
{
    if (script.satoshi_content_size() > 10000)
        return false;

    context.operation_counter = 0;

    context.codehash_begin = script.operations.begin();

    for (auto it = script.operations.begin(); it != script.operations.end(); ++it)
    {
        if (!next_step(parent_tx, input_index, it, script, context))
            return false;
    }

    return context.conditional.closed();
}

bool script::verify(const script& input_script, const script& output_script,
    const transaction& parent_tx, uint32_t input_index, bool bip16_enabled)
{
    evaluation_context input_context, output_context;

    if (!evaluate(parent_tx, input_index, input_script, input_context))
        return false;

    output_context.primary = input_context.primary;

    if (!evaluate(parent_tx, input_index, output_script, output_context))
        return false;

    if (output_context.primary.empty())
        return false;

    if (!cast_to_bool(output_context.primary.back()))
        return false;

    // Additional validation for spend-to-script-hash transactions
    if (bip16_enabled && (output_script.type() == payment_type::script_hash))
    {
        if (!is_push_only(input_script.operations))
            return false;

        // Load last input_script stack item as a script
        evaluation_context eval_context;
        eval_context.primary = input_context.primary;

        script eval_script;

        // Invalid script - parse-able only as raw_data
        if (!eval_script.from_data(input_context.primary.back(), false,
            script::parse_mode::raw_data_fallback))
            return false;

        // Pop last item and copy as starting stack to eval script
        eval_context.primary.pop_back();

        // Run script
        if (!evaluate(parent_tx, input_index, eval_script, eval_context))
            return false;

        if (eval_context.primary.empty())
            return false;

        return cast_to_bool(eval_context.primary.back());
    }

    return true;
}

} // end chain
} // end libbitcoin
