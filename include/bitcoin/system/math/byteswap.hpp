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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTESWAP_HPP
#define LIBBITCOIN_SYSTEM_MATH_BYTESWAP_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Byteswap (platform independent byte reversal).
/// ---------------------------------------------------------------------------

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint8_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint16_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint32_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true,
    if_size_of<Integral, sizeof(uint64_t)> = true,
    if_unique_object_representations<Integral> = true>
constexpr Integral byteswap(Integral value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/byteswap.ipp>

#endif
