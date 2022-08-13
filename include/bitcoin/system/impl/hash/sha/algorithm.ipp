/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_IPP

#include <bit>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// FIPS PUB 180-4 [Secure Hash Standard (SHS)].
// All aspects of FIPS180 are supported within the implmentation.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

namespace libbitcoin {
namespace system {
namespace sha {

// TODO: integrate sha-ni/neon.
// TODO: vectorize (2/4/8/16 lanes).
// TODO: implement 5.3.6 SHA-512/t initial vector derivation.
// TODO: add SHA-256/224, 512/384, 512/224, 512/256 constants/types.

#define TEMPLATE template <typename SHA, bool Compressed, bool Vectorized, \
    bool Cached, if_same<typename SHA::T, shah_t> If>
#define CLASS algorithm<SHA, Compressed, Vectorized, Cached, If>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#if !defined(PRIMITIVES)

#if defined(HAVE_MSC)

// Extended integrals are neither c++ integrals nor stdlib integers.
static_assert(!std::is_integral_v<__m128i>);
static_assert(!std::is_integral_v<__m256i>);
static_assert(!std::is_integral_v<__m512i>);
static_assert(!std::numeric_limits<__m128i>::is_integer);
static_assert(!std::numeric_limits<__m256i>::is_integer);
static_assert(!std::numeric_limits<__m512i>::is_integer);

// Types work as integrals.
static_assert(is_same_type<mint128_t, __m128i>);
static_assert(is_same_type<mint256_t, __m256i>);
static_assert(is_same_type<mint512_t, __m512i>);

// sizeof() works as expected.
static_assert(to_bits(sizeof(__m128i)) == 128u);
static_assert(to_bits(sizeof(__m256i)) == 256u);
static_assert(to_bits(sizeof(__m512i)) == 512u);

// Primitives implement rotr/rotl because these are not available in SIMD.
// S is required for a non-member ror_/rol_ since there is no way otherwise to
// know if we are rotating 4/8/16x32 (sha160/256) or 2/4/8x64 (sha512).

// TODO: make all primitives private members with auto parameters.
// TODO: use sha160/256 to differentiate word_t interally, invoking the
// TODO: appropriate intrinsic (23/62 bit). Currently these are all 32 bit
// TODO: except for the natives, which rely on auto and native 32/64 bit ops.

// SSE4 primitives (for 32 bit word_t).
// ----------------------------------------------------------------------------
// Intel/AMD is always little-endian, and SHA is always big-endian (swap).
// load/set instructions are unaligned.

constexpr std_array<uint32_t, 4> bswap_mask
{
    0x0c0d0e0ful, 0x08090a0bul, 0x04050607ul, 0x00010203ul     // from simd
    ////0x00010203ul, 0x04050607ul, 0x08090a0bul, 0x0c0d0e0ful // from sha-ni
};

template <typename Word, if_same<Word, mint128_t> = true>
INLINE Word byte_swap_mask() NOEXCEPT
{
    static const auto mask = _mm_set_epi32(
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3]);
    return mask;
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE Word load(const uint8_t* data) NOEXCEPT
{
    return _mm_loadu_si128(pointer_cast<const Word>(data));
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE void store(uint8_t* data, Word value) NOEXCEPT
{
    _mm_storeu_si128(pointer_cast<Word>(data), value);
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE Word load_big_endian(const uint8_t* data) NOEXCEPT
{
    return _mm_shuffle_epi8(load<mint128_t>(data), byte_swap_mask<Word>());
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE void store_big_endian(uint8_t* data, Word value) NOEXCEPT
{
    store(data, _mm_shuffle_epi8(value, byte_swap_mask<Word>()));
}

template <unsigned int B, typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto shr_(Word a) NOEXCEPT
{
    return _mm_srli_epi32(a, B);
}

template <unsigned int B, typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto shl_(Word a) NOEXCEPT
{
    return _mm_slli_epi32(a, B);
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto ror_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shr_<B>(a), shl_<S - B>(a));
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto rol_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shl_<B>(a), shr_<S - B>(a));
}

template <typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto add_(Word a, Word b) NOEXCEPT
{
    return _mm_add_epi32(a, b);
}

template <unsigned int K, typename Word,
    if_same<Word, mint128_t> = true>
INLINE auto add_(Word a) NOEXCEPT
{
    // Broadcast 32-bit integer to all elements.
    return add_(a, _mm_set1_epi32(K));
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE auto and_(Word a, Word b) NOEXCEPT
{
    return _mm_and_si128(a, b);
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE auto or_(Word a, Word b) NOEXCEPT
{
    return _mm_or_si128(a, b);
}

template <typename Word, if_same<Word, mint128_t> = true>
INLINE auto xor_(Word a, Word b) NOEXCEPT
{
    return _mm_xor_si128(a, b);
}

// AVX2 primitives (for 32 bit word_t).
// ----------------------------------------------------------------------------

template <typename Word, if_same<Word, mint256_t> = true>
INLINE Word byte_swap_mask() NOEXCEPT
{
    static const auto mask = _mm256_set_epi32(
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3],
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3]);
    return mask;
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE Word load(const uint8_t* data) NOEXCEPT
{
    return _mm256_loadu_epi32(pointer_cast<const Word>(data));
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE void store(uint8_t* data, Word value) NOEXCEPT
{
    _mm256_storeu_epi32(pointer_cast<Word>(data), value);
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE Word load_big_endian(const uint8_t* data) NOEXCEPT
{
    return _mm256_shuffle_epi8(load<mint256_t>(data), byte_swap_mask<Word>());
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE void store_big_endian(uint8_t* data, Word value) NOEXCEPT
{
    store(data, _mm256_shuffle_epi8(value, byte_swap_mask<Word>()));
}

template <unsigned int B, typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto shr_(Word a) NOEXCEPT
{
    return _mm256_srli_epi32(a, B);
}

template <unsigned int B, typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto shl_(Word a) NOEXCEPT
{
    return _mm256_slli_epi32(a, B);
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto ror_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shr_<B>(a), shl_<S - B>(a));
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto rol_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shl_<B>(a), shr_<S - B>(a));
}

template <typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto add_(Word a, Word b) NOEXCEPT
{
    return _mm256_add_epi32(a, b);
}

template <unsigned int K, typename Word,
    if_same<Word, mint256_t> = true>
INLINE auto add_(Word a) NOEXCEPT
{
    // Broadcast 32-bit integer to all elements.
    return add_(a, _mm256_set1_epi32(K));
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE auto and_(Word a, Word b) NOEXCEPT
{
    return _mm256_and_si256(a, b);
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE auto or_(Word a, Word b) NOEXCEPT
{
    return _mm256_or_si256(a, b);
}

template <typename Word, if_same<Word, mint256_t> = true>
INLINE auto xor_(Word a, Word b) NOEXCEPT
{
    return _mm256_xor_si256(a, b);
}

// AVX512 primitives (for 32 bit word_t).
// ----------------------------------------------------------------------------

template <typename Word, if_same<Word, mint512_t> = true>
INLINE Word byte_swap_mask() NOEXCEPT
{
    static const auto mask = _mm512_set_epi32(
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3],
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3],
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3],
        bswap_mask[0], bswap_mask[1], bswap_mask[2], bswap_mask[3]);
    return mask;
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE Word load(const uint8_t* data) NOEXCEPT
{
    return _mm512_loadu_epi32(pointer_cast<const Word>(data));
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE void store(uint8_t* data, Word value) NOEXCEPT
{
    _mm512_storeu_epi32(pointer_cast<Word>(data), value);
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE Word load_big_endian(const uint8_t* data) NOEXCEPT
{
    return _mm512_shuffle_epi8(load<mint512_t>(data), byte_swap_mask<Word>());
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE void store_big_endian(uint8_t* data, Word value) NOEXCEPT
{
    store(data, _mm512_shuffle_epi8(value, byte_swap_mask<Word>()));
}

template <unsigned int B, typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto shr_(Word a) NOEXCEPT
{
    return _mm512_srli_epi32(a, B);
}

template <unsigned int B, typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto shl_(Word a) NOEXCEPT
{
    return _mm512_slli_epi32(a, B);
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto ror_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shr_<B>(a), shl_<S - B>(a));
}

template <unsigned int B, unsigned int S, typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto rol_(Word a) NOEXCEPT
{
    // TODO: S will become unnecessary as the function set must be 32/64.
    return or_(shl_<B>(a), shr_<S - B>(a));
}

template <typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto add_(Word a, Word b) NOEXCEPT
{
    return _mm512_add_epi32(a, b);
}

template <unsigned int K, typename Word,
    if_same<Word, mint512_t> = true>
INLINE auto add_(Word a) NOEXCEPT
{
    // Broadcast 32-bit integer to all elements.
    return add_(a, _mm512_set1_epi32(K));
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE auto and_(Word a, Word b) NOEXCEPT
{
    return _mm512_and_si512(a, b);
}

template <typename Word, if_same<Word, mint512_t> = true>
INLINE auto or_(Word a, Word b) NOEXCEPT
{
    return _mm512_or_si512(a, b);
}
template <typename Word, if_same<Word, mint512_t> = true>
INLINE auto xor_(Word a, Word b) NOEXCEPT
{
    return _mm512_xor_si512(a, b);
}

#endif // HAVE_MSC

// Integral primitives (for 32/64 bit word_t).
// ----------------------------------------------------------------------------

template <unsigned int B, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto shr_(Word a) NOEXCEPT
{
    return a >> B;
}

// unused by sha
template <unsigned int B, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto shl_(Word a) NOEXCEPT
{
    return a << B;
}

template <unsigned int B, unsigned int = 0, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto ror_(Word a) NOEXCEPT
{
    return rotr<B>(a);
}

template <unsigned int B, unsigned int = 0, typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto rol_(Word a) NOEXCEPT
{
    return rotl<B>(a);
}

// auto in template?
template <auto B, typename Word,
    if_same<decltype(B), Word> = true,
    if_integral_integer<Word> = true>
INLINE constexpr auto add_(Word a) NOEXCEPT
{
    return a + B;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto add_(Word a, Word b) NOEXCEPT
{
    return a + b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto and_(Word a, Word b) NOEXCEPT
{
    return a & b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto or_(Word a, Word b) NOEXCEPT
{
    return a | b;
}

template <typename Word,
    if_integral_integer<Word> = true>
INLINE constexpr auto xor_(Word a, Word b) NOEXCEPT
{
    return a ^ b;
}

#endif // PRIMITIVES

// 4.1 Functions
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
parity(auto x, auto y, auto z) NOEXCEPT
{
    return xor_(xor_(x, y), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form reduced.
    ////return (x & y) ^ (~x & z);
    return xor_(and_(x, xor_(y, z)), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form reduced.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return or_(and_(x, or_(y, z)), and_(y, z));
}

TEMPLATE
template <unsigned int V, unsigned int W, unsigned int X, unsigned int Y,
    unsigned int Z>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    // TODO: not necesarily preferred for AVX (non-destructive), SSE optimal.
    // On AVX architectures, the VEX-encoded SIMD instructions are nondestructive.
    //
    // intel.com/content/dam/www/public/us/en/documents/white-papers/
    // fast-sha512-implementations-ia-processors-paper.pdf
    //
    // Examples:
    // s0(a) = (a >>>  1) ^ (a >>>  8) ^ (a >> 7)
    // s1(e) = (e >>> 19) ^ (e >>> 61) ^ (e >> 6)
    //
    // s0(a) = ((a >>  1) ^ (a << 63)) ^ ((a >>  8) ^ (a << 56)) ^ (a >> 7)
    // s1(e) = ((e >> 19) ^ (e << 45)) ^ ((e >> 61) ^ (e <<  3)) ^ (e >> 6)
    //
    // s0(a) = (((((a >>  1) ^ a) >>  6) ^ a) >> 1) ^ ((a <<  7) ^ a) << 56
    // s1(e) = (((((e >> 42) ^ e) >> 13) ^ e) >> 6) ^ ((e << 42) ^ e) <<  3
    //
    // Generalization:
    // s (n) = (n >>> x) ^ (n >>> y) ^ (n >> z)
    // s'(n) = (((((n >> V) ^ n) >> W) ^ n) >> X) ^ ((n << Y) ^ n) << Z
    // V =  y - z | y - x
    // W =  z - x | x - z
    // X =      x | z
    // Y =  y - x | y - x
    // Z = 64 - y | 64 - y
    // s0'(n) = (((((n >> (y-z)) ^ n) >> (z-x)) ^ n) >> (x)) ^ ((n << (y-x)) ^ n) << (64-y)
    // s1'(n) = (((((n >> (y-x)) ^ n) >> (x-z)) ^ n) >> (z)) ^ ((n << (y-x)) ^ n) << (64-y)
    //
    // Ambiguous (and mismatched) parenthetic in original, this is explicit and correct:
    // s0'(n) = [((((n >> (y-z)) ^ n) >> (z-x)) ^ n) >> (x)] ^ [((n << (y-x)) ^ n) << (64-y)]
    // s1'(n) = [((((n >> (y-x)) ^ n) >> (x-z)) ^ n) >> (z)] ^ [((n << (y-x)) ^ n) << (64-y)]

    constexpr auto s = SHA::word_bits;
    return xor_
    (
        shr_<V>(xor_(shr_<X>(xor_(shr_<Y>(x), x)), x)),
        shl_<s - W>(xor_(shl_<Z>(x), x))
    );
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C, if_equal<C, 7>>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    // Denormalized algorithm requires parameter shift for sigma0 sha512.
    return sigma_<A, B, C - A, B - C, B - A>(x);
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C, if_not_equal<C, 7>>
INLINE constexpr auto CLASS::
sigma_(auto x) NOEXCEPT
{
    return sigma_<C, B, A - C, B - A, B - A>(x);
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
sigma(auto x) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;

    // TODO: using Vectorized as a test switch for now.
    if constexpr (Vectorized)
    {
        return sigma_<A, B, C>(x);
    }
    else
    {
        return xor_(xor_(ror_<A, s>(x), ror_<B, s>(x)), shr_<C>(x));
    }
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
Sigma(auto x) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;

    // This is specialized for non-vector destructive ror.
    // "Because the ror instruction is destructive (that is, it overwrites the
    // operand), implementing the above as written would involve a number of
    // source register copy operations. If, however, the expressions are
    // rewritten as... Then the number of register copies can be minimized."
    // intel.com/content/dam/www/public/us/en/documents/white-papers/
    // sha-256-implementations-paper.pdf
    // AVX optimal (sha256)
    //
    // Examples:
    // S0(a) = (a >>> 2) ^ (a >>> 13) ^ (a >>> 22)
    // S1(e) = (e >>> 6) ^ (e >>> 11) ^ (e >>> 25)
    // S0(a) = ((((a >>>  9) ^ a) >>> 11) ^ a) >>> 2
    // S1(e) = ((((e >>> 14) ^ e) >>>  5) ^ e) >>> 6
    //
    // Generalization:
    // S (n) = (n >>> x) ^ (n >>> y) ^ (n >>> z)
    // S'(n) = ((((n >>> X) ^ n) >>> Y) ^ n) >>> Z
    // X = z - y
    // Y = y - x
    // Z = x
    // S'(n) = ((((n >>> (z-y)) ^ n) >>> (y-x)) ^ n) >>> x

    // TODO: using Compressed as a test switch for now.
    if constexpr (Compressed)
    {
        return ror_<A, s>(xor_(ror_<B - A, s>(xor_(ror_<C - B, s>(x), x)), x));
    }
    else
    {
        return xor_(xor_(ror_<A, s>(x), ror_<B, s>(x)), ror_<C, s>(x));
    }
}

// Wrappers
// ---------------------------------------------------------------------------

TEMPLATE
INLINE constexpr auto CLASS::
sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<7, 18, 3>(x);
    else
        return sigma<1, 8, 7>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<17, 19, 10>(x);
    else
        return sigma<19, 61, 6>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<2, 13, 22>(x);
    else
        return Sigma<28, 34, 39>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<6, 11, 25>(x);
    else
        return Sigma<14, 18, 41>(x);
}

// Rounds
// ---------------------------------------------------------------------------

TEMPLATE
template<size_t Round, typename Auto>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = Round / K::columns;

    if constexpr (fn == 0u)
        return &self::template choice<Auto, Auto, Auto>;
    else if constexpr (fn == 1u)
        return &self::template parity<Auto, Auto, Auto>;
    else if constexpr (fn == 2u)
        return &self::template majority<Auto, Auto, Auto>;
    else if constexpr (fn == 3u)
        return &self::template parity<Auto, Auto, Auto>;
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto a, auto& b, auto c, auto d, auto& e, auto wk) NOEXCEPT
{
    constexpr auto f = functor<Round, decltype(a)>();
    e = /*a =*/ add_(add_(add_(rol_<5>(a), f(b, c, d)), e), wk);
    b = /*c =*/ rol_<30>(b);

    // SHA-NI
    // Four rounds (total rounds 80/4).
    // First round is add(e, w), then sha1nexte(e, w).
    // fk is round-based enumeration implying f selection and k value.
    //     e1 = sha1nexte(e0, w);
    //     abcd = sha1rnds4(abcd, e0, fk);
    // NEON
    // f is implied by k in wk.
    //     e1 = vsha1h(vgetq_lane(abcd, 0);
    //     vsha1cq(abcd, e0, vaddq(w, k));
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto a, auto b, auto c, auto& d, auto e, auto f, auto g, auto& h,
    auto wk) NOEXCEPT
{
    // TODO: if open lanes, vectorize Sigma0 and Sigma1 (see Intel).
    const auto t = add_(add_(add_(Sigma1(e), choice(e, f, g)), h), wk);
    d = /*e =*/    add_(d, t);
    h = /*a =*/    add_(add_(Sigma0(a), majority(a, b, c)), t);

    // Rounds can be cut in half and this round doubled (intel paper).
    // Avoids the need for a temporary variable and aligns with SHA-NI.
    // Removing the temporary eliminates 2x64 instructions from the assembly.
    // h = /*a =*/ SIGMA0(a) + SIGMA1(e) + majority(a, b, c) +
    //             choice(e, f, g) + (k + w) + h;
    // d = /*e =*/ SIGMA1(e) + choice(e, f, g) + (k + w) + h + d;
    //
    // Each call is 2 rounds, 4 rounds total.
    // s, w and k are 128 (4 words each, s1/s2 is 8 word state).
    // SHA-NI
    //     const auto value = add(w, k);
    //     abcd = sha256rnds2(abcd, efgh, value);
    //     efgh = sha256rnds2(efgh, abcd, shuffle(value));
    // NEON
    //     const auto value = vaddq(w, k);
    //     abcd = vsha256hq(abcd, efgh, value);
    //     efgh = vsha256h2q(efgh, abcd, value);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto& state, const auto& wk) NOEXCEPT
{
    if constexpr (SHA::strength == 160)
    {
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words], // c->b
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words],
            state[(SHA::rounds + 4 - Round) % SHA::state_words], // a->e
            wk[Round]);

        // SNA-NI/NEON
        // State packs in 128 (one state variable), reduces above to 1 out[].
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
    else
    {
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words],
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words], // e->d
            state[(SHA::rounds + 4 - Round) % SHA::state_words],
            state[(SHA::rounds + 5 - Round) % SHA::state_words],
            state[(SHA::rounds + 6 - Round) % SHA::state_words],
            state[(SHA::rounds + 7 - Round) % SHA::state_words], // a->h
            wk[Round]);

        // SHA-NI/NEON
        // Each element is 128 (vs. 32), reduces above to 2 out[] (s0/s1).
        // Input value is 128 (w). Constants (k) statically initialized as 128.
    }
}

TEMPLATE
constexpr void CLASS::
compress(auto& state, const auto& buffer) NOEXCEPT
{
    // SHA-NI/256: 64/4 = 16 quad rounds, 8/4 = 2 state elements.
    const state_t start{ state };

    round< 0>(state, buffer);
    round< 1>(state, buffer);
    round< 2>(state, buffer);
    round< 3>(state, buffer);
    round< 4>(state, buffer);
    round< 5>(state, buffer);
    round< 6>(state, buffer);
    round< 7>(state, buffer);
    round< 8>(state, buffer);
    round< 9>(state, buffer);
    round<10>(state, buffer);
    round<11>(state, buffer);
    round<12>(state, buffer);
    round<13>(state, buffer);
    round<14>(state, buffer);
    round<15>(state, buffer);

    round<16>(state, buffer);
    round<17>(state, buffer);
    round<18>(state, buffer);
    round<19>(state, buffer);
    round<20>(state, buffer);
    round<21>(state, buffer);
    round<22>(state, buffer);
    round<23>(state, buffer);
    round<24>(state, buffer);
    round<25>(state, buffer);
    round<26>(state, buffer);
    round<27>(state, buffer);
    round<28>(state, buffer);
    round<29>(state, buffer);
    round<30>(state, buffer);
    round<31>(state, buffer);

    round<32>(state, buffer);
    round<33>(state, buffer);
    round<34>(state, buffer);
    round<35>(state, buffer);
    round<36>(state, buffer);
    round<37>(state, buffer);
    round<38>(state, buffer);
    round<39>(state, buffer);
    round<40>(state, buffer);
    round<41>(state, buffer);
    round<42>(state, buffer);
    round<43>(state, buffer);
    round<44>(state, buffer);
    round<45>(state, buffer);
    round<46>(state, buffer);
    round<47>(state, buffer);

    round<48>(state, buffer);
    round<49>(state, buffer);
    round<50>(state, buffer);
    round<51>(state, buffer);
    round<52>(state, buffer);
    round<53>(state, buffer);
    round<54>(state, buffer);
    round<55>(state, buffer);
    round<56>(state, buffer);
    round<57>(state, buffer);
    round<58>(state, buffer);
    round<59>(state, buffer);
    round<60>(state, buffer);
    round<61>(state, buffer);
    round<62>(state, buffer);
    round<63>(state, buffer);

    if constexpr (SHA::rounds == 80)
    {
        round<64>(state, buffer);
        round<65>(state, buffer);
        round<66>(state, buffer);
        round<67>(state, buffer);
        round<68>(state, buffer);
        round<69>(state, buffer);
        round<70>(state, buffer);
        round<71>(state, buffer);
        round<72>(state, buffer);
        round<73>(state, buffer);
        round<74>(state, buffer);
        round<75>(state, buffer);
        round<76>(state, buffer);
        round<77>(state, buffer);
        round<78>(state, buffer);
        round<79>(state, buffer);
    }

    summarize(state, start);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
prepare(auto& buffer) NOEXCEPT
{
    // K is added to schedule words because schedule is vectorizable.
    constexpr auto r00 = Round;
    constexpr auto r02 = r00 - 2;
    constexpr auto r03 = r00 - 3;
    constexpr auto r07 = r00 - 7;
    constexpr auto r08 = r00 - 8;
    constexpr auto r14 = r00 - 14;
    constexpr auto r15 = r00 - 15;
    constexpr auto r16 = r00 - 16;
    constexpr auto k0 = K::get[r16];
    constexpr auto k1 = K::get[add1(r16)];

    if constexpr (SHA::strength == 160)
    {
        buffer[r00] = rol_<1>(xor_(
            xor_(buffer[r16], buffer[r14]),
            xor_(buffer[r08], buffer[r03])));

        buffer[r16] = add_<k0>(buffer[r16]);

        buffer[add1(r00)] = rol_<1>(xor_(
            xor_(buffer[add1(r16)], buffer[add1(r14)]),
            xor_(buffer[add1(r08)], buffer[add1(r03)])));

        buffer[add1(r16)] = add_<k1>(buffer[add1(r16)]);

        // SHA-NI
        //     buffer[Round] = sha1msg2 // xor and rotl1
        //     (
        //         xor                // not using sha1msg1
        //         (
        //             sha1msg1       // xor (specialized)
        //             (
        //                 buffer[Round - 16],
        //                 buffer[Round - 14]
        //             ),
        //             buffer[Round -  8]
        //          ),
        //          buffer[Round -  3]
        //     );
        // NEON
        //     vsha1su1q/vsha1su0q
    }
    else
    {
        // TODO: if open lanes, vectorize sigma0 and sigma1 (see Intel).
        buffer[r00] = add_(
            add_(buffer[r16], sigma0(buffer[r15])),
            add_(buffer[r07], sigma1(buffer[r02])));

        buffer[r16] = add_<k0>(buffer[r16]);

        buffer[add1(r00)] = add_(
            add_(buffer[add1(r16)], sigma0(buffer[add1(r15)])),
            add_(buffer[add1(r07)], sigma1(buffer[add1(r02)])));

        buffer[add1(r16)] = add_<k1>(buffer[add1(r16)]);

        // Each word is 128, buffer goes from 64 to 16 words.
        // SHA-NI
        // buffer[Round] =
        //     sha256msg1(buffer[Round - 16], buffer[Round - 15]) +
        //     sha256msg2(buffer[Round -  7], buffer[Round -  2]);
        // NEON
        // Not sure about these indexes.
        // mijailovic.net/2018/06/06/sha256-armv8
        // buffer[Round] =
        //     vsha256su0q(buffer[Round - 13], buffer[Round - 9])
        //     vsha256su1q(buffer[Round - 13], buffer[Round - 5],
        //                 buffer[Round - 1]);
    }

    // Add K to last 16 words.
    if constexpr (Round == sub1(sub1(SHA::rounds)))
    {
        constexpr auto r = SHA::rounds - array_count<words_t>;
        buffer[r + 0] = add_<K::get[r + 0]>(buffer[r + 0]);
        buffer[r + 1] = add_<K::get[r + 1]>(buffer[r + 1]);
        buffer[r + 2] = add_<K::get[r + 2]>(buffer[r + 2]);
        buffer[r + 3] = add_<K::get[r + 3]>(buffer[r + 3]);
        buffer[r + 4] = add_<K::get[r + 4]>(buffer[r + 4]);
        buffer[r + 5] = add_<K::get[r + 5]>(buffer[r + 5]);
        buffer[r + 6] = add_<K::get[r + 6]>(buffer[r + 6]);
        buffer[r + 7] = add_<K::get[r + 7]>(buffer[r + 7]);
        buffer[r + 8] = add_<K::get[r + 8]>(buffer[r + 8]);
        buffer[r + 9] = add_<K::get[r + 9]>(buffer[r + 9]);
        buffer[r + 10] = add_<K::get[r + 10]>(buffer[r + 10]);
        buffer[r + 11] = add_<K::get[r + 11]>(buffer[r + 11]);
        buffer[r + 12] = add_<K::get[r + 12]>(buffer[r + 12]);
        buffer[r + 13] = add_<K::get[r + 13]>(buffer[r + 13]);
        buffer[r + 14] = add_<K::get[r + 14]>(buffer[r + 14]);
        buffer[r + 15] = add_<K::get[r + 15]>(buffer[r + 15]);
    }
}

TEMPLATE
constexpr void CLASS::
schedule(auto& buffer) NOEXCEPT
{
    // Schedule preparation rounds are compressed by two.

    prepare<16>(buffer);
    prepare<18>(buffer);
    prepare<20>(buffer);
    prepare<22>(buffer);
    prepare<24>(buffer);
    prepare<26>(buffer);
    prepare<28>(buffer);
    prepare<30>(buffer);

    prepare<32>(buffer);
    prepare<34>(buffer);
    prepare<36>(buffer);
    prepare<38>(buffer);
    prepare<40>(buffer);
    prepare<42>(buffer);
    prepare<44>(buffer);
    prepare<46>(buffer);

    prepare<48>(buffer);
    prepare<50>(buffer);
    prepare<52>(buffer);
    prepare<54>(buffer);
    prepare<56>(buffer);
    prepare<58>(buffer);
    prepare<60>(buffer);
    prepare<62>(buffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare<64>(buffer);
        prepare<66>(buffer);
        prepare<68>(buffer);
        prepare<70>(buffer);
        prepare<72>(buffer);
        prepare<74>(buffer);
        prepare<76>(buffer);
        prepare<78>(buffer);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
summarize(auto& out, const auto& in) NOEXCEPT
{
    out[0] = add_(out[0], in[0]);
    out[1] = add_(out[1], in[1]);
    out[2] = add_(out[2], in[2]);
    out[3] = add_(out[3], in[3]);
    out[4] = add_(out[4], in[4]);

    if constexpr (SHA::strength != 160)
    {
        out[5] = add_(out[5], in[5]);
        out[6] = add_(out[6], in[6]);
        out[7] = add_(out[7], in[7]);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input(buffer_t& buffer, const state_t& state) NOEXCEPT
{
    // This is a double hash optimization.
    if (std::is_constant_evaluated())
    {
        buffer[0] = state[0];
        buffer[1] = state[1];
        buffer[2] = state[2];
        buffer[3] = state[3];
        buffer[4] = state[4];

        if constexpr (SHA::strength != 160)
        {
            buffer[5] = state[5];
            buffer[6] = state[6];
            buffer[7] = state[7];
        }
    }
    else
    {
        array_cast<word_t, array_count<state_t>>(buffer) = state;
    }
}

// 5.2 Parsing the Message
// ---------------------------------------------------------------------------
// big-endian I/O is conventional for SHA.

TEMPLATE
INLINE constexpr void CLASS::
input(buffer_t& buffer, const block_t& block) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big< 0 * size>(buffer.at(0), block);
        from_big< 1 * size>(buffer.at(1), block);
        from_big< 2 * size>(buffer.at(2), block);
        from_big< 3 * size>(buffer.at(3), block);
        from_big< 4 * size>(buffer.at(4), block);
        from_big< 5 * size>(buffer.at(5), block);
        from_big< 6 * size>(buffer.at(6), block);
        from_big< 7 * size>(buffer.at(7), block);
        from_big< 8 * size>(buffer.at(8), block);
        from_big< 9 * size>(buffer.at(9), block);
        from_big<10 * size>(buffer.at(10), block);
        from_big<11 * size>(buffer.at(11), block);
        from_big<12 * size>(buffer.at(12), block);
        from_big<13 * size>(buffer.at(13), block);
        from_big<14 * size>(buffer.at(14), block);
        from_big<15 * size>(buffer.at(15), block);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word_t>(block);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
        buffer[8] = native_from_big_end(in[8]);
        buffer[9] = native_from_big_end(in[9]);
        buffer[10] = native_from_big_end(in[10]);
        buffer[11] = native_from_big_end(in[11]);
        buffer[12] = native_from_big_end(in[12]);
        buffer[13] = native_from_big_end(in[13]);
        buffer[14] = native_from_big_end(in[14]);
        buffer[15] = native_from_big_end(in[15]);
    }
    else
    {
        array_cast<word_t, array_count<words_t>>(buffer) =
            array_cast<word_t>(block);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input1(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(0), half);
        from_big<1 * size>(buffer.at(1), half);
        from_big<2 * size>(buffer.at(2), half);
        from_big<3 * size>(buffer.at(3), half);
        from_big<4 * size>(buffer.at(4), half);
        from_big<5 * size>(buffer.at(5), half);
        from_big<6 * size>(buffer.at(6), half);
        from_big<7 * size>(buffer.at(7), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word_t>(half);
        buffer[0] = native_from_big_end(in[0]);
        buffer[1] = native_from_big_end(in[1]);
        buffer[2] = native_from_big_end(in[2]);
        buffer[3] = native_from_big_end(in[3]);
        buffer[4] = native_from_big_end(in[4]);
        buffer[5] = native_from_big_end(in[5]);
        buffer[6] = native_from_big_end(in[6]);
        buffer[7] = native_from_big_end(in[7]);
    }
    else
    {
        array_cast<word_t, array_count<chunk_t>>(buffer) =
            array_cast<word_t>(half);
    }
}

TEMPLATE
INLINE constexpr void CLASS::
input2(buffer_t& buffer, const half_t& half) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        from_big<0 * size>(buffer.at(8), half);
        from_big<1 * size>(buffer.at(9), half);
        from_big<2 * size>(buffer.at(10), half);
        from_big<3 * size>(buffer.at(11), half);
        from_big<4 * size>(buffer.at(12), half);
        from_big<5 * size>(buffer.at(13), half);
        from_big<6 * size>(buffer.at(14), half);
        from_big<7 * size>(buffer.at(15), half);
    }
    else if constexpr (bc::is_little_endian)
    {
        const auto& in = array_cast<word_t>(half);
        buffer[8]  = native_from_big_end(in[0]);
        buffer[9]  = native_from_big_end(in[1]);
        buffer[10] = native_from_big_end(in[2]);
        buffer[11] = native_from_big_end(in[3]);
        buffer[12] = native_from_big_end(in[4]);
        buffer[13] = native_from_big_end(in[5]);
        buffer[14] = native_from_big_end(in[6]);
        buffer[15] = native_from_big_end(in[7]);
    }
    else
    {
        constexpr auto chunk = array_count<chunk_t>;
        array_cast<word_t, chunk, chunk>(buffer) =
            array_cast<word_t>(half);
    }
}

TEMPLATE
INLINE constexpr typename CLASS::digest_t CLASS::
output(const state_t& state) NOEXCEPT
{
    digest_t digest{};

    if (std::is_constant_evaluated())
    {
        constexpr auto size = SHA::word_bytes;
        to_big<0 * size>(digest, state.at(0));
        to_big<1 * size>(digest, state.at(1));
        to_big<2 * size>(digest, state.at(2));
        to_big<3 * size>(digest, state.at(3));
        to_big<4 * size>(digest, state.at(4));

        if constexpr (SHA::strength != 160)
        {
            to_big<5 * size>(digest, state.at(5));
            to_big<6 * size>(digest, state.at(6));
            to_big<7 * size>(digest, state.at(7));
        }
    }
    else if constexpr (bc::is_little_endian)
    {
        auto& out = array_cast<word_t>(digest);
        out[0] = native_to_big_end(state[0]);
        out[1] = native_to_big_end(state[1]);
        out[2] = native_to_big_end(state[2]);
        out[3] = native_to_big_end(state[3]);
        out[4] = native_to_big_end(state[4]);

        if constexpr (SHA::strength != 160)
        {
            out[5] = native_to_big_end(state[5]);
            out[6] = native_to_big_end(state[6]);
            out[7] = native_to_big_end(state[7]);
        }
    }
    else
    {
        array_cast<word_t>(digest) = state;
    }

    return digest;
}

// 5.1 Padding the Message
// ---------------------------------------------------------------------------

TEMPLATE
template<size_t Blocks>
CONSTEVAL typename CLASS::buffer_t CLASS::
scheduled_pad() NOEXCEPT
{
    // This precomputed padding is limited to one word of counter.
    static_assert(Blocks <= maximum<word_t> / byte_bits);

    // See comments in accumulator regarding padding endianness.
    constexpr auto index = sub1(array_count<words_t>);
    constexpr auto bytes = safe_multiply(Blocks, array_count<block_t>);

    buffer_t out{};
    out.front() = bit_hi<word_t>;
    out.at(index) = possible_narrow_cast<word_t>(to_bits(bytes));
    schedule(out);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::chunk_t CLASS::
chunk_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    constexpr auto bytes = possible_narrow_cast<word_t>(array_count<half_t>);

    chunk_t out{};
    out.front() = bit_hi<word_t>;
    out.back() = to_bits(bytes);
    return out;
}

TEMPLATE
CONSTEVAL typename CLASS::pad_t CLASS::
stream_pad() NOEXCEPT
{
    // See comments in accumulator regarding padding endianness.
    pad_t out{};
    out.front() = bit_hi<word_t>;
    return out;
}

TEMPLATE
template<size_t Blocks>
constexpr void CLASS::
schedule_n(buffer_t& buffer) NOEXCEPT
{
    // Scheduled padding for n whole blocks.
    // This will compile in 4*64 (sha256) or 4*128 (sha512) bytes for each
    // unique size of blocks array hashed by callers (by template expansion).
    // and one for each that is cached for block vectors in schedule_n().
    constexpr auto pad = scheduled_pad<Blocks>();
    buffer = pad;
}

TEMPLATE
constexpr void CLASS::
schedule_n(buffer_t& buffer, size_t blocks) NOEXCEPT
{
    // This optimization is saves ~30% in message scheduling for one out of
    // N blocks: (N + 70%)/(N + 100%). So the proportional benefit decreases
    // exponentially with increasing N. For arbitrary data lengths this will
    // benefit 1/64 hashes on average. All array-sized n-block hashes have
    // precomputed schedules - this benefits only finalized chunk hashing.
    // Testing shows a 5% performance improvement for 128 byte chunk hashes.
    // Accumulator passes all write() blocks here.
    if constexpr (Cached)
    {
        switch (blocks)
        {
            case 1: schedule_n<1>(buffer); return;
            case 2: schedule_n<2>(buffer); return;
            case 3: schedule_n<3>(buffer); return;
            case 4: schedule_n<4>(buffer); return;
            default:
            {
                pad_n(buffer, blocks);
                schedule(buffer);
                return;
            }
        }
    }
    else
    {
        pad_n(buffer, blocks);
        schedule(buffer);
        return;
    }
}

TEMPLATE
constexpr void CLASS::
schedule_1(buffer_t& buffer) NOEXCEPT
{
    // This ensures single block padding is always prescheduled.
    schedule_n<one>(buffer);
}

TEMPLATE
constexpr void CLASS::
pad_half(buffer_t& buffer) NOEXCEPT
{
    // Pad for any half block, unscheduled buffer.
    constexpr auto pad = chunk_pad();

    if (std::is_constant_evaluated())
    {
        buffer.at(8) = pad.at(0);
        buffer.at(9) = pad.at(1);
        buffer.at(10) = pad.at(2);
        buffer.at(11) = pad.at(3);
        buffer.at(12) = pad.at(4);
        buffer.at(13) = pad.at(5);
        buffer.at(14) = pad.at(6);
        buffer.at(15) = pad.at(7);
    }
    else
    {
        constexpr auto size = array_count<chunk_t>;
        array_cast<word_t, size, size>(buffer) = pad;
    }
}

TEMPLATE
constexpr void CLASS::
pad_n(buffer_t& buffer, count_t blocks) NOEXCEPT
{
    // Pad any number of whole blocks, unscheduled buffer.
    constexpr auto pad = stream_pad();
    const auto bits = to_bits(blocks * array_count<block_t>);

    if (std::is_constant_evaluated())
    {
        buffer.at(0)  = pad.at(0);
        buffer.at(1)  = pad.at(1);
        buffer.at(2)  = pad.at(2);
        buffer.at(3)  = pad.at(3);
        buffer.at(4)  = pad.at(4);
        buffer.at(5)  = pad.at(5);
        buffer.at(6)  = pad.at(6);
        buffer.at(7)  = pad.at(7);
        buffer.at(8)  = pad.at(8);
        buffer.at(9)  = pad.at(9);
        buffer.at(10) = pad.at(10);
        buffer.at(11) = pad.at(11);
        buffer.at(12) = pad.at(12);
        buffer.at(13) = pad.at(13);
        buffer.at(14) = hi_word<word_t>(bits);
        buffer.at(15) = lo_word<word_t>(bits);
    }
    else
    {
        array_cast<word_t, array_count<pad_t>>(buffer) = pad;

        // Split count into hi/low words and assign end of padded buffer.
        buffer[14] = hi_word<word_t>(bits);
        buffer[15] = lo_word<word_t>(bits);
    }
}

// Hashing.
// ---------------------------------------------------------------------------
// No hash(state_t) optimizations for sha160 (requires chunk_t/half_t).

TEMPLATE
typename CLASS::digest_t CLASS::
hash(const iterable<block_t>& blocks) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n(buffer, blocks.size());
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const blocks_t& blocks) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n(buffer, blocks.size());
    compress(state, buffer);
    return output(state);
}

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
hash(const std_array<block_t, Size>& blocks) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n<Size>(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const block_t& block) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const state_t& state) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state2 = H::get;
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    compress(state2, buffer);
    return output(state2);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& half) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
hash(const half_t& left, const half_t& right) NOEXCEPT
{
    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, left);
    input2(buffer, right);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);
    return output(state);
}

// Double Hashing.
// ---------------------------------------------------------------------------
// No double_hash optimizations for sha160 (requires chunk_t/half_t).

TEMPLATE
typename CLASS::digest_t CLASS::
double_hash(const iterable<block_t>& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n(buffer, blocks.size());
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const blocks_t& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n(buffer, blocks.size());
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
template <size_t Size>
constexpr typename CLASS::digest_t CLASS::
double_hash(const std_array<block_t, Size>& blocks) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;

    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }

    schedule_n<Size>(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const block_t& block) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};

    auto state = H::get;
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& half) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, half);
    pad_half(buffer);
    schedule(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

// [witness commitment, merkle root]
TEMPLATE
constexpr typename CLASS::digest_t CLASS::
double_hash(const half_t& left, const half_t& right) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);

    buffer_t buffer{};
    auto state = H::get;
    input1(buffer, left);
    input2(buffer, right);
    schedule(buffer);
    compress(state, buffer);
    schedule_1(buffer);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    state = H::get;
    compress(state, buffer);
    return output(state);
}

// Merkle Hashing (sha256/512).
// ------------------------------------------------------------------------
// No merkle_hash optimizations for sha160 (double_hash requires half_t).

TEMPLATE
VCONSTEXPR typename CLASS::digest_t CLASS::
merkle_root(digests_t&& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);
    const auto count = digests.size();

    if (is_zero(count))
        return {};

    if (is_one(count))
        return std::move(digests.front());

    if (is_odd(count))
        digests.push_back(digests.back());

    while (merkle_hash(digests).size() > one);
    return std::move(digests.front());
}

TEMPLATE
VCONSTEXPR typename CLASS::digests_t& CLASS::
merkle_hash(digests_t& digests) NOEXCEPT
{
    static_assert(is_same_type<state_t, chunk_t>);
    const auto half = to_half(digests.size());

    // This algorithm shifts results from a pair of digests into preceding
    // digests. This trades reduced allocation for lack of concurrency. But sha
    // concurrency tests as net performance loss due to synchronization cost.
    if (std::is_constant_evaluated())
    {
        for (size_t i = 0, j = 0; i < half; ++i, j += two)
            digests[i] = double_hash(digests[j], digests[add1(j)]);
    }
    else if constexpr (Vectorized)
    {
        // TODO: Factor the set of digests for optimal vectorization.
        for (size_t i = 0, j = 0; i < half; ++i, j += two)
            digests[i] = double_hash(digests[j], digests[add1(j)]);
    }
    else
    {
        for (size_t i = 0, j = 0; i < half; ++i, j += two)
            digests[i] = double_hash(digests[j], digests[add1(j)]);
    }

    digests.resize(half);
    return digests;
};

// Block.txs skip-parsed hash set.
////// Bitcoin hash set from an ordered set of ptrs [header commitment].
////INLINE hashes bitcoin_hash(std_vector<uint8_t*>&&) NOEXCEPT
////
////template <size_t Size>
////using set_t = std_vector<cref<std_array<block_t, Size>>>;
////using states_t = std_vector<state_t>;
////
////TEMPLATE
////template <size_t Size>
////VCONSTEXPR typename CLASS::states_t CLASS::
////accumulate(const set_t<Size>& sets) NOEXCEPT
////{
////    states_t states(sets.size());
////
////    // The set of sets is order independent across the sets (vectorizable).
////    std_transform(concurrency(), sets.begin(), sets.end(), states.begin(),
////        [&](const blocks_t& blocks)
////        {
////            buffer_t buffer{};
////            auto state = H::get;
////
////            // Each set is ordered (accumulated).
////            for (auto& block: blocks)
////            {
////                input(buffer, block);
////                schedule(buffer);
////                rounding(state, buffer);
////            }
////        });
////
////    return states;
////}

// Streaming (unfinalized).
// ---------------------------------------------------------------------------
// eprint.iacr.org/2012/067.pdf
// Message schedule vectorization across blocks.
// Message schedules/endianness are order independent.

#if defined(HAVE_MSC)

TEMPLATE
template <size_t Lanes,
    bool_if<Lanes == 16 || Lanes == 8 || Lanes == 4 || Lanes == 2>>
INLINE bool CLASS::
have() NOEXCEPT
{
    if constexpr (SHA::word_bytes == 8)
    {
        if constexpr (Lanes == 16)
            return false;
        if constexpr (Lanes == 8)
            return have_avx512();
        if constexpr (Lanes == 4)
            return have_avx2();
        if constexpr (Lanes == 2)
            return have_sse41();
    }
    else // if constexpr (SHA::word_bytes == 4)
    {
        if constexpr (Lanes == 16)
            return have_avx512();
        if constexpr (Lanes == 8)
            return have_avx2();
        if constexpr (Lanes == 4)
            return have_sse41();
        if constexpr (Lanes == 2)
            return have_sse41();
    }
}

TEMPLATE
template <size_t Lanes>
void CLASS::
input(bufferex_t<Lanes>& buffer, block_it& block) NOEXCEPT
{
    // Always 16 words per block.
    // bufferex_t is always round count and first 1/4 holds "block".
    // But is expanded by Lanes to hold Lanes blocks.
    // So always fill Lane blocks into one buffer here.
    // This is 16 wordex_t into buffer and (Lanes*4[8])*16 bytes from blocks.
    // Lanes is half with sha256 so counts line up.
    // word_t varies with SHA, wordex_t varies with SHA and Lanes.
    // block iterator must be incremented by lanes.
    // Array cast current block to array<wordex_t, 16> for iteration here.
    // Then advance block iterator by number of blocks consumed.
    // 16 * sizeof(wordex_t) / 64|128 -> sizeof(wordex_t) / 4|8.
    // sha256/160: 128 -> 4, 256 -> 8, 512 -> 16
    // sha512    : 128 -> 2, 256 -> 4, 512 ->  8
    // This is always the number of lanes: std::advance(block, Lanes);

    // Cast Lane blocks of bytes into an array of wordex_t chunks.
    using word = wordex_t<Lanes>;
    using chunk = data_array<sizeof(word)>;
    constexpr auto words = SHA::block_words;
    const auto& data = unsafe_array_cast<chunk, words>(&block->front());

    buffer[0]  = load_big_endian<word>(data[0]);
    buffer[1]  = load_big_endian<word>(data[1]);
    buffer[2]  = load_big_endian<word>(data[2]);
    buffer[3]  = load_big_endian<word>(data[3]);

    buffer[4]  = load_big_endian<word>(data[4]);
    buffer[5]  = load_big_endian<word>(data[5]);
    buffer[6]  = load_big_endian<word>(data[6]);
    buffer[7]  = load_big_endian<word>(data[7]);

    buffer[8]  = load_big_endian<word>(data[8]);
    buffer[9]  = load_big_endian<word>(data[9]);
    buffer[10] = load_big_endian<word>(data[10]);
    buffer[11] = load_big_endian<word>(data[11]);

    buffer[12] = load_big_endian<word>(data[12]);
    buffer[13] = load_big_endian<word>(data[13]);
    buffer[14] = load_big_endian<word>(data[14]);
    buffer[15] = load_big_endian<word>(data[15]);

    // TODO: this iterates over ++it, make random access iterator and can +=.
    // Advance block iterator by number of blocks consumed.
    std::advance(block, Lanes);
}

////TEMPLATE
////template <size_t Lanes>
////typename CLASS::digest_t CLASS::
////outputex(const statex_t<Lanes>& state) NOEXCEPT
////{
////    digest_t digest{};
////
////    // Always natively little endian because these are X86 optimizations.
////
////    auto& out = array_cast<wordex_t<Lanes>>(digest);
////
////    out[0] = native_to_big_end(state[0]);
////    out[1] = native_to_big_end(state[1]);
////    out[2] = native_to_big_end(state[2]);
////    out[3] = native_to_big_end(state[3]);
////    out[4] = native_to_big_end(state[4]);
////
////    if constexpr (SHA::strength != 160)
////    {
////        out[5] = native_to_big_end(state[5]);
////        out[6] = native_to_big_end(state[6]);
////        out[7] = native_to_big_end(state[7]);
////    }
////
////    return digest;
////}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 16>>
inline void CLASS::
vectorize(state_t& state, block_it& block, size_t& count) NOEXCEPT
{
    if (have<Lanes>())
    {
        for (; count >= Lanes; count -= Lanes)
        {
            bufferex_t<Lanes> buffers;
            input<Lanes>(buffers, block);
            schedule(buffers);

            // TODO: use the loop unroller to consolidate all vectorize().
            // array_cast requires integral types, could be overloaded here.
            const auto& buffer = unsafe_array_cast<buffer_t, Lanes>(&buffers.front());
            compress(state, buffer[0]);
            compress(state, buffer[1]);
            compress(state, buffer[2]);
            compress(state, buffer[3]);
            compress(state, buffer[4]);
            compress(state, buffer[5]);
            compress(state, buffer[6]);
            compress(state, buffer[7]);
            compress(state, buffer[8]);
            compress(state, buffer[9]);
            compress(state, buffer[10]);
            compress(state, buffer[11]);
            compress(state, buffer[12]);
            compress(state, buffer[13]);
            compress(state, buffer[14]);
            compress(state, buffer[15]);
        }
    }
}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 8>>
inline void CLASS::
vectorize(state_t& state, block_it& block, size_t& count) NOEXCEPT
{
    if (have<Lanes>())
    {
        for (; count >= Lanes; count -= Lanes)
        {
            bufferex_t<Lanes> buffers;
            input<Lanes>(buffers, block);
            schedule(buffers);

            // TODO: use the loop unroller to consolidate all vectorize().
            // array_cast requires integral types, could be overloaded here.
            const auto& buffer = unsafe_array_cast<buffer_t, Lanes>(&buffers.front());
            compress(state, buffer[0]);
            compress(state, buffer[1]);
            compress(state, buffer[2]);
            compress(state, buffer[3]);
            compress(state, buffer[4]);
            compress(state, buffer[5]);
            compress(state, buffer[6]);
            compress(state, buffer[7]);
        }
    }
}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 4>>
inline void CLASS::
vectorize(state_t& state, block_it& block, size_t& count) NOEXCEPT
{
    if (have<Lanes>())
    {
        for (; count >= Lanes; count -= Lanes)
        {
            bufferex_t<Lanes> buffers;
            input<Lanes>(buffers, block);
            schedule(buffers);

            // TODO: use the loop unroller to consolidate all vectorize().
            // array_cast requires integral types, could be overloaded here.
            const auto& buffer = unsafe_array_cast<buffer_t, Lanes>(&buffers.front());
            compress(state, buffer[0]);
            compress(state, buffer[1]);
            compress(state, buffer[2]);
            compress(state, buffer[3]);
        }
    }
}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 2>>
inline void CLASS::
vectorize(state_t& state, block_it& block, size_t& count) NOEXCEPT
{
    if (have<Lanes>())
    {
        for (; count >= Lanes; count -= Lanes)
        {
            bufferex_t<Lanes> buffers;
            input<Lanes>(buffers, block);
            schedule(buffers);

            // TODO: use the loop unroller to consolidate all vectorize().
            // array_cast requires integral types, could be overloaded here.
            const auto& buffer = unsafe_array_cast<buffer_t, Lanes>(&buffers.front());
            compress(state, buffer[0]);
            compress(state, buffer[1]);
        }
    }
}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 1>>
inline void CLASS::
vectorize(state_t& state, block_it& block, size_t& count) NOEXCEPT
{
    // Default processing with vectorization enabled by none available.
    for (; count >= Lanes; count -= Lanes)
    {
        buffer_t buffer;
        input(buffer, *block++);
        schedule(buffer);
        compress(state, buffer);
    }
}

TEMPLATE
template <size_t Lanes, if_equal<Lanes, 0>>
inline void CLASS::
vectorize(state_t&, block_it&, size_t&) NOEXCEPT
{
    // This will not be seen by any (nop-op).
}

#endif // HAVE_MSC

TEMPLATE
void CLASS::
accumulate(state_t& state, const iterable<block_t>& blocks) NOEXCEPT
{
    if constexpr (Compressed)
    {
        // TODO: sha intrinsics.
        buffer_t buffer{};
        for (auto& block: blocks)
        {
            input(buffer, block);
            schedule(buffer);
            compress(state, buffer);
        }
    }
#if defined(HAVE_MSC)
    else if constexpr (Vectorized)
    {
        // sha512 is twice the width of sha156/160.
        constexpr auto multiple = sizeof(word_t) / sizeof(uint32_t);

        auto count = blocks.size();
        auto block = blocks.begin();
        vectorize<16 / multiple>(state, block, count); //  16/8 AVX512/AVX512
        vectorize< 8 / multiple>(state, block, count); //   8/4 AVX2/AVX2
        vectorize< 4 / multiple>(state, block, count); //   4/2 SSE4/SSE4
        vectorize< 2 / multiple>(state, block, count); //   2/1 SSE4/Native
        vectorize< 1 / multiple>(state, block, count); //   1/0 Native/[n/a]
    }
#endif // HAVE_MSC
    else
    {
        buffer_t buffer{};
        for (auto& block: blocks)
        {
            input(buffer, block);
            schedule(buffer);
            compress(state, buffer);
        }
    }
}

TEMPLATE
VCONSTEXPR void CLASS::
accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT
{
    buffer_t buffer{};
    for (auto& block: blocks)
    {
        input(buffer, block);
        schedule(buffer);
        compress(state, buffer);
    }
}

TEMPLATE
constexpr void CLASS::
accumulate(state_t& state, const block_t& block) NOEXCEPT
{
    buffer_t buffer{};
    input(buffer, block);
    schedule(buffer);
    compress(state, buffer);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize(state_t& state, size_t blocks) NOEXCEPT
{
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);
    return output(state);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
finalize_double(state_t& state, size_t blocks) NOEXCEPT
{
    // The state out parameter is updated for first hash.
    buffer_t buffer{};
    schedule_n(buffer, blocks);
    compress(state, buffer);

    // Second hash
    input(buffer, state);
    pad_half(buffer);
    schedule(buffer);
    auto state2 = H::get;
    compress(state2, buffer);
    return output(state2);
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
normalize(const state_t& state) NOEXCEPT
{
    return output(state);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
