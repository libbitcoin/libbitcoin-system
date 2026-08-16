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
#include <bitcoin/system/crypto/chacha20.hpp>

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8439
// [Nir, Langley]

namespace libbitcoin {
namespace system {
    
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr auto word_bits = bits<uint32_t>;
constexpr std_array<uint32_t, 4> sigma
{
    0x61707865_u32, 0x3320646e_u32, 0x79622d32_u32, 0x6b206574_u32
};

template <size_t A, size_t B, size_t C, size_t D, typename Word>
INLINE constexpr void quarter(std_array<Word, 16>& x) NOEXCEPT
{
    x[A] = f::add<word_bits>(x[A], x[B]);
    x[D] = f::rol<16, word_bits>(f::xor_(x[D], x[A]));
    x[C] = f::add<word_bits>(x[C], x[D]);
    x[B] = f::rol<12, word_bits>(f::xor_(x[B], x[C]));
    x[A] = f::add<word_bits>(x[A], x[B]);
    x[D] = f::rol<8, word_bits>(f::xor_(x[D], x[A]));
    x[C] = f::add<word_bits>(x[C], x[D]);
    x[B] = f::rol<7, word_bits>(f::xor_(x[B], x[C]));
}

template <typename Word>
INLINE constexpr void rounds(std_array<Word, 16>& x) NOEXCEPT
{
    for (size_t round{}; round < 10_size; ++round)
    {
        quarter<0, 4,  8, 12>(x);
        quarter<1, 5,  9, 13>(x);
        quarter<2, 6, 10, 14>(x);
        quarter<3, 7, 11, 15>(x);
        quarter<0, 5, 10, 15>(x);
        quarter<1, 6, 11, 12>(x);
        quarter<2, 7,  8, 13>(x);
        quarter<3, 4,  9, 14>(x);
    }
}

// Vectorized concurrent blocks (one block per 32 bit lane).
// ----------------------------------------------------------------------------
// The keystream of lanes blocks is generated concurrently, with the block
// counter incremented across lanes, and transposed to sequential blocks.

#if defined(HAVE_SSE4)

#if defined(HAVE_AVX2)
using xword_t = xint256_t;
#else
using xword_t = xint128_t;
#endif

constexpr auto xlanes = capacity<xword_t, uint32_t>;
constexpr auto xparts = 16_size / xlanes;
constexpr auto xblock_size = chacha20::block_size * xlanes;

// Transposes a matrix of four rows of four 32 bit words.
#if !defined(HAVE_AVX2)
INLINE void transpose(std_array<xint128_t, 4>& row) NOEXCEPT
{
    constexpr auto x64 = bits<uint64_t>;

    const auto t0 = f::unpack_lo<word_bits>(row[0], row[1]);
    const auto t1 = f::unpack_lo<word_bits>(row[2], row[3]);
    const auto t2 = f::unpack_hi<word_bits>(row[0], row[1]);
    const auto t3 = f::unpack_hi<word_bits>(row[2], row[3]);

    row[0] = f::unpack_lo<x64>(t0, t1);
    row[1] = f::unpack_hi<x64>(t0, t1);
    row[2] = f::unpack_lo<x64>(t2, t3);
    row[3] = f::unpack_hi<x64>(t2, t3);
}
#endif

// Transposes a matrix of eight rows of eight 32 bit words.
#if defined(HAVE_AVX2)
INLINE void transpose(std_array<xint256_t, 8>& row) NOEXCEPT
{
    constexpr auto x64 = bits<uint64_t>;

    const auto t0 = f::unpack_lo<word_bits>(row[0], row[1]);
    const auto t1 = f::unpack_hi<word_bits>(row[0], row[1]);
    const auto t2 = f::unpack_lo<word_bits>(row[2], row[3]);
    const auto t3 = f::unpack_hi<word_bits>(row[2], row[3]);
    const auto t4 = f::unpack_lo<word_bits>(row[4], row[5]);
    const auto t5 = f::unpack_hi<word_bits>(row[4], row[5]);
    const auto t6 = f::unpack_lo<word_bits>(row[6], row[7]);
    const auto t7 = f::unpack_hi<word_bits>(row[6], row[7]);

    const auto u0 = f::unpack_lo<x64>(t0, t2);
    const auto u1 = f::unpack_hi<x64>(t0, t2);
    const auto u2 = f::unpack_lo<x64>(t1, t3);
    const auto u3 = f::unpack_hi<x64>(t1, t3);
    const auto u4 = f::unpack_lo<x64>(t4, t6);
    const auto u5 = f::unpack_hi<x64>(t4, t6);
    const auto u6 = f::unpack_lo<x64>(t5, t7);
    const auto u7 = f::unpack_hi<x64>(t5, t7);

    row[0] = f::merge_lo(u0, u4);
    row[1] = f::merge_lo(u1, u5);
    row[2] = f::merge_lo(u2, u6);
    row[3] = f::merge_lo(u3, u7);
    row[4] = f::merge_hi(u0, u4);
    row[5] = f::merge_hi(u1, u5);
    row[6] = f::merge_hi(u2, u6);
    row[7] = f::merge_hi(u3, u7);
}
#endif

// The counter word of each lane is offset by the lane number.
INLINE xword_t counters(uint32_t counter) NOEXCEPT
{
#if defined(HAVE_AVX2)
    return f::add<word_bits>(f::broadcast<xword_t>(counter),
        f::set<xword_t>(0_u32, 1_u32, 2_u32, 3_u32, 4_u32, 5_u32, 6_u32, 7_u32));
#else
    return f::add<word_bits>(f::broadcast<xword_t>(counter),
        f::set<xword_t>(0_u32, 1_u32, 2_u32, 3_u32));
#endif
}

// Generates lanes sequential keystream blocks, transposed such that the
// keystream of block lane is the concatenation of x[parts * lane + part].
INLINE void xnext(std_array<xword_t, 16>& x,
    const std_array<uint32_t, 8>& key, const std_array<uint32_t, 3>& nonce,
    uint32_t counter) NOEXCEPT
{
    x =
    {
        f::broadcast<xword_t>(sigma[0]),
        f::broadcast<xword_t>(sigma[1]),
        f::broadcast<xword_t>(sigma[2]),
        f::broadcast<xword_t>(sigma[3]),
        f::broadcast<xword_t>(key[0]),
        f::broadcast<xword_t>(key[1]),
        f::broadcast<xword_t>(key[2]),
        f::broadcast<xword_t>(key[3]),
        f::broadcast<xword_t>(key[4]),
        f::broadcast<xword_t>(key[5]),
        f::broadcast<xword_t>(key[6]),
        f::broadcast<xword_t>(key[7]),
        counters(counter),
        f::broadcast<xword_t>(nonce[0]),
        f::broadcast<xword_t>(nonce[1]),
        f::broadcast<xword_t>(nonce[2])
    };

    const auto start = x;
    rounds(x);

    // rfc8439
    // At the end of 20 rounds, we add the original input words to the output
    // words [serialization is the little-endian store of transposed lanes].
    for (size_t word{}; word < x.size(); ++word)
        x[word] = f::add<word_bits>(x[word], start[word]);

#if defined(HAVE_AVX2)
    transpose(array_cast<xword_t, 8, 0>(x));
    transpose(array_cast<xword_t, 8, 8>(x));
#else
    transpose(array_cast<xword_t, 4, 0>(x));
    transpose(array_cast<xword_t, 4, 4>(x));
    transpose(array_cast<xword_t, 4, 8>(x));
    transpose(array_cast<xword_t, 4, 12>(x));
#endif
}

#endif // HAVE_SSE4

// chacha20
// ----------------------------------------------------------------------------

chacha20::chacha20(const secret& key) NOEXCEPT
{
    set_key(key);
}

void chacha20::set_key(const secret& key) NOEXCEPT
{
    // The next eight words (4-11) are taken from the 256-bit key by reading
    // the bytes in little-endian order, in 4-byte chunks.
    from_little_endians(key_, array_cast<uint32_t>(key));
    nonce_ = {};
    counter_ = {};
    offset_ = block_size;
}

void chacha20::seek(uint32_t nonce32, uint64_t nonce64,
    uint32_t counter) NOEXCEPT
{
    // Words 13-15 are a nonce, which MUST not be repeated for the same key.
    nonce_[0] = nonce32;
    nonce_[1] = narrow_cast<uint32_t>(nonce64);
    nonce_[2] = narrow_cast<uint32_t>(shift_right(nonce64, 32u));
    counter_ = counter;
    offset_ = block_size;
}

// private
void chacha20::next(block& out) NOEXCEPT
{
    const std_array<uint32_t, 16> state
    {
        sigma[0], sigma[1], sigma[2], sigma[3],
        key_[0], key_[1], key_[2], key_[3],
        key_[4], key_[5], key_[6], key_[7],
        counter_, nonce_[0], nonce_[1], nonce_[2]
    };

    auto x = state;
    rounds(x);

    // At the end of 20 rounds, we add the original input words to the output
    // words, and serialize the result by sequencing the words one-by-one in
    // little-endian order.
    for (size_t word{}; word < x.size(); ++word)
        x[word] += state[word];

    to_little_endians(array_cast<uint32_t>(out), x);
    ++counter_;
}

void chacha20::crypt(const_byte_span in,
    byte_span out) NOEXCEPT
{
    BC_ASSERT(in.size() == out.size());
    const auto size = in.size();
    size_t byte{};

    // Continue buffered keystream.
    for (; (offset_ < block_size) && (byte < size); ++offset_, ++byte)
        out[byte] = in[byte] ^ buffer_[offset_];

#if defined(HAVE_SSE4)
    // Concurrent whole blocks (one block per 32 bit lane).
    for (; (size - byte) >= xblock_size; byte += xblock_size)
    {
        std_array<xword_t, 16> x{};
        xnext(x, key_, nonce_, counter_);
        counter_ += xlanes;

        // Sequence lanes into the aligned keystream (caller buffers are not
        // vector aligned, so cannot be cast to the extended word type).
        std_array<xword_t, 16> keystream{};
        for (size_t lane{}; lane < xlanes; ++lane)
            for (size_t part{}; part < xparts; ++part)
                keystream[xparts * lane + part] = x[xlanes * part + lane];

        const auto& stream = array_cast<uint8_t>(keystream);
        for (size_t index{}; index < xblock_size; ++index)
            out[byte + index] = in[byte + index] ^ stream[index];
    }
#endif

    // Sequential whole blocks fully consume buffered keystream.
    for (; (size - byte) >= block_size; byte += block_size)
    {
        next(buffer_);
        for (size_t index{}; index < block_size; ++index)
            out[byte + index] = in[byte + index] ^ buffer_[index];
    }

    // Partial block, keystream remainder buffered.
    if (byte < size)
    {
        next(buffer_);
        offset_ = zero;

        for (; byte < size; ++byte)
            out[byte] = in[byte] ^ buffer_[offset_++];
    }
}

void chacha20::stream(byte_span out) NOEXCEPT
{
    size_t byte{};
    const auto size = out.size();

    // Continue buffered keystream.
    for (; (offset_ < block_size) && (byte < size); ++offset_, ++byte)
        out[byte] = buffer_[offset_];

#if defined(HAVE_SSE4)
    // Concurrent whole blocks (one block per 32 bit lane).
    for (; (size - byte) >= xblock_size; byte += xblock_size)
    {
        std_array<xword_t, 16> x{};
        xnext(x, key_, nonce_, counter_);
        counter_ += xlanes;

        // Sequence lanes into the aligned keystream (caller buffers are not
        // vector aligned, so cannot be cast to the extended word type).
        std_array<xword_t, 16> keystream{};
        for (size_t lane{}; lane < xlanes; ++lane)
            for (size_t part{}; part < xparts; ++part)
                keystream[xparts * lane + part] = x[xlanes * part + lane];

        const auto& stream = array_cast<uint8_t>(keystream);
        for (size_t index{}; index < xblock_size; ++index)
            out[byte + index] = stream[index];
    }
#endif

    // Sequential whole blocks fully consume buffered keystream.
    for (; (size - byte) >= block_size; byte += block_size)
    {
        next(buffer_);
        for (size_t index{}; index < block_size; ++index)
            out[byte + index] = buffer_[index];
    }

    // Partial block, keystream remainder buffered.
    if (byte < size)
    {
        next(buffer_);
        offset_ = zero;

        for (; byte < size; ++byte)
            out[byte] = buffer_[offset_++];
    }
}

// fschacha20
// ----------------------------------------------------------------------------

fschacha20::fschacha20(const chacha20::secret& key, uint32_t interval) NOEXCEPT
  : cipher_(key), interval_(interval)
{
}

void fschacha20::crypt(const_byte_span in,
    byte_span out) NOEXCEPT
{
    cipher_.crypt(in, out);

    // bip324
    // The key is rotated after every rekey_interval chunks, to the next 32
    // keystream bytes, and the nonce set to { 0, rekey counter }.
    if (++chunks_ == interval_)
    {
        chacha20::secret key{};
        cipher_.stream(key);
        cipher_.set_key(key);
        cipher_.seek(0, ++rekeys_, 0);
        key = {};
        chunks_ = 0;
    }
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
