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
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8439
// [Nir, Langley]
// and poly1305-donna (32 bit) [Floodyberry], public domain.
// Lane parallelization from eprint.iacr.org/2014/850 [Goll, Gueron].

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr auto word_bits = bits<uint64_t>;

// The prime modulus is 2^130 - prime_delta, with the accumulator (and r) in
// five 26 bit limbs, so limbs mask to 26 bits and carry out of the top limb
// is folded back into the bottom, scaled by prime_delta.
constexpr auto limb_bits = 26_size;
constexpr auto limb_mask = 0x03ffffff_u32;
constexpr auto limb_bit = 0x04000000_u32;
constexpr auto prime_delta = 5_u32;

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

// multiplication
// ----------------------------------------------------------------------------

template <typename Word>
INLINE constexpr Word poly1305::fill(uint64_t value) NOEXCEPT
{
    if constexpr (is_integral_integer<Word>)
        return value;
    else
        return f::broadcast<Word>(value);
}

template <size_t Limb>
INLINE uint64_t poly1305::limb(const uint8_t* data, uint32_t hibit) NOEXCEPT
{
    if constexpr (Limb == sub1(limb_count))
    {
        // The top limb is short, read (aligned) from the end of the block.
        constexpr auto offset = block_size - sizeof(uint32_t);
        const auto at = std::next(data, offset);
        const auto word = unsafe_from_little_endian<uint32_t>(at);
        return bit_or(shift_right(word, byte_bits), hibit);
    }
    else
    {
        constexpr auto offset = (Limb * limb_bits) / byte_bits;
        constexpr auto shift = (Limb * limb_bits) % byte_bits;
        const auto at = std::next(data, offset);
        const auto word = unsafe_from_little_endian<uint32_t>(at);
        return bit_and(shift_right(word, shift), limb_mask);
    }
}

template <typename Word>
INLINE constexpr void poly1305::scale(limbs_t<Word>& s,
    const limbs_t<Word>& r) NOEXCEPT
{
    const auto delta = fill<Word>(prime_delta);
    s[0] = f::mul<word_bits>(r[0], delta);
    s[1] = f::mul<word_bits>(r[1], delta);
    s[2] = f::mul<word_bits>(r[2], delta);
    s[3] = f::mul<word_bits>(r[3], delta);
    s[4] = f::mul<word_bits>(r[4], delta);
}

template <typename Word>
INLINE constexpr Word poly1305::dot(const limbs_t<Word>& a, Word w0, Word w1,
    Word w2, Word w3, Word w4) NOEXCEPT
{
    const auto p0 = f::mul<word_bits>(a[0], w0);
    const auto p1 = f::mul<word_bits>(a[1], w1);
    const auto p2 = f::mul<word_bits>(a[2], w2);
    const auto p3 = f::mul<word_bits>(a[3], w3);
    const auto p4 = f::mul<word_bits>(a[4], w4);
    const auto q1 = f::add<word_bits>(p0, p1);
    const auto q2 = f::add<word_bits>(p2, p3);
    const auto q3 = f::add<word_bits>(q1, q2);
    return f::add<word_bits>(q3, p4);
}

// Multiply the accumulator by r modulo the prime, into unreduced products.
template <typename Word>
INLINE constexpr void poly1305::multiply(limbs_t<Word>& d,
    const limbs_t<Word>& a, const limbs_t<Word>& r,
    const limbs_t<Word>& s) NOEXCEPT
{
    d[0] = dot(a, r[0], s[4], s[3], s[2], s[1]);
    d[1] = dot(a, r[1], r[0], s[4], s[3], s[2]);
    d[2] = dot(a, r[2], r[1], r[0], s[4], s[3]);
    d[3] = dot(a, r[3], r[2], r[1], r[0], s[4]);
    d[4] = dot(a, r[4], r[3], r[2], r[1], r[0]);
}

// Partial carry propagation of the products into limbs of the accumulator.
template <typename Word>
INLINE constexpr void poly1305::reduce(limbs_t<Word>& a,
    const limbs_t<Word>& d) NOEXCEPT
{
    const auto mask = fill<Word>(limb_mask);
    const auto delta = fill<Word>(prime_delta);

    const auto c0 = f::shr<limb_bits, word_bits>(d[0]);
    const auto d1 = f::add<word_bits>(d[1], c0);
    const auto c1 = f::shr<limb_bits, word_bits>(d1);
    const auto d2 = f::add<word_bits>(d[2], c1);
    const auto c2 = f::shr<limb_bits, word_bits>(d2);
    const auto d3 = f::add<word_bits>(d[3], c2);
    const auto c3 = f::shr<limb_bits, word_bits>(d3);
    const auto d4 = f::add<word_bits>(d[4], c3);
    const auto c4 = f::shr<limb_bits, word_bits>(d4);
    const auto d0 = f::add<word_bits>(f::and_(d[0], mask),
        f::mul<word_bits>(c4, delta));
    const auto c5 = f::shr<limb_bits, word_bits>(d0);

    a[0] = f::and_(d0, mask);
    a[1] = f::add<word_bits>(f::and_(d1, mask), c5);
    a[2] = f::and_(d2, mask);
    a[3] = f::and_(d3, mask);
    a[4] = f::and_(d4, mask);
}

// vectorization
// ----------------------------------------------------------------------------
// One block is computed per 64 bit lane. Each lane accumulates every lanes-th
// block, advanced between groups by r^lanes, and the lanes are recombined by
// their remaining (descending) powers of r [Goll, Gueron].

template <size_t Limb, typename xWord>
INLINE xWord poly1305::load(const uint8_t* data, uint32_t hibit,
    uint64_t extra) NOEXCEPT
{
    if constexpr (lanes<xWord> == 2)
    {
        const auto x0 = limb<Limb>(std::next(data, 0 * block_size), hibit);
        const auto x1 = limb<Limb>(std::next(data, 1 * block_size), hibit);
        return f::set<xWord>(extra + x0, x1);
    }

    if constexpr (lanes<xWord> == 4)
    {
        const auto x0 = limb<Limb>(std::next(data, 0 * block_size), hibit);
        const auto x1 = limb<Limb>(std::next(data, 1 * block_size), hibit);
        const auto x2 = limb<Limb>(std::next(data, 2 * block_size), hibit);
        const auto x3 = limb<Limb>(std::next(data, 3 * block_size), hibit);
        return f::set<xWord>(extra + x0, x1, x2, x3);
    }

    if constexpr (lanes<xWord> == 8)
    {
        const auto x0 = limb<Limb>(std::next(data, 0 * block_size), hibit);
        const auto x1 = limb<Limb>(std::next(data, 1 * block_size), hibit);
        const auto x2 = limb<Limb>(std::next(data, 2 * block_size), hibit);
        const auto x3 = limb<Limb>(std::next(data, 3 * block_size), hibit);
        const auto x4 = limb<Limb>(std::next(data, 4 * block_size), hibit);
        const auto x5 = limb<Limb>(std::next(data, 5 * block_size), hibit);
        const auto x6 = limb<Limb>(std::next(data, 6 * block_size), hibit);
        const auto x7 = limb<Limb>(std::next(data, 7 * block_size), hibit);
        return f::set<xWord>(extra + x0, x1, x2, x3, x4, x5, x6, x7);
    }
}

// Lane zero holds the oldest block and so takes the highest power.
template <typename xWord>
INLINE xWord poly1305::descend(const ladder_t<xWord>& ladder,
    size_t index) NOEXCEPT
{
    if constexpr (lanes<xWord> == 2)
    {
        return f::set<xWord>(ladder[1][index], ladder[0][index]);
    }

    if constexpr (lanes<xWord> == 4)
    {
        return f::set<xWord>(ladder[3][index], ladder[2][index],
            ladder[1][index], ladder[0][index]);
    }

    if constexpr (lanes<xWord> == 8)
    {
        return f::set<xWord>(ladder[7][index], ladder[6][index],
            ladder[5][index], ladder[4][index], ladder[3][index],
            ladder[2][index], ladder[1][index], ladder[0][index]);
    }
}

template <typename xWord>
INLINE uint64_t poly1305::sum(xWord a) NOEXCEPT
{
    if constexpr (lanes<xWord> == 2)
    {
        const auto x0 = f::get<uint64_t, 0>(a);
        const auto x1 = f::get<uint64_t, 1>(a);
        return x0 + x1;
    }

    if constexpr (lanes<xWord> == 4)
    {
        const auto x0 = f::get<uint64_t, 0>(a);
        const auto x1 = f::get<uint64_t, 1>(a);
        const auto x2 = f::get<uint64_t, 2>(a);
        const auto x3 = f::get<uint64_t, 3>(a);
        return x0 + x1 + x2 + x3;
    }

    if constexpr (lanes<xWord> == 8)
    {
        const auto x0 = f::get<uint64_t, 0>(a);
        const auto x1 = f::get<uint64_t, 1>(a);
        const auto x2 = f::get<uint64_t, 2>(a);
        const auto x3 = f::get<uint64_t, 3>(a);
        const auto x4 = f::get<uint64_t, 4>(a);
        const auto x5 = f::get<uint64_t, 5>(a);
        const auto x6 = f::get<uint64_t, 6>(a);
        const auto x7 = f::get<uint64_t, 7>(a);
        return x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7;
    }
}

template <typename xWord>
INLINE void poly1305::xblocks(const uint8_t*& data, size_t& blocks,
    uint32_t hibit, const limbs_t<uint64_t>& r, const limbs_t<uint64_t>& s,
    limbs_t<uint64_t>& h) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        constexpr auto count = lanes<xWord>;
        if (blocks >= two * count)
        {
            // The scalar ladder of ascending powers of r.
            ladder_t<xWord> ladder{};
            limbs_t<uint64_t> d{};
            ladder[0] = r;
            for (auto power = one; power < count; ++power)
            {
                multiply(d, ladder[sub1(power)], r, s);
                reduce(ladder[power], d);
            }

            // The high power advances all lanes with each group.
            limbs_t<xWord> rn{};
            limbs_t<xWord> sn{};
            rn[0] = fill<xWord>(ladder[sub1(count)][0]);
            rn[1] = fill<xWord>(ladder[sub1(count)][1]);
            rn[2] = fill<xWord>(ladder[sub1(count)][2]);
            rn[3] = fill<xWord>(ladder[sub1(count)][3]);
            rn[4] = fill<xWord>(ladder[sub1(count)][4]);
            scale(sn, rn);

            // The first group of blocks, accumulator folded into lane zero.
            limbs_t<xWord> a{};
            a[0] = load<0, xWord>(data, hibit, h[0]);
            a[1] = load<1, xWord>(data, hibit, h[1]);
            a[2] = load<2, xWord>(data, hibit, h[2]);
            a[3] = load<3, xWord>(data, hibit, h[3]);
            a[4] = load<4, xWord>(data, hibit, h[4]);
            std::advance(data, count * block_size);
            blocks -= count;

            limbs_t<xWord> xd{};
            while (blocks >= count)
            {
                multiply(xd, a, rn, sn);
                reduce(a, xd);

                const auto m0 = load<0, xWord>(data, hibit, zero);
                const auto m1 = load<1, xWord>(data, hibit, zero);
                const auto m2 = load<2, xWord>(data, hibit, zero);
                const auto m3 = load<3, xWord>(data, hibit, zero);
                const auto m4 = load<4, xWord>(data, hibit, zero);
                a[0] = f::add<word_bits>(a[0], m0);
                a[1] = f::add<word_bits>(a[1], m1);
                a[2] = f::add<word_bits>(a[2], m2);
                a[3] = f::add<word_bits>(a[3], m3);
                a[4] = f::add<word_bits>(a[4], m4);
                std::advance(data, count * block_size);
                blocks -= count;
            }

            // Descending powers advance each lane to its terminal power.
            limbs_t<xWord> w{};
            limbs_t<xWord> sw{};
            w[0] = descend<xWord>(ladder, 0);
            w[1] = descend<xWord>(ladder, 1);
            w[2] = descend<xWord>(ladder, 2);
            w[3] = descend<xWord>(ladder, 3);
            w[4] = descend<xWord>(ladder, 4);
            scale(sw, w);
            multiply(xd, a, w, sw);

            // The lanes sum into the scalar accumulator.
            d[0] = sum<xWord>(xd[0]);
            d[1] = sum<xWord>(xd[1]);
            d[2] = sum<xWord>(xd[2]);
            d[3] = sum<xWord>(xd[3]);
            d[4] = sum<xWord>(xd[4]);
            reduce(h, d);
        }
    }
}

// accumulation
// ----------------------------------------------------------------------------

void poly1305::blocks(const uint8_t* data, size_t blocks,
    uint32_t hibit) NOEXCEPT
{
    limbs_t<uint64_t> h{ h_[0], h_[1], h_[2], h_[3], h_[4] };
    const limbs_t<uint64_t> r{ r_[0], r_[1], r_[2], r_[3], r_[4] };
    limbs_t<uint64_t> s{};
    scale(s, r);

    xblocks<xint512_t>(data, blocks, hibit, r, s, h);
    xblocks<xint256_t>(data, blocks, hibit, r, s, h);
    xblocks<xint128_t>(data, blocks, hibit, r, s, h);

    limbs_t<uint64_t> d{};
    while (!is_zero(blocks--))
    {
        // Add the message block (with high bit) to the accumulator.
        h[0] += limb<0>(data, hibit);
        h[1] += limb<1>(data, hibit);
        h[2] += limb<2>(data, hibit);
        h[3] += limb<3>(data, hibit);
        h[4] += limb<4>(data, hibit);

        // Multiply by r modulo the prime, with partial carry propagation.
        multiply(d, h, r, s);
        reduce(h, d);
        std::advance(data, block_size);
    }

    h_[0] = narrow_cast<uint32_t>(h[0]);
    h_[1] = narrow_cast<uint32_t>(h[1]);
    h_[2] = narrow_cast<uint32_t>(h[2]);
    h_[3] = narrow_cast<uint32_t>(h[3]);
    h_[4] = narrow_cast<uint32_t>(h[4]);
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
    h0 += carry * prime_delta;
    carry = shift_right(h0, limb_bits);
    h0 &= limb_mask;
    h1 += carry;

    // Compute h + -p.
    auto g0 = h0 + prime_delta;
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
