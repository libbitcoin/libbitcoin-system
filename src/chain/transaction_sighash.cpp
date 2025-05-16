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

// Signature hashing (shared).
// ----------------------------------------------------------------------------

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
coverage transaction::mask_sighash(uint8_t sighash_flags) NOEXCEPT
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

bool transaction::is_anyone_can_pay(uint8_t sighash_flags) NOEXCEPT
{
    return to_bool(bit_and<uint8_t>(sighash_flags, coverage::anyone_can_pay));
}

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

//*****************************************************************************
// CONSENSUS: if index exceeds outputs in signature hash, return one_hash.
// Related Bug: bitcointalk.org/index.php?topic=260595
// Exploit: joncave.co.uk/2014/08/bitcoin-sighash-single/
//*****************************************************************************
bool transaction::invalid_output_hash(coverage flag,
    const input_iterator& input) const NOEXCEPT
{
    return flag == coverage::hash_single && 
        input_index(input) >= outputs_->size();
}

bool transaction::unversioned_sighash(hash_digest& out,
    const input_iterator& input, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Mask anyone_can_pay and unused bits, and set hash_all by default.
    const auto flag = mask_sighash(sighash_flags);
    if (invalid_output_hash(flag, input))
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

// Dispatch.
// ----------------------------------------------------------------------------

// There are three versions of signature hashing and verification.
// Version: (unversioned) original, (v0) bip143/segwit, (v1) bip341/taproot.

bool transaction::signature_hash(hash_digest& out, const input_iterator& input,
    const script& subscript, uint64_t value, uint8_t sighash_flags,
    script_version version, bool bip143, bool bip342) const NOEXCEPT
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BC_ASSERT(!is_coinbase());

    // This is where the connection between bip141 and bip143 is made. If a
    // versioned 1 program (segwit) extracted by bip141 but bip143 (segwit
    // hashing) is not active, then drop down to unversioned signature hashing.
    if (bip143 && version == script_version::segwit)
        return version0_sighash(out, input, subscript, value, sighash_flags);

    // This is where the connection between bip341 and bip342 is made. If a
    // version 2 program (taproot) extracted by bip341 but bip342 (tapscript)
    // is not active then drop down to unversioned signature hashing. 
    if (bip342 && version == script_version::taproot)
        return version1_sighash(out, input, subscript, value, sighash_flags);

    // Given above forks are documented to activate together, this distinction
    // is moot, however these are distinct BIPs and therefore must be either be
    // differentiated as such in code, or the BIP distiction would be ignored.
    return unversioned_sighash(out, input, subscript, sighash_flags);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
