/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

/// Native does not change the buffer size (not expanded), just its "shape".
/// Four words are buffered into one xint128_t, resulting in 1/4 the buffer
/// array size and number of rounds. Four state words are  packed into each of
/// two state variables. This applies to sha160 and sha256, but sha512 native
/// is not supported.

namespace libbitcoin {
namespace system {
namespace sha {

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_native(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended buffer is not native dispatched.
    schedule_(xbuffer);
}

TEMPLATE
INLINE void CLASS::
schedule_native(buffer_t&) NOEXCEPT
{
    ////if constexpr (SHA::strength != 160 && have_lanes<word_t, 8>())
    ////{
    ////    prepare8<16>(buffer);
    ////    prepare8<24>(buffer);
    ////    prepare8<32>(buffer);
    ////    prepare8<40>(buffer);
    ////    prepare8<48>(buffer);
    ////    prepare8<56>(buffer);

    ////    if constexpr (SHA::rounds == 80)
    ////    {
    ////        prepare8<64>(buffer);
    ////        prepare8<72>(buffer);
    ////    }

    ////    add_k(buffer);
    ////}
    ////else
    ////{
    ////    schedule_(buffer);
    ////}
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
