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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_POINTS_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_POINTS_IPP

// Points
// ============================================================================
// The addition and doubling formulas are complete: correct for all inputs,
// including infinity and equal points, without branches.

namespace libbitcoin {
namespace system {
namespace nist {

TEMPLATE
constexpr typename CLASS::projective_t CLASS::
infinity() NOEXCEPT
{
    return { {}, field().one, {} };
}

TEMPLATE
constexpr typename CLASS::projective_t CLASS::
generator() NOEXCEPT
{
    constexpr auto m = field();
    return
    {
        to_montgomery(to_limbs(C::gx), m),
        to_montgomery(to_limbs(C::gy), m),
        m.one
    };
}

// x^3 - 3x + b.
TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
curve(const limbs_t& x) NOEXCEPT
{
    constexpr auto m = field();
    constexpr auto& b = curve_b;
    const auto x3 = multiply(multiply(x, x, m), x, m);
    const auto x2 = add(x, x, m);
    return add(subtract(x3, add(x2, x, m), m), b, m);
}

// [Renes, Costello, Batina] algorithm 4.
TEMPLATE
constexpr typename CLASS::projective_t CLASS::
add(const projective_t& p, const projective_t& q) NOEXCEPT
{
    constexpr auto m = field();
    constexpr auto& b = curve_b;
    const auto mul = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return multiply(x, y, m);
    };
    const auto sum = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return add(x, y, m);
    };
    const auto sub = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return subtract(x, y, m);
    };

    auto t0 = mul(p.x, q.x);
    auto t1 = mul(p.y, q.y);
    auto t2 = mul(p.z, q.z);
    auto t3 = sum(p.x, p.y);
    auto t4 = sum(q.x, q.y);
    t3 = mul(t3, t4);
    t4 = sum(t0, t1);
    t3 = sub(t3, t4);
    t4 = sum(p.y, p.z);
    auto x3 = sum(q.y, q.z);
    t4 = mul(t4, x3);
    x3 = sum(t1, t2);
    t4 = sub(t4, x3);
    x3 = sum(p.x, p.z);
    auto y3 = sum(q.x, q.z);
    x3 = mul(x3, y3);
    y3 = sum(t0, t2);
    y3 = sub(x3, y3);
    auto z3 = mul(b, t2);
    x3 = sub(y3, z3);
    z3 = sum(x3, x3);
    x3 = sum(x3, z3);
    z3 = sub(t1, x3);
    x3 = sum(t1, x3);
    y3 = mul(b, y3);
    t1 = sum(t2, t2);
    t2 = sum(t1, t2);
    y3 = sub(y3, t2);
    y3 = sub(y3, t0);
    t1 = sum(y3, y3);
    y3 = sum(t1, y3);
    t1 = sum(t0, t0);
    t0 = sum(t1, t0);
    t0 = sub(t0, t2);
    t1 = mul(t4, y3);
    t2 = mul(t0, y3);
    y3 = mul(x3, z3);
    y3 = sum(y3, t2);
    x3 = mul(t3, x3);
    x3 = sub(x3, t1);
    z3 = mul(t4, z3);
    t1 = mul(t3, t0);
    z3 = sum(z3, t1);
    return { x3, y3, z3 };
}

// [Renes, Costello, Batina] algorithm 6.
TEMPLATE
constexpr typename CLASS::projective_t CLASS::
twice(const projective_t& p) NOEXCEPT
{
    constexpr auto m = field();
    constexpr auto& b = curve_b;
    const auto mul = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return multiply(x, y, m);
    };
    const auto sum = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return add(x, y, m);
    };
    const auto sub = [&](const limbs_t& x, const limbs_t& y) NOEXCEPT
    {
        return subtract(x, y, m);
    };

    auto t0 = mul(p.x, p.x);
    auto t1 = mul(p.y, p.y);
    auto t2 = mul(p.z, p.z);
    auto t3 = mul(p.x, p.y);
    t3 = sum(t3, t3);
    auto z3 = mul(p.x, p.z);
    z3 = sum(z3, z3);
    auto y3 = mul(b, t2);
    y3 = sub(y3, z3);
    auto x3 = sum(y3, y3);
    y3 = sum(x3, y3);
    x3 = sub(t1, y3);
    y3 = sum(t1, y3);
    y3 = mul(x3, y3);
    x3 = mul(x3, t3);
    t3 = sum(t2, t2);
    t2 = sum(t2, t3);
    z3 = mul(b, z3);
    z3 = sub(z3, t2);
    z3 = sub(z3, t0);
    t3 = sum(z3, z3);
    z3 = sum(z3, t3);
    t3 = sum(t0, t0);
    t0 = sum(t3, t0);
    t0 = sub(t0, t2);
    t0 = mul(t0, z3);
    y3 = sum(y3, t0);
    t0 = mul(p.y, p.z);
    t0 = sum(t0, t0);
    z3 = mul(t0, z3);
    x3 = sub(x3, z3);
    z3 = mul(t0, t1);
    z3 = sum(z3, z3);
    z3 = sum(z3, z3);
    return { x3, y3, z3 };
}

// Normal (not Montgomery) affine coordinates, false if infinity.
TEMPLATE
constexpr bool CLASS::
to_affine(limbs_t& x, limbs_t& y, const projective_t& p) NOEXCEPT
{
    constexpr auto m = field();
    if (is_zero(p.z))
        return false;

    const auto z = inverse(p.z, m);
    x = from_montgomery(multiply(p.x, z, m), m);
    y = from_montgomery(multiply(p.y, z, m), m);
    return true;
}

// Uncompressed (sec1) point, validated to be on the curve.
TEMPLATE
constexpr bool CLASS::
parse(projective_t& out, const point_t& point) NOEXCEPT
{
    constexpr auto m = field();
    if (point.front() != 0x04)
        return false;

    const auto x = to_limbs(slice<one, add1(size)>(point));
    const auto y = to_limbs(slice<add1(size), add1(two * size)>(point));
    if (!is_less(x, m.value) || !is_less(y, m.value))
        return false;

    out.x = to_montgomery(x, m);
    out.y = to_montgomery(y, m);
    out.z = m.one;
    return multiply(out.y, out.y, m) == curve(out.x);
}

TEMPLATE
constexpr typename CLASS::point_t CLASS::
serialize(const limbs_t& x, const limbs_t& y) NOEXCEPT
{
    return splice(data_array<one>{ 0x04 }, to_bytes(x), to_bytes(y));
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
