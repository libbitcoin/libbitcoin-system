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
#include <bitcoin/system/crypto/poly1305.hpp>

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8439
// [Nir, Langley]
// and poly1305-donna (32 bit) [Floodyberry], public domain.

namespace libbitcoin {
namespace system {
    
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// The prime modulus is 2^130 - delta, with the accumulator (and r) held as
// five 26 bit limbs, so limbs mask to 26 bits and carry out of the top limb
// is folded back into the bottom, scaled by delta.
constexpr auto limb_bits = 26_size;
constexpr auto limb_mask = 0x03ffffff_u32;
constexpr auto limb_bit = 0x04000000_u32;
constexpr auto delta = 5_u32;

// The block high bit (2^128) as positioned within the fifth limb.
constexpr auto high_bit = 0x01000000_u32;

// The shift that isolates the sign of a 32 bit limb difference.
constexpr auto sign_shift = sub1(bits<uint32_t>);

// The one byte that pads a final partial block.
constexpr auto pad_byte = 0x01_u8;

// r is clamped: r &= 0x0ffffffc0ffffffc0ffffffc0fffffff (per limb).
constexpr std_array<uint32_t, 5> clamp
{
    0x03ffffff_u32, 0x03ffff03_u32, 0x03ffc0ff_u32, 0x03f03fff_u32,
    0x000fffff_u32
};

static constexpr uint64_t wide(uint32_t left, uint32_t right) NOEXCEPT
{
    return wide_cast<uint64_t>(left) * right;
}

poly1305::poly1305(const secret& key) NOEXCEPT
{
    // r is clamped: r &= 0x0ffffffc0ffffffc0ffffffc0fffffff.
    // [r in five 26-bit limbs, s (pad) in four 32-bit words.]
    r_[0] = bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&key[ 0]), 0), clamp[0]);
    r_[1] = bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&key[ 3]), 2), clamp[1]);
    r_[2] = bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&key[ 6]), 4), clamp[2]);
    r_[3] = bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&key[ 9]), 6), clamp[3]);
    r_[4] = bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&key[12]), 8), clamp[4]);

    pad_[0] = unsafe_from_little_endian<uint32_t>(&key[16]);
    pad_[1] = unsafe_from_little_endian<uint32_t>(&key[20]);
    pad_[2] = unsafe_from_little_endian<uint32_t>(&key[24]);
    pad_[3] = unsafe_from_little_endian<uint32_t>(&key[28]);
}

// private
void poly1305::blocks(const uint8_t* data, size_t blocks,
    uint32_t hibit) NOEXCEPT
{
    const auto r0 = r_[0];
    const auto r1 = r_[1];
    const auto r2 = r_[2];
    const auto r3 = r_[3];
    const auto r4 = r_[4];

    const auto s1 = r1 * delta;
    const auto s2 = r2 * delta;
    const auto s3 = r3 * delta;
    const auto s4 = r4 * delta;

    auto h0 = h_[0];
    auto h1 = h_[1];
    auto h2 = h_[2];
    auto h3 = h_[3];
    auto h4 = h_[4];

    for (size_t block{}; block < blocks; ++block)
    {
        // Add the message block (with high bit) to the accumulator.
        h0 += bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&data[ 0]), 0), limb_mask);
        h1 += bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&data[ 3]), 2), limb_mask);
        h2 += bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&data[ 6]), 4), limb_mask);
        h3 += bit_and(shift_right(unsafe_from_little_endian<uint32_t>(&data[ 9]), 6), limb_mask);
        h4 +=  bit_or(shift_right(unsafe_from_little_endian<uint32_t>(&data[12]), 8), hibit);

        // Multiply by r modulo 2^130 - delta, with partial carry propagation.
        const auto d0 =
            wide(h0, r0) + wide(h1, s4) +
            wide(h2, s3) + wide(h3, s2) +
            wide(h4, s1);
        auto d1 =
            wide(h0, r1) + wide(h1, r0) +
            wide(h2, s4) + wide(h3, s3) +
            wide(h4, s2);
        auto d2 =
            wide(h0, r2) + wide(h1, r1) +
            wide(h2, r0) + wide(h3, s4) +
            wide(h4, s3);
        auto d3 =
            wide(h0, r3) + wide(h1, r2) +
            wide(h2, r1) + wide(h3, r0) +
            wide(h4, s4);
        auto d4 =
            wide(h0, r4) + wide(h1, r3) +
            wide(h2, r2) + wide(h3, r1) +
            wide(h4, r0);

        auto carry = narrow_cast<uint32_t>(shift_right(d0, limb_bits));
        h0 = bit_and(narrow_cast<uint32_t>(d0), limb_mask);
        d1 += carry;
        carry = narrow_cast<uint32_t>(shift_right(d1, limb_bits));
        h1 = bit_and(narrow_cast<uint32_t>(d1), limb_mask);
        d2 += carry;
        carry = narrow_cast<uint32_t>(shift_right(d2, limb_bits));
        h2 = bit_and(narrow_cast<uint32_t>(d2), limb_mask);
        d3 += carry;
        carry = narrow_cast<uint32_t>(shift_right(d3, limb_bits));
        h3 = bit_and(narrow_cast<uint32_t>(d3), limb_mask);
        d4 += carry;
        carry = narrow_cast<uint32_t>(shift_right(d4, limb_bits));
        h4 = bit_and(narrow_cast<uint32_t>(d4), limb_mask);
        h0 += carry * delta;
        carry = shift_right(h0, limb_bits);
        h0 &= limb_mask;
        h1 += carry;

        std::advance(data, block_size);
    }

    h_[0] = h0;
    h_[1] = h1;
    h_[2] = h2;
    h_[3] = h3;
    h_[4] = h4;
}

void poly1305::write(const_byte_span data) NOEXCEPT
{
    auto bytes = data.data();
    auto size = data.size();

    // Fill and process buffered partial block.
    if (!is_zero(offset_))
    {
        const auto fill = std::min(size, block_size - offset_);
        std::copy_n(bytes, fill, std::next(buffer_.begin(), offset_));
        offset_ += fill;
        std::advance(bytes, fill);
        size -= fill;

        if (offset_ == block_size)
        {
            blocks(buffer_.data(), one, high_bit);
            offset_ = zero;
        }
    }

    // Process whole blocks and buffer the remainder (offset is zero unless
    // the fill above consumed all input, leaving size zero).
    if (!is_zero(size))
    {
        const auto whole = size / block_size;
        blocks(bytes, whole, high_bit);
        std::advance(bytes, whole * block_size);
        size -= whole * block_size;
        std::copy_n(bytes, size, buffer_.begin());
        offset_ = size;
    }
}

void poly1305::flush(tag& out) NOEXCEPT
{
    // Final partial block padded with a 1 byte then zeros, no high bit added.
    if (!is_zero(offset_))
    {
        buffer_[offset_++] = pad_byte;
        std::fill(std::next(buffer_.begin(), offset_), buffer_.end(), 0x00_u8);
        blocks(buffer_.data(), one, 0_u32);
    }

    auto h0 = h_[0];
    auto h1 = h_[1];
    auto h2 = h_[2];
    auto h3 = h_[3];
    auto h4 = h_[4];

    // Full carry propagation.
    auto carry = shift_right(h1, limb_bits);
    h1 &= limb_mask;
    h2 += carry;
    carry = shift_right(h2, limb_bits);
    h2 &= limb_mask;
    h3 += carry;
    carry = shift_right(h3, limb_bits);
    h3 &= limb_mask;
    h4 += carry;
    carry = shift_right(h4, limb_bits);
    h4 &= limb_mask;
    h0 += carry * delta;
    carry = shift_right(h0, limb_bits);
    h0 &= limb_mask;
    h1 += carry;

    // Compute h + -p.
    auto g0 = h0 + delta;
    carry = shift_right(g0, limb_bits);
    g0 &= limb_mask;
    auto g1 = h1 + carry;
    carry = shift_right(g1, limb_bits);
    g1 &= limb_mask;
    auto g2 = h2 + carry;
    carry = shift_right(g2, limb_bits);
    g2 &= limb_mask;
    auto g3 = h3 + carry;
    carry = shift_right(g3, limb_bits);
    g3 &= limb_mask;
    auto g4 = h4 + carry - limb_bit;

    // Select h if h < p, or h + -p if h >= p (constant time).
    auto mask = sub1(shift_right(g4, sign_shift));
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    g3 &= mask;
    g4 &= mask;
    mask = bit_not(mask);
    h0 = bit_or(bit_and(h0, mask), g0);
    h1 = bit_or(bit_and(h1, mask), g1);
    h2 = bit_or(bit_and(h2, mask), g2);
    h3 = bit_or(bit_and(h3, mask), g3);
    h4 = bit_or(bit_and(h4, mask), g4);

    // h = h % (2^128).
    h0 = bit_or(shift_right(h0,  0), shift_left(h1, 26));
    h1 = bit_or(shift_right(h1,  6), shift_left(h2, 20));
    h2 = bit_or(shift_right(h2, 12), shift_left(h3, 14));
    h3 = bit_or(shift_right(h3, 18), shift_left(h4,  8));

    // Finally, the value of the secret key s is added to the accumulator,
    // and the 128 least significant bits are serialized in little-endian
    // order to form the tag.
    auto f = wide_cast<uint64_t>(h0) + pad_[0];
    h0 = narrow_cast<uint32_t>(f);
    f = wide_cast<uint64_t>(h1) + pad_[1] + shift_right(f, 32);
    h1 = narrow_cast<uint32_t>(f);
    f = wide_cast<uint64_t>(h2) + pad_[2] + shift_right(f, 32);
    h2 = narrow_cast<uint32_t>(f);
    f = wide_cast<uint64_t>(h3) + pad_[3] + shift_right(f, 32);
    h3 = narrow_cast<uint32_t>(f);
    to_little_endians<uint32_t, 4>(array_cast<uint32_t>(out),
        { h0, h1, h2, h3 });
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
