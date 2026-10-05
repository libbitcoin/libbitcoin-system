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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_BITSLICE_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_BITSLICE_IPP

// Bitslicing
// ============================================================================
// Four blocks are sliced into eight 64 bit planes, plane i holding bit i of
// each of the 64 state bytes, at position 16 * row + 4 * column + block. An
// extended word carries one such group of four blocks in each 64 bit lane.

// Derived in part from BearSSL (aes_ct64.c):
/*
 * Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

namespace libbitcoin {
namespace system {
namespace aes {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Word>
INLINE constexpr Word CLASS::
constant(uint64_t value) NOEXCEPT
{
    return f::broadcast<Word>(value);
}

TEMPLATE
template <typename Word, typename... Words>
INLINE constexpr Word CLASS::
xors(Word a, Words... b) NOEXCEPT
{
    ((a = f::xor_(a, b)), ...);
    return a;
}

TEMPLATE
template <typename Word, typename... Words>
INLINE constexpr Word CLASS::
ors(Word a, Words... b) NOEXCEPT
{
    ((a = f::or_(a, b)), ...);
    return a;
}

TEMPLATE
template <uint64_t Lo, uint64_t Hi, size_t Shift, typename Word>
INLINE constexpr void CLASS::
swap(Word& x, Word& y) NOEXCEPT
{
    constexpr auto s = bits<uint64_t>;
    const auto lo = constant<Word>(Lo);
    const auto hi = constant<Word>(Hi);
    const auto a = x;
    const auto b = y;
    const auto moved_up = f::shl<Shift, s>(f::and_(b, lo));
    const auto moved_down = f::shr<Shift, s>(f::and_(a, hi));
    x = f::or_(f::and_(a, lo), moved_up);
    y = f::or_(moved_down, f::and_(b, hi));
}

// Transposes each 8x8 bit matrix of the eight words (self-inverse).
TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
ortho(xplanes_t<Word>& q) NOEXCEPT
{
    constexpr auto lo1 = 0x5555555555555555_u64;
    constexpr auto hi1 = 0xaaaaaaaaaaaaaaaa_u64;
    constexpr auto lo2 = 0x3333333333333333_u64;
    constexpr auto hi2 = 0xcccccccccccccccc_u64;
    constexpr auto lo4 = 0x0f0f0f0f0f0f0f0f_u64;
    constexpr auto hi4 = 0xf0f0f0f0f0f0f0f0_u64;

    swap<lo1, hi1, 1>(q[0], q[1]);
    swap<lo1, hi1, 1>(q[2], q[3]);
    swap<lo1, hi1, 1>(q[4], q[5]);
    swap<lo1, hi1, 1>(q[6], q[7]);

    swap<lo2, hi2, 2>(q[0], q[2]);
    swap<lo2, hi2, 2>(q[1], q[3]);
    swap<lo2, hi2, 2>(q[4], q[6]);
    swap<lo2, hi2, 2>(q[5], q[7]);

    swap<lo4, hi4, 4>(q[0], q[4]);
    swap<lo4, hi4, 4>(q[1], q[5]);
    swap<lo4, hi4, 4>(q[2], q[6]);
    swap<lo4, hi4, 4>(q[3], q[7]);
}

// Spreads the four words of a block across two words, bytes of the first
// (and third) words to even byte positions, second (and fourth) to odd.
TEMPLATE
INLINE constexpr void CLASS::
interleave_in(uint64_t& q0, uint64_t& q1, const words_t& w) NOEXCEPT
{
    constexpr auto mask16 = 0x0000ffff0000ffff_u64;
    constexpr auto mask8 = 0x00ff00ff00ff00ff_u64;

    auto x0 = wide_cast<uint64_t>(w[0]), x1 = wide_cast<uint64_t>(w[1]);
    auto x2 = wide_cast<uint64_t>(w[2]), x3 = wide_cast<uint64_t>(w[3]);
    x0 = bit_and(bit_or(x0, shift_left(x0, 16)), mask16);
    x1 = bit_and(bit_or(x1, shift_left(x1, 16)), mask16);
    x2 = bit_and(bit_or(x2, shift_left(x2, 16)), mask16);
    x3 = bit_and(bit_or(x3, shift_left(x3, 16)), mask16);
    x0 = bit_and(bit_or(x0, shift_left(x0, 8)), mask8);
    x1 = bit_and(bit_or(x1, shift_left(x1, 8)), mask8);
    x2 = bit_and(bit_or(x2, shift_left(x2, 8)), mask8);
    x3 = bit_and(bit_or(x3, shift_left(x3, 8)), mask8);
    q0 = bit_or(x0, shift_left(x2, 8));
    q1 = bit_or(x1, shift_left(x3, 8));
}

TEMPLATE
INLINE constexpr void CLASS::
interleave_out(words_t& w, uint64_t q0, uint64_t q1) NOEXCEPT
{
    constexpr auto mask16 = 0x0000ffff0000ffff_u64;
    constexpr auto mask8 = 0x00ff00ff00ff00ff_u64;

    auto x0 = bit_and(q0, mask8);
    auto x1 = bit_and(q1, mask8);
    auto x2 = bit_and(shift_right(q0, 8), mask8);
    auto x3 = bit_and(shift_right(q1, 8), mask8);
    x0 = bit_and(bit_or(x0, shift_right(x0, 8)), mask16);
    x1 = bit_and(bit_or(x1, shift_right(x1, 8)), mask16);
    x2 = bit_and(bit_or(x2, shift_right(x2, 8)), mask16);
    x3 = bit_and(bit_or(x3, shift_right(x3, 8)), mask16);
    w[0] = narrow_cast<uint32_t>(bit_or(x0, shift_right(x0, 16)));
    w[1] = narrow_cast<uint32_t>(bit_or(x1, shift_right(x1, 16)));
    w[2] = narrow_cast<uint32_t>(bit_or(x2, shift_right(x2, 16)));
    w[3] = narrow_cast<uint32_t>(bit_or(x3, shift_right(x3, 16)));
}

TEMPLATE
INLINE constexpr typename CLASS::words_t CLASS::
to_words(const block_t& block) NOEXCEPT
{
    words_t words{};
    from_little<0>(words[0], block);
    from_little<4>(words[1], block);
    from_little<8>(words[2], block);
    from_little<12>(words[3], block);
    return words;
}

TEMPLATE
INLINE constexpr typename CLASS::block_t CLASS::
from_words(const words_t& words) NOEXCEPT
{
    block_t block{};
    to_little<0>(block, words[0]);
    to_little<4>(block, words[1]);
    to_little<8>(block, words[2]);
    to_little<12>(block, words[3]);
    return block;
}

// Blocks.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Word, size_t Lanes>
INLINE constexpr void CLASS::
xinput(xplanes_t<Word>& q, const xblocks_t<Lanes>& in) NOEXCEPT
{
    constexpr auto words = capacity<Word, uint64_t>;
    static_assert(Lanes == slice_blocks * words);

    if constexpr (is_same_type<Word, uint64_t>)
    {
        for (size_t block{}; block < slice_blocks; ++block)
        {
            const auto value = to_words(in[block]);
            interleave_in(q[block], q[block + slice_blocks], value);
        }
    }
    else
    {
        std_array<std_array<uint64_t, words>, planes> scalar{};
        for (size_t lane{}; lane < words; ++lane)
        {
            for (size_t block{}; block < slice_blocks; ++block)
            {
                auto& low = scalar[block][lane];
                auto& high = scalar[block + slice_blocks][lane];
                const auto index = lane * slice_blocks + block;
                interleave_in(low, high, to_words(in[index]));
            }
        }

        for (size_t plane{}; plane < planes; ++plane)
        {
            const auto& xwords = array_cast<Word>(scalar[plane]).front();
            q[plane] = f::load<uint64_t>(xwords);
        }
    }

    ortho(q);
}

TEMPLATE
template <typename Word, size_t Lanes>
INLINE constexpr void CLASS::
xoutput(xblocks_t<Lanes>& out, const xplanes_t<Word>& q) NOEXCEPT
{
    constexpr auto words = capacity<Word, uint64_t>;
    static_assert(Lanes == slice_blocks * words);

    auto copy = q;
    ortho(copy);
    words_t value{};

    if constexpr (is_same_type<Word, uint64_t>)
    {
        for (size_t block{}; block < slice_blocks; ++block)
        {
            interleave_out(value, copy[block], copy[block + slice_blocks]);
            out[block] = from_words(value);
        }
    }
    else
    {
        std_array<std_array<uint64_t, words>, planes> scalar{};
        for (size_t plane{}; plane < planes; ++plane)
        {
            auto& xwords = array_cast<Word>(scalar[plane]).front();
            f::store<uint64_t>(xwords, copy[plane]);
        }

        for (size_t lane{}; lane < words; ++lane)
        {
            for (size_t block{}; block < slice_blocks; ++block)
            {
                const auto& low = scalar[block][lane];
                const auto& high = scalar[block + slice_blocks][lane];
                const auto index = lane * slice_blocks + block;
                interleave_out(value, low, high);
                out[index] = from_words(value);
            }
        }
    }
}

TEMPLATE
template <typename Word, size_t Lanes>
constexpr void CLASS::
xencrypt(xblocks_t<Lanes>& xblocks, const xkeys_t<Word>& keys) NOEXCEPT
{
    xplanes_t<Word> q{};
    xinput(q, xblocks);
    encrypt_planes(q, keys);
    xoutput(xblocks, q);
}

TEMPLATE
template <typename Word, size_t Lanes>
constexpr void CLASS::
xdecrypt(xblocks_t<Lanes>& xblocks, const xkeys_t<Word>& keys) NOEXCEPT
{
    xplanes_t<Word> q{};
    xinput(q, xblocks);
    decrypt_planes(q, keys);
    xoutput(xblocks, q);
}

TEMPLATE
constexpr void CLASS::
encrypt_sliced(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    xblocks_t<slice_blocks> xblocks{ block };
    xencrypt<uint64_t>(xblocks, slice(schedule));
    block = xblocks.front();
}

TEMPLATE
constexpr void CLASS::
decrypt_sliced(block_t& block, const schedule_t& schedule) NOEXCEPT
{
    xblocks_t<slice_blocks> xblocks{ block };
    xdecrypt<uint64_t>(xblocks, slice(schedule));
    block = xblocks.front();
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
