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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_GHASH_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_GHASH_IPP

#include <algorithm>

// Portable multiplication derived in part from BearSSL (ghash_ctmul64.c):
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

// public
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS::
ghash(const block_t& key) NOEXCEPT
  : key_(to_words(key))
{
}

TEMPLATE
constexpr void CLASS::
write(const_byte_span data) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        write_sliced(data);
    }
    else if constexpr (use_clmul)
    {
        write_native(data);
    }
    else
    {
        write_sliced(data);
    }
}

TEMPLATE
constexpr typename CLASS::block_t CLASS::
flush() const NOEXCEPT
{
    return from_words(hash_);
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::words_t CLASS::
to_words(const block_t& block) NOEXCEPT
{
    return { from_big<uint64_t, 0>(block), from_big<uint64_t, 8>(block) };
}

TEMPLATE
constexpr typename CLASS::block_t CLASS::
from_words(const words_t& words) NOEXCEPT
{
    block_t block{};
    to_big<0>(block, words.front());
    to_big<8>(block, words.back());
    return block;
}

// The block at start, zero padded.
TEMPLATE
constexpr typename CLASS::block_t CLASS::
next(const_byte_span data, size_t start) NOEXCEPT
{
    block_t block{};
    const auto size = std::min(block_bytes, data.size() - start);
    for (size_t byte{}; byte < size; ++byte)
        block[byte] = data[start + byte];

    return block;
}

// Portable.
// ----------------------------------------------------------------------------
// Each 64 bit product is computed from four integer multiplications of
// operands masked to every fourth bit, so that carries fall into the holes.
// The high half of the 128 bit product is the bit reversed low half of the
// product of the bit reversed operands.

TEMPLATE
constexpr uint64_t CLASS::
reverse(uint64_t value) NOEXCEPT
{
    const auto swap = [&](uint64_t mask, size_t shift) NOEXCEPT
    {
        const auto low = shift_left(bit_and(value, mask), shift);
        const auto high = bit_and(shift_right(value, shift), mask);
        value = bit_or(low, high);
    };

    swap(0x5555555555555555_u64, 1);
    swap(0x3333333333333333_u64, 2);
    swap(0x0f0f0f0f0f0f0f0f_u64, 4);
    swap(0x00ff00ff00ff00ff_u64, 8);
    swap(0x0000ffff0000ffff_u64, 16);
    return rotr<32>(value);
}

TEMPLATE
constexpr uint64_t CLASS::
multiply(uint64_t x, uint64_t y) NOEXCEPT
{
    constexpr auto m0 = 0x1111111111111111_u64;
    constexpr auto m1 = 0x2222222222222222_u64;
    constexpr auto m2 = 0x4444444444444444_u64;
    constexpr auto m3 = 0x8888888888888888_u64;

    const auto x0 = bit_and(x, m0), x1 = bit_and(x, m1);
    const auto x2 = bit_and(x, m2), x3 = bit_and(x, m3);
    const auto y0 = bit_and(y, m0), y1 = bit_and(y, m1);
    const auto y2 = bit_and(y, m2), y3 = bit_and(y, m3);

    const auto combine = [](auto a, auto b, auto c, auto d) NOEXCEPT
    {
        return bit_xor(bit_xor(a, b), bit_xor(c, d));
    };

    const auto z0 = combine(x0 * y0, x1 * y3, x2 * y2, x3 * y1);
    const auto z1 = combine(x0 * y1, x1 * y0, x2 * y3, x3 * y2);
    const auto z2 = combine(x0 * y2, x1 * y1, x2 * y0, x3 * y3);
    const auto z3 = combine(x0 * y3, x1 * y2, x2 * y1, x3 * y0);

    const auto low = bit_or(bit_and(z0, m0), bit_and(z1, m1));
    const auto high = bit_or(bit_and(z2, m2), bit_and(z3, m3));
    return bit_or(low, high);
}

// y = y * h in GF(2^128), words big-endian (high, low).
TEMPLATE
constexpr void CLASS::
multiply(words_t& y, const words_t& h) NOEXCEPT
{
    const auto h1 = h.front(), h0 = h.back();
    const auto y1 = y.front(), y0 = y.back();
    const auto h0r = reverse(h0), h1r = reverse(h1);
    const auto y0r = reverse(y0), y1r = reverse(y1);
    const auto h2 = bit_xor(h0, h1), h2r = bit_xor(h0r, h1r);
    const auto y2 = bit_xor(y0, y1), y2r = bit_xor(y0r, y1r);

    const auto z0 = multiply(y0, h0);
    const auto z1 = multiply(y1, h1);
    const auto z2 = bit_xor(bit_xor(multiply(y2, h2), z0), z1);
    const auto z0r = multiply(y0r, h0r);
    const auto z1r = multiply(y1r, h1r);
    const auto z2r = bit_xor(bit_xor(multiply(y2r, h2r), z0r), z1r);
    const auto z0h = shift_right(reverse(z0r));
    const auto z1h = shift_right(reverse(z1r));
    const auto z2h = shift_right(reverse(z2r));

    // Shift the 256 bit product left one bit (reflection).
    auto v0 = z0;
    auto v1 = bit_xor(z0h, z2);
    auto v2 = bit_xor(z1, z2h);
    auto v3 = z1h;
    v3 = bit_or(shift_left(v3), shift_right(v2, 63));
    v2 = bit_or(shift_left(v2), shift_right(v1, 63));
    v1 = bit_or(shift_left(v1), shift_right(v0, 63));
    v0 = shift_left(v0);

    // Reduce modulo x^128 + x^7 + x^2 + x + 1.
    const auto down = [](uint64_t v) NOEXCEPT
    {
        const auto a = bit_xor(v, shift_right(v));
        const auto b = bit_xor(shift_right(v, 2), shift_right(v, 7));
        return bit_xor(a, b);
    };

    const auto up = [](uint64_t v) NOEXCEPT
    {
        const auto a = bit_xor(shift_left(v, 63), shift_left(v, 62));
        return bit_xor(a, shift_left(v, 57));
    };

    v2 = bit_xor(v2, down(v0));
    v1 = bit_xor(v1, up(v0));
    v3 = bit_xor(v3, down(v1));
    v2 = bit_xor(v2, up(v1));

    y = { v3, v2 };
}

TEMPLATE
constexpr void CLASS::
write_sliced(const_byte_span data) NOEXCEPT
{
    for (size_t byte{}; byte < data.size(); byte += block_bytes)
    {
        const auto block = to_words(next(data, byte));
        hash_.front() = bit_xor(hash_.front(), block.front());
        hash_.back() = bit_xor(hash_.back(), block.back());
        multiply(hash_, key_);
    }
}

// Native.
// ----------------------------------------------------------------------------
// Operands are byte reflected, so that the reduction applies to the bit
// reflected field representation. Four blocks are multiplied by descending
// powers of the key and summed before a single reduction.

TEMPLATE
INLINE xint128_t CLASS::
load(const block_t& block) NOEXCEPT
{
    return aes::reflect(f::load(array_cast<xint128_t>(block).front()));
}

TEMPLATE
INLINE typename CLASS::block_t CLASS::
store(xint128_t value) NOEXCEPT
{
    block_t block{};
    f::store(array_cast<xint128_t>(block).front(), aes::reflect(value));
    return block;
}

TEMPLATE
INLINE xint128_t CLASS::
multiply(xint128_t a, xint128_t b) NOEXCEPT
{
    xint128_t lo{}, hi{};
    aes::multiply(lo, hi, a, b);
    return aes::reduce(lo, hi);
}

TEMPLATE
void CLASS::
write_native(const_byte_span data) NOEXCEPT
{
    constexpr auto size = aggregate * block_bytes;
    const auto h1 = load(from_words(key_));
    auto hash = load(from_words(hash_));
    size_t byte{};

    if (data.size() >= size)
    {
        const auto h2 = multiply(h1, h1);
        const auto h3 = multiply(h2, h1);
        const auto h4 = multiply(h3, h1);

        for (; (data.size() - byte) >= size; byte += size)
        {
            xint128_t lo{}, hi{}, lo1{}, hi1{}, lo2{}, hi2{}, lo3{}, hi3{};
            const auto& blocks = unsafe_array_cast<block_t, aggregate>(
                std::next(data.data(), byte));

            const auto b0 = aes::sum(load(blocks[0]), hash);
            aes::multiply(lo, hi, b0, h4);
            aes::multiply(lo1, hi1, load(blocks[1]), h3);
            aes::multiply(lo2, hi2, load(blocks[2]), h2);
            aes::multiply(lo3, hi3, load(blocks[3]), h1);
            lo = aes::sum(aes::sum(lo, lo1), aes::sum(lo2, lo3));
            hi = aes::sum(aes::sum(hi, hi1), aes::sum(hi2, hi3));
            hash = aes::reduce(lo, hi);
        }
    }

    for (; byte < data.size(); byte += block_bytes)
        hash = multiply(aes::sum(hash, load(next(data, byte))), h1);

    hash_ = to_words(store(hash));
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
