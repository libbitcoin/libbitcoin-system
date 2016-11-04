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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/interpreter.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/rule_fork.hpp>
#include <bitcoin/bitcoin/chain/script/script_pattern.hpp>
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
  : valid_(false), cached_(false)
{
}

script::script(script&& other)
  : bytes_(std::move(other.bytes_)), valid_(other.valid_), cached_(false)
{
    // TODO: implement safe private accessor for conditional cache transfer.
}

script::script(const script& other)
  : bytes_(other.bytes_), valid_(other.valid_), cached_(false)
{
    // TODO: implement safe private accessor for conditional cache transfer.
}

script::script(const operation_stack& ops)
{
    from_stack(ops);
}

script::script(operation_stack&& ops)
{
    from_stack(ops);
}

script::script(data_chunk&& encoded, bool prefix)
{
    // TODO: store prefix in bytes_ to simplify and optimize this.
    if (prefix)
    {
        valid_ = from_data(encoded, prefix);
        return;
    }

    bytes_ = std::move(encoded);
    valid_ = true;
}

script::script(const data_chunk& encoded, bool prefix)
{
    valid_ = from_data(encoded, prefix);
}

// Operators.
//-----------------------------------------------------------------------------

// Concurrent read/write is not supported, so no critical section.
script& script::operator=(script&& other)
{
    // TODO: implement safe private accessor for conditional cache transfer.
    reset();
    bytes_ = std::move(other.bytes_);
    valid_ = other.valid_;
    return *this;
}

// Concurrent read/write is not supported, so no critical section.
script& script::operator=(const script& other)
{
    // TODO: implement safe private accessor for conditional cache transfer.
    reset();
    bytes_ = other.bytes_;
    valid_ = other.valid_;
    return *this;
}

bool script::operator==(const script& other) const
{
    return bytes_ == other.bytes_;
}

bool script::operator!=(const script& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
script script::factory_from_data(const data_chunk& encoded, bool prefix)
{
    script instance;
    instance.from_data(encoded, prefix);
    return instance;
}

// static
script script::factory_from_data(std::istream& stream, bool prefix)
{
    script instance;
    instance.from_data(stream, prefix);
    return instance;
}

// static
script script::factory_from_data(reader& source, bool prefix)
{
    script instance;
    instance.from_data(source, prefix);
    return instance;
}

bool script::from_data(const data_chunk& encoded, bool prefix)
{
    data_source istream(encoded);
    return from_data(istream, prefix);
}

bool script::from_data(std::istream& stream, bool prefix)
{
    istream_reader source(stream);
    return from_data(source, prefix);
}

// Concurrent read/write is not supported, so no critical section.
bool script::from_data(reader& source, bool prefix)
{
    reset();
    valid_ = true;

    bytes_ = prefix ?
        source.read_bytes(source.read_size_little_endian()) :
        source.read_bytes();

    if (!source)
        reset();

    return source;
}

// Concurrent read/write is not supported, so no critical section.
bool script::from_string(const std::string& mnemonic)
{
    reset();

    // There is strictly one operation per string token.
    const auto tokens = split(mnemonic);
    operation_stack ops;
    ops.resize(tokens.size());

    // Create an op stack from the split tokens, one operation per token.
    for (size_t index = 0; index < ops.size(); ++index)
        if (!ops[index].from_string(tokens[index]))
            return false;

    from_stack(ops);
    return true;
}

// Concurrent read/write is not supported, so no critical section.
void script::from_stack(const operation_stack& ops)
{
    reset();
    valid_ = true;
    bytes_ = stack_to_data(ops);
    stack_ = ops;
    cached_ = true;
}

// private/static
data_chunk script::stack_to_data(const operation_stack& ops)
{
    data_chunk out;
    out.reserve(serialized_size(ops));

    const auto concatenate = [&out](const operation& op)
    {
        auto bytes = op.to_data();
        std::move(bytes.begin(), bytes.end(), std::back_inserter(out));
    };

    std::for_each(ops.begin(), ops.end(), concatenate);
    BITCOIN_ASSERT(out.size() == serialized_size(ops));
    return out;
}

// private/static
size_t script::serialized_size(const operation_stack& ops)
{
    const auto op_size = [](size_t total, const operation& op)
    {
        return total + op.serialized_size();
    };

    return std::accumulate(ops.begin(), ops.end(), size_t{0}, op_size);
}

// protected
// Concurrent read/write is not supported, so no critical section.
void script::reset()
{
    bytes_.clear();
    bytes_.shrink_to_fit();
    valid_ = false;
    cached_ = false;
    stack_.clear();
    stack_.shrink_to_fit();
}

bool script::is_valid() const
{
    // All script bytes are valid under some circumstance (e.g. coinbase).
    // This returns false if a prefix and byte count does not match.
    return valid_;
}

bool script::is_valid_stack() const
{
    // Script validity is independent of individual operation validity.
    // There is a trailing invalid/default op if a push op had a size mismatch.
    return stack().empty() || stack_.back().is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk script::to_data(bool prefix) const
{
    data_chunk data;
    data.reserve(serialized_size(prefix));
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
    // TODO: optimize by always storing the prefixed serialization.
    if (prefix)
        sink.write_variable_little_endian(satoshi_content_size());

    sink.write_bytes(bytes_);
}

std::string script::to_string(uint32_t active_forks) const
{
    auto first = true;
    std::ostringstream text;

    for (const auto& op: stack())
    {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

// Iteration.
//-----------------------------------------------------------------------------
// The first stack access must be method-based to guarantee the cache.

bool script::empty() const
{
    return stack().empty();
}

size_t script::size() const
{
    return stack().size();
}

const operation& script::front() const
{
    BITCOIN_ASSERT(!stack().empty());
    return stack().front();
}

const operation& script::back() const
{
    BITCOIN_ASSERT(!stack().empty());
    return stack().back();
}

const operation& script::operator[](std::size_t index) const
{
    BITCOIN_ASSERT(index < stack().size());
    return stack()[index];
}

operation::const_iterator script::begin() const
{
    return stack().begin();
}

operation::const_iterator script::end() const
{
    return stack().end();
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

uint64_t script::satoshi_content_size() const
{
    return bytes_.size();
}

uint64_t script::serialized_size(bool prefix) const
{
    auto size = satoshi_content_size();

    if (prefix)
        size += variable_uint_size(size);

    return size;
}

// protected
const operation_stack& script::stack() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (cached_)
    {
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return stack_;
    }

    operation op;
    data_source istream(bytes_);
    istream_reader source(istream);
    const auto size = bytes_.size();

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    mutex_.unlock_upgrade_and_lock();

    // One operation per byte is the upper limit of operations.
    stack_.reserve(size);

    // If an op fails it is placed on the stack and the loop terminates.
    // To validate the ops the caller must test the last op.is_valid().
    // This is not necessary during script validation as it is autmoatic.
    while (!source.is_exhausted())
    {
        op.from_data(source);
        stack_.push_back(std::move(op));
    }

    stack_.shrink_to_fit();
    cached_ = true;

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return stack_;
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

static script strip_code_seperators(const script& script_code)
{
    operation_stack ops;

    for (auto op = script_code.begin(); op != script_code.end(); ++op)
        if (op->code() != opcode::codeseparator)
            ops.push_back(*op);

    return script(std::move(ops));
}

// static
hash_digest script::generate_signature_hash(const transaction& tx,
    uint32_t input_index, const script& script_code, uint8_t sighash_type)
{
    const auto any = is_sighash_flag(sighash_type, anyone_flag);
    const auto single = is_sighash_enum(sighash_type, sighash_single);

    if (input_index >= tx.inputs().size() || 
        (input_index >= tx.outputs().size() && single))
    {
        //*********************************************************************
        // CONSENSUS: wacky satoshi behavior we must perpetuate.
        //*********************************************************************
        return one_hash;
    }

    //*************************************************************************
    // CONSENSUS: more wacky satoshi behavior we must perpetuate.
    //*************************************************************************
    const auto stripped = strip_code_seperators(script_code);

    switch (to_sighash_enum(sighash_type))
    {
        case sighash_none:
            return sign_none(tx, input_index, stripped, sighash_type, any);
        case sighash_single:
            return sign_single(tx, input_index, stripped, sighash_type, any);
        default:
        case sighash_all:
            return sign_all(tx, input_index, stripped, sighash_type, any);
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
bool script::is_enabled(uint32_t flags, rule_fork flag)
{
    return (flag & flags) != 0;
}

//*****************************************************************************
// CONSENSUS: this includes opcode::reserved_80 despite it being reserved.
// This affects the operation count in p2sh script evaluation.
//*****************************************************************************
bool script::is_relaxed_push_data(opcode code) const
{
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto value = static_cast<uint8_t>(code);
    return value <= op_96;
}

bool script::is_relaxed_push_data_only() const
{
    const auto push = [&](const operation& op)
    {
        return is_relaxed_push_data(op.code());
    };

    return std::all_of(stack().begin(), stack().end(), push);
}

script_pattern script::pattern() const
{
    // The first stack access must be method-based to guarantee the cache.
    if (is_null_data_pattern(stack()))
        return script_pattern::null_data;

    if (is_pay_multisig_pattern(stack_))
        return script_pattern::pay_multisig;

    if (is_pay_public_key_pattern(stack_))
        return script_pattern::pay_public_key;

    if (is_pay_key_hash_pattern(stack_))
        return script_pattern::pay_key_hash;

    if (is_pay_script_hash_pattern(stack_))
        return script_pattern::pay_script_hash;

    if (is_sign_multisig_pattern(stack_))
        return script_pattern::sign_multisig;

    if (is_sign_public_key_pattern(stack_))
        return script_pattern::sign_public_key;

    if (is_sign_key_hash_pattern(stack_))
        return script_pattern::sign_key_hash;

    if (is_sign_script_hash_pattern(stack_))
        return script_pattern::sign_script_hash;

    return script_pattern::non_standard;
}

// TODO: distinct cache property for serialized_script total.
size_t script::sigops(bool embedded) const
{
    size_t total = 0;
    auto preceding = opcode::reserved_255;

    //*************************************************************************
    // CONSENSUS: this short-circuits sigop counting but ultimately has no
    // effect since p2sh sigops are not counted for coinbase txs and that
    // would be the only case in which a non-push-data p2sh input script would
    // not invalidate the block, due to the p2sh push data check. So we avoid
    // the check here as a sigop count optimization.
    //*************************************************************************
    ////if (embedded && !is_relaxed_push_data_only())
    ////    return total;

    // The first stack access must be method-based to guarantee the cache.
    for (const auto& op: stack())
    {
        const auto code = op.code();

        if (code == opcode::checksig ||
            code == opcode::checksigverify)
        {
            total++;
        }
        else if (code == opcode::checkmultisig ||
            code == opcode::checkmultisigverify)
        {
            total += embedded && operation::is_positive(preceding) ?
                operation::opcode_to_positive(preceding) :
                multisig_default_sigops;
        }

        preceding = code;
    }

    return total;
}

// This is used internally as an optimization over using script::pattern.
bool script::is_pay_to_script_hash(uint32_t flags) const
{
    // The prevout stack access must be method-based to guarantee the cache.
    return is_enabled(flags, rule_fork::bip16_rule) &&
        is_pay_script_hash_pattern(stack());
}

// TODO: cache (default to max_size_t sentinel).
size_t script::pay_script_hash_sigops(const script& prevout_script) const
{
    // We count p2sh sigops when the previout script is p2sh.
    if (!prevout_script.is_pay_to_script_hash(rule_fork::bip16_rule))
        return 0;

    // Obtain the embedded script from the last input script item (data).
    script embedded;

    // The first stack access must be method-based to guarantee the cache.
    if (stack().empty() || !embedded.from_data(stack_.back().data(), false))
        return 0;

    // Count the sigops in the embedded script using BIP16 rules.
    return embedded.sigops(true);
}

//*****************************************************************************
// CONSENSUS: this is a pointless, broken, premature optimization attempt.
//*****************************************************************************
void script::find_and_delete(const data_chunk& endorsement)
{
    // If this is empty it would produce an empty script but not operation.
    // So we test it for empty prior to operation reserialization.
    if (endorsement.empty())
        return;

    // The value must be serialized to script using non-minimal encoding.
    // Non-minimally-encoded target values will therefore not match.
    const auto value = operation(endorsement, false).to_data();

    // No copying occurs below. If a match is found the remainder is shifted
    // into its place (erase). No memory allocation is caused by the shift.

    operation op;
    data_source stream(bytes_);
    istream_reader source(stream);
    auto begin = bytes_.begin();

    // This test handles stream end and op deserialization failure.
    while (!source.is_exhausted())
    {
        // This is the 'broken' aspect of this method. The comparison and erase
        // are not limited to a single operation and so can erase arbitrary
        // upstream data from the script. Unfortunately that is now consensus.
        while (starts_with(begin, bytes_.end(), value))
            begin = bytes_.erase(begin, begin + value.size());

        // The source is not affected by changes upstream of its position.
        op.from_data(source);
        begin += op.serialized_size();
    }
}

// Concurrent read/write is not supported, so no critical section.
void script::purge(const data_stack& endorsements)
{
    for (auto& endorsement: endorsements)
        find_and_delete(endorsement);

    // Invalidate the cache so that the op stack may be regenerated.
    stack_.clear();
    cached_ = false;
    bytes_.shrink_to_fit();
}

// Validation.
//-----------------------------------------------------------------------------
// static

code script::verify(const transaction& tx, uint32_t input_index,
    uint32_t flags)
{
    if (input_index >= tx.inputs().size())
        return error::operation_failed;

    // Obtain the previous output script from the cached previous output.
    auto& prevout = tx.inputs()[input_index].previous_output().validation;
    return verify(tx, input_index, prevout.cache.script(), flags);
}

// TODO: return detailed result code indicating failure condition.
code script::verify(const transaction& tx, uint32_t input_index,
    const script& prevout_script, uint32_t flags)
{
    if (input_index >= tx.inputs().size())
        return error::operation_failed;

    // Create a context for evaluation of the input script.
    evaluation_context input_context(flags);
    const auto& input_script = tx.inputs()[input_index].script();

    // Evaluate the input script.
    if (!interpreter::run(tx, input_index, input_script, input_context))
        return error::validate_inputs_failed;
    
    // We need to preserve the input stack for potential p2sh evaluation.
    // COPY input context stack (only) and flags for eval of prevout script.
    evaluation_context out_context(input_context);

    // Evaluate the prevout script against the input stack.
    if (!interpreter::run(tx, input_index, prevout_script, out_context))
        return error::validate_inputs_failed;

    // Return invalid if stack is not true.
    if (out_context.empty() || !out_context.stack_true())
        return error::validate_inputs_failed;

    // If the previout script is not p2sh with bip16 enabled we are done.
    if (!prevout_script.is_pay_to_script_hash(flags))
        return error::success;

    // Additional validation for bip16 pay-to-script-hash script.
    return pay_hash(tx, input_index, input_script, input_context);
}

// private
code script::pay_hash(const transaction& tx, uint32_t input_index,
    const script& input_script, evaluation_context& input_context)
{
    if (!input_script.is_relaxed_push_data_only())
        return error::validate_inputs_failed;

    script embedded;

    // The last stack item is the embedded script.
    // input_context.stack cannot be empty here because out_context was true.
    if (!embedded.from_data(input_context.pop(), false))
        return error::validate_inputs_failed;

    // MOVE popped stack (only) and flags for evaluation of eval script.
    evaluation_context context(std::move(input_context));

    // Evaluate the embedded script.
    if (!interpreter::run(tx, input_index, embedded, context))
        return error::validate_inputs_failed;

    // Return invalid if stack is not true, otherwise valid.
    return context.empty() || !context.stack_true() ?
        error::validate_inputs_failed : error::success;
}

} // namespace chain
} // namespace libbitcoin
