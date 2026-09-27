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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_ROUNDS_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_ROUNDS_IPP

// Bitsliced rounds
// ============================================================================
// The S-box is the depth 16 circuit of [Boyar, Peralta] "A depth-16 circuit
// for the AES S-box" (113 gates), with plane 7 the most significant bit.

// Derived in part from BearSSL (aes_ct64.c, aes_ct64_enc.c, aes_ct64_dec.c):
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

// Substitution.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
sbox(xplanes_t<Word>& q) NOEXCEPT
{
    const auto x0 = q[7];
    const auto x1 = q[6];
    const auto x2 = q[5];
    const auto x3 = q[4];
    const auto x4 = q[3];
    const auto x5 = q[2];
    const auto x6 = q[1];
    const auto x7 = q[0];

    // Top linear transformation.
    const auto y14 = xors(x3, x5);
    const auto y13 = xors(x0, x6);
    const auto y9  = xors(x0, x3);
    const auto y8  = xors(x0, x5);
    const auto t0  = xors(x1, x2);
    const auto y1  = xors(t0, x7);
    const auto y4  = xors(y1, x3);
    const auto y12 = xors(y13, y14);
    const auto y2  = xors(y1, x0);
    const auto y5  = xors(y1, x6);
    const auto y3  = xors(y5, y8);
    const auto t1  = xors(x4, y12);
    const auto y15 = xors(t1, x5);
    const auto y20 = xors(t1, x1);
    const auto y6  = xors(y15, x7);
    const auto y10 = xors(y15, t0);
    const auto y11 = xors(y20, y9);
    const auto y7  = xors(x7, y11);
    const auto y17 = xors(y10, y11);
    const auto y19 = xors(y10, y8);
    const auto y16 = xors(t0, y11);
    const auto y21 = xors(y13, y16);
    const auto y18 = xors(x0, y16);

    // Non-linear section.
    const auto t2  = f::and_(y12, y15);
    const auto t3  = f::and_(y3, y6);
    const auto t4  = xors(t3, t2);
    const auto t5  = f::and_(y4, x7);
    const auto t6  = xors(t5, t2);
    const auto t7  = f::and_(y13, y16);
    const auto t8  = f::and_(y5, y1);
    const auto t9  = xors(t8, t7);
    const auto t10 = f::and_(y2, y7);
    const auto t11 = xors(t10, t7);
    const auto t12 = f::and_(y9, y11);
    const auto t13 = f::and_(y14, y17);
    const auto t14 = xors(t13, t12);
    const auto t15 = f::and_(y8, y10);
    const auto t16 = xors(t15, t12);
    const auto t17 = xors(t4, t14);
    const auto t18 = xors(t6, t16);
    const auto t19 = xors(t9, t14);
    const auto t20 = xors(t11, t16);
    const auto t21 = xors(t17, y20);
    const auto t22 = xors(t18, y19);
    const auto t23 = xors(t19, y21);
    const auto t24 = xors(t20, y18);

    const auto t25 = xors(t21, t22);
    const auto t26 = f::and_(t21, t23);
    const auto t27 = xors(t24, t26);
    const auto t28 = f::and_(t25, t27);
    const auto t29 = xors(t28, t22);
    const auto t30 = xors(t23, t24);
    const auto t31 = xors(t22, t26);
    const auto t32 = f::and_(t31, t30);
    const auto t33 = xors(t32, t24);
    const auto t34 = xors(t23, t33);
    const auto t35 = xors(t27, t33);
    const auto t36 = f::and_(t24, t35);
    const auto t37 = xors(t36, t34);
    const auto t38 = xors(t27, t36);
    const auto t39 = f::and_(t29, t38);
    const auto t40 = xors(t25, t39);

    const auto t41 = xors(t40, t37);
    const auto t42 = xors(t29, t33);
    const auto t43 = xors(t29, t40);
    const auto t44 = xors(t33, t37);
    const auto t45 = xors(t42, t41);
    const auto z0  = f::and_(t44, y15);
    const auto z1  = f::and_(t37, y6);
    const auto z2  = f::and_(t33, x7);
    const auto z3  = f::and_(t43, y16);
    const auto z4  = f::and_(t40, y1);
    const auto z5  = f::and_(t29, y7);
    const auto z6  = f::and_(t42, y11);
    const auto z7  = f::and_(t45, y17);
    const auto z8  = f::and_(t41, y10);
    const auto z9  = f::and_(t44, y12);
    const auto z10 = f::and_(t37, y3);
    const auto z11 = f::and_(t33, y4);
    const auto z12 = f::and_(t43, y13);
    const auto z13 = f::and_(t40, y5);
    const auto z14 = f::and_(t29, y2);
    const auto z15 = f::and_(t42, y9);
    const auto z16 = f::and_(t45, y14);
    const auto z17 = f::and_(t41, y8);

    // Bottom linear transformation.
    const auto t46 = xors(z15, z16);
    const auto t47 = xors(z10, z11);
    const auto t48 = xors(z5, z13);
    const auto t49 = xors(z9, z10);
    const auto t50 = xors(z2, z12);
    const auto t51 = xors(z2, z5);
    const auto t52 = xors(z7, z8);
    const auto t53 = xors(z0, z3);
    const auto t54 = xors(z6, z7);
    const auto t55 = xors(z16, z17);
    const auto t56 = xors(z12, t48);
    const auto t57 = xors(t50, t53);
    const auto t58 = xors(z4, t46);
    const auto t59 = xors(z3, t54);
    const auto t60 = xors(t46, t57);
    const auto t61 = xors(z14, t57);
    const auto t62 = xors(t52, t58);
    const auto t63 = xors(t49, t58);
    const auto t64 = xors(z4, t59);
    const auto t65 = xors(t61, t62);
    const auto t66 = xors(z1, t63);
    const auto s0  = xors(t59, t63);
    const auto s6  = xors(t56, f::not_(t62));
    const auto s7  = xors(t48, f::not_(t60));
    const auto t67 = xors(t64, t65);
    const auto s3  = xors(t53, t66);
    const auto s4  = xors(t51, t66);
    const auto s5  = xors(t47, t65);
    const auto s1  = xors(t64, f::not_(s3));
    const auto s2  = xors(t55, f::not_(t67));

    q[7] = s0;
    q[6] = s1;
    q[5] = s2;
    q[4] = s3;
    q[3] = s4;
    q[2] = s5;
    q[1] = s6;
    q[0] = s7;
}

// The inverse of the affine transformation, including its constant (0x63),
// so that the inverse substitution is the substitution between two of these.
TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
inverse_affine(xplanes_t<Word>& q) NOEXCEPT
{
    const auto q0 = f::not_(q[0]);
    const auto q1 = f::not_(q[1]);
    const auto q2 = q[2];
    const auto q3 = q[3];
    const auto q4 = q[4];
    const auto q5 = f::not_(q[5]);
    const auto q6 = f::not_(q[6]);
    const auto q7 = q[7];

    q[7] = xors(q1, q4, q6);
    q[6] = xors(q0, q3, q5);
    q[5] = xors(q7, q2, q4);
    q[4] = xors(q6, q1, q3);
    q[3] = xors(q5, q0, q2);
    q[2] = xors(q4, q7, q1);
    q[1] = xors(q3, q6, q0);
    q[0] = xors(q2, q5, q7);
}

TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
inverse_sbox(xplanes_t<Word>& q) NOEXCEPT
{
    inverse_affine(q);
    sbox(q);
    inverse_affine(q);
}

// Rows.
// ----------------------------------------------------------------------------
// Row r of the state occupies bits [16r, 16r + 16) of each plane, as four
// columns of four blocks. Row r rotates left (right for inverse) r columns.

TEMPLATE
template <typename Word>
INLINE constexpr Word CLASS::
shift_row(Word x) NOEXCEPT
{
    constexpr auto s = bits<uint64_t>;
    const auto row0 = f::and_(x, constant<Word>(0x000000000000ffff_u64));
    const auto row1a = f::and_(x, constant<Word>(0x00000000fff00000_u64));
    const auto row1b = f::and_(x, constant<Word>(0x00000000000f0000_u64));
    const auto row2a = f::and_(x, constant<Word>(0x0000ff0000000000_u64));
    const auto row2b = f::and_(x, constant<Word>(0x000000ff00000000_u64));
    const auto row3a = f::and_(x, constant<Word>(0xf000000000000000_u64));
    const auto row3b = f::and_(x, constant<Word>(0x0fff000000000000_u64));
    const auto row1 = f::or_(f::shr<4, s>(row1a), f::shl<12, s>(row1b));
    const auto row2 = f::or_(f::shr<8, s>(row2a), f::shl<8, s>(row2b));
    const auto row3 = f::or_(f::shr<12, s>(row3a), f::shl<4, s>(row3b));
    return ors(row0, row1, row2, row3);
}

TEMPLATE
template <typename Word>
INLINE constexpr Word CLASS::
inverse_shift_row(Word x) NOEXCEPT
{
    constexpr auto s = bits<uint64_t>;
    const auto row0 = f::and_(x, constant<Word>(0x000000000000ffff_u64));
    const auto row1a = f::and_(x, constant<Word>(0x000000000fff0000_u64));
    const auto row1b = f::and_(x, constant<Word>(0x00000000f0000000_u64));
    const auto row2a = f::and_(x, constant<Word>(0x000000ff00000000_u64));
    const auto row2b = f::and_(x, constant<Word>(0x0000ff0000000000_u64));
    const auto row3a = f::and_(x, constant<Word>(0x000f000000000000_u64));
    const auto row3b = f::and_(x, constant<Word>(0xfff0000000000000_u64));
    const auto row1 = f::or_(f::shl<4, s>(row1a), f::shr<12, s>(row1b));
    const auto row2 = f::or_(f::shl<8, s>(row2a), f::shr<8, s>(row2b));
    const auto row3 = f::or_(f::shl<12, s>(row3a), f::shr<4, s>(row3b));
    return ors(row0, row1, row2, row3);
}

TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
shift_rows(xplanes_t<Word>& q) NOEXCEPT
{
    for (auto& plane: q)
        plane = shift_row(plane);
}

TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
inverse_shift_rows(xplanes_t<Word>& q) NOEXCEPT
{
    for (auto& plane: q)
        plane = inverse_shift_row(plane);
}

// Columns.
// ----------------------------------------------------------------------------
// Rotating a plane right 16 (32) bits aligns row r + 1 (r + 2) to row r.

// Multiplication of each byte by x in GF(2^8).
TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
xtime(xplanes_t<Word>& q) NOEXCEPT
{
    const auto q7 = q[7];
    q[7] = q[6];
    q[6] = q[5];
    q[5] = q[4];
    q[4] = xors(q[3], q7);
    q[3] = xors(q[2], q7);
    q[2] = q[1];
    q[1] = xors(q[0], q7);
    q[0] = q7;
}

// Each byte becomes 2a + 3b + c + d, as 2(a + b) + b + (c + d) for rows
// r (a), r + 1 (b), r + 2 (c), r + 3 (d) of its column.
TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
mix_columns(xplanes_t<Word>& q) NOEXCEPT
{
    constexpr auto s = bits<uint64_t>;
    const auto q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];
    const auto q4 = q[4], q5 = q[5], q6 = q[6], q7 = q[7];
    const auto r0 = f::ror<16, s>(q0), r1 = f::ror<16, s>(q1);
    const auto r2 = f::ror<16, s>(q2), r3 = f::ror<16, s>(q3);
    const auto r4 = f::ror<16, s>(q4), r5 = f::ror<16, s>(q5);
    const auto r6 = f::ror<16, s>(q6), r7 = f::ror<16, s>(q7);

    q[0] = xors(q7, r7, r0, f::ror<32, s>(xors(q0, r0)));
    q[1] = xors(q0, r0, q7, r7, r1, f::ror<32, s>(xors(q1, r1)));
    q[2] = xors(q1, r1, r2, f::ror<32, s>(xors(q2, r2)));
    q[3] = xors(q2, r2, q7, r7, r3, f::ror<32, s>(xors(q3, r3)));
    q[4] = xors(q3, r3, q7, r7, r4, f::ror<32, s>(xors(q4, r4)));
    q[5] = xors(q4, r4, r5, f::ror<32, s>(xors(q5, r5)));
    q[6] = xors(q5, r5, r6, f::ror<32, s>(xors(q6, r6)));
    q[7] = xors(q6, r6, r7, f::ror<32, s>(xors(q7, r7)));
}

// The inverse is the mix of each byte first becoming 5a + 4c.
TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
inverse_mix_columns(xplanes_t<Word>& q) NOEXCEPT
{
    constexpr auto s = bits<uint64_t>;
    xplanes_t<Word> t{};
    for (size_t plane{}; plane < planes; ++plane)
        t[plane] = xors(q[plane], f::ror<32, s>(q[plane]));

    xtime(t);
    xtime(t);
    for (size_t plane{}; plane < planes; ++plane)
        q[plane] = xors(q[plane], t[plane]);

    mix_columns(q);
}

TEMPLATE
template <typename Word>
INLINE constexpr void CLASS::
add_key(xplanes_t<Word>& q, const xplanes_t<Word>& key) NOEXCEPT
{
    for (size_t plane{}; plane < planes; ++plane)
        q[plane] = xors(q[plane], key[plane]);
}

// Ciphers.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Word>
constexpr void CLASS::
encrypt_planes(xplanes_t<Word>& q, const xkeys_t<Word>& keys) NOEXCEPT
{
    add_key(q, keys.front());
    for (auto round = one; round < K::rounds; ++round)
    {
        sbox(q);
        shift_rows(q);
        mix_columns(q);
        add_key(q, keys[round]);
    }

    sbox(q);
    shift_rows(q);
    add_key(q, keys.back());
}

TEMPLATE
template <typename Word>
constexpr void CLASS::
decrypt_planes(xplanes_t<Word>& q, const xkeys_t<Word>& keys) NOEXCEPT
{
    add_key(q, keys.back());
    for (auto round = sub1(K::rounds); !is_zero(round); --round)
    {
        inverse_shift_rows(q);
        inverse_sbox(q);
        add_key(q, keys[round]);
        inverse_mix_columns(q);
    }

    inverse_shift_rows(q);
    inverse_sbox(q);
    add_key(q, keys.front());
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
