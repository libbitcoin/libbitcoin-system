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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_INVERSION_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_INVERSION_IPP

// Based on:
// gcd.cr.yp.to/safegcd-20190413.pdf
// github.com/bitcoin-core/secp256k1 (modinv64, variable time)

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Inversion.
// ----------------------------------------------------------------------------
// protected

// Starting from f = m, g = x, d = 0, e = 1, each batch of divsteps applies a
// transition matrix to (f, g) and (d, e), preserving d * x = f and e * x = g
// modulo m. When g reaches zero, f is +/-1 and d is the signed inverse.
constexpr void algorithm::invert(signed62_t& x, const modulus_t& m) NOEXCEPT
{
    signed62_t d{}, e{ 1 }, f{ m.value }, g{ x };
    auto length = array_count<signed62_t>;
    int64_t eta{ -1 };

    while (true)
    {
        transition_t t{};
        eta = divsteps(t, eta, to_unsigned(f[0]), to_unsigned(g[0]));

        update_de(d, e, t, m);
        update_fg(f, g, length, t);

        if (is_zero(g[0]))
        {
            int64_t rest{};
            for (auto limb = one; limb < length; ++limb)
                rest |= g[limb];

            if (is_zero(rest))
                break;
        }

        // Shorten when the top limbs of f and g are both sign extension.
        const auto top_f = f[sub1(length)];
        const auto top_g = g[sub1(length)];
        if (length > one && is_zero((top_f ^ (top_f >> 63)) |
            (top_g ^ (top_g >> 63))))
        {
            f[length - two] |= to_signed(shift_left(to_unsigned(top_f), 62));
            g[length - two] |= to_signed(shift_left(to_unsigned(top_g), 62));
            --length;
        }
    }

    normalize(d, f[sub1(length)], m);
    x = d;
}

// Inversion internals.
// ----------------------------------------------------------------------------
// protected

// Runs of zeros in g are divided out at once, and otherwise up to six (eta
// negative) or four low bits of g are cancelled by a multiple of f.
constexpr int64_t algorithm::divsteps(transition_t& t, int64_t eta,
    uint64_t f, uint64_t g) NOEXCEPT
{
    constexpr size_t steps = 62;
    uint64_t u{ 1 }, v{}, q{}, r{ 1 };
    auto remaining = steps;

    while (true)
    {
        const auto zeros = right_zeros(bit_or(g,
            shift_left(max_uint64, remaining)));

        g >>= zeros;
        u <<= zeros;
        v <<= zeros;
        eta -= possible_narrow_and_sign_cast<int64_t>(zeros);
        remaining -= zeros;
        if (is_zero(remaining))
            break;

        uint64_t w{};
        if (is_negative(eta))
        {
            eta = twos_complement(eta);
            auto swap = f;
            f = g;
            g = twos_complement(swap);
            swap = u;
            u = q;
            q = twos_complement(swap);
            swap = v;
            v = r;
            r = twos_complement(swap);

            const auto limit = std::min(add1(
                possible_narrow_and_sign_cast<size_t>(eta)), remaining);
            const auto mask = bit_and(unmask_right<uint64_t>(limit), 63_u64);
            w = bit_and(f * g * (f * f - 2_u64), mask);
        }
        else
        {
            const auto limit = std::min(add1(
                possible_narrow_and_sign_cast<size_t>(eta)), remaining);
            const auto mask = bit_and(unmask_right<uint64_t>(limit), 15_u64);
            w = f + shift_left(bit_and(add1(f), 4_u64));
            w = bit_and(twos_complement(w) * g, mask);
        }

        g += f * w;
        q += u * w;
        r += v * w;
    }

    t =
    {
        to_signed(u),
        to_signed(v),
        to_signed(q),
        to_signed(r)
    };

    return eta;
}

// Adds multiples of m that make the low 62 bits of t * (d, e) zero, then
// shifts them out, keeping d and e in (-2m, m).
constexpr void algorithm::update_de(signed62_t& d, signed62_t& e,
    const transition_t& t, const modulus_t& m) NOEXCEPT
{
    constexpr auto mask = unmask_right<uint64_t>(62);

    const auto sign_d = d[4] >> 63;
    const auto sign_e = e[4] >> 63;
    auto md = (t.u & sign_d) + (t.v & sign_e);
    auto me = (t.q & sign_d) + (t.r & sign_e);

    signed128_t cd{}, ce{};
    multiply_add(cd, t.u, d[0]);
    multiply_add(cd, t.v, e[0]);
    multiply_add(ce, t.q, d[0]);
    multiply_add(ce, t.r, e[0]);

    md -= to_signed(bit_and(m.inverse * low(cd) + to_unsigned(md), mask));
    me -= to_signed(bit_and(m.inverse * low(ce) + to_unsigned(me), mask));

    multiply_add(cd, m.value[0], md);
    multiply_add(ce, m.value[0], me);
    shift(cd);
    shift(ce);

    for (auto limb = one; limb < d.size(); ++limb)
    {
        multiply_add(cd, t.u, d[limb]);
        multiply_add(cd, t.v, e[limb]);
        multiply_add(ce, t.q, d[limb]);
        multiply_add(ce, t.r, e[limb]);
        multiply_add(cd, m.value[limb], md);
        multiply_add(ce, m.value[limb], me);
        d[sub1(limb)] = to_signed(bit_and(low(cd), mask));
        e[sub1(limb)] = to_signed(bit_and(low(ce), mask));
        shift(cd);
        shift(ce);
    }

    d[4] = to_signed(low(cd));
    e[4] = to_signed(low(ce));
}

constexpr void algorithm::update_fg(signed62_t& f, signed62_t& g,
    size_t length, const transition_t& t) NOEXCEPT
{
    constexpr auto mask = unmask_right<uint64_t>(62);

    signed128_t cf{}, cg{};
    multiply_add(cf, t.u, f[0]);
    multiply_add(cf, t.v, g[0]);
    multiply_add(cg, t.q, f[0]);
    multiply_add(cg, t.r, g[0]);
    shift(cf);
    shift(cg);

    for (auto limb = one; limb < length; ++limb)
    {
        multiply_add(cf, t.u, f[limb]);
        multiply_add(cf, t.v, g[limb]);
        multiply_add(cg, t.q, f[limb]);
        multiply_add(cg, t.r, g[limb]);
        f[sub1(limb)] = to_signed(bit_and(low(cf), mask));
        g[sub1(limb)] = to_signed(bit_and(low(cg), mask));
        shift(cf);
        shift(cg);
    }

    f[sub1(length)] = to_signed(low(cf));
    g[sub1(length)] = to_signed(low(cg));
}

// From (-2m, m) to [0, m), negated if sign is negative.
constexpr void algorithm::normalize(signed62_t& r, int64_t sign,
    const modulus_t& m) NOEXCEPT
{
    constexpr auto mask = to_signed(unmask_right<uint64_t>(62));

    auto negative = r[4] >> 63;
    for (size_t limb{}; limb < r.size(); ++limb)
        r[limb] += bit_and(m.value[limb], negative);

    const auto negation = sign >> 63;
    for (size_t limb{}; limb < r.size(); ++limb)
        r[limb] = bit_xor(r[limb], negation) - negation;

    for (size_t limb{}; limb < sub1(r.size()); ++limb)
    {
        r[add1(limb)] += r[limb] >> 62;
        r[limb] = bit_and(r[limb], mask);
    }

    negative = r[4] >> 63;
    for (size_t limb{}; limb < r.size(); ++limb)
        r[limb] += bit_and(m.value[limb], negative);

    for (size_t limb{}; limb < sub1(r.size()); ++limb)
    {
        r[add1(limb)] += r[limb] >> 62;
        r[limb] = bit_and(r[limb], mask);
    }
}

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

// r = a * b + r, as signed 128 bit values.
INLINE constexpr void algorithm::multiply_add(signed128_t& r, int64_t a,
    int64_t b) NOEXCEPT
{
#if defined(__SIZEOF_INT128__)
    r += signed128_t{ a } * b;
#else
    const auto left = to_unsigned(a);
    const auto right = to_unsigned(b);

    uint64_t upper{}, lower{};
    mul_wide(upper, lower, left, right);
    if (is_negative(a))
        upper -= right;

    if (is_negative(b))
        upper -= left;

    const auto carry = add_carry(r.low, r.low, lower, false);
    add_carry(r.high, r.high, upper, carry);
#endif
}

// r = r / 2^62, rounded toward negative infinity.
INLINE constexpr void algorithm::shift(signed128_t& r) NOEXCEPT
{
#if defined(__SIZEOF_INT128__)
    r >>= 62;
#else
    r.low = bit_or(shift_right(r.low, 62), shift_left(r.high, 2));
    r.high = to_unsigned(to_signed(r.high) >> 62);
#endif
}

// Low 64 bits of a.
INLINE constexpr uint64_t algorithm::low(const signed128_t& a) NOEXCEPT
{
#if defined(__SIZEOF_INT128__)
    return static_cast<uint64_t>(a);
#else
    return a.low;
#endif
}

BC_POP_WARNING()

constexpr void algorithm::to_signed62(signed62_t& r,
    const field_t<uint64_t>& a) NOEXCEPT
{
    constexpr auto mask = unmask_right<uint64_t>(62);
    r[0] = to_signed(( a[0]        | (a[1] << 52)) & mask);
    r[1] = to_signed(((a[1] >> 10) | (a[2] << 42)) & mask);
    r[2] = to_signed(((a[2] >> 20) | (a[3] << 32)) & mask);
    r[3] = to_signed(((a[3] >> 30) | (a[4] << 22)) & mask);
    r[4] = to_signed(  a[4] >> 40);
}

constexpr void algorithm::to_signed62(signed62_t& r,
    const scalar_t& a) NOEXCEPT
{
    constexpr auto mask = unmask_right<uint64_t>(62);
    r[0] = to_signed(  a[0]                       & mask);
    r[1] = to_signed(((a[0] >> 62) | (a[1] <<  2)) & mask);
    r[2] = to_signed(((a[1] >> 60) | (a[2] <<  4)) & mask);
    r[3] = to_signed(((a[2] >> 58) | (a[3] <<  6)) & mask);
    r[4] = to_signed(  a[3] >> 56);
}

// From [0, m).
constexpr void algorithm::from_signed62(field_t<uint64_t>& r,
    const signed62_t& a) NOEXCEPT
{
    const auto a0 = to_unsigned(a[0]);
    const auto a1 = to_unsigned(a[1]);
    const auto a2 = to_unsigned(a[2]);
    const auto a3 = to_unsigned(a[3]);
    const auto a4 = to_unsigned(a[4]);
    r[0] =   a0                        & limb_mask;
    r[1] = ((a0 >> 52) | (a1 << 10))  & limb_mask;
    r[2] = ((a1 >> 42) | (a2 << 20))  & limb_mask;
    r[3] = ((a2 >> 32) | (a3 << 30))  & limb_mask;
    r[4] =   (a3 >> 22) | (a4 << 40);
}

// From [0, m).
constexpr void algorithm::from_signed62(scalar_t& r,
    const signed62_t& a) NOEXCEPT
{
    const auto a0 = to_unsigned(a[0]);
    const auto a1 = to_unsigned(a[1]);
    const auto a2 = to_unsigned(a[2]);
    const auto a3 = to_unsigned(a[3]);
    const auto a4 = to_unsigned(a[4]);
    r[0] =  a0        | (a1 << 62);
    r[1] = (a1 >>  2) | (a2 << 60);
    r[2] = (a2 >>  4) | (a3 << 58);
    r[3] = (a3 >>  6) | (a4 << 56);
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
