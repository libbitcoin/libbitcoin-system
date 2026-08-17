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


// rounds
// ----------------------------------------------------------------------------

// The basic operation of the ChaCha algorithm is the quarter round.
template <size_t A, size_t B, size_t C, size_t D, typename Word>
INLINE constexpr void chacha20::quarter(state_t<Word>& x) NOEXCEPT
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

// A double round is four column rounds followed by four diagonal rounds.
template <typename Word>
INLINE constexpr void chacha20::double_round(state_t<Word>& x) NOEXCEPT
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

// ChaCha20 runs 20 rounds, alternating between column and diagonal rounds,
// and then adds the original input words to the output words.
template <size_t Index, typename Word>
INLINE constexpr void chacha20::summate(state_t<Word>& x,
    const state_t<Word>& start) NOEXCEPT
{
    x[Index] = f::add<word_bits>(x[Index], start[Index]);
}

template <typename Word>
INLINE constexpr void chacha20::rounds(state_t<Word>& x) NOEXCEPT
{
    const auto start = x;

    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);
    double_round(x);

    summate<0>(x, start);
    summate<1>(x, start);
    summate<2>(x, start);
    summate<3>(x, start);
    summate<4>(x, start);
    summate<5>(x, start);
    summate<6>(x, start);
    summate<7>(x, start);
    summate<8>(x, start);
    summate<9>(x, start);
    summate<10>(x, start);
    summate<11>(x, start);
    summate<12>(x, start);
    summate<13>(x, start);
    summate<14>(x, start);
    summate<15>(x, start);
}

// vectorization
// ----------------------------------------------------------------------------
// One block is computed per 32 bit lane, so the words of a given block are
// distributed across the state vectors. Transposition of each group of lanes
// vectors, and sequencing of those groups, produces sequential blocks.

// The block counter of each lane is offset by its lane number.
template <typename xWord>
INLINE xWord chacha20::counters(uint32_t counter) NOEXCEPT
{
    const auto base = f::broadcast<xWord>(counter);

    if constexpr (lanes<xWord> == 4)
        return f::add<word_bits>(base, f::set<xWord>(0, 1, 2, 3));

    if constexpr (lanes<xWord> == 8)
        return f::add<word_bits>(base, f::set<xWord>(0, 1, 2, 3, 4, 5, 6, 7));

    if constexpr (lanes<xWord> == 16)
        return f::add<word_bits>(base, f::set<xWord>(0, 1, 2, 3, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15));
}

template <size_t Index, typename xWord>
INLINE void chacha20::interleave32(rows_t<xWord>& to,
    const rows_t<xWord>& from) NOEXCEPT
{
    constexpr auto x32 = bits<uint32_t>;
    to[2 * Index + 0] = f::unpack_lo<x32>(from[2 * Index], from[2 * Index + 1]);
    to[2 * Index + 1] = f::unpack_hi<x32>(from[2 * Index], from[2 * Index + 1]);
}

template <size_t Index, typename xWord>
INLINE void chacha20::interleave64(rows_t<xWord>& to,
    const rows_t<xWord>& from) NOEXCEPT
{
    constexpr auto x64 = bits<uint64_t>;
    to[4 * Index + 0] = f::unpack_lo<x64>(from[4 * Index], from[4 * Index + 2]);
    to[4 * Index + 1] = f::unpack_hi<x64>(from[4 * Index], from[4 * Index + 2]);
    to[4 * Index + 2] = f::unpack_lo<x64>(from[4 * Index + 1],
        from[4 * Index + 3]);
    to[4 * Index + 3] = f::unpack_hi<x64>(from[4 * Index + 1],
        from[4 * Index + 3]);
}

template <size_t Index, size_t Span, typename xWord>
INLINE void chacha20::exchange(rows_t<xWord>& to,
    const rows_t<xWord>& from) NOEXCEPT
{
    to[Index] = f::tile_lo(from[Index], from[Index + Span]);
    to[Index + Span] = f::tile_hi(from[Index], from[Index + Span]);
}

// Transposes a square matrix of lanes rows of lanes 32 bit words.
template <typename xWord>
INLINE void chacha20::transpose(rows_t<xWord>& row) NOEXCEPT
{
    constexpr auto size = lanes<xWord>;

    // Interleave adjacent rows by 32 and then by 64 bit words.
    rows_t<xWord> t{};
    rows_t<xWord> u{};

    if constexpr (size == 4)
    {
        interleave32<0>(t, row);
        interleave32<1>(t, row);
        interleave64<0>(u, t);
        row = u;
    }

    if constexpr (size == 8)
    {
        interleave32<0>(t, row);
        interleave32<1>(t, row);
        interleave32<2>(t, row);
        interleave32<3>(t, row);
        interleave64<0>(u, t);
        interleave64<1>(u, t);
        exchange<0, 4>(row, u);
        exchange<1, 4>(row, u);
        exchange<2, 4>(row, u);
        exchange<3, 4>(row, u);
    }

    if constexpr (size == 16)
    {
        interleave32<0>(t, row);
        interleave32<1>(t, row);
        interleave32<2>(t, row);
        interleave32<3>(t, row);
        interleave32<4>(t, row);
        interleave32<5>(t, row);
        interleave32<6>(t, row);
        interleave32<7>(t, row);
        interleave64<0>(u, t);
        interleave64<1>(u, t);
        interleave64<2>(u, t);
        interleave64<3>(u, t);

        rows_t<xWord> v{};
        exchange<0, 4>(v, u);
        exchange<1, 4>(v, u);
        exchange<2, 4>(v, u);
        exchange<3, 4>(v, u);
        exchange<8, 4>(v, u);
        exchange<9, 4>(v, u);
        exchange<10, 4>(v, u);
        exchange<11, 4>(v, u);

        exchange<0, 8>(row, v);
        exchange<1, 8>(row, v);
        exchange<2, 8>(row, v);
        exchange<3, 8>(row, v);
        exchange<4, 8>(row, v);
        exchange<5, 8>(row, v);
        exchange<6, 8>(row, v);
        exchange<7, 8>(row, v);
    }
}

// Transposes each group of lanes vectors of the state, in place.
template <typename xWord>
INLINE void chacha20::transpose_groups(state_t<xWord>& x) NOEXCEPT
{
    if constexpr (lanes<xWord> == 4)
    {
        transpose<xWord>(array_cast<xWord, 4, 0>(x));
        transpose<xWord>(array_cast<xWord, 4, 4>(x));
        transpose<xWord>(array_cast<xWord, 4, 8>(x));
        transpose<xWord>(array_cast<xWord, 4, 12>(x));
    }

    if constexpr (lanes<xWord> == 8)
    {
        transpose<xWord>(array_cast<xWord, 8, 0>(x));
        transpose<xWord>(array_cast<xWord, 8, 8>(x));
    }

    if constexpr (lanes<xWord> == 16)
    {
        transpose<xWord>(array_cast<xWord, 16, 0>(x));
    }
}

template <size_t Lane, typename xWord>
INLINE void chacha20::sequence(state_t<xWord>& keystream,
    const state_t<xWord>& x) NOEXCEPT
{
    constexpr auto part = parts<xWord>;
    constexpr auto lane = lanes<xWord>;

    if constexpr (part > 0)
        keystream[part * Lane + 0] = x[lane * 0 + Lane];
    if constexpr (part > 1)
        keystream[part * Lane + 1] = x[lane * 1 + Lane];
    if constexpr (part > 2)
        keystream[part * Lane + 2] = x[lane * 2 + Lane];
    if constexpr (part > 3)
        keystream[part * Lane + 3] = x[lane * 3 + Lane];
}

// Generates lanes sequential keystream blocks.
template <typename xWord>
INLINE void chacha20::xnext(state_t<xWord>& keystream,
    const keys_t& key, const nonce_t& nonce,
    uint32_t counter) NOEXCEPT
{
    state_t<xWord> x
    {
        f::broadcast<xWord>(sigma[0]),
        f::broadcast<xWord>(sigma[1]),
        f::broadcast<xWord>(sigma[2]),
        f::broadcast<xWord>(sigma[3]),
        f::broadcast<xWord>(key[0]),
        f::broadcast<xWord>(key[1]),
        f::broadcast<xWord>(key[2]),
        f::broadcast<xWord>(key[3]),
        f::broadcast<xWord>(key[4]),
        f::broadcast<xWord>(key[5]),
        f::broadcast<xWord>(key[6]),
        f::broadcast<xWord>(key[7]),
        counters<xWord>(counter),
        f::broadcast<xWord>(nonce[0]),
        f::broadcast<xWord>(nonce[1]),
        f::broadcast<xWord>(nonce[2])
    };

    rounds(x);
    transpose_groups(x);

    // Sequence the transposed groups into blocks of contiguous words.
    sequence<0>(keystream, x);
    sequence<1>(keystream, x);
    sequence<2>(keystream, x);
    sequence<3>(keystream, x);

    if constexpr (lanes<xWord> > 4)
    {
        sequence<4>(keystream, x);
        sequence<5>(keystream, x);
        sequence<6>(keystream, x);
        sequence<7>(keystream, x);
    }

    if constexpr (lanes<xWord> > 8)
    {
        sequence<8>(keystream, x);
        sequence<9>(keystream, x);
        sequence<10>(keystream, x);
        sequence<11>(keystream, x);
        sequence<12>(keystream, x);
        sequence<13>(keystream, x);
        sequence<14>(keystream, x);
        sequence<15>(keystream, x);
    }
}

// Crypts whole multiples of the extended block, returns bytes consumed.
template <typename xWord>
INLINE size_t chacha20::xcrypt(const_byte_span in, byte_span out, size_t start,
    const keys_t& key, const nonce_t& nonce,
    uint32_t& counter) NOEXCEPT
{
    auto byte = start;

    if constexpr (have<xWord>)
    {
        constexpr auto size = xblock_size<xWord>;

        while ((out.size() - byte) >= size)
        {
            state_t<xWord> keystream{};
            xnext(keystream, key, nonce, counter);
            counter += lanes<xWord>;

            // Caller buffers are not vector aligned, so the keystream is
            // applied from the aligned state.
            const auto& stream = array_cast<uint8_t>(keystream);

            if (in.empty())
                for (size_t index{}; index < size; ++index)
                    out[byte + index] = stream[index];
            else
                for (size_t index{}; index < size; ++index)
                    out[byte + index] = bit_xor(in[byte + index], stream[index]);

            byte += size;
        }
    }

    return byte;
}

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
    state_t<uint32_t> x
    {
        sigma[0],  sigma[1],  sigma[2],  sigma[3],
        key_[0],   key_[1],   key_[2],   key_[3],
        key_[4],   key_[5],   key_[6],   key_[7],
        counter_,  nonce_[0], nonce_[1], nonce_[2]
    };

    rounds(x);

    // Serialize by sequencing the words one-by-one in little-endian order.
    to_little_endians(array_cast<uint32_t>(out), x);
    ++counter_;
}

void chacha20::crypt(const_byte_span in, byte_span out) NOEXCEPT
{
    BC_ASSERT(in.empty() || in.size() == out.size());
    const auto size = out.size();
    size_t byte{};

    // Continue buffered keystream.
    for (; (offset_ < block_size) && (byte < size); ++offset_, ++byte)
        out[byte] = in.empty() ? buffer_[offset_] :
            bit_xor(in[byte], buffer_[offset_]);

    // Concurrent blocks, widest first (each is a no-op if unavailable).
    byte = xcrypt<xint512_t>(in, out, byte, key_, nonce_, counter_);
    byte = xcrypt<xint256_t>(in, out, byte, key_, nonce_, counter_);
    byte = xcrypt<xint128_t>(in, out, byte, key_, nonce_, counter_);

    // Sequential whole blocks fully consume buffered keystream.
    for (; (size - byte) >= block_size; byte += block_size)
    {
        next(buffer_);
        for (size_t index{}; index < block_size; ++index)
            out[byte + index] = in.empty() ? buffer_[index] :
                bit_xor(in[byte + index], buffer_[index]);
    }

    // Partial block, keystream remainder buffered.
    if (byte < size)
    {
        next(buffer_);
        offset_ = zero;

        for (; byte < size; ++byte)
            out[byte] = in.empty() ? buffer_[offset_++] :
                bit_xor(in[byte], buffer_[offset_++]);
    }
}

void chacha20::stream(byte_span out) NOEXCEPT
{
    crypt({}, out);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
