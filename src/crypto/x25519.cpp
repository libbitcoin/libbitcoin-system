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

#include <bitcoin/system/crypto/maybe_random.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc7748
// tweetnacl.cr.yp.to (field arithmetic)
// [Bernstein, Josefsson, Liusvaara]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Limbs are radix 2^16 over int64, and may be negative between carries. The
// limb count and the radix width are both sixteen, but are not related.
constexpr auto radix_bits = 16_size;
constexpr auto radix = bit_right<int64_t>(radix_bits);

// The field prime p = 2^255 - 19 in radix 2^16 limbs (low, middle, high).
constexpr int64_t prime_low = 0xffed;
constexpr int64_t prime_middle = 0xffff;
constexpr int64_t prime_high = 0x7fff;

// The fold of a limb carried beyond 2^256, which is 38 = 2 * 19 (mod p).
constexpr int64_t fold = 38;

// x25519
// ----------------------------------------------------------------------------

bool x25519::multiply(key& out, const key& scalar, const key& point) NOEXCEPT
{
    // The Montgomery ladder constant (a - 2) / 4 = 121665 in radix 2^16.
    constexpr field a24{ 0xdb41, 1 };

    // Clamp the scalar: clear the low three bits, clear the high bit and set
    // the next highest bit (rfc7748 decodeScalar25519).
    auto clamped = scalar;
    mask_right_into(clamped[0], 3);
    mask_left_into(clamped[31], 1);
    set_right_into(clamped[31], 6);

    field x{};
    unpack(x, point);

    field a{};
    field b = x;
    field c{};
    field d{};
    field e{};
    field f{};
    a[zero] = 1;
    d[zero] = 1;

    // Montgomery ladder over the scalar bits, from 254 down to 0.
    for (size_t index{ 255 }; index > zero; --index)
    {
        const auto bit = sub1(index);
        const auto byte = clamped[shift_right(bit, 3)];
        const auto swap = to_int<int64_t>(get_right(byte, bit_and(bit, 7_size)));

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

// This sacrifices about 1.1 bits of entropy, out of 251 (acceptable).
// About one third of Z85 key encodings contain a hash sign (a config comment).
static bool allowed(const x25519::key& key, bool sanitize) NOEXCEPT
{
    if (!sanitize)
        return true;

    std::string encoded{};
    return encode_base85(encoded, key) && !contains(encoded, '#');
}

void x25519::generate(key& private_key, key& public_key, bool sanitize) NOEXCEPT
{
    // A low order result is astronomically improbable.
    do
    {
        maybe_random::fill(private_key);
    }
    while (!multiply(public_key, private_key) ||
        !allowed(private_key, sanitize) || !allowed(public_key, sanitize));
}

// field
// ----------------------------------------------------------------------------

// Propagate limb carries, folding top limb overflow (p = 2^255 - 19).
constexpr void x25519::carry(field& out) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
    {
        // The radix is added so that the floored carry of a negative limb is
        // one less than that of a positive limb, which is then subtracted.
        out[index] += radix;
        const auto carried = floored_divide(out[index], radix);

        if (index < sub1(limbs))
            out[add1(index)] += sub1(carried);
        else
            out[zero] += sub1(carried) * fold;

        out[index] -= carried * radix;
    }
}

// Constant time conditional swap (bit is zero or one).
constexpr void x25519::select(field& p, field& q, int64_t bit) NOEXCEPT
{
    const auto mask = negate(bit);

    for (size_t index{}; index < limbs; ++index)
    {
        const auto swap = bit_and(mask, bit_xor(p[index], q[index]));
        p[index] ^= swap;
        q[index] ^= swap;
    }
}

constexpr void x25519::add(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
        out[index] = (a[index] + b[index]);
}

constexpr void x25519::subtract(field& out, const field& a,
    const field& b) NOEXCEPT
{
    for (size_t index{}; index < limbs; ++index)
        out[index] = (a[index] - b[index]);
}

constexpr void x25519::multiply(field& out, const field& a,
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

constexpr void x25519::square(field& out, const field& a) NOEXCEPT
{
    multiply(out, a, a);
}

// Inversion by exponentiation to p - 2 (Fermat), constant time.
constexpr void x25519::invert(field& out, const field& a) NOEXCEPT
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
void x25519::pack(key& out, const field& in) NOEXCEPT
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
            previous = floored_modulo(previous, radix);
        }

        auto& penultimate = m[sub1(sub1(limbs))];
        auto& last = m[sub1(limbs)];
        last = t[sub1(limbs)] - prime_high - to_int(is_negative(penultimate));
        penultimate = floored_modulo(penultimate, radix);

        // The subtraction is kept only if the top limb did not borrow.
        select(t, m, to_int(!is_negative(last)));
    }

    std_array<uint16_t, limbs> words{};
    for (size_t index{}; index < limbs; ++index)
        words[index] = possible_narrow_and_sign_cast<uint16_t>(t[index]);

    to_little_endians(array_cast<uint16_t>(out), words);
}

// Deserialize little-endian, masking the unused top bit.
void x25519::unpack(field& out, const key& in) NOEXCEPT
{
    std_array<uint16_t, limbs> words{};
    from_little_endians(words, array_cast<uint16_t>(in));

    for (size_t index{}; index < limbs; ++index)
        out[index] = words[index];

    set_right_into(out[sub1(limbs)], sub1(radix_bits), false);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
