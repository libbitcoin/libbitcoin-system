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

constexpr auto point_size = chain::point::serialized_size();

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

// Signature hashing.
// ----------------------------------------------------------------------------

bool transaction_view::signature_hash(hash_digest& out,
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

uint32_t transaction_view::input_index(
    const input_iterator& input) const NOEXCEPT
{
    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_begin_, input));
}

bool transaction_view::output_overflow(size_t input) const NOEXCEPT
{
    return input >= out_count_;
}

void transaction_view::write_outputs(writer& sink) const NOEXCEPT
{
    sink.write_variable(out_count_);
    sink.write_bytes(at_outputs(), outputs_size());
}

// Unversioned.
// ----------------------------------------------------------------------------

void transaction_view::signature_hash_single(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = transaction::is_anyone_can_pay(sighash_flags);
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

void transaction_view::signature_hash_none(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = transaction::is_anyone_can_pay(sighash_flags);
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

void transaction_view::signature_hash_all(writer& sink,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &subscript, sighash_flags](
        writer& sink) NOEXCEPT
    {
        input_iterator in;
        const auto anyone = transaction::is_anyone_can_pay(sighash_flags);
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

void transaction_view::unversioned_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto flag = transaction::mask_sighash(sighash_flags);

    if (flag == coverage::hash_single && output_overflow(input_index(input)))
    {
        out = one_hash;
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

hash_digest transaction_view::output_hash_v0(
    const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);
    if (output_overflow(index))
        return null_hash;

    const auto output = output_at(index);
    return bitcoin_hash(output.serialized_size(), output.data());
}

void transaction_view::version0_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript, uint64_t value,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto flag = transaction::mask_sighash(sighash_flags);
    const auto anyone = transaction::is_anyone_can_pay(sighash_flags);
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

bool transaction_view::version1_sighash(hash_digest& out,
    const input_iterator& input, const script& script, uint64_t value,
    const hash_cptr& tapleaf, uint8_t sighash_flags) const NOEXCEPT
{
    constexpr uint8_t epoch{};
    const auto& in = *input;
    const auto& annex = in.witness().annex();

    const auto flag = transaction::mask_sighash(sighash_flags);
    const auto anyone = transaction::is_anyone_can_pay(sighash_flags);
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

    sink.write_byte(transaction::spend_type_v1(annex, !is_null(tapleaf)));

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
        sink.write_4_bytes_little_endian(transaction::subscript_v1(script));
    }

    sink.flush();
    return true;
}

// Signature hash caching (not thread safe).
// ----------------------------------------------------------------------------

hash_digest transaction_view::x1_base_hash_points() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
        sink.write_bytes(in->point_data(), point_size);

    sink.flush();
    return digest;
}

hash_digest transaction_view::x1_base_hash_sequences() const NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    for (auto in = inputs_begin_; in != inputs_end_; ++in)
        sink.write_4_bytes_little_endian(in->sequence());

    sink.flush();
    return digest;
}

hash_digest transaction_view::x1_base_hash_outputs() const NOEXCEPT
{
    return accumulator<sha256>::hash(outputs_size(), at_outputs());
}

// This requires ALL prevouts of the tx are populated (new in taproot).
hash_digest transaction_view::v1_only_hash_amounts() const NOEXCEPT
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
hash_digest transaction_view::v1_only_hash_scripts() const NOEXCEPT
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

void transaction_view::set_x1_base_hash() const NOEXCEPT
{
    if (!x1_base_cache_)
        x1_base_cache_ = std::make_shared<base_cache>
        (
            x1_base_hash_points(),
            x1_base_hash_sequences(),
            x1_base_hash_outputs()
        );
}

void transaction_view::set_x2_base_hash() const NOEXCEPT
{
    if (!x2_base_cache_)
        x2_base_cache_ = std::make_shared<base_cache>
        (
            sha256_hash(single_hash_points()),
            sha256_hash(single_hash_sequences()),
            sha256_hash(single_hash_outputs())
        );
}

void transaction_view::set_v1_only_hash() const NOEXCEPT
{
    if (!v1_only_cache_)
        v1_only_cache_ = std::make_shared<only_cache>
        (
            v1_only_hash_amounts(),
            v1_only_hash_scripts()
        );
}

BC_POP_WARNING()

const hash_digest& transaction_view::single_hash_points() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->points;
}

const hash_digest& transaction_view::single_hash_sequences() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->sequences;
}

const hash_digest& transaction_view::single_hash_outputs() const NOEXCEPT
{
    set_x1_base_hash();
    return x1_base_cache_->outputs;
}

const hash_digest& transaction_view::single_hash_amounts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->amounts;
}

const hash_digest& transaction_view::single_hash_scripts() const NOEXCEPT
{
    set_v1_only_hash();
    return v1_only_cache_->scripts;
}

const hash_digest& transaction_view::double_hash_points() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->points;
}

const hash_digest& transaction_view::double_hash_sequences() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->sequences;
}

const hash_digest& transaction_view::double_hash_outputs() const NOEXCEPT
{
    set_x2_base_hash();
    return x2_base_cache_->outputs;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
