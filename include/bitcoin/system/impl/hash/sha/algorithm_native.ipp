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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_NATIVE_IPP

// Native (SHA-NI or NEON)
// ============================================================================
// Native does not change the buffer size (not expanded), just its "shape".
// Four words are buffered into one xint128_t, resulting in 1/4 the buffer
// array size and number of rounds. Four state words are  packed into each of
// two state variables. This applies to sha160 and sha256, but sha512 native
// is not supported.

// The base buffer is already populated with proper endianness.
// Input could be optimized using intrinsics (see comments in parse).
// The unextended state vector is already output with proper endianness.
// Output could also be optimized using intrinsics (see comments in parse).

namespace libbitcoin {
namespace system {
namespace sha {

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare(cbuffer_t& buffer) NOEXCEPT
{
    // K-adding is shifted 16 words, with last 16 added after scheduling.

    if constexpr (SHA::strength == 160)
    {
        ////static_assert(false, "sha160 not implemented");
    }
    else
    {
        static_assert(SHA::strength == 256);

        constexpr auto r1 = Round - 1;
        constexpr auto r2 = Round - 2;
        constexpr auto r3 = Round - 3;
        constexpr auto r4 = Round - 4;
        constexpr auto k0 = Round * 4 - 16;
        constexpr auto k1 = Round * 4 - 15;
        constexpr auto k2 = Round * 4 - 14;
        constexpr auto k3 = Round * 4 - 13;

        buffer[Round] = mm_sha256msg2_epu32
            (
                mm_add_epi32
                (
                    mm_alignr_epi8
                    (
                        buffer[r1], buffer[r2], SHA::word_bytes
                    ),
                    mm_sha256msg1_epu32
                    (
                        buffer[r4], buffer[r3]
                    )
                ),
                buffer[r1]
            );

        buffer[r4] = mm_add_epi32
            (
                buffer[r4],
                mm_set_epi32(K::get[k3], K::get[k2], K::get[k1], K::get[k0])
            );
    }
}

TEMPLATE
INLINE void CLASS::
add_k(cbuffer_t& buffer) NOEXCEPT
{
    // Add K to last 16 words.
    // TODO: Consolidated K-adding can be performed in 4/8/16 lanes.
    constexpr auto k = SHA::rounds - SHA::block_words;
    constexpr auto r = k / native_lanes;

    buffer[r + 0] = mm_add_epi32
        (
            buffer[r + 0],
            mm_set_epi32(
                K::get[k + 3], K::get[k + 2],
                K::get[k + 1], K::get[k + 0])
        );

    buffer[r + 1] = mm_add_epi32
        (
            buffer[r + 1],
            mm_set_epi32(
                K::get[k + 7], K::get[k + 6],
                K::get[k + 5], K::get[k + 4])
        );

    buffer[r + 2] = mm_add_epi32
        (
            buffer[r + 2],
            mm_set_epi32(
                K::get[k + 11], K::get[k + 10],
                K::get[k + 9], K::get[k + 8])
        );

    buffer[r + 3] = mm_add_epi32
        (
            buffer[r + 3],
            mm_set_epi32(
                K::get[k + 15], K::get[k + 14],
                K::get[k + 13], K::get[k + 12])
        );
}

TEMPLATE
INLINE void CLASS::
schedule(cbuffer_t& buffer) NOEXCEPT
{
    auto& cbuffer = array_cast<xint128_t>(buffer);

    prepare<4>(cbuffer);
    prepare<5>(cbuffer);
    prepare<6>(cbuffer);
    prepare<7>(cbuffer);
    prepare<8>(cbuffer);
    prepare<9>(cbuffer);
    prepare<10>(cbuffer);
    prepare<11>(cbuffer);
    prepare<12>(cbuffer);
    prepare<13>(cbuffer);
    prepare<14>(cbuffer);
    prepare<15>(cbuffer);

    ////if constexpr (SHA::rounds == 80)
    ////{
    ////    prepare<16>(buffer);
    ////    prepare<17>(buffer);
    ////    prepare<18>(buffer);
    ////    prepare<19>(buffer);
    ////}

    add_k(buffer);
}

// schedule
// ----------------------------------------------------------------------------
// protected

TEMPLATE
INLINE void CLASS::
schedule_native(buffer_t& buffer) NOEXCEPT
{
    // neon and sha160 not yet implemented, sha512 is not native.
    if constexpr (SHA::strength == 160 || SHA::strength == 512 || use_neon)
    {
        schedule_(buffer);
    }
    else
    {
        schedule(array_cast<xint128_t>(buffer));
    }
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_native(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended buffer is not native dispatched.
    schedule_(xbuffer);
}

// compression
// ----------------------------------------------------------------------------
// protected

TEMPLATE
template <typename xWord, size_t Lane>
INLINE void CLASS::
compress_native(xstate_t<xWord>& xstate,
    const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended state/buffer is not native dispatched.
    compress_<Lane>(xstate, xbuffer);
}

TEMPLATE
template <typename xWord, size_t Lane>
INLINE void CLASS::
compress_native(state_t& state, const xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Iterate extended buffer is not native dispatched.
    compress_<Lane>(state, xbuffer);
}

TEMPLATE
template <size_t Lane>
INLINE void CLASS::
compress_native(state_t& state, const buffer_t& buffer) NOEXCEPT
{
    // TODO: Single block compression.
    compress_<Lane>(state, buffer);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
