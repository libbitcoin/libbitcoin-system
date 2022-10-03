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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UNSAFE_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UNSAFE_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/swaps.hpp>

namespace libbitcoin {
namespace system {
    
// Unguarded endian read from byte pointer.
// ----------------------------------------------------------------------------
// byte_cast is not constexpr.

template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline Integral unsafe_from_big_endian(const Byte* data) NOEXCEPT
{
    return native_from_big_end(unsafe_byte_cast<Integral>(data));
}

template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline Integral unsafe_from_little_endian(const Byte* data) NOEXCEPT
{
    return native_from_little_end(unsafe_byte_cast<Integral>(data));
}


template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline void unsafe_to_big_endian(Byte* data, Integral value) NOEXCEPT
{
    unsafe_byte_cast<Integral>(data) = native_to_big_end(value);
}

template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline void unsafe_to_little_endian(Byte* data, Integral value) NOEXCEPT
{
    unsafe_byte_cast<Integral>(data) = native_to_little_end(value);
}

} // namespace system
} // namespace libbitcoin

#endif
