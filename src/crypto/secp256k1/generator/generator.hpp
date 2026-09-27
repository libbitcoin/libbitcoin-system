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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_GENERATOR_GENERATOR_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_GENERATOR_GENERATOR_HPP

// The generator table is computed at compile time in slices, one translation
// unit each, as compiler memory grows with the arithmetic evaluated in a unit.
// This header includes no library headers, as each unit otherwise pays for
// parsing them.

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace precompute {

using word = std::uint64_t;
using size = std::size_t;

/// Table shape, matching the algorithm class.
constexpr size slice_size = 512;
constexpr size block_size = 16;
constexpr size limbs = 5;
constexpr size slice_blocks = slice_size / block_size;
constexpr size block_words = 2 * limbs * block_size;
constexpr size table_words = slice_blocks * block_words;
constexpr size slice_words = 2 * table_words;

/// Field element, four 64 bit words (normal).
struct element
{
    word value[4]{};
};

struct affine
{
    element x{};
    element y{};
};

struct jacobian
{
    element x{};
    element y{};
    element z{};
};

constexpr element prime
{
    { 0xfffffffefffffc2f, 0xffffffffffffffff, 0xffffffffffffffff,
      0xffffffffffffffff }
};

constexpr word fold = 0x00000001000003d1;

constexpr affine generator
{
    { { 0x59f2815b16f81798, 0x029bfcdb2dce28d9, 0x55a06295ce870b07,
        0x79be667ef9dcbbac } },
    { { 0x9c47d08ffb10d4b8, 0xfd17b448a6855419, 0x5da4fbfc0e1108a8,
        0x483ada7726a3c465 } }
};

constexpr affine twice
{
    { { 0xabac09b95c709ee5, 0x5c778e4b8cef3ca7, 0x3045406e95c07cd8,
        0xc6047f9441ed7d6d } },
    { { 0x236431a950cfe52a, 0xf7f632653266d0e1, 0xa3c58419466ceaee,
        0x1ae168fea63dc339 } }
};

constexpr element beta
{
    { 0xc1396c28719501ee, 0x9cf0497512f58995, 0x6e64479eac3434e9,
      0x7ae96a2b657c0710 }
};

// Field arithmetic.
// ----------------------------------------------------------------------------

// high:low = a * b.
constexpr void multiply(word& high, word& low, word a, word b) noexcept
{
    constexpr word half = 0xffffffff;
    const auto al = a & half, ah = a >> 32;
    const auto bl = b & half, bh = b >> 32;
    const auto ll = al * bl, lh = al * bh, hl = ah * bl, hh = ah * bh;
    const auto middle = (ll >> 32) + (lh & half) + (hl & half);
    low = (middle << 32) | (ll & half);
    high = hh + (lh >> 32) + (hl >> 32) + (middle >> 32);
}

constexpr bool below_prime(const element& a) noexcept
{
    for (size i = 4; i-- > 0;)
        if (a.value[i] != prime.value[i])
            return a.value[i] < prime.value[i];

    return false;
}

// r = a - p, for a not below p.
constexpr element reduce(const element& a) noexcept
{
    element r{};
    word borrow{};
    for (size i = 0; i < 4; ++i)
    {
        const auto subtrahend = prime.value[i] + borrow;
        borrow = (a.value[i] < subtrahend || subtrahend < borrow) ? 1 : 0;
        r.value[i] = a.value[i] - subtrahend;
    }

    return r;
}

// r = a + carry * 2^256, reduced.
constexpr element fold_carry(element r, word carry) noexcept
{
    while (carry != 0)
    {
        word high{}, low{};
        multiply(high, low, carry, fold);
        word next{};
        for (size i = 0; i < 4; ++i)
        {
            const auto addend = i == 0 ? low : (i == 1 ? high : 0);
            const auto sum = r.value[i] + addend;
            const word over = sum < addend ? 1 : 0;
            r.value[i] = sum + next;
            next = over + (r.value[i] < sum ? 1 : 0);
        }

        carry = next;
    }

    return below_prime(r) ? r : reduce(r);
}

constexpr element add(const element& a, const element& b) noexcept
{
    element r{};
    word carry{};
    for (size i = 0; i < 4; ++i)
    {
        const auto sum = a.value[i] + b.value[i];
        const word over = sum < a.value[i] ? 1 : 0;
        r.value[i] = sum + carry;
        carry = over + (r.value[i] < sum ? 1 : 0);
    }

    return fold_carry(r, carry);
}

constexpr element negate(const element& a) noexcept
{
    element r{};
    word borrow{};
    auto zero = true;
    for (size i = 0; i < 4; ++i)
    {
        zero = zero && a.value[i] == 0;
        const auto subtrahend = a.value[i] + borrow;
        borrow = (prime.value[i] < subtrahend || subtrahend < borrow) ? 1 : 0;
        r.value[i] = prime.value[i] - subtrahend;
    }

    return zero ? a : r;
}

constexpr element subtract(const element& a, const element& b) noexcept
{
    return add(a, negate(b));
}

constexpr element multiply(const element& a, const element& b) noexcept
{
    word product[8]{};
    for (size i = 0; i < 4; ++i)
    {
        word carry{};
        for (size j = 0; j < 4; ++j)
        {
            word high{}, low{};
            multiply(high, low, a.value[i], b.value[j]);
            low += carry;
            high += low < carry ? 1 : 0;
            product[i + j] += low;
            high += product[i + j] < low ? 1 : 0;
            carry = high;
        }

        product[i + 4] = carry;
    }

    // Words above 2^256 fold by 2^256 mod p.
    element r{};
    word carry{};
    for (size i = 0; i < 4; ++i)
    {
        word high{}, low{};
        multiply(high, low, product[i + 4], fold);
        low += carry;
        high += low < carry ? 1 : 0;
        r.value[i] = product[i] + low;
        high += r.value[i] < low ? 1 : 0;
        carry = high;
    }

    return fold_carry(r, carry);
}

constexpr element square(const element& a) noexcept
{
    return multiply(a, a);
}

// a^(p - 2).
constexpr element inverse(const element& a) noexcept
{
    constexpr element exponent
    {
        { 0xfffffffefffffc2d, 0xffffffffffffffff, 0xffffffffffffffff,
          0xffffffffffffffff }
    };

    element r{ { 1 } };
    for (size i = 4; i-- > 0;)
        for (size bit = 64; bit-- > 0;)
        {
            r = square(r);
            if (((exponent.value[i] >> bit) & 1) != 0)
                r = multiply(r, a);
        }

    return r;
}

// Group arithmetic.
// ----------------------------------------------------------------------------

constexpr jacobian twofold(const jacobian& a) noexcept
{
    const auto yy = square(a.y);
    const auto xy = multiply(a.x, yy);
    const auto s = add(add(xy, xy), add(xy, xy));
    const auto xx = square(a.x);
    const auto m = add(add(xx, xx), xx);
    const auto x3 = subtract(square(m), add(s, s));
    auto y4 = square(yy);
    y4 = add(y4, y4);
    y4 = add(y4, y4);
    y4 = add(y4, y4);
    const auto y3 = subtract(multiply(m, subtract(s, x3)), y4);
    const auto z3 = multiply(add(a.y, a.y), a.z);
    return { x3, y3, z3 };
}

// a + b, for a not b or -b.
constexpr jacobian sum(const jacobian& a, const affine& b) noexcept
{
    const auto zz = square(a.z);
    const auto u2 = multiply(b.x, zz);
    const auto s2 = multiply(b.y, multiply(zz, a.z));
    const auto h = subtract(u2, a.x);
    const auto r = subtract(s2, a.y);
    const auto hh = square(h);
    const auto hhh = multiply(h, hh);
    const auto v = multiply(a.x, hh);
    const auto x3 = subtract(subtract(square(r), hhh), add(v, v));
    const auto y3 = subtract(multiply(r, subtract(v, x3)),
        multiply(a.y, hhh));
    const auto z3 = multiply(a.z, h);
    return { x3, y3, z3 };
}

// Table computation.
// ----------------------------------------------------------------------------

/// Odd multiples of a block and running products of their z in the slice.
struct forward
{
    jacobian points[block_size]{};
    element products[block_size]{};
};

/// Affine odd multiples of a block, their x * beta, and inverse of the product
/// before the block.
struct backward
{
    affine points[block_size]{};
    element mapped[block_size]{};
    element inverse{};
};

// (2 * Slice * slice_size + 1)G, the first multiple of the slice.
template <size Slice>
constexpr jacobian first() noexcept
{
    constexpr auto multiple = 2 * Slice * slice_size + 1;

    jacobian r{ generator.x, generator.y, { { 1 } } };
    auto bit = size{ 63 };
    while (((multiple >> bit) & 1) == 0)
        --bit;

    while (bit-- > 0)
    {
        r = twofold(r);
        if (((multiple >> bit) & 1) != 0)
            r = sum(r, generator);
    }

    return r;
}

template <size Slice, size Block>
constexpr forward forwarded() noexcept;

template <size Slice, size Block>
constexpr backward backwarded() noexcept;

template <size Slice, size Block>
constexpr forward forwards = forwarded<Slice, Block>();

template <size Slice, size Block>
constexpr backward backwards = backwarded<Slice, Block>();

template <size Slice, size Block>
constexpr forward forwarded() noexcept
{
    forward out{};
    if constexpr (Block == 0)
    {
        out.points[0] = first<Slice>();
        out.products[0] = out.points[0].z;
    }
    else
    {
        const auto& prior = forwards<Slice, Block - 1>;
        out.points[0] = sum(prior.points[block_size - 1], twice);
        out.products[0] = multiply(prior.products[block_size - 1],
            out.points[0].z);
    }

    for (size point = 1; point < block_size; ++point)
    {
        out.points[point] = sum(out.points[point - 1], twice);
        out.products[point] = multiply(out.products[point - 1],
            out.points[point].z);
    }

    return out;
}

// The block after the last holds only the inverse of the slice product, and
// each block unwinds its prefix products into affine points.
template <size Slice, size Block>
constexpr backward backwarded() noexcept
{
    backward out{};
    if constexpr (Block == slice_blocks)
    {
        const auto& last = forwards<Slice, slice_blocks - 1>;
        out.inverse = inverse(last.products[block_size - 1]);
    }
    else
    {
        const auto& chunk = forwards<Slice, Block>;
        out.inverse = backwards<Slice, Block + 1>.inverse;
        for (size point = block_size; point-- > 0;)
        {
            element inverse_z{};
            if (point != 0)
                inverse_z = multiply(out.inverse, chunk.products[point - 1]);
            else if constexpr (Block != 0)
                inverse_z = multiply(out.inverse,
                    forwards<Slice, Block - 1>.products[block_size - 1]);
            else
                inverse_z = out.inverse;

            out.inverse = multiply(out.inverse, chunk.points[point].z);
            const auto zz = square(inverse_z);
            out.points[point] =
            {
                multiply(chunk.points[point].x, zz),
                multiply(chunk.points[point].y, multiply(zz, inverse_z))
            };
            out.mapped[point] = multiply(out.points[point].x, beta);
        }
    }

    return out;
}

// Five 52 bit limbs of a normal element.
constexpr void limbs52(word* out, size stride, const element& a) noexcept
{
    constexpr word mask = 0x000fffffffffffff;
    out[0 * stride] = a.value[0] & mask;
    out[1 * stride] = ((a.value[0] >> 52) | (a.value[1] << 12)) & mask;
    out[2 * stride] = ((a.value[1] >> 40) | (a.value[2] << 24)) & mask;
    out[3 * stride] = ((a.value[2] >> 28) | (a.value[3] << 36)) & mask;
    out[4 * stride] = a.value[3] >> 16;
}

// Block b of a table holds x limbs then y limbs, each as a column of the block
// size, and the endomorphism table (x * beta) follows the generator table.
template <size Slice, size Block>
constexpr void emit(std::array<word, slice_words>& out) noexcept
{
    const auto& chunk = backwards<Slice, Block>;
    const auto base = Block * block_words;
    for (size point = 0; point < block_size; ++point)
    {
        const auto& entry = chunk.points[point];
        const auto& mapped = chunk.mapped[point];
        const auto at = base + point;
        limbs52(&out[at], block_size, entry.x);
        limbs52(&out[at + limbs * block_size], block_size, entry.y);
        limbs52(&out[table_words + at], block_size, mapped);
        limbs52(&out[table_words + at + limbs * block_size], block_size,
            entry.y);
    }
}

template <size Slice, size... Blocks>
constexpr std::array<word, slice_words> slice(
    std::index_sequence<Blocks...>) noexcept
{
    std::array<word, slice_words> out{};
    (emit<Slice, Blocks>(out), ...);
    return out;
}

template <size Slice>
constexpr std::array<word, slice_words> slice() noexcept
{
    return slice<Slice>(std::make_index_sequence<slice_blocks>{});
}

} // namespace precompute

/// Generator table slices, each defined in its own translation unit.
extern const std::array<precompute::word, precompute::slice_words>
    generator_slice_00, generator_slice_01, generator_slice_02,
    generator_slice_03, generator_slice_04, generator_slice_05,
    generator_slice_06, generator_slice_07, generator_slice_08,
    generator_slice_09, generator_slice_10, generator_slice_11,
    generator_slice_12, generator_slice_13, generator_slice_14,
    generator_slice_15;

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
