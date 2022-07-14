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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UNCHECKED_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UNCHECKED_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// iterator (unknown size) -> integral (implicit size)
/// Must be guarded by the caller, use when passing end would be redundant.

// Integral from_big|little_endian_unchecked(Iterator)
// Integral   to_big|little_endian_unchecked(Iterator)

template <typename Integral, typename Iterator,
    if_integral_integer<Integral> = true>
inline Integral from_big_endian_unchecked(const Iterator& data) NOEXCEPT;

template <typename Integral, typename Iterator,
    if_integral_integer<Integral> = true>
inline Integral from_little_endian_unchecked(const Iterator& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/unchecked.ipp>

#endif
