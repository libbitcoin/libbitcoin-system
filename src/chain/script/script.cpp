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
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/interpreter.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/sighash_algorithm.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/variable_uint_size.hpp>

namespace libbitcoin {
namespace chain {

static const auto sighash_all = sighash_algorithm::all;
static const auto sighash_none = sighash_algorithm::none;
static const auto sighash_single = sighash_algorithm::single;
static const auto anyone_flag = sighash_algorithm::anyone_can_pay;

// bit.ly/2cPazSa
static const auto one_hash = hash_literal(
    "0000000000000000000000000000000000000000000000000000000000000001");

// Constructors.
//-----------------------------------------------------------------------------

// A default instance is invalid (until modified).
script::script()
  : operations_(), is_raw_(false), valid_(false)
{
}

script::script(script&& other)
  : operations_(std::move(other.operations_)), is_raw_(other.is_raw_),
    valid_(other.valid_)
{
}

script::script(const script& other)
  : operations_(other.operations_), is_raw_(other.is_raw_),
    valid_(other.valid_)
{
}

script::script(operation::stack&& operations)
  : operations_(std::move(operations)), is_raw_(false), valid_(true)
{
}

script::script(const operation::stack& operations)
  : operations_(operations), is_raw_(false), valid_(true)
{
}

// Operators.
//-----------------------------------------------------------------------------

script& script::operator=(script&& other)
{
    operations_ = std::move(other.operations_);
    is_raw_ = other.is_raw_;
    valid_ = other.valid_;
    return *this;
}

script& script::operator=(const script& other)
{
    operations_ = other.operations_;
    is_raw_ = other.is_raw_;
    valid_ = other.valid_;
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

// Deserialization.
//-----------------------------------------------------------------------------

// static
script script::factory_from_data(const data_chunk& data, bool prefix,
    parse_mode mode)
{
    script instance;
    instance.from_data(data, prefix, mode);
    return instance;
}

// static
script script::factory_from_data(std::istream& stream, bool prefix,
    parse_mode mode)
{
    script instance;
    instance.from_data(stream, prefix, mode);
    return instance;
}

// static
script script::factory_from_data(reader& source, bool prefix, parse_mode mode)
{
    script instance;
    instance.from_data(source, prefix, mode);
    return instance;
}

bool script::from_data(const data_chunk& data, bool prefix, parse_mode mode)
{
    data_source istream(data);
    return from_data(istream, prefix, mode);
}

bool script::from_data(std::istream& stream, bool prefix, parse_mode mode)
{
    istream_reader source(stream);
    return from_data(source, prefix, mode);
}

bool script::from_data(reader& source, bool prefix, parse_mode mode)
{
    reset();

    valid_ = true;
    auto bytes = prefix ?
        source.read_bytes(source.read_size_little_endian()) :
        source.read_bytes();

    if (source)
    {
        const auto deserialize =
            (mode != parse_mode::raw_data && parse(bytes)) ||
            (mode != parse_mode::strict && emplace(std::move(bytes)));

        if (!deserialize)
            source.invalidate();
    }

    if (!source)
        reset();

    return source;
}

// private
bool script::emplace(data_chunk&& raw_script)
{
    // The raw_data opcode is ignored thanks to the is_raw_ flag.
    is_raw_ = true;
    operations_.push_back({ opcode::raw_data, std::move(raw_script) });
    return true;
}

// private
bool script::parse(const data_chunk& raw_script)
{
    data_source istream(raw_script);
    istream_reader source(istream);

    while (!source.is_exhausted())
    {
        operations_.emplace_back();

        if (!operations_.back().from_data(source))
        {
            operations_.clear();
            return false;
        }
    }

    return true;
}

// protected
void script::reset()
{
    operations_.clear();
    operations_.shrink_to_fit();
    is_raw_ = false;
    valid_ = false;
}

bool script::is_valid() const
{
    return valid_ || !operations_.empty() || is_raw_;
}

// protected
bool script::is_raw_data() const
{
    return (operations_.size() == 1) && is_raw_;
}

bool script::from_string(const std::string& mnemonic)
{
    reset();

    valid_ = true;
    const auto tokens = split(mnemonic);

    for (auto token = tokens.begin(); token != tokens.end(); ++token)
    {
        opcode code;
        data_chunk data;

        if (*token == "[")
        {
            data_chunk raw_data;

            if (!decode_base16(raw_data, *++token))
            {
                valid_ = false;
                break;
            }

            if (raw_data.empty() || *++token != "]")
            {
                valid_ = false;
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
            valid_ = false;
            break;
        }

        operations_.push_back({ code, data });
    }

    if (!valid_)
        reset();

    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

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
        sink.write_bytes(operations_.front().data());
        return;
    }

    for (const auto& op: operations_)
        op.to_data(sink);
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

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

// TODO: cache.
uint64_t script::satoshi_content_size() const
{
    if (is_raw_data())
        return operations_.front().data().size();

    const auto value = [](uint64_t total, const operation& op)
    {
        return safe_add(total, op.serialized_size());
    };

    const auto& ops = operations_;
    return std::accumulate(ops.begin(), ops.end(), uint64_t{ 0 }, value);
}

uint64_t script::serialized_size(bool prefix) const
{
    auto size = satoshi_content_size();

    if (prefix)
        size += variable_uint_size(size);

    return size;
}

// deprecated (unsafe)
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
    valid_ = true;
    operations_ = value;
}

void script::set_operations(operation::stack&& value)
{
    valid_ = true;
    operations_ = std::move(value);
}

// Signing.
//-----------------------------------------------------------------------------

inline sighash_algorithm to_sighash_enum(uint8_t sighash_type)
{
    return static_cast<sighash_algorithm>(
        sighash_type & sighash_algorithm::mask);
}

inline uint8_t is_sighash_enum(uint8_t sighash_type,
    sighash_algorithm value)
{
    return to_sighash_enum(sighash_type) == value;
}

inline bool is_sighash_flag(uint8_t sighash_type,
    sighash_algorithm value)
{
    return (sighash_type & value) != 0;
}

static hash_digest sign_none(const transaction& tx, uint32_t input_index,
    const script& script_code, uint8_t sighash_type, bool anyone)
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
    }

    // Move new inputs to new transaction and drop outputs.
    return transaction(tx.version(), tx.locktime(), std::move(ins),
        output::list{}).hash(sighash_type);
}

static hash_digest sign_single(const transaction& tx, uint32_t input_index,
    const script& script_code, uint8_t sighash_type, bool anyone)
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
    }

    // Trim and clear outputs except that of specified input index.
    const auto& outputs = tx.outputs();
    output::list outs(input_index + 1);

    BITCOIN_ASSERT(input_index < outputs.size());
    outs.back() = outputs[input_index];

    // Move new inputs and new outputs to new transaction.
    return transaction(tx.version(), tx.locktime(), std::move(ins),
        std::move(outs)).hash(sighash_type);
}

static hash_digest sign_all(const transaction& tx, uint32_t input_index,
    const script& script_code, uint8_t sighash_type, bool anyone)
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
    }

    // Move new inputs and copy outputs to new transaction.
    transaction out(tx.version(), tx.locktime(), input::list{}, tx.outputs());
    out.set_inputs(std::move(ins));
    return out.hash(sighash_type);
}

// static
hash_digest script::generate_signature_hash(const transaction& tx,
    uint32_t input_index, const script& script_code, uint8_t sighash_type)
{
    const auto any = is_sighash_flag(sighash_type, anyone_flag);
    const auto single = is_sighash_enum(sighash_type, sighash_single);

    // Bounds are verified here and therefore only asserted in the helpers.
    if (input_index >= tx.inputs().size() || 
        (input_index >= tx.outputs().size() && single))
    {
        // This is a wacky bitcoind behavior we necessarily perpetuate.
        return one_hash;
    }

    switch (to_sighash_enum(sighash_type))
    {
        case sighash_none:
            return sign_none(tx, input_index, script_code, sighash_type, any);

        case sighash_single:
            return sign_single(tx, input_index, script_code, sighash_type, any);

        default:
        case sighash_all:
            return sign_all(tx, input_index, script_code, sighash_type, any);
    }
}

// static
bool script::check_signature(const ec_signature& signature,
    uint8_t sighash_type, const data_chunk& public_key,
    const script& script_code, const transaction& tx, uint32_t input_index)
{
    if (public_key.empty())
        return false;

    // This always produces a valid signature hash, including one_hash.
    const auto sighash = script::generate_signature_hash(tx, input_index,
        script_code, sighash_type);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

// static
bool script::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& prevout_script, const transaction& tx, uint32_t input_index,
    uint8_t sighash_type)
{
    // This always produces a valid signature hash, including one_hash.
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

// Utilities.
//-----------------------------------------------------------------------------

// static
// Test rule_fork flag for a given context.
bool script::is_enabled(uint32_t flags, rule_fork flag)
{
    return (flag & flags) != 0;
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

// Validation.
//-----------------------------------------------------------------------------

// static
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

// static
// TODO: return detailed result code indicating failure condition.
code script::verify(const transaction& tx, uint32_t input_index,
    const script& prevout_script, uint32_t flags)
{
    if (input_index >= tx.inputs().size())
        return error::operation_failed;

    const auto& input_script = tx.inputs()[input_index].script();
    evaluation_context in_context(flags);

    // Evaluate the input script.
    if (!interpreter::run(tx, input_index, input_script, in_context, flags))
        return error::validate_inputs_failed;

    evaluation_context out_context(flags, in_context.stack);

    // Evaluate the output script.
    if (!interpreter::run(tx, input_index, prevout_script, out_context, flags))
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
        if (!interpreter::run(tx, input_index, eval, eval_context, flags))
            return error::validate_inputs_failed;

        // Return the stack state.
        if (!stack_result(eval_context))
            return error::validate_inputs_failed;
    }

    return error::success;
}

} // namespace chain
} // namespace libbitcoin
