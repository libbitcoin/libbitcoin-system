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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SCALAR_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SCALAR_IPP

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Scalar internals.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::is_overflow(const scalar_t& a) NOEXCEPT
{
    return !is_less(a, order);
}

// The borrow of a - b is the complement of the carry of a + ~b + 1.
constexpr bool algorithm::is_less(const scalar_t& a, const scalar_t& b) NOEXCEPT
{
    uint64_t difference{};
    auto carry = true;
    for (size_t limb{}; limb < a.size(); ++limb)
        carry = add_carry(difference, a[limb], ~b[limb], carry);

    return !carry;
}

// Adds 2^256 - n (modulo 2^256) where overflow, which subtracts n from a value
// not below n.
constexpr void algorithm::reduce(scalar_t& r, bool overflow) NOEXCEPT
{
    const auto mask = overflow ? max_uint64 : 0_u64;
    auto carry = false;
    for (size_t limb{}; limb < r.size(); ++limb)
        carry = add_carry(r[limb], r[limb], order_complement[limb] & mask,
            carry);
}

// Folds limbs above 2^256 by 2^256 = 2^256 - n (mod n), from 512 to 385 to
// 258 to 256 bits, leaving at most one subtraction of n, where 2^256 - n is
// (c0, c1, 1).
constexpr void algorithm::reduce(scalar_t& r, const wide_t& l) NOEXCEPT
{
    constexpr auto c0 = order_complement[0];
    constexpr auto c1 = order_complement[1];
    static_assert(order_complement[2] == 1u && is_zero(order_complement[3]));

    column_t c{ l[0], 0, 0 };
    multiply_add(c, l[4], c0);
    const auto m0 = take(c);
    add(c, l[1]);
    multiply_add(c, l[5], c0);
    multiply_add(c, l[4], c1);
    const auto m1 = take(c);
    add(c, l[2]);
    multiply_add(c, l[6], c0);
    multiply_add(c, l[5], c1);
    add(c, l[4]);
    const auto m2 = take(c);
    add(c, l[3]);
    multiply_add(c, l[7], c0);
    multiply_add(c, l[6], c1);
    add(c, l[5]);
    const auto m3 = take(c);
    multiply_add(c, l[7], c1);
    add(c, l[6]);
    const auto m4 = take(c);
    add(c, l[7]);
    const auto m5 = take(c);
    const auto m6 = c[0];

    c = { m0, 0, 0 };
    multiply_add(c, m4, c0);
    const auto p0 = take(c);
    add(c, m1);
    multiply_add(c, m5, c0);
    multiply_add(c, m4, c1);
    const auto p1 = take(c);
    add(c, m2);
    multiply_add(c, m6, c0);
    multiply_add(c, m5, c1);
    add(c, m4);
    const auto p2 = take(c);
    add(c, m3);
    multiply_add(c, m6, c1);
    add(c, m5);
    const auto p3 = take(c);
    const auto p4 = c[0] + m6;

    c = { p0, 0, 0 };
    multiply_add(c, p4, c0);
    r[0] = take(c);
    add(c, p1);
    multiply_add(c, p4, c1);
    r[1] = take(c);
    add(c, p2);
    add(c, p4);
    r[2] = take(c);
    add(c, p3);
    r[3] = take(c);
    reduce(r, is_nonzero(c[0]) || is_overflow(r));
}

INLINE constexpr void algorithm::multiply_add(column_t& r, uint64_t a,
    uint64_t b) NOEXCEPT
{
    uint64_t high{}, low{};
    mul_wide(high, low, a, b);
    const auto carry = add_carry(r[0], r[0], low, false);
    r[2] += to_int<uint64_t>(add_carry(r[1], r[1], high, carry));
}

INLINE constexpr void algorithm::add(column_t& r, uint64_t a) NOEXCEPT
{
    const auto carry = add_carry(r[0], r[0], a, false);
    r[2] += to_int<uint64_t>(add_carry(r[1], r[1], uint64_t{}, carry));
}

INLINE constexpr uint64_t algorithm::take(column_t& a) NOEXCEPT
{
    const auto word = a[0];
    a = { a[1], a[2], 0 };
    return word;
}

// Columns of the product, low first.
constexpr void algorithm::product(wide_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    column_t c{};
    multiply_add(c, a[0], b[0]);
    r[0] = take(c);
    multiply_add(c, a[0], b[1]);
    multiply_add(c, a[1], b[0]);
    r[1] = take(c);
    multiply_add(c, a[0], b[2]);
    multiply_add(c, a[1], b[1]);
    multiply_add(c, a[2], b[0]);
    r[2] = take(c);
    multiply_add(c, a[0], b[3]);
    multiply_add(c, a[1], b[2]);
    multiply_add(c, a[2], b[1]);
    multiply_add(c, a[3], b[0]);
    r[3] = take(c);
    multiply_add(c, a[1], b[3]);
    multiply_add(c, a[2], b[2]);
    multiply_add(c, a[3], b[1]);
    r[4] = take(c);
    multiply_add(c, a[2], b[3]);
    multiply_add(c, a[3], b[2]);
    r[5] = take(c);
    multiply_add(c, a[3], b[3]);
    r[6] = take(c);
    r[7] = c[0];
}

// r = round(a * b / 2^384).
constexpr void algorithm::multiply_shift(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    wide_t value{};
    product(value, a, b);
    const auto round = value[5] >> sub1(bits<uint64_t>);
    auto carry = add_carry(r[0], value[6], round, false);
    carry = add_carry(r[1], value[7], uint64_t{}, carry);
    r[2] = to_int<uint64_t>(carry);
    r[3] = 0;
}

// Scalar arithmetic.
// ----------------------------------------------------------------------------
// protected

constexpr void algorithm::add(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    auto carry = false;
    for (size_t limb{}; limb < r.size(); ++limb)
        carry = add_carry(r[limb], a[limb], b[limb], carry);

    reduce(r, carry || is_overflow(r));
}

// n - a as n + ~a + 1, masked to zero for zero a.
constexpr void algorithm::negate(scalar_t& r, const scalar_t& a) NOEXCEPT
{
    const auto mask = is_zero_scalar(a) ? 0_u64 : max_uint64;
    auto carry = true;
    for (size_t limb{}; limb < r.size(); ++limb)
    {
        carry = add_carry(r[limb], order[limb], ~a[limb], carry);
        r[limb] &= mask;
    }
}

constexpr void algorithm::multiply(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    wide_t value{};
    product(value, a, b);
    reduce(r, value);
}

constexpr void algorithm::inverse(scalar_t& r, const scalar_t& a) NOEXCEPT
{
    signed62_t x{};
    to_signed62(x, a);
    invert(x, order_modulus);
    from_signed62(r, x);
}

// Exponent n - 2 by windows of four bits, from a table of the powers of a
// below sixteen, where only the exponent selects the entry.
constexpr void algorithm::inverse_power(scalar_t& r,
    const scalar_t& a) NOEXCEPT
{
    constexpr size_t window_bits = 4;
    constexpr auto limb_windows = bits<uint64_t> / window_bits;
    constexpr auto windows = array_count<scalar_t> * limb_windows;
    constexpr auto mask = unmask_right<uint64_t>(window_bits);

    std_array<scalar_t, power2(window_bits)> table{};
    table.front() = { 1 };
    for (auto entry = one; entry < table.size(); ++entry)
        multiply(table[entry], table[sub1(entry)], a);

    scalar_t out{ 1 };
    for (auto window = windows; is_nonzero(window--);)
    {
        const auto shift = (window % limb_windows) * window_bits;
        const auto entry = (order_minus_two[window / limb_windows] >> shift) &
            mask;

        for (size_t count{}; count < window_bits; ++count)
            multiply(out, out, out);

        multiply(out, out, table[entry]);
    }

    r = out;
}

constexpr void algorithm::split(scalar_t& k1, scalar_t& k2,
    const scalar_t& k) NOEXCEPT
{
    scalar_t c1{}, c2{};
    multiply_shift(c1, k, g1);
    multiply_shift(c2, k, g2);
    multiply(c1, c1, minus_b1);
    multiply(c2, c2, minus_b2);
    add(k2, c1, c2);
    multiply(c1, k2, lambda);
    negate(c1, c1);
    add(k1, c1, k);
}

// Each digit is the low Bits + 1 bits less 2^Bits, which is odd and nonzero,
// and the remainder (value - digit) / 2^Bits is again odd.
template <size_t Bits, size_t Count>
constexpr void algorithm::recode(digits_t<Count>& digits,
    const scalar_t& value) NOEXCEPT
{
    static_assert(is_nonzero(Bits) && Bits < sub1(bits<int16_t>));
    static_assert(is_nonzero(Count));
    static_assert(Bits * Count <= array_count<scalar_t> * bits<uint64_t>);

    constexpr auto window = unmask_right<uint64_t>(add1(Bits));
    constexpr auto offset = power2<int64_t>(Bits);
    constexpr auto rest = bits<uint64_t> - Bits;

    auto remainder = value;
    for (size_t digit{}; digit < sub1(Count); ++digit)
    {
        const auto low = to_signed(remainder[0] & window);
        digits[digit] = narrow_cast<int16_t>(low - offset);

        remainder[0] = set_right((remainder[0] >> Bits) |
            (remainder[1] << rest));
        remainder[1] = (remainder[1] >> Bits) | (remainder[2] << rest);
        remainder[2] = (remainder[2] >> Bits) | (remainder[3] << rest);
        remainder[3] = (remainder[3] >> Bits);
    }

    digits[sub1(Count)] = narrow_sign_cast<int16_t>(remainder[0]);
}

// Scalar predicates.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::is_zero_scalar(const scalar_t& a) NOEXCEPT
{
    return is_zero(a[0] | a[1] | a[2] | a[3]);
}

constexpr bool algorithm::is_high(const scalar_t& a) NOEXCEPT
{
    return is_less(half_order, a);
}

// Scalar encoding.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::from_bytes(scalar_t& r,
    const bytes_t& bytes) NOEXCEPT
{
    decode(r, bytes);
    const auto overflow = is_overflow(r);
    reduce(r, overflow);
    return !overflow;
}

constexpr void algorithm::to_bytes(bytes_t& out, const scalar_t& a) NOEXCEPT
{
    encode(out, a);
}

// Scalar encoding internals.
// ----------------------------------------------------------------------------
// protected

// Limbs are least significant first, and bytes are big-endian.
template <size_t Offset, size_t Size>
constexpr void algorithm::decode(scalar_t& r,
    const data_array<Size>& bytes) NOEXCEPT
{
    constexpr auto size = sizeof(uint64_t);
    if (std::is_constant_evaluated())
    {
        from_big<Offset + 3 * size>(r[0], bytes);
        from_big<Offset + 2 * size>(r[1], bytes);
        from_big<Offset + 1 * size>(r[2], bytes);
        from_big<Offset + 0 * size>(r[3], bytes);
    }
    else
    {
        const auto& words = array_cast<uint64_t, array_count<scalar_t>,
            Offset>(bytes);

        r[0] = native_from_big_end(words[3]);
        r[1] = native_from_big_end(words[2]);
        r[2] = native_from_big_end(words[1]);
        r[3] = native_from_big_end(words[0]);
    }
}

constexpr void algorithm::encode(bytes_t& out, const scalar_t& a) NOEXCEPT
{
    constexpr auto size = sizeof(uint64_t);
    if (std::is_constant_evaluated())
    {
        to_big<0 * size>(out, a[3]);
        to_big<1 * size>(out, a[2]);
        to_big<2 * size>(out, a[1]);
        to_big<3 * size>(out, a[0]);
    }
    else
    {
        auto& words = array_cast<uint64_t>(out);
        words[0] = native_to_big_end(a[3]);
        words[1] = native_to_big_end(a[2]);
        words[2] = native_to_big_end(a[1]);
        words[3] = native_to_big_end(a[0]);
    }
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
