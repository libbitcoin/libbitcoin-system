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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_ELLSWIFT_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_ELLSWIFT_IPP

// Based on:
// github.com/bitcoin/bips/blob/master/bip-0324.mediawiki
// github.com/bitcoin-core/secp256k1 (modules/ellswift)

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// ElligatorSwift.
// ----------------------------------------------------------------------------
// protected

// With u and t made nonzero, s = t^2 and g = u^3 + 7 (s made 4s where g + s
// is zero), x is the first on the curve of x3 = (3su^3 - (g + s)^2) / 3su^2,
// x2 = u(c1 s + c2 g) / (g + s) and x1 = -(x2 + u).
constexpr void algorithm::swift_fraction(field_t<uint64_t>& xn,
    field_t<uint64_t>& xd, const field_t<uint64_t>& u,
    const field_t<uint64_t>& t) NOEXCEPT
{
    constexpr field_t<uint64_t> unit{ 1 };
    const auto& u1 = normalizes_to_zero(u) ? unit : u;

    field_t<uint64_t> s{ unit }, g{}, p{}, d{}, n{}, l{};
    if (!normalizes_to_zero(t))
        square(s, t);

    square(l, u1);
    multiply(g, l, u1);
    add(g, g, curve_b);
    carry(g);
    add(p, g, s);
    carry(p);
    if (normalizes_to_zero(p))
    {
        scale<4>(s, s);
        carry(s);
        add(p, g, s);
        carry(p);
    }

    multiply(d, s, l);
    scale<3>(d, d);
    carry(d);
    square(l, p);
    negate(l, l);
    multiply(n, d, u1);
    add(n, n, l);
    carry(n);
    if (is_curve_fraction(n, d))
    {
        xn = n;
        xd = d;
        return;
    }

    xd = p;
    multiply(l, swift_c1, s);
    multiply(n, beta, g);
    add(n, n, l);
    carry(n);
    multiply(n, n, u1);
    if (is_curve_fraction(n, p))
    {
        xn = n;
        return;
    }

    multiply(l, p, u1);
    add(n, n, l);
    carry(n);
    negate(xn, n);
    carry(xn);
}

constexpr void algorithm::swift_decode(affine_t<uint64_t>& r,
    const field_t<uint64_t>& u, const field_t<uint64_t>& t) NOEXCEPT
{
    field_t<uint64_t> xn{}, xd{}, x{}, parity{ t };
    swift_fraction(xn, xd, u, t);
    inverse(xd, xd);
    multiply(x, xn, xd);
    normalize(x);
    normalize(parity);
    /* bool */ lift(r, x, is_odd_element(parity));
}

// Branches 0, 1, 4 and 5 invert x1 or x2, for s = -(u^3 + 7) / (u^2 + ux + x^2)
// and v = x, failing where -u - x is on the curve (it would decode by x3).
// Branches 2, 3, 6 and 7 invert x3, for s = x - u, r = sqrt(-s(4(u^3 + 7) +
// 3u^2 s)) and v = (r / s - u) / 2. With w = sqrt(s), t is -w(c3 u + v) for
// c & 5 = 0, w(c4 u + v) for 1, w(c3 u + v) for 4, and -w(c4 u + v) for 5.
constexpr bool algorithm::swift_inverse(field_t<uint64_t>& t,
    const field_t<uint64_t>& x, const field_t<uint64_t>& u,
    uint8_t c) NOEXCEPT
{
    field_t<uint64_t> g{}, v{}, s{}, m{}, q{}, root{}, w{};

    if (!get_right(c, one))
    {
        add(m, x, u);
        negate<2>(m, m);
        normalize(m);
        if (is_curve_x(m))
            return false;

        square(s, m);
        negate(s, s);
        multiply(m, u, x);
        add(s, s, m);
        carry(s);

        square(g, u);
        multiply(g, g, u);
        add(g, g, curve_b);
        carry(g);
        multiply(m, s, g);
        if (!is_square(m))
            return false;

        inverse(s, s);
        multiply(s, s, g);
        v = x;
    }
    else
    {
        constexpr field_t<uint64_t> four_b{ 28 };

        negate(m, u);
        add(s, m, x);
        carry(s);
        if (!is_square(s))
            return false;

        square(g, u);
        multiply(q, s, g);
        scale<3>(q, q);
        multiply(g, g, u);
        scale<4>(g, g);
        add(g, g, four_b);
        add(q, q, g);
        carry(q);
        multiply(q, q, s);
        negate(q, q);
        carry(q);
        if (!f::any(square_root(root, q)))
            return false;

        if (get_right(c, zero) && normalizes_to_zero(root))
            return false;

        if (normalizes_to_zero(s))
            return false;

        inverse(v, s);
        multiply(v, v, root);
        add(v, v, m);
        carry(v);
        halve(v, v);
        carry(v);
    }

    /* bool */ square_root(w, s);
    const auto branch = c & 5u;
    if (is_zero(branch) || branch == 5u)
    {
        negate(w, w);
        carry(w);
    }

    multiply(m, u, get_right(c, zero) ? swift_c4 : swift_c3);
    add(m, m, v);
    carry(m);
    multiply(t, w, m);
    normalize(t);
    return true;
}

constexpr bool algorithm::is_curve_x(const field_t<uint64_t>& x) NOEXCEPT
{
    field_t<uint64_t> yy{};
    square(yy, x);
    multiply(yy, yy, x);
    add(yy, yy, curve_b);
    carry(yy);
    return is_square(yy);
}

// (xn / xd)^3 + 7 is square where xd xn^3 + 7 xd^4 is (a multiple by xd^4).
constexpr bool algorithm::is_curve_fraction(const field_t<uint64_t>& xn,
    const field_t<uint64_t>& xd) NOEXCEPT
{
    field_t<uint64_t> r{}, t{};
    multiply(r, xd, xn);
    square(t, xn);
    multiply(r, r, t);
    square(t, xd);
    square(t, t);
    scale<7>(t, t);
    add(r, r, t);
    carry(r);
    return is_square(r);
}

constexpr bool algorithm::is_square(const field_t<uint64_t>& a) NOEXCEPT
{
    field_t<uint64_t> root{};
    return f::any(square_root(root, a));
}

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
