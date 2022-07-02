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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// data[] -> integral[] (explicit size)
// integral[] -> data[] (explicit size)

// This is just a collection-based byteswap.
// void from_big|little_endian(data[], integral[])
// void   to_big|little_endian(data[], integral[])

// TODO: change to std::array& args and move to /math/bytes/.
// TODO: change parameterization for in-place update.

template <size_t Count, typename Integral,
    if_integral_integer<Integral> = true>
constexpr void from_big_endian(Integral to[Count],
    const uint8_t from[Count * sizeof(Integral)]) NOEXCEPT;

template <size_t Count, typename Integral,
    if_integral_integer<Integral> = true>
constexpr void from_little_endian(Integral to[Count],
    const uint8_t from[Count * sizeof(Integral)]) NOEXCEPT;

template <size_t Count, typename Integral,
    if_integral_integer<Integral> = true>
constexpr void to_big_endian(uint8_t to[Count * sizeof(Integral)],
    const Integral from[Count]) NOEXCEPT;

template <size_t Count, typename Integral,
    if_integral_integer<Integral> = true>
constexpr void to_little_endian(uint8_t to[Count * sizeof(Integral)],
    const Integral from[Count]) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/collection.ipp>

#endif
