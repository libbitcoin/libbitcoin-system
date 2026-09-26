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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_GROUP_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_GROUP_IPP

// Based on:
// hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// Group arithmetic.
// ----------------------------------------------------------------------------
// protected

// m = 3x^2, s = 4xy^2, x3 = m^2 - 2s, y3 = m(s - x3) - 8y^4, z3 = 2yz.
template <typename Word>
constexpr void algorithm::double_(jacobian_t<Word>& r,
    const jacobian_t<Word>& a) NOEXCEPT
{
    field_t<Word> yy{}, xx{}, s{}, m{}, y4{}, x3{}, y3{}, z3{}, t{};

    square(yy, a.y);
    square(xx, a.x);
    multiply(s, a.x, yy);
    scale<4>(s, s);
    carry(s);
    scale<3>(m, xx);
    carry(m);
    square(y4, yy);
    scale<8>(y4, y4);
    carry(y4);

    square(x3, m);
    scale<2>(t, s);
    carry(t);
    subtract(x3, x3, t);
    carry(x3);

    subtract(t, s, x3);
    carry(t);
    multiply(y3, m, t);
    subtract(y3, y3, y4);
    carry(y3);

    multiply(z3, a.y, a.z);
    scale<2>(z3, z3);
    carry(z3);

    r = { x3, y3, z3, a.infinity };
}

// u2 = x2 z1^2, s2 = y2 z1^3, h = u2 - x1, r = s2 - y1, v = x1 h^2,
// x3 = r^2 - h^3 - 2v, y3 = r(v - x3) - y1 h^3, z3 = z1 h.
template <typename Word>
constexpr Word algorithm::add(jacobian_t<Word>& r, const jacobian_t<Word>& a,
    const affine_t<Word>& b) NOEXCEPT
{
    field_t<Word> zz{}, u2{}, s2{}, h{}, rh{}, hh{}, hhh{}, v{}, x3{}, y3{},
        z3{}, t{};

    square(zz, a.z);
    multiply(u2, b.x, zz);
    multiply(s2, b.y, zz);
    multiply(s2, s2, a.z);
    subtract(h, u2, a.x);
    carry(h);
    subtract(rh, s2, a.y);
    carry(rh);

    square(hh, h);
    multiply(hhh, h, hh);
    multiply(v, a.x, hh);

    square(x3, rh);
    subtract(x3, x3, hhh);
    scale<2>(t, v);
    carry(t);
    subtract(x3, x3, t);
    carry(x3);

    subtract(t, v, x3);
    carry(t);
    multiply(y3, rh, t);
    multiply(t, a.y, hhh);
    subtract(y3, y3, t);
    carry(y3);

    multiply(z3, a.z, h);

    field_t<Word> normal{ h };
    normalize(normal);
    r = { x3, y3, z3, f::broadcast<Word>(uint64_t{}) };
    return f::or_(a.infinity, is_zero(normal));
}

// u1 = x1 z2^2, u2 = x2 z1^2, s1 = y1 z2^3, s2 = y2 z1^3, h = u2 - u1,
// r = s2 - s1, v = u1 h^2, x3 = r^2 - h^3 - 2v, y3 = r(v - x3) - s1 h^3,
// z3 = z1 z2 h.
template <typename Word>
constexpr Word algorithm::add(jacobian_t<Word>& r, const jacobian_t<Word>& a,
    const jacobian_t<Word>& b) NOEXCEPT
{
    field_t<Word> z1z1{}, z2z2{}, u1{}, u2{}, s1{}, s2{}, h{}, rh{}, hh{},
        hhh{}, v{}, x3{}, y3{}, z3{}, t{};

    square(z1z1, a.z);
    square(z2z2, b.z);
    multiply(u1, a.x, z2z2);
    multiply(u2, b.x, z1z1);
    multiply(s1, a.y, b.z);
    multiply(s1, s1, z2z2);
    multiply(s2, b.y, a.z);
    multiply(s2, s2, z1z1);
    subtract(h, u2, u1);
    carry(h);
    subtract(rh, s2, s1);
    carry(rh);

    square(hh, h);
    multiply(hhh, h, hh);
    multiply(v, u1, hh);

    square(x3, rh);
    subtract(x3, x3, hhh);
    scale<2>(t, v);
    carry(t);
    subtract(x3, x3, t);
    carry(x3);

    subtract(t, v, x3);
    carry(t);
    multiply(y3, rh, t);
    multiply(t, s1, hhh);
    subtract(y3, y3, t);
    carry(y3);

    multiply(z3, a.z, b.z);
    multiply(z3, z3, h);

    field_t<Word> normal{ h };
    normalize(normal);
    r = { x3, y3, z3, f::broadcast<Word>(uint64_t{}) };
    return f::or_(f::or_(a.infinity, b.infinity), is_zero(normal));
}

// An uncomputed sum is of an infinite operand, or of equal x, where equal y
// doubles and opposite y is infinity.
constexpr void algorithm::add_complete(jacobian_t<uint64_t>& r,
    const jacobian_t<uint64_t>& a, const affine_t<uint64_t>& b) NOEXCEPT
{
    jacobian_t<uint64_t> sum{};
    if (!f::any(add(sum, a, b)))
    {
        r = sum;
        return;
    }

    jacobian_t<uint64_t> lifted{};
    to_jacobian(lifted, b);
    add_complete(r, a, lifted);
}

constexpr void algorithm::add_complete(jacobian_t<uint64_t>& r,
    const jacobian_t<uint64_t>& a, const jacobian_t<uint64_t>& b) NOEXCEPT
{
    jacobian_t<uint64_t> sum{};
    if (!f::any(add(sum, a, b)))
    {
        r = sum;
        return;
    }

    if (f::any(a.infinity))
    {
        r = b;
        return;
    }

    if (f::any(b.infinity))
    {
        r = a;
        return;
    }

    field_t<uint64_t> z1z1{}, z2z2{}, s1{}, s2{};
    square(z1z1, a.z);
    square(z2z2, b.z);
    multiply(s1, a.y, b.z);
    multiply(s1, s1, z2z2);
    multiply(s2, b.y, a.z);
    multiply(s2, s2, z1z1);
    normalize(s1);
    normalize(s2);

    if (f::any(equal(s1, s2)))
    {
        double_(r, a);
        return;
    }

    r = {};
    r.infinity = max_uint64;
}

template <typename Word>
constexpr void algorithm::negate(affine_t<Word>& r,
    const affine_t<Word>& a) NOEXCEPT
{
    field_t<Word> y{};
    negate(y, a.y);
    carry(y);
    r = { a.x, y };
}

template <typename Word>
constexpr void algorithm::negate(jacobian_t<Word>& r,
    const jacobian_t<Word>& a) NOEXCEPT
{
    field_t<Word> y{};
    negate(y, a.y);
    carry(y);
    r = { a.x, y, a.z, a.infinity };
}

template <typename Word>
constexpr void algorithm::endomorphism(affine_t<Word>& r,
    const affine_t<Word>& a) NOEXCEPT
{
    field_t<Word> x{};
    multiply(x, a.x, broadcast<Word>(beta));
    r = { x, a.y };
}

// Group conversion.
// ----------------------------------------------------------------------------
// protected

template <typename Word>
constexpr void algorithm::to_jacobian(jacobian_t<Word>& r,
    const affine_t<Word>& a) NOEXCEPT
{
    r = { a.x, a.y, broadcast<Word>({ 1 }), f::broadcast<Word>(uint64_t{}) };
}

template <typename Word>
constexpr void algorithm::to_affine(affine_t<Word>& r,
    const jacobian_t<Word>& a, const field_t<Word>& inverse_z) NOEXCEPT
{
    field_t<Word> zz{}, zzz{}, x{}, y{};
    square(zz, inverse_z);
    multiply(zzz, zz, inverse_z);
    multiply(x, a.x, zz);
    multiply(y, a.y, zzz);
    normalize(x);
    normalize(y);
    r = { x, y };
}

template <typename Word>
constexpr void algorithm::to_affine(affine_t<Word>& r,
    const jacobian_t<Word>& a) NOEXCEPT
{
    field_t<Word> inverse_z{};
    inverse(inverse_z, a.z);
    to_affine(r, a, inverse_z);
}

// Prefix products of z share one inversion, unwound from the last element.
template <size_t Count, typename Word>
constexpr void algorithm::to_affine(std_array<affine_t<Word>, Count>& r,
    const std_array<jacobian_t<Word>, Count>& a) NOEXCEPT
{
    static_assert(is_nonzero(Count));

    std_array<field_t<Word>, Count> prefix{};
    prefix[zero] = a[zero].z;
    for (auto point = one; point < Count; ++point)
        multiply(prefix[point], prefix[sub1(point)], a[point].z);

    field_t<Word> inverted{}, inverse_z{};
    inverse(inverted, prefix[sub1(Count)]);
    for (auto point = sub1(Count); is_nonzero(point); --point)
    {
        multiply(inverse_z, inverted, prefix[sub1(point)]);
        multiply(inverted, inverted, a[point].z);
        to_affine(r[point], a[point], inverse_z);
    }

    to_affine(r[zero], a[zero], inverted);
}

template <typename Word>
constexpr Word algorithm::lift(affine_t<Word>& r, const field_t<Word>& x,
    Word odd) NOEXCEPT
{
    field_t<Word> xx{}, xxx{}, yy{}, y{}, negated{};
    square(xx, x);
    multiply(xxx, xx, x);
    add(yy, xxx, broadcast<Word>(curve_b));
    carry(yy);

    const auto valid = square_root(y, yy);
    normalize(y);
    negate(negated, y);
    normalize(negated);

    select(y, f::xor_(is_odd(y), odd), negated, y);
    r = { x, y };
    return valid;
}

template <typename Word>
constexpr Word algorithm::is_on_curve(const affine_t<Word>& a) NOEXCEPT
{
    field_t<Word> xx{}, xxx{}, right{}, left{};
    square(xx, a.x);
    multiply(xxx, xx, a.x);
    add(right, xxx, broadcast<Word>(curve_b));
    square(left, a.y);
    normalize(right);
    normalize(left);
    return equal(left, right);
}

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
