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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NONE_SHA_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NONE_SHA_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>

#if !defined(HAVE_SHA)

namespace libbitcoin {
namespace system {
namespace sha {

INLINE void schedule(xint128_t&, xint128_t) NOEXCEPT
{
}

INLINE void schedule(xint128_t&, xint128_t, xint128_t) NOEXCEPT
{
}

INLINE void compress(xint128_t&, xint128_t&, xint128_t) NOEXCEPT
{
}

INLINE void shuffle(xint128_t&, xint128_t&) NOEXCEPT
{
}

INLINE void unshuffle(xint128_t&, xint128_t&) NOEXCEPT
{
}

INLINE void schedule_160(xint128_t&, xint128_t, xint128_t) NOEXCEPT
{
}

INLINE void schedule_160(xint128_t&, xint128_t) NOEXCEPT
{
}

template <size_t Round>
INLINE void compress_160(xint128_t&, xint128_t&, xint128_t) NOEXCEPT
{
}

INLINE xint128_t next_160(xint128_t, xint128_t e) NOEXCEPT
{
    return e;
}

INLINE xint128_t order_160(xint128_t message) NOEXCEPT
{
    return message;
}

INLINE void shuffle_160(xint128_t&) NOEXCEPT
{
}

INLINE void unshuffle_160(xint128_t&) NOEXCEPT
{
}

INLINE xint128_t set_160(uint32_t) NOEXCEPT
{
    return {};
}

INLINE uint32_t get_160(xint128_t) NOEXCEPT
{
    return {};
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif // HAVE_SHA

#endif
