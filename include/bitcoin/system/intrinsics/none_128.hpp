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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NONE_128_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NONE_128_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>

namespace libbitcoin {
namespace system {

#if !defined(HAVE_128)

template <typename Word, if_same<Word, uint8_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint16_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint32_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

template <typename Word, if_same<Word, uint64_t> = true>
INLINE xint128_t byteswap(xint128_t) NOEXCEPT
{
    return {};
}

#endif // HAVE_128

} // namespace system
} // namespace libbitcoin

#endif
