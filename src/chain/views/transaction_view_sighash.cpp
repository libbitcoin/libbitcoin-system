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
#include <bitcoin/system/chain/views/transaction_view.hpp>

#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/key_version.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/views/input_view.hpp>
#include <bitcoin/system/chain/views/output_view.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
namespace view {

constexpr auto point_size = chain::point::serialized_size();

// Point, empty script, and sequence of a non-signed input in hash_all.
constexpr auto blank_input_size = point_size + sizeof(uint8_t) +
    sizeof(uint32_t);

static const auto& null_output() NOEXCEPT
{
    static const auto null = chain::output{}.to_data();
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

// Signature hashing.
// ----------------------------------------------------------------------------

bool transaction::signature_hash(hash_digest& out,
    const input_iterator& input, const script& subscript, uint64_t value,
    const hash_cptr& tapleaf, script_version version, uint8_t sighash_flags,
    uint32_t flags) const NOEXCEPT
{
    BC_ASSERT(!coinbase_ && populated_);

    const auto bip143 = script::is_enabled(flags, flags::bip143_rule);
    const auto bip342 = script::is_enabled(flags, flags::bip342_rule);

    if (bip143 && version == script_version::segwit)
    {
        version0_sighash(out, input, subscript, value, sighash_flags);
        return true;
    }

    if (bip342 && version == script_version::taproot)
    {
        return version1_sighash(out, input, subscript, value, tapleaf,
            sighash_flags);
    }

    unversioned_sighash(out, input, subscript, sighash_flags);
    return true;
}

// private
// ----------------------------------------------------------------------------

uint32_t transaction::input_index(
    const input_iterator& input) const NOEXCEPT
{
    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_begin_, input));
}

bool transaction::output_overflow(size_t input) const NOEXCEPT
{
    return input >= out_count_;
}

void transaction::write_outputs(writer& sink) const NOEXCEPT
{
    sink.write_variable(out_count_);
    sink.write_bytes(at_outputs(), outputs_size());
}

// Unversioned.
// ----------------------------------------------------------------------------

void transaction::signature_hash_single(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = chain::transaction::is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : in_count_);

        for (in = inputs_begin_; !anyone && in != input; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        sink.write_bytes(input->point_data(), point_size);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian(input->sequence());

        for (++in; !anyone && in != inputs_end_; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
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
        output_at(index).to_data(sink);
    };

    sink.write_4_bytes_little_endian(version());
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime());
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_none(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = chain::transaction::is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : in_count_);

        for (in = inputs_begin_; !anyone && in != input; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        sink.write_bytes(input->point_data(), point_size);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian(input->sequence());

        for (++in; !anyone && in != inputs_end_; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }
    };

    sink.write_4_bytes_little_endian(version());
    write_inputs(sink);
    sink.write_variable(zero);
    sink.write_4_bytes_little_endian(locktime());
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_all(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = chain::transaction::is_anyone_can_pay(sighash_flags);
        sink.write_variable(anyone ? one : in_count_);

        for (in = inputs_begin_; !anyone && in != input; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian(in->sequence());
        }

        sink.write_bytes(input->point_data(), point_size);
        subscript.to_data(sink, true);
        sink.write_4_bytes_little_endian(input->sequence());

        for (++in; !anyone && in != inputs_end_; ++in)
        {
            sink.write_bytes(in->point_data(), point_size);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian(in->sequence());
        }
    };

    sink.write_4_bytes_little_endian(version());
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime());
    sink.write_4_bytes_little_endian(sighash_flags);
}

// Equivalent to signature_hash_all(writer&) when !anyone_can_pay, but hashes
// only preimage bytes following the last whole block preceding signed input.
void transaction::signature_hash_all(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    BC_ASSERT(!chain::transaction::is_anyone_can_pay(sighash_flags));
    constexpr auto block_size = array_count<sha256::block_t>;
    constexpr auto sequence_offset = point_size + sizeof(uint8_t);

    set_x0_base_cache();
    const auto& inputs = x0_base_cache_->inputs;
    const auto preamble = sizeof(uint32_t) + variable_size(in_count_);
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
    context.write(outputs_size(), at_outputs());
    context.write(to_little_endian<uint32_t>(locktime()));
    context.write(to_little_endian<uint32_t>(sighash_flags));
    context.double_flush(out);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

void transaction::unversioned_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto flag = chain::transaction::mask_sighash(sighash_flags);

    if (flag == coverage::hash_single && output_overflow(input_index(input)))
    {
        out = one_hash;
        return;
    }

    // Default (hash_all) without anyone_can_pay is cached (sink not created).
    // The cache is not reused by a single input tx (excluding multisig).
    const auto all = flag != coverage::hash_single &&
        flag != coverage::hash_none;

    if (all && !chain::transaction::is_anyone_can_pay(sighash_flags) &&
        !is_one(in_count_))
    {
        signature_hash_all(out, input, subscript, sighash_flags);
        return;
    }

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

// Version 0 (segwit).
// ----------------------------------------------------------------------------

hash_digest transaction::output_hash_v0(
    const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);
    if (output_overflow(index))
        return null_hash;

    const auto output = output_at(index);
    return bitcoin_hash(output.serialized_size(), output.data());
}

void transaction::version0_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript, uint64_t value,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto flag = chain::transaction::mask_sighash(sighash_flags);
    const auto anyone = chain::transaction::is_anyone_can_pay(sighash_flags);
    const auto single = (flag == coverage::hash_single);
    const auto all = (flag == coverage::hash_all);

    stream::out::fast stream{ out };
    hash::sha256x2::fast sink{ stream };

    sink.write_4_bytes_little_endian(version());
    sink.write_bytes(!anyone ? double_hash_points() : null_hash);
    sink.write_bytes(!anyone && all ? double_hash_sequences() : null_hash);

    sink.write_bytes(input->point_data(), point_size);
    subscript.to_data(sink, true);
    sink.write_8_bytes_little_endian(value);
    sink.write_4_bytes_little_endian(input->sequence());

    if (single)
        sink.write_bytes(output_hash_v0(input));
    else
        sink.write_bytes(all ? double_hash_outputs() : null_hash);

    sink.write_4_bytes_little_endian(locktime());
    sink.write_4_bytes_little_endian(sighash_flags);

    sink.flush();
}

// Version 1 (taproot).
// ----------------------------------------------------------------------------

bool transaction::version1_sighash(hash_digest& out,
    const input_iterator& input, const script& script, uint64_t value,
    const hash_cptr& tapleaf, uint8_t sighash_flags) const NOEXCEPT
{
    constexpr uint8_t epoch{};
    const auto& in = *input;
    const auto& annex = in.witness().annex();

    const auto flag = chain::transaction::mask_sighash(sighash_flags);
    const auto anyone = chain::transaction::is_anyone_can_pay(sighash_flags);
    const auto single = (flag == coverage::hash_single);
    const auto all = (flag == coverage::hash_all);

    if (anyone && is_null(in.prevout))
        return false;

    if (single && output_overflow(input_index(input)))
        return false;

    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapSighash"> sink{ stream };

    sink.write_byte(epoch);
    sink.write_byte(sighash_flags);
    sink.write_4_bytes_little_endian(version());
    sink.write_4_bytes_little_endian(locktime());

    if (!anyone)
    {
        sink.write_bytes(single_hash_points());
        sink.write_bytes(single_hash_amounts());
        sink.write_bytes(single_hash_scripts());
        sink.write_bytes(single_hash_sequences());
    }

    if (all)
    {
        sink.write_bytes(single_hash_outputs());
    }

    sink.write_byte(chain::transaction::spend_type_v1(annex, !is_null(tapleaf)));

    if (anyone)
    {
        sink.write_bytes(in.point_data(), point_size);
        sink.write_8_bytes_little_endian(value);
        sink.write_variable(in.prevout->script_size());
        sink.write_bytes(in.prevout->script_data());
        sink.write_4_bytes_little_endian(in.sequence());
    }
    else
    {
        sink.write_4_bytes_little_endian(input_index(input));
    }

    if (annex)
    {
        sink.write_bytes(annex.hash(true));
    }

    if (single)
    {
        sink.write_bytes(output_at(input_index(input)).hash());
    }

    if (tapleaf)
    {
        sink.write_bytes(*tapleaf);
        sink.write_byte(to_value(key_version::tapscript));
        sink.write_4_bytes_little_endian(chain::transaction::subscript_v1(script));
    }

    sink.flush();
    return true;
}

// Signature hash caching (not thread safe).
// ----------------------------------------------------------------------------

// Cached hash_all preimage, independent of signed input and its subscript.
// For each input of an n input tx the uncached preimage serializes and hashes
// all n inputs and all outputs, which is quadratic in inputs. This caches the
// serialized preimage of inputs once per tx, and the sha256 context at each
// whole block of it, from which the hash of the preimage up to the signed
// input continues. Outputs are contiguous in the tx buffer so are not copied.
void transaction::set_x0_base_cache() const NOEXCEPT
{
    if (x0_base_cache_)
        return;

    constexpr auto block_size = array_count<sha256::block_t>;
    const auto cache = std::make_shared<x0_base_cache>();

    // Size of inputs part is guarded by is_oversized (as is the tx).
    const auto preamble = sizeof(uint32_t) + variable_size(in_count_);
    const auto blanks = ceilinged_multiply(in_count_, blank_input_size);
    const auto inputs = ceilinged_add(preamble, blanks);
    cache->inputs.resize(ceilinged_add(inputs, variable_size(out_count_)));

    stream::out::fast stream{ cache->inputs };
    write::bytes::fast sink{ stream };
    sink.write_4_bytes_little_endian(version());
    sink.write_variable(in_count_);
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
    {
        sink.write_bytes(in->point_data(), point_size);
        sink.write_bytes(empty_script());
        sink.write_4_bytes_little_endian(in->sequence());
    }

    sink.write_variable(out_count_);

    // Context at each whole block of inputs part, including the initial.
    // Hashing is not inlined here (accumulator), as an inline vector copy of
    // state (dirty upper state) preceding legacy sse sha-ni is very costly.
    const auto blocks = inputs / block_size;
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

hash_digest transaction::x1_base_hash_points() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
        sink.write_bytes(in->point_data(), point_size);

    sink.flush();
    return digest;
}

hash_digest transaction::x1_base_hash_sequences() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
        sink.write_4_bytes_little_endian(in->sequence());

    sink.flush();
    return digest;
}

hash_digest transaction::x1_base_hash_outputs() const NOEXCEPT
{
    return accumulator<sha256>::hash(outputs_size(), at_outputs());
}

// This requires ALL prevouts of the tx are populated (new in taproot).
hash_digest transaction::v1_only_hash_amounts() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
        sink.write_8_bytes_little_endian(in->prevout->value());

    sink.flush();
    return digest;
}

// This requires ALL prevouts of the tx are populated (new in taproot).
hash_digest transaction::v1_only_hash_scripts() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
    {
        sink.write_variable(in->prevout->script_size());
        sink.write_bytes(in->prevout->script_data());
    }

    sink.flush();
    return digest;
}

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

void transaction::set_x1_base_hash() const NOEXCEPT
{
    if (!x1_base_cache_)
        x1_base_cache_ = std::make_shared<base_cache>
        (
            x1_base_hash_points(),
            x1_base_hash_sequences(),
            x1_base_hash_outputs()
        );
}

void transaction::set_x2_base_hash() const NOEXCEPT
{
    if (!x2_base_cache_)
        x2_base_cache_ = std::make_shared<base_cache>
        (
            sha256_hash(single_hash_points()),
            sha256_hash(single_hash_sequences()),
            sha256_hash(single_hash_outputs())
        );
}

void transaction::set_v1_only_hash() const NOEXCEPT
{
    if (!v1_only_cache_)
        v1_only_cache_ = std::make_shared<only_cache>
        (
            v1_only_hash_amounts(),
            v1_only_hash_scripts()
        );
}

BC_POP_WARNING()

const hash_digest& transaction::single_hash_points() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->points;
}

const hash_digest& transaction::single_hash_sequences() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->sequences;
}

const hash_digest& transaction::single_hash_outputs() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->outputs;
}

const hash_digest& transaction::single_hash_amounts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->amounts;
}

const hash_digest& transaction::single_hash_scripts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->scripts;
}

const hash_digest& transaction::double_hash_points() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->points;
}

const hash_digest& transaction::double_hash_sequences() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->sequences;
}

const hash_digest& transaction::double_hash_outputs() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->outputs;
}

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin
