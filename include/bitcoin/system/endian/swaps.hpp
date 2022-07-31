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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_SWAPS_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_SWAPS_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Native endianness integer to/from big/little endianness integer (byteswap).
/// ---------------------------------------------------------------------------
/// The "from" functions are merely convenience aliases for the "to" functions.

/// Convert a native integral integer to big-endian.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_to_big_end(Integral big) NOEXCEPT;

/// Convert a native integral integer to little-endian.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_to_little_end(Integral big) NOEXCEPT;

/// Convert a big-endian integral integer to native.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_from_big_end(Integral big) NOEXCEPT;

/// Convert a little-endian integral integer to native.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr Integral native_from_little_end(Integral little) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/swaps.ipp>

#endif
