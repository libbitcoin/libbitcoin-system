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
#include <bitcoin/system/crypto/x25519.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc7748
// tweetnacl.cr.yp.to (field arithmetic)
// [Bernstein, Josefsson, Liusvaara]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// field
// ----------------------------------------------------------------------------

// Propagate limb carries, folding top limb overflow (p = 2^255 - 19).
constexpr void x25519::carry(field& out) NOEXCEPT
{
    for (size_t index{}; index < limb_count; ++index)
    {
        out[index] += (1_i64 << 16);
        const auto carried = out[index] >> 16;

        if (index < sub1(limb_count))
            out[add1(index)] += carried - 1;
        else
            out[0] += 38 * (carried - 1);

        out[index] -= (carried << 16);
    }
}

// Constant time conditional swap (bit is zero or one).
constexpr void x25519::select(field& p, field& q, int64_t bit) NOEXCEPT
{
    const auto mask = -bit;

    for (size_t index{}; index < limb_count; ++index)
    {
        const auto swap = mask & (p[index] ^ q[index]);
        p[index] ^= swap;
        q[index] ^= swap;
    }
}

// Fully reduce and serialize little-endian.
void x25519::pack(key& out, const field& in) NOEXCEPT
{
    auto t = in;
    carry(t);
    carry(t);
    carry(t);

    // Subtract p twice, keeping the result only if non-negative.
    for (size_t pass{}; pass < 2; ++pass)
    {
        field m{};
        m[0] = t[0] - 0xffed;

        for (size_t index{ 1 }; index < sub1(limb_count); ++index)
        {
            m[index] = t[index] - 0xffff - ((m[sub1(index)] >> 16) & 1);
            m[sub1(index)] &= 0xffff;
        }

        m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
        const auto borrow = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        select(t, m, 1 - borrow);
    }

    std_array<uint16_t, limb_count> words{};
    for (size_t index{}; index < limb_count; ++index)
        words[index] = possible_narrow_and_sign_cast<uint16_t>(t[index]);

    to_little_endians(array_cast<uint16_t>(out), words);
}

// Deserialize little-endian, masking the unused top bit.
void x25519::unpack(field& out, const key& in) NOEXCEPT
{
    std_array<uint16_t, limb_count> words{};
    from_little_endians(words, array_cast<uint16_t>(in));

    for (size_t index{}; index < limb_count; ++index)
        out[index] = words[index];

    out[15] &= 0x7fff;
}

constexpr void x25519::add(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limb_count; ++index)
        out[index] = a[index] + b[index];
}

constexpr void x25519::subtract(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limb_count; ++index)
        out[index] = a[index] - b[index];
}

constexpr void x25519::multiply(field& out, const field& a,
    const field& b) NOEXCEPT
{
    std_array<int64_t, sub1(2 * limb_count)> product{};

    for (size_t i{}; i < limb_count; ++i)
        for (size_t j{}; j < limb_count; ++j)
            product[i + j] += a[i] * b[j];

    // Reduce the upper half by 2^256 = 38 (mod p).
    for (size_t index{}; index < sub1(limb_count); ++index)
        product[index] += 38 * product[index + limb_count];

    for (size_t index{}; index < limb_count; ++index)
        out[index] = product[index];

    carry(out);
    carry(out);
}

constexpr void x25519::square(field& out, const field& a) NOEXCEPT
{
    multiply(out, a, a);
}

// Inversion by exponentiation to p - 2 (Fermat), constant time.
constexpr void x25519::invert(field& out, const field& a) NOEXCEPT
{
    auto c = a;

    for (size_t index{ 254 }; index > 0; --index)
    {
        const auto exponent = sub1(index);
        square(c, c);

        if (exponent != 2 && exponent != 4)
            multiply(c, c, a);
    }

    out = c;
}

// x25519
// ----------------------------------------------------------------------------

bool x25519::multiply(key& out, const key& scalar, const key& point) NOEXCEPT
{
    // The Montgomery ladder constant (a - 2) / 4 = 121665 in radix 2^16.
    constexpr field a24{ 0xdb41, 1 };

    // Clamp the scalar.
    auto clamped = scalar;
    clamped[0] &= 248;
    clamped[31] &= 127;
    clamped[31] |= 64;

    field x{};
    unpack(x, point);

    field a{};
    field b = x;
    field c{};
    field d{};
    field e{};
    field f{};
    a[0] = 1;
    d[0] = 1;

    // Montgomery ladder over the scalar bits, from 254 down to 0.
    for (size_t index{ 255 }; index > 0; --index)
    {
        const auto bit = sub1(index);
        const int64_t swap = (clamped[bit >> 3] >> (bit & 7)) & 1;

        select(a, b, swap);
        select(c, d, swap);
        add(e, a, c);
        subtract(a, a, c);
        add(c, b, d);
        subtract(b, b, d);
        square(d, e);
        square(f, a);
        multiply(a, c, a);
        multiply(c, b, e);
        add(e, a, c);
        subtract(a, a, c);
        square(b, a);
        subtract(c, d, f);
        multiply(a, c, a24);
        add(a, a, d);
        multiply(c, c, a);
        multiply(a, d, f);
        multiply(d, b, x);
        square(b, e);
        select(a, b, swap);
        select(c, d, swap);
    }

    invert(c, c);
    multiply(a, a, c);
    pack(out, a);
    clamped = {};

    // An all zero result indicates a low order point.
    return std::any_of(out.begin(), out.end(), [](uint8_t byte) NOEXCEPT
    {
        return !is_zero(byte);
    });
}

bool x25519::multiply(key& out, const key& scalar) NOEXCEPT
{
    constexpr key base{ 9 };
    return multiply(out, scalar, base);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
