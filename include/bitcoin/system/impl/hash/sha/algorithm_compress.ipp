/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_COMPRESS_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_COMPRESS_IPP

#include <type_traits>

// Buffer compression.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Word, size_t Lane>
INLINE constexpr auto CLASS::
extract(Word a) NOEXCEPT
{
    // Bypass lane extraction for non-expanded (normal form) buffer.
    static_assert(Lane == zero);
    return a;
}

TEMPLATE
template<size_t Round, typename Auto>
CONSTEVAL auto CLASS::
functor() NOEXCEPT
{
    using self = CLASS;
    constexpr auto fn = Round / K::columns;
    static_assert(SHA::strength == 160);

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
    constexpr auto s = SHA::word_bits;
    constexpr auto fn = functor<Round, decltype(a)>();
    static_assert(SHA::strength == 160);

    e = /*a =*/ f::add<s>(f::add<s>(f::add<s>(f::rol<5, s>(a), fn(b, c, d)), e), wk);
    b = /*c =*/ f::rol<30, s>(b);
}

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
round(auto a, auto b, auto c, auto& d, auto e, auto f, auto g, auto& h,
    auto wk) NOEXCEPT
{
    // TODO: This can be doubled and Sigmas vectorized (Intel).
    constexpr auto s = SHA::word_bits;
    static_assert(SHA::strength != 160);

    const auto t = f::add<s>(f::add<s>(f::add<s>(Sigma1(e), choice(e, f, g)), h), wk);
    d = /*e =*/    f::add<s>(d, t);
    h = /*a =*/    f::add<s>(f::add<s>(Sigma0(a), majority(a, b, c)), t);
}

TEMPLATE
template<size_t Round, size_t Lane>
INLINE constexpr void CLASS::
round(auto& state, const auto& wk) NOEXCEPT
{
    using word = array_element<decltype(state)>;

    if constexpr (SHA::strength == 160)
    {
        round<Round>(
            state[(SHA::rounds + 0 - Round) % SHA::state_words],
            state[(SHA::rounds + 1 - Round) % SHA::state_words], // c->b
            state[(SHA::rounds + 2 - Round) % SHA::state_words],
            state[(SHA::rounds + 3 - Round) % SHA::state_words],
            state[(SHA::rounds + 4 - Round) % SHA::state_words], // a->e
            extract<word, Lane>(wk[Round]));
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
            extract<word, Lane>(wk[Round]));
    }
}

TEMPLATE
INLINE constexpr void CLASS::
summarize(auto& out, const auto& in) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;
    out[0] = f::add<s>(out[0], in[0]);
    out[1] = f::add<s>(out[1], in[1]);
    out[2] = f::add<s>(out[2], in[2]);
    out[3] = f::add<s>(out[3], in[3]);
    out[4] = f::add<s>(out[4], in[4]);

    if constexpr (SHA::strength != 160)
    {
        out[5] = f::add<s>(out[5], in[5]);
        out[6] = f::add<s>(out[6], in[6]);
        out[7] = f::add<s>(out[7], in[7]);
    }
}

TEMPLATE
template <size_t Lane>
constexpr void CLASS::
compress_(auto& state, const auto& buffer) NOEXCEPT
{
    // This is a copy (state type varies due to vectorization).
    const auto start = state;

    round< 0, Lane>(state, buffer);
    round< 1, Lane>(state, buffer);
    round< 2, Lane>(state, buffer);
    round< 3, Lane>(state, buffer);
    round< 4, Lane>(state, buffer);
    round< 5, Lane>(state, buffer);
    round< 6, Lane>(state, buffer);
    round< 7, Lane>(state, buffer);
    round< 8, Lane>(state, buffer);
    round< 9, Lane>(state, buffer);
    round<10, Lane>(state, buffer);
    round<11, Lane>(state, buffer);
    round<12, Lane>(state, buffer);
    round<13, Lane>(state, buffer);
    round<14, Lane>(state, buffer);
    round<15, Lane>(state, buffer);

    round<16, Lane>(state, buffer);
    round<17, Lane>(state, buffer);
    round<18, Lane>(state, buffer);
    round<19, Lane>(state, buffer);
    round<20, Lane>(state, buffer);
    round<21, Lane>(state, buffer);
    round<22, Lane>(state, buffer);
    round<23, Lane>(state, buffer);
    round<24, Lane>(state, buffer);
    round<25, Lane>(state, buffer);
    round<26, Lane>(state, buffer);
    round<27, Lane>(state, buffer);
    round<28, Lane>(state, buffer);
    round<29, Lane>(state, buffer);
    round<30, Lane>(state, buffer);
    round<31, Lane>(state, buffer);

    round<32, Lane>(state, buffer);
    round<33, Lane>(state, buffer);
    round<34, Lane>(state, buffer);
    round<35, Lane>(state, buffer);
    round<36, Lane>(state, buffer);
    round<37, Lane>(state, buffer);
    round<38, Lane>(state, buffer);
    round<39, Lane>(state, buffer);
    round<40, Lane>(state, buffer);
    round<41, Lane>(state, buffer);
    round<42, Lane>(state, buffer);
    round<43, Lane>(state, buffer);
    round<44, Lane>(state, buffer);
    round<45, Lane>(state, buffer);
    round<46, Lane>(state, buffer);
    round<47, Lane>(state, buffer);

    round<48, Lane>(state, buffer);
    round<49, Lane>(state, buffer);
    round<50, Lane>(state, buffer);
    round<51, Lane>(state, buffer);
    round<52, Lane>(state, buffer);
    round<53, Lane>(state, buffer);
    round<54, Lane>(state, buffer);
    round<55, Lane>(state, buffer);
    round<56, Lane>(state, buffer);
    round<57, Lane>(state, buffer);
    round<58, Lane>(state, buffer);
    round<59, Lane>(state, buffer);
    round<60, Lane>(state, buffer);
    round<61, Lane>(state, buffer);
    round<62, Lane>(state, buffer);
    round<63, Lane>(state, buffer);

    if constexpr (SHA::rounds == 80)
    {
        round<64, Lane>(state, buffer);
        round<65, Lane>(state, buffer);
        round<66, Lane>(state, buffer);
        round<67, Lane>(state, buffer);
        round<68, Lane>(state, buffer);
        round<69, Lane>(state, buffer);
        round<70, Lane>(state, buffer);
        round<71, Lane>(state, buffer);
        round<72, Lane>(state, buffer);
        round<73, Lane>(state, buffer);
        round<74, Lane>(state, buffer);
        round<75, Lane>(state, buffer);
        round<76, Lane>(state, buffer);
        round<77, Lane>(state, buffer);
        round<78, Lane>(state, buffer);
        round<79, Lane>(state, buffer);
    }

    summarize(state, start);
}

TEMPLATE
template <size_t Lane>
constexpr void CLASS::
compress(state_t& state, const buffer_t& buffer) NOEXCEPT
{
    // block-internal vectorization is suboptimal.
    compress_<Lane>(state, buffer);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
