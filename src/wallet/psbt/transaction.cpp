/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/wallet/psbt/transaction.hpp>

#include <algorithm>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

using namespace system::chain;

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

// [magic:4=transaction][separator:1=0xff]
constexpr uint32_t psbt_magic = 0x70736274;
constexpr uint8_t psbt_separator = 0xff;

// The xpub keydata is the BIP32 serialization (without checksum).
constexpr size_t xpub_size = 78;

const uint32_t transaction::version_0 = 0;
const uint32_t transaction::version_2 = 2;

// Serialization emits repeated-type entries in key order (deterministic).
static entry::list sorted(const entry::list& entries) NOEXCEPT
{
    auto copy = entries;
    std::sort(copy.begin(), copy.end(),
        [](const entry& left, const entry& right) NOEXCEPT
        {
            return left.key < right.key;
        });

    return copy;
}

static xpub::list sorted(const xpub::list& entries) NOEXCEPT
{
    auto copy = entries;
    std::sort(copy.begin(), copy.end(),
        [](const xpub& left, const xpub& right) NOEXCEPT
        {
            return left.key < right.key;
        });

    return copy;
}

// The full key of each pair must be unique within the map.
static bool is_duplicate(std_vector<data_chunk>& keys,
    const data_chunk& key) NOEXCEPT
{
    if (std::find(keys.begin(), keys.end(), key) != keys.end())
        return true;

    keys.push_back(key);
    return false;
}

static bool to_uint32(uint32_t& out, const data_chunk& value) NOEXCEPT
{
    if (value.size() != sizeof(uint32_t))
        return false;

    out = from_little_endian<uint32_t>(value);
    return true;
}

static bool to_count(size_t& out, const data_chunk& value) NOEXCEPT
{
    stream::in::fast stream(value);
    read::bytes::fast source(stream);
    out = source.read_size();
    return !!source && source.is_exhausted();
}

// Constructors.
// ----------------------------------------------------------------------------

transaction::transaction() NOEXCEPT
{
}

transaction::transaction(const data_chunk& decoded) NOEXCEPT
  : transaction(from_data(decoded))
{
}

transaction::transaction(const std::string& encoded) NOEXCEPT
  : transaction(from_string(encoded))
{
}

transaction::transaction(const chain::transaction& unsigned_tx) NOEXCEPT
  : transaction(from_transaction(unsigned_tx))
{
}

transaction::transaction(uint32_t tx_version) NOEXCEPT
  : valid_(true), version_(version_2), tx_version_(tx_version)
{
}

// Factories.
// ----------------------------------------------------------------------------

transaction transaction::from_data(const data_chunk& decoded) NOEXCEPT
{
    stream::in::fast stream(decoded);
    read::bytes::fast source(stream);

    transaction out{};
    out.valid_ = out.parse(source);
    return out.valid_ ? out : transaction{};
}

transaction transaction::from_string(const std::string& encoded) NOEXCEPT
{
    data_chunk decoded{};
    return decode_base64(decoded, encoded) ? transaction(decoded) : transaction{};
}

// The creator role (BIP174).
transaction transaction::from_transaction(const chain::transaction& tx) NOEXCEPT
{
    const auto unsigned_input = [](const auto& in) NOEXCEPT
    {
        return in->script().ops().empty() && in->witness().stack().empty();
    };

    const auto& inputs = *tx.inputs_ptr();
    if (!tx.is_valid() ||
        !std::all_of(inputs.begin(), inputs.end(), unsigned_input))
        return {};

    transaction out{};
    out.valid_ = true;
    out.tx_ = tx;
    out.inputs_.resize(inputs.size());
    out.outputs_.resize(tx.outputs_ptr()->size());
    return out;
}

// Parse.
// ----------------------------------------------------------------------------

bool transaction::parse(reader& source) NOEXCEPT
{
    if (source.read_4_bytes_big_endian() != psbt_magic ||
        source.read_byte() != psbt_separator)
        return false;

    if (!parse_global(source))
        return false;

    const auto version0 = (version_ == version_0);
    const auto ins = version0 ? tx_.inputs_ptr()->size() : inputs_.size();
    const auto outs = version0 ? tx_.outputs_ptr()->size() : outputs_.size();

    inputs_.clear();
    for (auto count = ins; !is_zero(count); --count)
    {
        input input{};
        if (!input.from_data(source, version_))
            return false;

        inputs_.push_back(std::move(input));
    }

    outputs_.clear();
    for (auto count = outs; !is_zero(count); --count)
    {
        output output{};
        if (!output.from_data(source, version_))
            return false;

        outputs_.push_back(std::move(output));
    }

    return source.is_exhausted();
}

bool transaction::parse_global(reader& source) NOEXCEPT
{
    std_vector<data_chunk> keys{};
    entry entry{};
    bool has_tx{};
    bool has_tx_version{};
    bool has_input_count{};
    bool has_output_count{};
    size_t input_count{};
    size_t output_count{};

    while (entry.from_data(source))
    {
        if (is_duplicate(keys, entry.key))
            return false;

        auto data = entry.keydata();
        const auto typed = data.empty();
        switch (static_cast<global_key>(entry.type()))
        {
            case global_key::unsigned_tx:
            {
                const auto unsigned_input = [](const auto& in) NOEXCEPT
                {
                    return in->script().ops().empty();
                };

                stream::in::fast stream(entry.value);
                read::bytes::fast tx(stream);
                tx_ = chain::transaction{ tx, false };
                const auto& inputs = *tx_.inputs_ptr();
                if (!typed || !tx_.is_valid() || !tx.is_exhausted() ||
                    !std::all_of(inputs.begin(), inputs.end(),
                        unsigned_input))
                    return false;

                has_tx = true;
                break;
            }
            case global_key::xpub:
            {
                xpub key{ std::move(data), {} };
                if (key.key.size() != xpub_size ||
                    !key.origin.from_value(entry.value))
                    return false;

                xpubs_.push_back(std::move(key));
                break;
            }
            case global_key::tx_version:
            {
                if (!typed || !to_uint32(tx_version_, entry.value))
                    return false;

                has_tx_version = true;
                break;
            }
            case global_key::fallback_locktime:
            {
                uint32_t fallback{};
                if (!typed || !to_uint32(fallback, entry.value))
                    return false;

                fallback_locktime_ = fallback;
                break;
            }
            case global_key::input_count:
            {
                if (!typed || !to_count(input_count, entry.value))
                    return false;

                has_input_count = true;
                break;
            }
            case global_key::output_count:
            {
                if (!typed || !to_count(output_count, entry.value))
                    return false;

                has_output_count = true;
                break;
            }
            case global_key::tx_modifiable:
            {
                if (!typed || entry.value.size() != sizeof(uint8_t))
                    return false;

                tx_modifiable_ = entry.value.front();
                break;
            }
            case global_key::version:
            {
                if (!typed || !to_uint32(version_, entry.value))
                    return false;
                break;
            }
            default:
            {
                others_.push_back(std::move(entry));
                break;
            }
        }
    }

    if (!source)
        return false;

    // BIP174 requires the unsigned tx and excludes BIP370 fields.
    if (version_ == version_0)
        return has_tx && !has_tx_version && !has_input_count &&
            !has_output_count && !fallback_locktime_.has_value() &&
            !tx_modifiable_.has_value();

    // BIP370 requires the version, counts and excludes the unsigned tx.
    if (version_ == version_2)
    {
        if (has_tx || !has_tx_version || !has_input_count ||
            !has_output_count)
            return false;

        inputs_.resize(input_count);
        outputs_.resize(output_count);
        return true;
    }

    return false;
}

// Operators.
// ----------------------------------------------------------------------------

transaction::operator bool() const NOEXCEPT
{
    return valid_;
}

bool transaction::operator==(const transaction& other) const NOEXCEPT
{
    return to_chunk() == other.to_chunk() && valid_ == other.valid_;
}

bool transaction::operator!=(const transaction& other) const NOEXCEPT
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, transaction& to)
{
    std::string value{};
    in >> value;
    to = transaction(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const transaction& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

// Serializers.
// ----------------------------------------------------------------------------

std::string transaction::encoded() const NOEXCEPT
{
    return encode_base64(to_chunk());
}

data_chunk transaction::to_chunk() const NOEXCEPT
{
    const auto write = [](writer& sink, global_key type,
        const data_chunk& data, const data_chunk& value) NOEXCEPT
    {
        sink.write_variable(add1(data.size()));
        sink.write_byte(static_cast<uint8_t>(type));
        sink.write_bytes(data);
        sink.write_variable(value.size());
        sink.write_bytes(value);
    };

    data_chunk out{};
    stream::out::data stream(out);
    write::bytes::ostream sink(stream);

    sink.write_4_bytes_big_endian(psbt_magic);
    sink.write_byte(psbt_separator);

    if (version_ == version_0)
        write(sink, global_key::unsigned_tx, {}, tx_.to_data(false));

    for (const auto& key: sorted(xpubs_))
        write(sink, global_key::xpub, key.key,
            key.origin.to_value());

    if (version_ == version_2)
    {
        write(sink, global_key::tx_version, {},
            system::to_chunk(to_little_endian(tx_version_)));

        if (fallback_locktime_.has_value())
            write(sink, global_key::fallback_locktime, {},
                system::to_chunk(to_little_endian(fallback_locktime_.value())));

        data_chunk ins{};
        stream::out::data ins_stream(ins);
        write::bytes::ostream ins_sink(ins_stream);
        ins_sink.write_variable(inputs_.size());
        ins_sink.flush();
        write(sink, global_key::input_count, {}, ins);

        data_chunk outs{};
        stream::out::data outs_stream(outs);
        write::bytes::ostream outs_sink(outs_stream);
        outs_sink.write_variable(outputs_.size());
        outs_sink.flush();
        write(sink, global_key::output_count, {}, outs);

        if (tx_modifiable_.has_value())
            write(sink, global_key::tx_modifiable, {},
                { tx_modifiable_.value() });

        write(sink, global_key::version, {},
            system::to_chunk(to_little_endian(version_)));
    }

    for (const auto& field: sorted(others_))
        field.to_data(sink);

    // The global map terminator.
    sink.write_byte(0);

    for (const auto& input: inputs_)
        input.to_data(sink, version_);

    for (const auto& output: outputs_)
        output.to_data(sink, version_);

    sink.flush();
    return out;
}

// Accessors.
// ----------------------------------------------------------------------------

uint32_t transaction::version() const NOEXCEPT
{
    return version_;
}

uint32_t transaction::tx_version() const NOEXCEPT
{
    return version_ == version_0 ? tx_.version() : tx_version_;
}

const std::optional<uint32_t>& transaction::fallback_locktime() const NOEXCEPT
{
    return fallback_locktime_;
}

const std::optional<uint8_t>& transaction::tx_modifiable() const NOEXCEPT
{
    return tx_modifiable_;
}

const chain::transaction& transaction::unsigned_tx() const NOEXCEPT
{
    return tx_;
}

const xpub::list& transaction::xpubs() const NOEXCEPT
{
    return xpubs_;
}

const input::list& transaction::inputs() const NOEXCEPT
{
    return inputs_;
}

const output::list& transaction::outputs() const NOEXCEPT
{
    return outputs_;
}

const entry::list& transaction::others() const NOEXCEPT
{
    return others_;
}

input::list& transaction::inputs() NOEXCEPT
{
    return inputs_;
}

output::list& transaction::outputs() NOEXCEPT
{
    return outputs_;
}

chain::output::cptr transaction::prevout(size_t input) const NOEXCEPT
{
    if (input >= inputs_.size())
        return {};

    const auto& in = inputs_.at(input);
    const auto index = (version_ == version_0) ?
        tx_.inputs_ptr()->at(input)->point().index() :
        in.output_index.value_or(0);

    const auto out = in.prevout(index);
    if (!out)
        return {};

    // A non-witness utxo must be the transaction of the input point.
    if (in.non_witness_utxo && !in.witness_utxo)
    {
        const auto& hash = (version_ == version_0) ?
            tx_.inputs_ptr()->at(input)->point().hash() :
            in.previous_txid.value_or(null_hash);

        if (in.non_witness_utxo->hash(false) != hash)
            return {};
    }

    return out;
}

std::optional<uint64_t> transaction::fee() const NOEXCEPT
{
    uint64_t in{};
    for (size_t index = 0; index < inputs_.size(); ++index)
    {
        const auto out = prevout(index);
        if (!out)
            return {};

        in += out->value();
    }

    uint64_t out{};
    if (version_ == version_0)
    {
        for (const auto& put: *tx_.outputs_ptr())
            out += put->value();
    }
    else
    {
        for (const auto& put: outputs_)
            out += put.amount.value_or(0);
    }

    if (out > in)
        return {};

    return in - out;
}

bool transaction::is_final() const NOEXCEPT
{
    const auto finalized = [](const auto& in) NOEXCEPT
    {
        return in.is_final();
    };

    return valid_ && !inputs_.empty() &&
        std::all_of(inputs_.begin(), inputs_.end(), finalized);
}

std::optional<uint32_t> transaction::locktime() const NOEXCEPT
{
    if (version_ == version_0)
        return tx_.locktime();

    // BIP370: the field available to all constrained inputs is chosen,
    // preferring height, and its value is the greatest requirement.
    uint32_t height{};
    uint32_t time{};
    auto constrained = false;
    auto heights = true;
    auto times = true;

    for (const auto& input: inputs_)
    {
        const auto in_height = input.required_height_locktime;
        const auto in_time = input.required_time_locktime;
        if (!in_height.has_value() && !in_time.has_value())
            continue;

        constrained = true;
        heights = heights && in_height.has_value();
        times = times && in_time.has_value();
        height = std::max(height, in_height.value_or(0));
        time = std::max(time, in_time.value_or(0));
    }

    if (!constrained)
        return fallback_locktime_.value_or(0);

    if (heights)
        return height;

    if (times)
        return time;

    return {};
}

// Methods.
// ----------------------------------------------------------------------------

bool transaction::combine(const transaction& other) NOEXCEPT
{
    if (!valid_ || !other.valid_ || version_ != other.version_ ||
        inputs_.size() != other.inputs_.size() ||
        outputs_.size() != other.outputs_.size())
        return false;

    if (version_ == version_0 && tx_ != other.tx_)
        return false;

    if (version_ == version_2)
    {
        if (tx_version_ != other.tx_version_)
            return false;

        for (size_t index = 0; index < inputs_.size(); ++index)
            if (inputs_.at(index).previous_txid !=
                other.inputs_.at(index).previous_txid ||
                inputs_.at(index).output_index !=
                other.inputs_.at(index).output_index)
                return false;
    }

    for (size_t index = 0; index < inputs_.size(); ++index)
        inputs_.at(index).combine(other.inputs_.at(index));

    for (size_t index = 0; index < outputs_.size(); ++index)
        outputs_.at(index).combine(other.outputs_.at(index));

    for (const auto& xpub: other.xpubs_)
        if (std::find(xpubs_.begin(), xpubs_.end(), xpub) == xpubs_.end())
            xpubs_.push_back(xpub);

    for (const auto& entry: other.others_)
        if (std::find(others_.begin(), others_.end(), entry) ==
            others_.end())
            others_.push_back(entry);

    if (!fallback_locktime_.has_value())
        fallback_locktime_ = other.fallback_locktime_;

    if (!tx_modifiable_.has_value())
        tx_modifiable_ = other.tx_modifiable_;

    return true;
}

bool transaction::join(const transaction& other) NOEXCEPT
{
    if (!valid_ || !other.valid_ || version_ != other.version_)
        return false;

    const auto point = [this](size_t index) NOEXCEPT
    {
        return (version_ == version_0) ?
            tx_.inputs_ptr()->at(index)->point() :
            chain::point{ inputs_.at(index).previous_txid.value_or(
                null_hash), inputs_.at(index).output_index.value_or(0) };
    };

    const auto other_point = [&other](size_t index) NOEXCEPT
    {
        return (other.version_ == transaction::version_0) ?
            other.tx_.inputs_ptr()->at(index)->point() :
            chain::point{ other.inputs_.at(index).previous_txid.value_or(
                null_hash), other.inputs_.at(index).output_index.value_or(
                    0) };
    };

    // Duplicated input points cannot be joined.
    for (size_t left = 0; left < inputs_.size(); ++left)
        for (size_t right = 0; right < other.inputs_.size(); ++right)
            if (point(left) == other_point(right))
                return false;

    if (version_ == version_0)
    {
        auto inputs = *tx_.inputs_ptr();
        auto outputs = *tx_.outputs_ptr();
        const auto& more_ins = *other.tx_.inputs_ptr();
        const auto& more_outs = *other.tx_.outputs_ptr();
        inputs.insert(inputs.end(), more_ins.begin(), more_ins.end());
        outputs.insert(outputs.end(), more_outs.begin(), more_outs.end());
        tx_ = { tx_.version(), to_shared<input_cptrs>(std::move(inputs)),
            to_shared<output_cptrs>(std::move(outputs)), tx_.locktime() };
    }

    inputs_.insert(inputs_.end(), other.inputs_.begin(),
        other.inputs_.end());
    outputs_.insert(outputs_.end(), other.outputs_.begin(),
        other.outputs_.end());
    return true;
}

// Finalize.
// ----------------------------------------------------------------------------

// The entry for the public key whose hash160 is the given short hash.
static entry::list::const_iterator find_entry_by_hash(
    const entry::list& signatures, const short_hash& hash) NOEXCEPT
{
    return std::find_if(signatures.begin(), signatures.end(),
        [&](const auto& item) NOEXCEPT
        {
            return bitcoin_short_hash(item.keydata()) == hash;
        });
}

// The entry for the given public key.
static entry::list::const_iterator find_entry_by_key(
    const entry::list& signatures, const data_chunk& key) NOEXCEPT
{
    return std::find_if(signatures.begin(), signatures.end(),
        [&](const auto& item) NOEXCEPT
        {
            return item.keydata() == key;
        });
}

// The satisfaction push data stack for supported script patterns.
static bool satisfy(data_stack& stack, const script& spend,
    const entry::list& signatures) NOEXCEPT
{
    const auto& ops = spend.ops();
    if (script::is_pay_public_key_pattern(ops))
    {
        const auto found = find_entry_by_key(signatures, ops.front().data());
        if (found == signatures.end())
            return false;

        stack.push_back(found->value);
        return true;
    }

    if (script::is_pay_key_hash_pattern(ops))
    {
        const short_hash hash = unsafe_array_cast<uint8_t, short_hash_size>(
            ops.at(2).data().data());
        const auto found = find_entry_by_hash(signatures, hash);
        if (found == signatures.end())
            return false;

        stack.push_back(found->value);
        stack.push_back(found->keydata());
        return true;
    }

    if (script::is_pay_multisig_pattern(ops))
    {
        const auto required = operation::opcode_to_positive(
            ops.front().code());

        // The op_check_multi_sig dummy element.
        stack.push_back(data_chunk{});

        // Signatures are ordered by public key position in the script.
        for (auto op = std::next(ops.begin());
            op != std::prev(ops.end(), 2); ++op)
        {
            const auto found = find_entry_by_key(signatures, op->data());
            if (found != signatures.end())
                stack.push_back(found->value);
        }

        return stack.size() == add1<size_t>(required);
    }

    return false;
}

static chain::script to_push_script(const data_stack& stack) NOEXCEPT
{
    operations ops{};
    for (const auto& data: stack)
        ops.emplace_back(data, false);

    return chain::script{ std::move(ops) };
}

bool transaction::finalize(input& in, uint32_t index) NOEXCEPT
{
    const auto out = prevout(index);
    if (!out)
        return false;

    auto spend = out->script();
    auto witnessed = false;

    // The spend conditions of a p2sh input are its embedded script.
    if (script::is_pay_script_hash_pattern(spend.ops()))
    {
        if (!in.embedded_script)
            return false;

        const short_hash hash = unsafe_array_cast<uint8_t, short_hash_size>(
            spend.ops().at(1).data().data());
        if (bitcoin_short_hash(in.embedded_script->to_data(false)) != hash)
            return false;

        spend = *in.embedded_script;
    }

    // The spend conditions of a witness program are its witness script.
    if (script::is_pay_witness_script_hash_pattern(spend.ops()))
    {
        if (!in.witness_script)
            return false;

        const hash_digest hash = unsafe_array_cast<uint8_t, hash_size>(
            spend.ops().at(1).data().data());
        if (sha256_hash(in.witness_script->to_data(false)) != hash)
            return false;

        witnessed = true;
        data_stack stack{};
        if (!satisfy(stack, *in.witness_script, in.partial_signatures))
            return false;

        stack.push_back(in.witness_script->to_data(false));
        in.final_script_witness = to_shared<chain::witness>(std::move(stack));
    }
    else if (script::is_pay_witness_key_hash_pattern(spend.ops()))
    {
        const short_hash hash = unsafe_array_cast<uint8_t, short_hash_size>(
            spend.ops().at(1).data().data());
        const auto found = find_entry_by_hash(in.partial_signatures, hash);
        if (found == in.partial_signatures.end())
            return false;

        witnessed = true;
        in.final_script_witness = to_shared<chain::witness>(
            data_stack{ found->value, found->keydata() });
    }
    else
    {
        data_stack stack{};
        if (!satisfy(stack, spend, in.partial_signatures))
            return false;

        in.final_script_sig = to_shared<chain::script>(to_push_script(stack));
    }

    // A p2sh wrapper places the embedded script push in the signature script.
    if (in.embedded_script && witnessed)
    {
        in.final_script_sig = to_shared<chain::script>(to_push_script(
            data_stack{ in.embedded_script->to_data(false) }));
    }
    else if (in.embedded_script && !witnessed)
    {
        auto ops = in.final_script_sig->ops();
        ops.emplace_back(in.embedded_script->to_data(false), false);
        in.final_script_sig = to_shared<chain::script>(std::move(ops));
    }

    // BIP174: the finalizer removes superseded fields.
    in.partial_signatures.clear();
    in.derivations.clear();
    in.sighash_type.reset();
    in.embedded_script.reset();
    in.witness_script.reset();
    return true;
}

bool transaction::finalize() NOEXCEPT
{
    if (!valid_ || inputs_.empty())
        return false;

    auto complete = true;
    for (uint32_t index = 0; index < inputs_.size(); ++index)
    {
        auto& in = inputs_.at(index);
        if (!in.is_final())
            complete &= finalize(in, index);
    }

    return complete;
}

// Extract.
// ----------------------------------------------------------------------------

chain::input::cptr transaction::extract_input(size_t index) const NOEXCEPT
{
    const auto& in = inputs_.at(index);
    const auto script_sig = in.final_script_sig ? *in.final_script_sig :
        script{};
    const auto script_witness = in.final_script_witness ?
        *in.final_script_witness : witness{};

    if (version_ == version_0)
    {
        const auto& original = *tx_.inputs_ptr()->at(index);
        return to_shared<chain::input>(original.point(), script_sig,
            script_witness, original.sequence());
    }

    const point prevout{ in.previous_txid.value_or(null_hash),
        in.output_index.value_or(0) };
    return to_shared<chain::input>(prevout, script_sig, script_witness,
        in.sequence.value_or(max_input_sequence));
}

chain::transaction transaction::extract() const NOEXCEPT
{
    if (!is_final())
        return {};

    const auto time = locktime();
    if (!time.has_value())
        return {};

    input_cptrs inputs{};
    for (size_t index = 0; index < inputs_.size(); ++index)
        inputs.push_back(extract_input(index));

    output_cptrs outputs{};
    if (version_ == version_0)
    {
        const auto& puts = *tx_.outputs_ptr();
        outputs.assign(puts.begin(), puts.end());
    }
    else
    {
        for (const auto& put: outputs_)
            outputs.push_back(to_shared<chain::output>(put.amount.value_or(0),
                put.script ? *put.script : script{}));
    }

    return { tx_version(), to_shared<input_cptrs>(std::move(inputs)),
        to_shared<output_cptrs>(std::move(outputs)), time.value() };
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin
