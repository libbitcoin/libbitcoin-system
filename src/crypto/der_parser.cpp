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
#include <bitcoin/system/crypto/der_parser.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include "secp256k1/ec_context.hpp"

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)

using namespace bc;
using namespace bc::system;

static constexpr auto sequence_tag = 0x30_u8;
static constexpr auto integer_tag = 0x02_u8;
static constexpr auto long_form = 0x80_u8;
static constexpr auto scalar_size = to_half(ec_signature_size);

// Strict
// ----------------------------------------------------------------------------

// A positive integer in its shortest encoding.
static bool is_minimal_positive(const uint8_t* value, size_t size) NOEXCEPT
{
    if (is_zero(size) || is_nonzero(value[0] & long_form))
        return false;

    return !(size > one && is_zero(value[0]) &&
        is_zero(value[1] & long_form));
}

// 0x30 [size] 0x02 [r size] [r] 0x02 [s size] [s], without sighash type.
bool is_valid_bip66_signature_encoding(const data_slice& sig) NOEXCEPT
{
    constexpr size_t minimum = 8;
    constexpr size_t maximum = 72;

    const auto size = sig.size();
    if (size < minimum || size > maximum)
        return false;

    if (sig[0] != sequence_tag || sig[1] != size - 2u)
        return false;

    const size_t r_size = sig[3];
    if (5u + r_size >= size)
        return false;

    const size_t s_size = sig[5u + r_size];
    if (r_size + s_size + 6u != size)
        return false;

    const auto data = sig.data();
    return sig[2] == integer_tag
        && is_minimal_positive(&data[4], r_size)
        && sig[4u + r_size] == integer_tag
        && is_minimal_positive(&data[6u + r_size], s_size);
}

// Lax
// ----------------------------------------------------------------------------

// Locates a tagged integer and advances past it. Leading zeros of a long form
// length are ignored and the length is bounded by the remaining input.
static bool read_integer(size_t& start, size_t& size, size_t& position,
    const uint8_t* input, size_t length) NOEXCEPT
{
    if (position == length || input[position++] != integer_tag)
        return false;

    if (position == length)
        return false;

    size_t bytes = input[position++];
    if (is_zero(bytes & long_form))
    {
        size = bytes;
    }
    else
    {
        bytes -= long_form;
        if (bytes > length - position)
            return false;

        for (; is_nonzero(bytes) && is_zero(input[position]); --bytes)
            ++position;

        if (bytes >= sizeof(size_t))
            return false;

        for (size = zero; is_nonzero(bytes); --bytes)
            size = (size << byte_bits) + input[position++];
    }

    if (size > length - position)
        return false;

    start = position;
    position += size;
    return true;
}

// Right-aligns the integer, without leading zeros, into a scalar.
static bool copy_scalar(uint8_t* scalar, const uint8_t* input, size_t start,
    size_t size) NOEXCEPT
{
    for (; is_nonzero(size) && is_zero(input[start]); --size)
        ++start;

    if (size > scalar_size)
        return false;

    std::copy_n(&input[start], size, &scalar[scalar_size - size]);
    return true;
}

// The signature is set to a valid encoding of an invalid signature whenever
// parsing fails or a scalar is out of range.
bool ecdsa_signature_parse_der_lax(ec_signature& out, const uint8_t* input,
    size_t inputlen) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    const auto signature = pointer_cast<secp256k1_ecdsa_signature>(
        out.data());

    ec_signature compact{};
    secp256k1_ecdsa_signature_parse_compact(context, signature,
        compact.data());

    size_t position{};
    if (position == inputlen || input[position++] != sequence_tag)
        return false;

    if (position == inputlen)
        return false;

    // The sequence length is skipped, not validated.
    size_t bytes = input[position++];
    if (is_nonzero(bytes & long_form))
    {
        bytes -= long_form;
        if (bytes > inputlen - position)
            return false;

        position += bytes;
    }

    size_t r_start{}, r_size{}, s_start{}, s_size{};
    if (!read_integer(r_start, r_size, position, input, inputlen) ||
        !read_integer(s_start, s_size, position, input, inputlen))
        return false;

    const auto r = copy_scalar(&compact[0], input, r_start, r_size);
    const auto s = copy_scalar(&compact[scalar_size], input, s_start, s_size);

    if (!r || !s || is_zero(secp256k1_ecdsa_signature_parse_compact(context,
        signature, compact.data())))
    {
        compact.fill(0);
        secp256k1_ecdsa_signature_parse_compact(context, signature,
            compact.data());
    }

    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
