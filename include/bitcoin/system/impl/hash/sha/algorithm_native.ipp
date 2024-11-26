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
prepare_native(wbuffer_t<xint128_t>& wbuffer) NOEXCEPT
{
    if constexpr (SHA::strength == 160)
    {
        ////static_assert(false, "sha160 not implemented");
    }
    else if constexpr (use_neon)
    {
        static_assert(SHA::strength == 256);

        ////static_assert(false, "neon not implemented");
    }
    else if constexpr (use_shani)
    {
        static_assert(SHA::strength == 256);

        wbuffer[Round] = mm_sha256msg2_epu32
            (
                mm_add_epi32
                (
                    mm_alignr_epi8
                    (
                        wbuffer[Round - 1], wbuffer[Round - 2], SHA::word_bytes
                    ),
                    mm_sha256msg1_epu32
                    (
                        wbuffer[Round - 4], wbuffer[Round - 3]
                    )
                ),
                wbuffer[Round - 1]
            );
    }
}

TEMPLATE
INLINE void CLASS::
schedule(wbuffer_t<xint128_t>& wbuffer) NOEXCEPT
{
    prepare_native<4>(wbuffer);
    prepare_native<5>(wbuffer);
    prepare_native<6>(wbuffer);
    prepare_native<7>(wbuffer);
    prepare_native<8>(wbuffer);
    prepare_native<9>(wbuffer);
    prepare_native<10>(wbuffer);
    prepare_native<11>(wbuffer);
    prepare_native<12>(wbuffer);
    prepare_native<13>(wbuffer);
    prepare_native<14>(wbuffer);
    prepare_native<15>(wbuffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare_native<16>(wbuffer);
        prepare_native<17>(wbuffer);
        prepare_native<18>(wbuffer);
        prepare_native<19>(wbuffer);
    }

    konstant(array_cast<word_t>(wbuffer));
}

// schedule
// ----------------------------------------------------------------------------
// protected

TEMPLATE
INLINE void CLASS::
schedule_native(buffer_t& buffer) NOEXCEPT
{
    // neon and sha160 not yet implemented, sha512 is not native.
    if constexpr (SHA::strength != 160 && SHA::strength != 512 && !use_neon)
    {
        schedule(array_cast<xint128_t>(buffer));
    }
    else
    {
        schedule_(buffer);
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
