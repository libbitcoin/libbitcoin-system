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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_SWAPS_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_SWAPS_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Most efficient endianness functions - byteswap (or no-op).
// ----------------------------------------------------------------------------

template <typename Integral, if_integral_integer<Integral>>
INLINE constexpr Integral native_to_big_end(Integral big) NOEXCEPT
{
    if constexpr (is_little_endian)
        return byteswap(big);
    else
        return big;
}

template <typename Integral, if_integral_integer<Integral>>
INLINE constexpr Integral native_to_little_end(Integral little) NOEXCEPT
{
    if constexpr (is_big_endian)
        return byteswap(little);
    else
        return little;
}

template <typename Integral, if_integral_integer<Integral>>
INLINE constexpr Integral native_from_big_end(Integral big) NOEXCEPT
{
    return native_to_big_end(big);
}

template <typename Integral, if_integral_integer<Integral>>
INLINE constexpr Integral native_from_little_end(Integral little) NOEXCEPT
{
    return native_to_little_end(little);
}

} // namespace system
} // namespace libbitcoin

#endif
