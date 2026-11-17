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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_CURVE25519_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_CURVE25519_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// tweetnacl.cr.yp.to (field arithmetic)
// [Bernstein, van Gastel, Janssen, Lange, Schwabe, Smetsers]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Propagate limb carries, folding top limb overflow (p = 2^255 - 19).
constexpr void curve25519::carry(field& out) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
    {
        // The radix is added so that the floored carry of a negative limb is
        // one less than that of a positive limb, which is then subtracted.
        // The shift of a signed limb is arithmetic (floored, branchless).
        out[index] += radix;
        const auto carried = out[index] >> radix_bits;

        if (index < sub1(limbs))
            out[add1(index)] += sub1(carried);
        else
            out[zero] += sub1(carried) * fold;

        out[index] -= carried * radix;
    }
}

// Constant time conditional swap (bit is zero or one).
constexpr void curve25519::select(field& p, field& q, int64_t bit) NOEXCEPT
{
    const auto mask = opaque(negate(bit));

    for (size_t index{}; index < limbs; ++index)
    {
        const auto swap = bit_and(mask, bit_xor(p[index], q[index]));
        p[index] ^= swap;
        q[index] ^= swap;
    }
}

constexpr void curve25519::add(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
        out[index] = (a[index] + b[index]);
}

constexpr void curve25519::subtract(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
        out[index] = (a[index] - b[index]);
}

constexpr void curve25519::multiply(field& out, const field& a,
    const field& b) NOEXCEPT
{
    std_array<int64_t, sub1(two * limbs)> product{};

    for (size_t i{}; i < limbs; ++i)
        for (size_t j{}; j < limbs; ++j)
            product[i + j] += (a[i] * b[j]);

    // Reduce the upper half, as 2^256 = 38 (mod p).
    for (size_t index{}; index < sub1(limbs); ++index)
        product[index] += (product[index + limbs] * fold);

    for (size_t index{}; index < limbs; ++index)
        out[index] = product[index];

    carry(out);
    carry(out);
}

constexpr void curve25519::square(field& out, const field& a) NOEXCEPT
{
    multiply(out, a, a);
}

// Inversion by exponentiation to p - 2 (Fermat), constant time.
constexpr void curve25519::invert(field& out, const field& a) NOEXCEPT
{
    auto c = a;

    for (size_t index{ 254 }; index > zero; --index)
    {
        const auto exponent = sub1(index);
        square(c, c);

        if (exponent != 2 && exponent != 4)
            multiply(c, c, a);
    }

    out = c;
}

// Fully reduce and serialize little-endian.
inline void curve25519::pack(element& out, const field& in) NOEXCEPT
{
    auto t = in;
    carry(t);
    carry(t);
    carry(t);

    // Subtract p twice, keeping the result only if non-negative.
    for (size_t pass{}; pass < two; ++pass)
    {
        // A negative limb borrows from the next and is normalized to radix.
        field m{};
        m[zero] = t[zero] - prime_low;

        for (auto index = one; index < sub1(limbs); ++index)
        {
            auto& previous = m[sub1(index)];
            m[index] = t[index] - prime_middle - to_int(is_negative(previous));
            previous = bit_and(previous, sub1(radix));
        }

        auto& penultimate = m[sub1(sub1(limbs))];
        auto& last = m[sub1(limbs)];
        last = t[sub1(limbs)] - prime_high - to_int(is_negative(penultimate));
        penultimate = bit_and(penultimate, sub1(radix));

        // The subtraction is kept only if the top limb did not borrow.
        select(t, m, to_int(!is_negative(last)));
    }

    std_array<uint16_t, limbs> words{};
    for (size_t index{}; index < limbs; ++index)
        words[index] = possible_narrow_and_sign_cast<uint16_t>(t[index]);

    to_little_endians(out, words);
}

// Deserialize little-endian, masking the unused top bit.
inline void curve25519::unpack(field& out, const element& in) NOEXCEPT
{
    std_array<uint16_t, limbs> words{};
    from_little_endians(words, in);

    for (size_t index{}; index < limbs; ++index)
        out[index] = words[index];

    set_right_into(out[sub1(limbs)], sub1(radix_bits), false);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
