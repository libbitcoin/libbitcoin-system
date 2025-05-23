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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP

#include <utility>
#include <bitcoin/system/data/array_cast.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
    
// byte_cast (integral to std_array<Byte>)
// ----------------------------------------------------------------------------

template <typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline std_array<Byte, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT
{
    // Safe because passing sizeof(value).
    return unsafe_array_cast<Byte, sizeof(Integral)>(&value);
}

template <typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline const std_array<Byte, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT
{
    // Safe because passing sizeof(value).
    return unsafe_array_cast<Byte, sizeof(Integral)>(&value);
}

// Avoids cast of rvalue to reference, which would dangle.
template <typename Byte, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline std_array<Byte, sizeof(Integral)>
byte_cast(Integral&& value) NOEXCEPT
{
    return byte_cast<Byte>(unmove(value));
}

// byte_cast (std_array<Byte> to integral)
// ----------------------------------------------------------------------------

template <typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_size<Size>>
inline unsigned_type<Size>&
byte_cast(std_array<Byte, Size>& bytes) NOEXCEPT
{
    return *pointer_cast<unsigned_type<Size>>(&bytes);
}

template <typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_size<Size>>
inline const unsigned_type<Size>&
byte_cast(const std_array<Byte, Size>& bytes) NOEXCEPT
{
    return *pointer_cast<const unsigned_type<Size>>(&bytes);
}

// Avoids cast of rvalue to reference, which would dangle.
template <typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_size<Size>>
inline unsigned_type<Size>
byte_cast(std_array<Byte, Size>&& bytes) NOEXCEPT
{
    return byte_cast(unmove(bytes));
}

// unsafe_byte_cast (byte* to integral)
// ----------------------------------------------------------------------------

template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline Integral& unsafe_byte_cast(Byte* bytes) NOEXCEPT
{
    return *pointer_cast<Integral>(bytes);
}

template <typename Integral, typename Byte,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
inline const Integral& unsafe_byte_cast(const Byte* bytes) NOEXCEPT
{
    return *pointer_cast<const Integral>(bytes);
}

} // namespace system
} // namespace libbitcoin

#endif
