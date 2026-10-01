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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_COUNT_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_COUNT_HPP

#include <bit>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

#if defined(HAVE_PTX)

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr int count_left_zeros(Unsigned value) NOEXCEPT
{
    return __builtin_clzg(value, static_cast<int>(bits<Unsigned>));
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr int count_right_zeros(Unsigned value) NOEXCEPT
{
    return __builtin_ctzg(value, static_cast<int>(bits<Unsigned>));
}

#else

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr int count_left_zeros(Unsigned value) NOEXCEPT
{
    return std::countl_zero<Unsigned>(value);
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr int count_right_zeros(Unsigned value) NOEXCEPT
{
    return std::countr_zero<Unsigned>(value);
}

#endif

} // namespace system
} // namespace libbitcoin

#endif
