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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_FIELD_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_FIELD_IPP

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// Field internals.
// ----------------------------------------------------------------------------
// protected

template <size_t Index, typename Word>
INLINE constexpr void algorithm::propagate(auto& limbs, Word mask) NOEXCEPT
{
    constexpr auto next = add1(Index);
    limbs[next] = f::add<64>(limbs[next], f::shr<limb_bits, 64>(limbs[Index]));
    limbs[Index] = f::and_(limbs[Index], mask);
}

template <size_t Left, size_t Right, typename Word>
INLINE constexpr void algorithm::accumulate(product_t<Word>& c,
    const field_t<Word>& a, const field_t<Word>& b) NOEXCEPT
{
    constexpr auto low = Left + Right;
    constexpr auto high = add1(low);
    c[low] = f::madd52lo<64>(c[low], a[Left], b[Right]);
    c[high] = f::madd52hi<64>(c[high], a[Left], b[Right]);
}

template <size_t Index, typename Word>
INLINE constexpr void algorithm::fold(product_t<Word>& c, Word value,
    Word factor) NOEXCEPT
{
    constexpr auto next = add1(Index);
    c[Index] = f::madd52lo<64>(c[Index], value, factor);
    c[next] = f::madd52hi<64>(c[next], value, factor);
}

template <typename Word>
INLINE constexpr void algorithm::reduce(field_t<Word>& r,
    product_t<Word>& c) NOEXCEPT
{
    const auto mask = f::broadcast<Word>(limb_mask);
    const auto factor = f::broadcast<Word>(fold_260);
    const auto zero = f::broadcast<Word>(uint64_t{});

    propagate<0>(c, mask);
    propagate<1>(c, mask);
    propagate<2>(c, mask);
    propagate<3>(c, mask);
    propagate<4>(c, mask);
    propagate<5>(c, mask);
    propagate<6>(c, mask);
    propagate<7>(c, mask);
    propagate<8>(c, mask);

    const auto top = f::madd52hi<64>(zero, c[9], factor);
    fold<0>(c, c[5], factor);
    fold<1>(c, c[6], factor);
    fold<2>(c, c[7], factor);
    fold<3>(c, c[8], factor);
    c[4] = f::madd52lo<64>(c[4], c[9], factor);
    fold<0>(c, top, factor);

    r = { c[0], c[1], c[2], c[3], c[4] };
    carry(r);
}

// r = a * b + r.
INLINE constexpr void algorithm::multiply_add(unsigned128_t& r, uint64_t a,
    uint64_t b) NOEXCEPT
{
    uint64_t high{}, low{};
    mul_wide(high, low, a, b);
    const auto carry = add_carry(r.low, r.low, low, false);
    add_carry(r.high, r.high, high, carry);
}

// r = r + a.
INLINE constexpr void algorithm::add(unsigned128_t& r,
    const unsigned128_t& a) NOEXCEPT
{
    const auto carry = add_carry(r.low, r.low, a.low, false);
    add_carry(r.high, r.high, a.high, carry);
}

// Low 52 bits of a, with a shifted right by 52.
INLINE constexpr uint64_t algorithm::take(unsigned128_t& a) NOEXCEPT
{
    const auto low = a.low & limb_mask;
    a.low = (a.low >> limb_bits) | (a.high << (bits<uint64_t> - limb_bits));
    a.high >>= limb_bits;
    return low;
}

template <typename Word>
constexpr void algorithm::powers(field_t<Word>& x2, field_t<Word>& x22,
    field_t<Word>& x223, const field_t<Word>& a) NOEXCEPT
{
    field_t<Word> x3{}, x6{}, x9{}, x11{}, x44{}, x88{}, x176{}, x220{};

    square(x2, a);
    multiply(x2, x2, a);
    square(x3, x2);
    multiply(x3, x3, a);
    square<3>(x6, x3);
    multiply(x6, x6, x3);
    square<3>(x9, x6);
    multiply(x9, x9, x3);
    square<2>(x11, x9);
    multiply(x11, x11, x2);
    square<11>(x22, x11);
    multiply(x22, x22, x11);
    square<22>(x44, x22);
    multiply(x44, x44, x22);
    square<44>(x88, x44);
    multiply(x88, x88, x44);
    square<88>(x176, x88);
    multiply(x176, x176, x88);
    square<44>(x220, x176);
    multiply(x220, x220, x44);
    square<3>(x223, x220);
    multiply(x223, x223, x3);
}

// Field arithmetic.
// ----------------------------------------------------------------------------
// protected

template <typename Word>
constexpr void algorithm::add(field_t<Word>& r, const field_t<Word>& a,
    const field_t<Word>& b) NOEXCEPT
{
    r[0] = f::add<64>(a[0], b[0]);
    r[1] = f::add<64>(a[1], b[1]);
    r[2] = f::add<64>(a[2], b[2]);
    r[3] = f::add<64>(a[3], b[3]);
    r[4] = f::add<64>(a[4], b[4]);
}

template <typename Word>
constexpr void algorithm::negate(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    r[0] = f::sub<64>(f::broadcast<Word>(prime[0] << two), a[0]);
    r[1] = f::sub<64>(f::broadcast<Word>(prime[1] << two), a[1]);
    r[2] = f::sub<64>(f::broadcast<Word>(prime[2] << two), a[2]);
    r[3] = f::sub<64>(f::broadcast<Word>(prime[3] << two), a[3]);
    r[4] = f::sub<64>(f::broadcast<Word>(prime[4] << two), a[4]);
}

template <typename Word>
constexpr void algorithm::subtract(field_t<Word>& r, const field_t<Word>& a,
    const field_t<Word>& b) NOEXCEPT
{
    field_t<Word> negated{};
    negate(negated, b);
    add(r, a, negated);
}

template <size_t Factor, typename Word>
constexpr void algorithm::scale(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    static_assert(is_nonzero(Factor) && Factor < 1024u);

    field_t<Word> out{ a };
    for (auto term = one; term < Factor; ++term)
        add(out, out, a);

    r = out;
}

template <typename Word>
constexpr void algorithm::select(field_t<Word>& r, Word mask,
    const field_t<Word>& a, const field_t<Word>& b) NOEXCEPT
{
    r[0] = f::select(mask, a[0], b[0]);
    r[1] = f::select(mask, a[1], b[1]);
    r[2] = f::select(mask, a[2], b[2]);
    r[3] = f::select(mask, a[3], b[3]);
    r[4] = f::select(mask, a[4], b[4]);
}

template <typename Word>
constexpr algorithm::field_t<Word> algorithm::broadcast(
    const field_t<uint64_t>& a) NOEXCEPT
{
    return
    {
        f::broadcast<Word>(a[0]),
        f::broadcast<Word>(a[1]),
        f::broadcast<Word>(a[2]),
        f::broadcast<Word>(a[3]),
        f::broadcast<Word>(a[4])
    };
}

template <typename Word>
constexpr void algorithm::carry(field_t<Word>& a) NOEXCEPT
{
    const auto mask = f::broadcast<Word>(limb_mask);
    const auto over = f::shr<top_bits, 64>(a[4]);
    a[4] = f::and_(a[4], f::broadcast<Word>(top_mask));
    a[0] = f::madd52lo<64>(a[0], over, f::broadcast<Word>(fold_256));

    propagate<0>(a, mask);
    propagate<1>(a, mask);
    propagate<2>(a, mask);
    propagate<3>(a, mask);
}

template <typename Word>
constexpr void algorithm::carry_extended(field_t<Word>& a) NOEXCEPT
{
    if constexpr (!is_same_type<Word, uint64_t>)
    {
        carry(a);
    }
}

template <typename Word>
constexpr void algorithm::normalize(field_t<Word>& a) NOEXCEPT
{
    carry(a);

    const auto mask = f::broadcast<Word>(limb_mask);
    const auto top = f::broadcast<Word>(top_mask);
    const auto factor = f::broadcast<Word>(fold_256);
    const auto middle = f::and_(f::and_(a[1], a[2]), a[3]);
    const auto high = f::and_(f::eq<64>(a[4], top), f::eq<64>(middle, mask));
    const auto low = f::shr<limb_bits, 64>(f::add<64>(a[0], factor));
    const auto over = f::or_(f::shr<top_bits, 64>(a[4]), f::and_(high, low));
    a[0] = f::madd52lo<64>(a[0], over, factor);

    propagate<0>(a, mask);
    propagate<1>(a, mask);
    propagate<2>(a, mask);
    propagate<3>(a, mask);
    a[4] = f::and_(a[4], top);
}

// Integral products accumulate in two 128 bit columns, where each column above
// 2^256 folds into a column below it as it completes.
template <typename Word>
constexpr void algorithm::multiply(field_t<Word>& r, const field_t<Word>& a,
    const field_t<Word>& b) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        field_t<uint64_t> out{};
        unsigned128_t c{}, d{};
        multiply_add(d, a[0], b[3]);
        multiply_add(d, a[1], b[2]);
        multiply_add(d, a[2], b[1]);
        multiply_add(d, a[3], b[0]);
        multiply_add(c, a[4], b[4]);
        multiply_add(d, c.low, fold_260);
        c = { 0, c.high };
        const auto t3 = take(d);

        multiply_add(d, a[0], b[4]);
        multiply_add(d, a[1], b[3]);
        multiply_add(d, a[2], b[2]);
        multiply_add(d, a[3], b[1]);
        multiply_add(d, a[4], b[0]);
        multiply_add(d, c.low, fold_272);
        auto t4 = take(d);
        const auto tx = t4 >> top_bits;
        t4 &= top_mask;

        c = {};
        multiply_add(c, a[0], b[0]);
        multiply_add(d, a[1], b[4]);
        multiply_add(d, a[2], b[3]);
        multiply_add(d, a[3], b[2]);
        multiply_add(d, a[4], b[1]);
        multiply_add(c, (take(d) << 4) | tx, fold_256);
        out[0] = take(c);

        multiply_add(c, a[0], b[1]);
        multiply_add(c, a[1], b[0]);
        multiply_add(d, a[2], b[4]);
        multiply_add(d, a[3], b[3]);
        multiply_add(d, a[4], b[2]);
        multiply_add(c, take(d), fold_260);
        out[1] = take(c);

        multiply_add(c, a[0], b[2]);
        multiply_add(c, a[1], b[1]);
        multiply_add(c, a[2], b[0]);
        multiply_add(d, a[3], b[4]);
        multiply_add(d, a[4], b[3]);
        multiply_add(c, d.low, fold_260);
        d = { 0, d.high };
        out[2] = take(c);

        multiply_add(c, d.low, fold_272);
        add(c, { 0, t3 });
        out[3] = take(c);
        out[4] = c.low + t4;
        r = out;
    }
    else
    {
        product_t<Word> c{};
        accumulate<0, 0>(c, a, b);
        accumulate<0, 1>(c, a, b);
        accumulate<0, 2>(c, a, b);
        accumulate<0, 3>(c, a, b);
        accumulate<0, 4>(c, a, b);
        accumulate<1, 0>(c, a, b);
        accumulate<1, 1>(c, a, b);
        accumulate<1, 2>(c, a, b);
        accumulate<1, 3>(c, a, b);
        accumulate<1, 4>(c, a, b);
        accumulate<2, 0>(c, a, b);
        accumulate<2, 1>(c, a, b);
        accumulate<2, 2>(c, a, b);
        accumulate<2, 3>(c, a, b);
        accumulate<2, 4>(c, a, b);
        accumulate<3, 0>(c, a, b);
        accumulate<3, 1>(c, a, b);
        accumulate<3, 2>(c, a, b);
        accumulate<3, 3>(c, a, b);
        accumulate<3, 4>(c, a, b);
        accumulate<4, 0>(c, a, b);
        accumulate<4, 1>(c, a, b);
        accumulate<4, 2>(c, a, b);
        accumulate<4, 3>(c, a, b);
        accumulate<4, 4>(c, a, b);
        reduce(r, c);
    }
}

template <typename Word>
constexpr void algorithm::square(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        const auto a0 = shift_left(a[0]);
        const auto a1 = shift_left(a[1]);
        const auto a2 = shift_left(a[2]);
        const auto a4 = shift_left(a[4]);

        field_t<uint64_t> out{};
        unsigned128_t c{}, d{};
        multiply_add(d, a0, a[3]);
        multiply_add(d, a1, a[2]);
        multiply_add(c, a[4], a[4]);
        multiply_add(d, c.low, fold_260);
        c = { 0, c.high };
        const auto t3 = take(d);

        multiply_add(d, a[0], a4);
        multiply_add(d, a1, a[3]);
        multiply_add(d, a[2], a[2]);
        multiply_add(d, c.low, fold_272);
        auto t4 = take(d);
        const auto tx = t4 >> top_bits;
        t4 &= top_mask;

        c = {};
        multiply_add(c, a[0], a[0]);
        multiply_add(d, a[1], a4);
        multiply_add(d, a2, a[3]);
        multiply_add(c, (take(d) << 4) | tx, fold_256);
        out[0] = take(c);

        multiply_add(c, a0, a[1]);
        multiply_add(d, a[2], a4);
        multiply_add(d, a[3], a[3]);
        multiply_add(c, take(d), fold_260);
        out[1] = take(c);

        multiply_add(c, a0, a[2]);
        multiply_add(c, a[1], a[1]);
        multiply_add(d, a[3], a4);
        multiply_add(c, d.low, fold_260);
        d = { 0, d.high };
        out[2] = take(c);

        multiply_add(c, d.low, fold_272);
        add(c, { 0, t3 });
        out[3] = take(c);
        out[4] = c.low + t4;
        r = out;
    }
    else
    {
        product_t<Word> c{};
        accumulate<0, 1>(c, a, a);
        accumulate<0, 2>(c, a, a);
        accumulate<0, 3>(c, a, a);
        accumulate<0, 4>(c, a, a);
        accumulate<1, 2>(c, a, a);
        accumulate<1, 3>(c, a, a);
        accumulate<1, 4>(c, a, a);
        accumulate<2, 3>(c, a, a);
        accumulate<2, 4>(c, a, a);
        accumulate<3, 4>(c, a, a);

        c[0] = f::add<64>(c[0], c[0]);
        c[1] = f::add<64>(c[1], c[1]);
        c[2] = f::add<64>(c[2], c[2]);
        c[3] = f::add<64>(c[3], c[3]);
        c[4] = f::add<64>(c[4], c[4]);
        c[5] = f::add<64>(c[5], c[5]);
        c[6] = f::add<64>(c[6], c[6]);
        c[7] = f::add<64>(c[7], c[7]);
        c[8] = f::add<64>(c[8], c[8]);
        c[9] = f::add<64>(c[9], c[9]);

        accumulate<0, 0>(c, a, a);
        accumulate<1, 1>(c, a, a);
        accumulate<2, 2>(c, a, a);
        accumulate<3, 3>(c, a, a);
        accumulate<4, 4>(c, a, a);
        reduce(r, c);
    }
}

template <size_t Count, typename Word>
constexpr void algorithm::square(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    r = a;
    for (size_t round{}; round < Count; ++round)
        square(r, r);
}

// Integral words invert by safegcd, and lanes by exponent p - 2, which is 1s
// blocks of { 223, 22, 1, 2, 1 } separated by 0s.
template <typename Word>
constexpr void algorithm::inverse(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    if constexpr (is_same_type<Word, uint64_t>)
    {
        auto value = a;
        normalize(value);

        signed62_t x{};
        to_signed62(x, value);
        invert(x, prime_modulus);
        from_signed62(r, x);
    }
    else
    {
        field_t<Word> x2{}, x22{}, x223{}, t{};
        powers(x2, x22, x223, a);

        square<23>(t, x223);
        multiply(t, t, x22);
        square<5>(t, t);
        multiply(t, t, a);
        square<3>(t, t);
        multiply(t, t, x2);
        square<2>(t, t);
        multiply(r, t, a);
    }
}

// Exponent (p + 1) / 4 is 1s blocks of { 223, 22, 2 } separated by 0s.
template <typename Word>
constexpr Word algorithm::square_root(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    field_t<Word> x2{}, x22{}, x223{}, t{};
    powers(x2, x22, x223, a);

    square<23>(t, x223);
    multiply(t, t, x22);
    square<6>(t, t);
    multiply(t, t, x2);
    square<2>(r, t);

    field_t<Word> root{}, value{ a };
    square(root, r);
    normalize(root);
    normalize(value);
    return equal(root, value);
}

// Field predicates.
// ----------------------------------------------------------------------------
// protected

template <typename Word>
constexpr Word algorithm::is_zero_element(const field_t<Word>& a) NOEXCEPT
{
    const auto merged = f::or_(f::or_(f::or_(f::or_(a[0], a[1]), a[2]), a[3]), a[4]);
    return f::eq<64>(merged, f::broadcast<Word>(uint64_t{}));
}

template <typename Word>
constexpr Word algorithm::is_odd_element(const field_t<Word>& a) NOEXCEPT
{
    const auto one = f::broadcast<Word>(uint64_t{ 1 });
    return f::eq<64>(f::and_(a[0], one), one);
}

template <typename Word>
constexpr Word algorithm::equal(const field_t<Word>& a,
    const field_t<Word>& b) NOEXCEPT
{
    const auto merged = f::or_(f::or_(f::or_(f::or_(
        f::xor_(a[0], b[0]),
        f::xor_(a[1], b[1])),
        f::xor_(a[2], b[2])),
        f::xor_(a[3], b[3])),
        f::xor_(a[4], b[4]));

    return f::eq<64>(merged, f::broadcast<Word>(uint64_t{}));
}

// Field encoding.
// ----------------------------------------------------------------------------
// protected

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

template <size_t Offset, size_t Size>
constexpr bool algorithm::from_bytes(field_t<uint64_t>& r,
    const data_array<Size>& bytes) NOEXCEPT
{
    scalar_t words{};
    decode<Offset>(words, bytes);
    to_field(r, words);

    return !((r[4] == top_mask) && ((r[1] & r[2] & r[3]) == limb_mask) &&
        (r[0] >= prime[0]));
}

constexpr void algorithm::to_field(field_t<uint64_t>& r,
    const scalar_t& words) NOEXCEPT
{
    r[0] =   words[0]                            & limb_mask;
    r[1] = ((words[0] >> 52) | (words[1] << 12)) & limb_mask;
    r[2] = ((words[1] >> 40) | (words[2] << 24)) & limb_mask;
    r[3] = ((words[2] >> 28) | (words[3] << 36)) & limb_mask;
    r[4] =   words[3] >> 16;
}

constexpr void algorithm::to_bytes(bytes_t& out,
    const field_t<uint64_t>& a) NOEXCEPT
{
    const scalar_t words
    {
        (a[0] >>  0) | (a[1] << 52),
        (a[1] >> 12) | (a[2] << 40),
        (a[2] >> 24) | (a[3] << 28),
        (a[3] >> 36) | (a[4] << 16)
    };

    encode(out, words);
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
