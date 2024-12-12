/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_KONSTANT_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_KONSTANT_IPP

#include <type_traits>

// [K]onstant adding.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// single or expanded (vectorized) buffer
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <size_t Round>
INLINE constexpr void CLASS::
konstant(auto& buffer) NOEXCEPT
{
    // K is broadcast across blocks.
    buffer[Round] = f::addc<K::get[Round], SHA::word_bits>(buffer[Round]);
}

// wide (vectorized) buffer
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template<size_t Round, typename xWord>
INLINE void CLASS::
vector_konstant(wbuffer_t<xWord>& wbuffer) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, word_t>;
    constexpr auto r = Round * lanes;

    if constexpr (lanes == 16)
    {
        wbuffer[Round] = add<word_t>(wbuffer[Round], set<xWord>(
            K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3],
            K::get[r + 4], K::get[r + 5], K::get[r + 6], K::get[r + 7],
            K::get[r + 8], K::get[r + 9], K::get[r + 10], K::get[r + 11],
            K::get[r + 12], K::get[r + 13], K::get[r + 14], K::get[r + 15]));
    }
    else if constexpr (lanes == 8)
    {
        wbuffer[Round] = add<word_t>(wbuffer[Round], set<xWord>(
            K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3],
            K::get[r + 4], K::get[r + 5], K::get[r + 6], K::get[r + 7]));
    }
    else if constexpr (lanes == 4)
    {
        wbuffer[Round] = add<word_t>(wbuffer[Round], set<xWord>(
            K::get[r + 0], K::get[r + 1], K::get[r + 2], K::get[r + 3]));
    }
}

TEMPLATE
void CLASS::
vector_konstant(buffer_t& buffer) NOEXCEPT
{
    if constexpr (have_lanes<word_t, 16>)
    {
        auto& wbuffer = array_cast<to_extended<word_t, 16>>(buffer);
        vector_konstant<0>(wbuffer);
        vector_konstant<1>(wbuffer);
        vector_konstant<2>(wbuffer);
        vector_konstant<3>(wbuffer);

        if constexpr (SHA::rounds == 80)
        {
            vector_konstant<4>(wbuffer);
        }
    }
    else if constexpr (have_lanes<word_t, 8>)
    {
        auto& wbuffer = array_cast<to_extended<word_t, 8>>(buffer);
        vector_konstant<0>(wbuffer);
        vector_konstant<1>(wbuffer);
        vector_konstant<2>(wbuffer);
        vector_konstant<3>(wbuffer);
        vector_konstant<4>(wbuffer);
        vector_konstant<5>(wbuffer);
        vector_konstant<6>(wbuffer);
        vector_konstant<7>(wbuffer);

        if constexpr (SHA::rounds == 80)
        {
            vector_konstant<8>(wbuffer);
            vector_konstant<9>(wbuffer);
        }
    }
    else if constexpr (have_lanes<word_t, 4>)
    {
        auto& wbuffer = array_cast<to_extended<word_t, 4>>(buffer);
        vector_konstant<0>(wbuffer);
        vector_konstant<1>(wbuffer);
        vector_konstant<2>(wbuffer);
        vector_konstant<3>(wbuffer);
        vector_konstant<4>(wbuffer);
        vector_konstant<5>(wbuffer);
        vector_konstant<6>(wbuffer);
        vector_konstant<7>(wbuffer);
        vector_konstant<8>(wbuffer);
        vector_konstant<9>(wbuffer);
        vector_konstant<10>(wbuffer);
        vector_konstant<11>(wbuffer);
        vector_konstant<12>(wbuffer);
        vector_konstant<13>(wbuffer);
        vector_konstant<14>(wbuffer);
        vector_konstant<15>(wbuffer);

        if constexpr (SHA::rounds == 80)
        {
            vector_konstant<16>(wbuffer);
            vector_konstant<17>(wbuffer);
            vector_konstant<18>(wbuffer);
            vector_konstant<19>(wbuffer);
        }
    }
    else
    {
        konstant_(buffer);
    }
}

// dispatch
// ----------------------------------------------------------------------------
// protected

TEMPLATE
constexpr void CLASS::
konstant_(auto& buffer) NOEXCEPT
{
    konstant<0>(buffer);
    konstant<1>(buffer);
    konstant<2>(buffer);
    konstant<3>(buffer);
    konstant<4>(buffer);
    konstant<5>(buffer);
    konstant<6>(buffer);
    konstant<7>(buffer);
    konstant<8>(buffer);
    konstant<9>(buffer);
    konstant<10>(buffer);
    konstant<11>(buffer);
    konstant<12>(buffer);
    konstant<13>(buffer);
    konstant<14>(buffer);
    konstant<15>(buffer);

    konstant<16>(buffer);
    konstant<17>(buffer);
    konstant<18>(buffer);
    konstant<19>(buffer);
    konstant<20>(buffer);
    konstant<21>(buffer);
    konstant<22>(buffer);
    konstant<23>(buffer);
    konstant<24>(buffer);
    konstant<25>(buffer);
    konstant<26>(buffer);
    konstant<27>(buffer);
    konstant<28>(buffer);
    konstant<29>(buffer);
    konstant<30>(buffer);
    konstant<31>(buffer);

    konstant<32>(buffer);
    konstant<33>(buffer);
    konstant<34>(buffer);
    konstant<35>(buffer);
    konstant<36>(buffer);
    konstant<37>(buffer);
    konstant<38>(buffer);
    konstant<39>(buffer);
    konstant<40>(buffer);
    konstant<41>(buffer);
    konstant<42>(buffer);
    konstant<43>(buffer);
    konstant<44>(buffer);
    konstant<45>(buffer);
    konstant<46>(buffer);
    konstant<47>(buffer);

    konstant<48>(buffer);
    konstant<49>(buffer);
    konstant<50>(buffer);
    konstant<51>(buffer);
    konstant<52>(buffer);
    konstant<53>(buffer);
    konstant<54>(buffer);
    konstant<55>(buffer);
    konstant<56>(buffer);
    konstant<57>(buffer);
    konstant<58>(buffer);
    konstant<59>(buffer);
    konstant<60>(buffer);
    konstant<61>(buffer);
    konstant<62>(buffer);
    konstant<63>(buffer);

    if constexpr (SHA::rounds == 80)
    {
        konstant<64>(buffer);
        konstant<65>(buffer);
        konstant<66>(buffer);
        konstant<67>(buffer);
        konstant<68>(buffer);
        konstant<69>(buffer);
        konstant<70>(buffer);
        konstant<71>(buffer);
        konstant<72>(buffer);
        konstant<73>(buffer);
        konstant<74>(buffer);
        konstant<75>(buffer);
        konstant<76>(buffer);
        konstant<77>(buffer);
        konstant<78>(buffer);
        konstant<79>(buffer);
    }
}

TEMPLATE
template <typename xWord>
INLINE constexpr void CLASS::
konstant(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    konstant_(xbuffer);
}

TEMPLATE
INLINE constexpr void CLASS::
konstant(buffer_t& buffer) NOEXCEPT
{
    // This optimization is neutral in 4/8/16 lane sha256 perf.
    ////if (std::is_constant_evaluated())
    ////{
    ////    konstant_(buffer);
    ////}
    ////else if constexpr (vector && !with_clang)
    ////{
    ////    vector_konstant(buffer);
    ////}
    ////else
    {
        konstant_(buffer);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
