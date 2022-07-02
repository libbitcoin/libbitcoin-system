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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UNCHECKED_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UNCHECKED_IPP

#include <iterator>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/nominal.hpp>

namespace libbitcoin {
namespace system {

// data_slice is not constexpr, and data is unbounded here.

template <typename Integral, typename Iterator,
    if_integral_integer<Integral>>
inline Integral from_big_endian_unchecked(const Iterator& data) NOEXCEPT
{
    // data_slice construction/nominal, change to bytecast.
    return from_big_endian<Integral>({ data, std::next(data, sizeof(Integral)) });
}

template <typename Integral, typename Iterator,
    if_integral_integer<Integral>>
inline Integral from_little_endian_unchecked(const Iterator& data) NOEXCEPT
{
    // data_slice construction/nominal, change to bytecast.
    return from_little_endian<Integral>({ data, std::next(data, sizeof(Integral)) });
}

} // namespace system
} // namespace libbitcoin

#endif
