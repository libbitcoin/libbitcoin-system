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
    c[low] = f::madd52lo(c[low], a[Left], b[Right]);
    c[high] = f::madd52hi(c[high], a[Left], b[Right]);
}

template <size_t Index, typename Word>
INLINE constexpr void algorithm::fold(product_t<Word>& c, Word value,
    Word factor) NOEXCEPT
{
    constexpr auto next = add1(Index);
    c[Index] = f::madd52lo(c[Index], value, factor);
    c[next] = f::madd52hi(c[next], value, factor);
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

    const auto top = f::madd52hi(zero, c[9], factor);
    fold<0>(c, c[5], factor);
    fold<1>(c, c[6], factor);
    fold<2>(c, c[7], factor);
    fold<3>(c, c[8], factor);
    c[4] = f::madd52lo(c[4], c[9], factor);
    fold<0>(c, top, factor);

    r = { c[0], c[1], c[2], c[3], c[4] };
    carry(r);
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
    constexpr auto twice = 2u;
    r[0] = f::sub<64>(f::broadcast<Word>(prime[0] << twice), a[0]);
    r[1] = f::sub<64>(f::broadcast<Word>(prime[1] << twice), a[1]);
    r[2] = f::sub<64>(f::broadcast<Word>(prime[2] << twice), a[2]);
    r[3] = f::sub<64>(f::broadcast<Word>(prime[3] << twice), a[3]);
    r[4] = f::sub<64>(f::broadcast<Word>(prime[4] << twice), a[4]);
}

template <typename Word>
constexpr void algorithm::carry(field_t<Word>& a) NOEXCEPT
{
    const auto mask = f::broadcast<Word>(limb_mask);
    const auto over = f::shr<top_bits, 64>(a[4]);
    a[4] = f::and_(a[4], f::broadcast<Word>(top_mask));
    a[0] = f::madd52lo(a[0], over, f::broadcast<Word>(fold_256));

    propagate<0>(a, mask);
    propagate<1>(a, mask);
    propagate<2>(a, mask);
    propagate<3>(a, mask);
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
    a[0] = f::madd52lo(a[0], over, factor);

    propagate<0>(a, mask);
    propagate<1>(a, mask);
    propagate<2>(a, mask);
    propagate<3>(a, mask);
    a[4] = f::and_(a[4], top);
}

template <typename Word>
constexpr void algorithm::multiply(field_t<Word>& r, const field_t<Word>& a,
    const field_t<Word>& b) NOEXCEPT
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

template <typename Word>
constexpr void algorithm::square(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
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

template <size_t Count, typename Word>
constexpr void algorithm::square(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
{
    r = a;
    for (size_t round{}; round < Count; ++round)
        square(r, r);
}

// Exponent p - 2 is 1s blocks of { 223, 22, 1, 2, 1 } separated by 0s.
template <typename Word>
constexpr void algorithm::inverse(field_t<Word>& r,
    const field_t<Word>& a) NOEXCEPT
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
constexpr Word algorithm::is_zero(const field_t<Word>& a) NOEXCEPT
{
    const auto merged = f::or_(f::or_(f::or_(f::or_(a[0], a[1]), a[2]), a[3]),
        a[4]);

    return f::eq<64>(merged, f::broadcast<Word>(uint64_t{}));
}

template <typename Word>
constexpr Word algorithm::is_odd(const field_t<Word>& a) NOEXCEPT
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

constexpr bool algorithm::from_bytes(field_t<uint64_t>& r,
    const bytes_t& bytes) NOEXCEPT
{
    constexpr auto size = sizeof(uint64_t);
    std_array<uint64_t, 4> words{};
    for (size_t byte{}; byte < array_count<bytes_t>; ++byte)
    {
        auto& word = words[sub1(words.size()) - byte / size];
        word = (word << byte_bits) | bytes[byte];
    }

    r[0] = words[0] & limb_mask;
    r[1] = ((words[0] >> 52) | (words[1] << 12)) & limb_mask;
    r[2] = ((words[1] >> 40) | (words[2] << 24)) & limb_mask;
    r[3] = ((words[2] >> 28) | (words[3] << 36)) & limb_mask;
    r[4] = words[3] >> 16;

    return !((r[4] == top_mask) && ((r[1] & r[2] & r[3]) == limb_mask) &&
        (r[0] >= prime[0]));
}

constexpr void algorithm::to_bytes(bytes_t& out,
    const field_t<uint64_t>& a) NOEXCEPT
{
    constexpr auto size = sizeof(uint64_t);
    const std_array<uint64_t, 4> words
    {
        a[0] | (a[1] << 52),
        (a[1] >> 12) | (a[2] << 40),
        (a[2] >> 24) | (a[3] << 28),
        (a[3] >> 36) | (a[4] << 16)
    };

    for (size_t byte{}; byte < array_count<bytes_t>; ++byte)
    {
        const auto word = words[sub1(words.size()) - byte / size];
        const auto shift = (sub1(size) - byte % size) * byte_bits;
        out[byte] = narrow_cast<uint8_t>(word >> shift);
    }
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
