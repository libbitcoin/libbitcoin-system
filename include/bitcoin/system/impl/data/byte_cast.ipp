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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP

#include <array>
#include <bitcoin/system/data/array_cast.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

template <typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr std::array<Byte, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT
{
    // Safe because passing sizeof(value).
    return unsafe_array_cast<Byte, sizeof(Integral)>(&value);
}

template <typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr const std::array<Byte, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT
{
    // Safe because passing sizeof(value).
    return unsafe_array_cast<Byte, sizeof(Integral)>(&value);
}

template <typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_size<Size>>
constexpr unsigned_type<Size>&
byte_cast(std::array<Byte, Size>& value) NOEXCEPT
{
    return *pointer_cast<unsigned_type<Size>>(&value);
}

template <typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_size<Size>>
constexpr const unsigned_type<Size>&
byte_cast(const std::array<Byte, Size>& value) NOEXCEPT
{
    return *pointer_cast<const unsigned_type<Size>>(&value);
}

} // namespace system
} // namespace libbitcoin

#endif
