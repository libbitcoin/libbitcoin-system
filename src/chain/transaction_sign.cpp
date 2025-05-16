/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <iterator>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/extension.hpp>
#include <bitcoin/system/chain/enums/key_version.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// All private methods pertaining to the validation of a signature.
// There are three versions of signature hashing and verification implemented.
// Version: (unversioned) original, (0) bip143/segwit, (1) bip341/taproot.

// Share fixed elements of signature hashing.
// ----------------------------------------------------------------------------

constexpr auto prefixed = true;

static const auto& null_output() NOEXCEPT
{
    static const auto null = output{}.to_data();
    return null;
}

static const auto& empty_script() NOEXCEPT
{
    static const auto empty = script{}.to_data(prefixed);
    return empty;
}

static const auto& zero_sequence() NOEXCEPT
{
    static const auto sequence = to_little_endian<uint32_t>(0);
    return sequence;
}

// Compute signature hash subcomponents.
// ----------------------------------------------------------------------------

hash_digest transaction::outputs_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->outputs;

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& output: *outputs_)
        output->to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::points_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->points;

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& input: *inputs_)
        input->point().to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::sequences_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->sequences;

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    for (const auto& input: *inputs_)
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return digest;
}

// Signature hashing (unversioned).
// ----------------------------------------------------------------------------

transaction::input_iterator transaction::input_at(
    uint32_t index) const NOEXCEPT
{
    // Guarded by check_signature and create_endorsement.
    BC_ASSERT_MSG(index < inputs_->size(), "invalid input index");

    return std::next(inputs_->begin(), index);
}

uint32_t transaction::input_index(const input_iterator& input) const NOEXCEPT
{
    // Guarded by unversioned_sighash and output_hash.
    BC_ASSERT_MSG(inputs_->begin() != inputs_->end(), "invalid input iterator");

    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_->begin(), input));
}

// static
//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline coverage transaction::mask_sighash(uint8_t sighash_flags) NOEXCEPT
{
    switch (bit_and<uint8_t>(sighash_flags, coverage::mask))
    {
        case coverage::hash_single:
            return coverage::hash_single;
        case coverage::hash_none:
            return coverage::hash_none;
        default:
            return coverage::hash_all;
    }
}

inline bool transaction::is_anyone_can_pay(uint8_t sighash_flags) NOEXCEPT
{
    return to_bool(bit_and<uint8_t>(sighash_flags, coverage::anyone_can_pay));
}

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
        subscript.to_data(sink, prefixed);
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

        for (size_t output = 0; output < index; ++output)
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
        subscript.to_data(sink, prefixed);
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
        subscript.to_data(sink, prefixed);
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

// private
bool transaction::unversioned_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Mask anyone_can_pay and unused bits, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);

    //*************************************************************************
    // CONSENSUS: return one_hash if index exceeds outputs in sighash.
    // Related Bug: bitcointalk.org/index.php?topic=260595
    // Exploit: joncave.co.uk/2014/08/bitcoin-sighash-single/
    //*************************************************************************
    if (flag == coverage::hash_single &&
        input_index(input) >= outputs_->size())
    {
        out = one_hash;
        return true;
    }

    // Create hash writer.
    stream::out::fast stream{ out };
    hash::sha256x2::fast sink{ stream };

    switch (flag)
    {
        case coverage::hash_single:
        {
            signature_hash_single(sink, input, subscript, sighash_flags);
            break;
        }
        case coverage::hash_none:
        {
            signature_hash_none(sink, input, subscript, sighash_flags);
            break;
        }
        default:
        case coverage::hash_all:
        {
            signature_hash_all(sink, input, subscript, sighash_flags);
        }
    }

    sink.flush();
    return true;
}

// Signature hashing (version 0 - segwit).
// ----------------------------------------------------------------------------

// TODO: taproot requires both single and double hash of each.
void transaction::initialize_sighash_cache() const NOEXCEPT
{
    if (!segregated_)
        return;

    // This overconstructs the cache (anyone or !all), however it is simple.
    sighash_cache_ =
    {
        outputs_hash(),
        points_hash(),
        sequences_hash()
    };
}

hash_digest transaction::output_hash_v0(
    const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);

    //*************************************************************************
    // CONSENSUS: if index exceeds outputs in signature hash, return null_hash.
    //*************************************************************************
    if (index >= outputs_->size())
        return null_hash;

    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    outputs_->at(index)->to_data(sink);
    sink.flush();
    return digest;
}

// bip143
bool transaction::version_0_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript, uint64_t value,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Mask anyone_can_pay and unused bits, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);
    const auto anyone = is_anyone_can_pay(sighash_flags);
    const auto single = (flag == coverage::hash_single);
    const auto all = (flag == coverage::hash_all);

    // Create hash writer.
    stream::out::fast stream{ out };
    hash::sha256x2::fast sink{ stream };

    // Create signature hash.
    sink.write_4_bytes_little_endian(version_);

    // Conditioning points, sequences, and outputs writes on cache_ instead of
    // conditionally passing them from methods avoids copying the cached hash.

    // points
    sink.write_bytes(!anyone ? points_hash() : null_hash);

    // sequences
    sink.write_bytes(!anyone && all ? sequences_hash() : null_hash);

    (*input)->point().to_data(sink);
    subscript.to_data(sink, prefixed);
    sink.write_8_bytes_little_endian(value);
    sink.write_4_bytes_little_endian((*input)->sequence());

    // outputs
    if (single)
        sink.write_bytes(output_hash_v0(input));
    else
        sink.write_bytes(all ? outputs_hash() : null_hash);

    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);

    sink.flush();
    return true;
}

// Signature hashing (version 1 - taproot).
// ----------------------------------------------------------------------------

bool transaction::output_hash_v1(hash_digest& out,
    const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);

    if (index >= outputs_->size())
        return false;

    stream::out::fast stream{ out };
    hash::sha256::fast sink{ stream };
    outputs_->at(index)->to_data(sink);
    sink.flush();
    return true;
}

bool transaction::version_1_sighash(hash_digest& out,
    const input_iterator& input, const script& script, uint64_t value,
    uint8_t sighash_flags) const NOEXCEPT
{
    // sighash_flags previously verified (see schnorr_split).
    // Mask anyone_can_pay, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);
    const auto anyone = is_anyone_can_pay(sighash_flags);
    const auto single = (flag == coverage::hash_single);
    const auto all = (flag == coverage::hash_all);

    // TODO: compute/cache these.
    constexpr hash_digest hash1_prevouts{};
    constexpr hash_digest hash1_amounts{};
    constexpr hash_digest hash1_scripts{};
    constexpr hash_digest hash1_sequences{};
    constexpr hash_digest hash1_outputs{};

    // TODO: derive bool tapscript from extension flag value.
    constexpr uint8_t ext_flag{ to_value(extension::tapscript) };
    constexpr bool tapscript{ ext_flag == to_value(extension::tapscript) };

    // Signature hash is prefixed with 0x00, called the epoch.
    constexpr uint8_t epoch{ 0 };

    const auto& in = **input;
    const auto& witness = in.witness();
    const auto has_annex = witness.is_annex_pattern();

    // Create tagged hash writer.
    stream::out::fast stream{ out };
    hash::sha256t::fast<"TapSighash"> sink{ stream };

    // Create signature hash.
    sink.write_byte(epoch);
    sink.write_byte(sighash_flags);
    sink.write_4_bytes_little_endian(version_);
    sink.write_4_bytes_little_endian(locktime_);

    if (!anyone)
    {
        sink.write_bytes(hash1_prevouts);
        sink.write_bytes(hash1_amounts);
        sink.write_bytes(hash1_scripts);
        sink.write_bytes(hash1_sequences);
    }

    if (all)
    {
        sink.write_bytes(hash1_outputs);
    }

    // Extension and annex flag are combined into a single byte. It might have
    // been simpler to just use independent bytes and not waste one below.
    const auto spend_type = set_right(shift_left(ext_flag), 0, has_annex);
    sink.write_byte(spend_type);

    if (anyone)
    {
        // ********************************************************************
        // CONSENSUS: Script is always 35 bytes, and the script is 2 opcodes
        // and 32 bytes: `<1> <32-byte-tweaked-public-key>` so an unnecessary
        // size prefix is implied in prevout script sighash serialization.
        // ********************************************************************
        in.point().to_data(sink);
        sink.write_8_bytes_little_endian(value);
        script.to_data(sink, true);
        sink.write_4_bytes_little_endian(in.sequence());
    }
    else
    {
        sink.write_4_bytes_little_endian(input_index(input));
    }

    if (has_annex)
    {
        const auto& annex = *witness.stack().back();

        // The sha256 of (compact_size(size of annex) || prefixed annex).
        hash::sha256::fast annexer{ stream };
        annexer.write_variable(annex.size());
        annexer.write_bytes(annex);
        annexer.flush();
    }

    // TODO: can hash_output be cached?
    if (single)
    {
        hash_digest hash_output{};
        if (!output_hash_v1(hash_output, input))
            return false;

        sink.write_bytes(hash_output);
    }

    // Total message length is at most 206 bytes (!anyone, no epoch) [bip341].
    BC_ASSERT(!anyone && sink.get_write_position() == add1(206u));
    BC_ASSERT( anyone && sink.get_write_position() == add1(157u));

    // Additional for TAPSCRIPT [bip342].
    // ========================================================================

    if (tapscript)
    {
        // TODO:
        // The tapleaf hash (as defined in BIP341).
        constexpr hash_digest tapleaf_hash{};

        // TODO:
        // The opcode position of last executed op_codeseparator before the
        // currently executing signature opcode. Because code_separator is the
        // last input to signature hash, the signature hash sha256 midstate
        // (above) can be cached for multiple op_codeseparators.
        constexpr uint32_t code_separator{ 0 };

        sink.write_bytes(tapleaf_hash);
        sink.write_byte(to_value(key_version::tapscript));
        sink.write_4_bytes_little_endian(code_separator);
    }

    sink.flush();
    return true;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
