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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Implicitly typed/sized return.
/// ---------------------------------------------------------------------------

template <size_t Size>
constexpr unsigned_type<Size>
from_big_endian(const data_array<Size>& data) NOEXCEPT;

template <size_t Size>
constexpr unsigned_type<Size>
from_little_endian(const data_array<Size>& data) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)>
to_big_endian(Integral value) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)>
to_little_endian(Integral value) NOEXCEPT;

/// Variably-sized input returns specified integral (padded or truncated).
/// ---------------------------------------------------------------------------

template <typename Integral, size_t Size,
    if_integral_integer<Integral> = true>
constexpr Integral from_big_endian(const data_array<Size>& data) NOEXCEPT;

template <typename Integral, size_t Size,
    if_integral_integer<Integral> = true>
constexpr Integral from_little_endian(const data_array<Size>& data) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr Integral from_big_endian(const data_chunk& data) NOEXCEPT;

template <typename Integral,
    if_integral_integer<Integral> = true>
constexpr Integral from_little_endian(const data_chunk& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/nominal.ipp>

#endif
