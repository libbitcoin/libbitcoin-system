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
#include <bitcoin/system/chain/transaction.hpp>

#include <numeric>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Signature hashing (unversioned).
// ----------------------------------------------------------------------------

static const auto& null_output() NOEXCEPT
{
    static const auto null = output{}.to_data();
    return null;
}

static const auto& empty_script() NOEXCEPT
{
    static const auto empty = script{}.to_data(true);
    return empty;
}

static const auto& zero_sequence() NOEXCEPT
{
    static const auto sequence = to_little_endian<uint32_t>(0);
    return sequence;
}

// Point, empty script, and sequence of a non-signed input in hash_all.
static constexpr auto blank_input_size = point::serialized_size() +
    sizeof(uint8_t) + sizeof(uint32_t);

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************

void transaction::signature_hash_single(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_cptrs::const_iterator in;
        const auto anyone = is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        (*input)->point().to_data(sink);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian((*input)->sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }
    };

    const auto write_outputs = [this, &input](writer& sink) NOEXCEPT
    {
        const auto index = input_index(input);
        sink.write_variable(add1(index));

        for (size_t output{}; output < index; ++output)
            sink.write_bytes(null_output());

        // Guarded by unversioned_sighash().
        outputs_->at(index)->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_none(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_cptrs::const_iterator in;
        const auto anyone = is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        (*input)->point().to_data(sink);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian((*input)->sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    sink.write_variable(zero);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_all(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_cptrs::const_iterator in;
        const auto anyone = is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian((*in)->sequence());
        }

        (*input)->point().to_data(sink);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian((*input)->sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian((*in)->sequence());
        }
    };

    const auto write_outputs = [this](writer& sink) NOEXCEPT
    {
        sink.write_variable(outputs_->size());
        for (const auto& output: *outputs_)
            output->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);
}

// Cached hash_all preimage, independent of signed input and its subscript.
// For each input of an n input tx the uncached preimage serializes and hashes
// all n inputs and all outputs, which is quadratic in inputs. This caches the
// serialized preimage parts once per tx, and the sha256 context at each whole
// block of the inputs part, from which the hash of the preimage up to the
// signed input continues. This is not thread safe until the cache is set.
void transaction::set_x0_base_cache() const NOEXCEPT
{
    if (x0_base_cache_)
        return;

    constexpr auto block_size = array_count<sha256::block_t>;
    const auto cache = std::make_shared<x0_base_cache>();

    // Size of inputs part is guarded by is_oversized (as is the tx).
    const auto ins = inputs_->size();
    cache->inputs.resize(ceilinged_add(sizeof(version_) + variable_size(ins),
        ceilinged_multiply(ins, blank_input_size)));

    stream::out::fast in_stream{ cache->inputs };
    write::bytes::fast in_sink{ in_stream };
    in_sink.write_4_bytes_little_endian(version_);
    in_sink.write_variable(ins);
    for (const auto& input: *inputs_)
    {
        input->point().to_data(in_sink);
        in_sink.write_bytes(empty_script());
        in_sink.write_4_bytes_little_endian(input->sequence());
    }

    const auto outs = [](size_t total, const auto& output) NOEXCEPT
    {
        return ceilinged_add(total, output->serialized_size());
    };

    cache->outputs.resize(ceilinged_add(variable_size(outputs_->size()) +
        sizeof(locktime_), std::accumulate(outputs_->begin(),
            outputs_->end(), zero, outs)));

    stream::out::fast out_stream{ cache->outputs };
    write::bytes::fast out_sink{ out_stream };
    out_sink.write_variable(outputs_->size());
    for (const auto& output: *outputs_)
        output->to_data(out_sink);

    out_sink.write_4_bytes_little_endian(locktime_);

    // Context at each whole block of inputs part, including the initial.
    // Hashing is not inlined here (accumulator), as an inline vector copy of
    // state (dirty upper state) preceding legacy sse sha-ni is very costly.
    const auto blocks = cache->inputs.size() / block_size;
    accumulator<sha256> context{};
    cache->contexts.reserve(add1(blocks));
    cache->contexts.push_back(context);
    auto block = cache->inputs.data();

    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    for (size_t index{}; index < blocks; ++index, block += block_size)
    {
        context.write(block_size, block);
        cache->contexts.push_back(context);
    }
    BC_POP_WARNING()

    x0_base_cache_ = cache;
}

// Equivalent to signature_hash_all(writer&) when !anyone_can_pay, but hashes
// only preimage bytes following the last whole block preceding signed input.
void transaction::signature_hash_all(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    BC_ASSERT(!is_anyone_can_pay(sighash_flags));
    constexpr auto block_size = array_count<sha256::block_t>;
    constexpr auto point_size = point::serialized_size();
    constexpr auto sequence_offset = point_size + sizeof(uint8_t);

    set_x0_base_cache();
    const auto& inputs = x0_base_cache_->inputs;
    const auto preamble = sizeof(version_) + variable_size(inputs_->size());
    const auto signed_input = preamble + input_index(input) * blank_input_size;
    const auto blocks = signed_input / block_size;
    const auto data = inputs.data();

    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    auto context = x0_base_cache_->contexts[blocks];
    context.write(signed_input - blocks * block_size,
        data + blocks * block_size);
    context.write(point_size, data + signed_input);
    context.write(subscript.to_data(true));
    context.write(sizeof(uint32_t), data + signed_input + sequence_offset);
    context.write(inputs.size() - (signed_input + blank_input_size),
        data + signed_input + blank_input_size);
    context.write(x0_base_cache_->outputs);
    context.write(to_little_endian<uint32_t>(sighash_flags));
    context.double_flush(out);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

void transaction::unversioned_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Mask anyone_can_pay and unused bits, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);

    if (flag == coverage::hash_single && output_overflow(input_index(input)))
    {
        out = one_hash;
        return;
    }

    // Default (hash_all) without anyone_can_pay is cached (sink not created).
    // The cache is not reused by a single input tx (excluding multisig).
    const auto all = flag != coverage::hash_single &&
        flag != coverage::hash_none;

    if (all && !is_anyone_can_pay(sighash_flags) && !is_one(inputs_->size()))
    {
        signature_hash_all(out, input, subscript, sighash_flags);
        return;
    }

    // Create hash writer.
    stream::out::fast stream{ out };
    hash::sha256x2::fast sink{ stream };

    switch (flag)
    {
        case coverage::hash_single:
            signature_hash_single(sink, input, subscript, sighash_flags);
            break;
        case coverage::hash_none:
            signature_hash_none(sink, input, subscript, sighash_flags);
            break;
        default:
        case coverage::hash_all:
            signature_hash_all(sink, input, subscript, sighash_flags);
    }

    sink.flush();
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
