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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/integers.hpp>
#include <bitcoin/system/endian/integrals.hpp>
#include <bitcoin/system/endian/swaps.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Implicitly typed/sized return (nominal conversions).
// ----------------------------------------------------------------------------

template <size_t Size>
constexpr unsigned_type<Size>
from_big_endian(const data_array<Size>& data) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        // integral, zero offset.
        return from_big<unsigned_type<Size>>(data);
    }
    else
    {
        // cast/swap.
        return native_from_big_end(byte_cast(data));
    }
}

template <size_t Size>
constexpr unsigned_type<Size>
from_little_endian(const data_array<Size>& data) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        // integral, zero offset.
        return from_little<unsigned_type<Size>>(data);
    }
    else
    {
        // cast/swap.
        return native_from_little_end(byte_cast(data));
    }
}

template <typename Integral, if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)>
to_big_endian(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        // integral, zero offset.
        return to_big(value);
    }
    else
    {
        // swap/cast.
        return byte_cast(native_to_big_end(value));
    }
}

template <typename Integral, if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)>
to_little_endian(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        // integral, zero offset.
        return to_little(value);
    }
    else
    {
        // swap/cast.
        return byte_cast(native_to_little_end(value));
    }
}

// Variably-sized input returns specified integral (padded or truncated).
// ----------------------------------------------------------------------------
// Any-sized data input, requires integer (shift loop) conversion.

template <typename Integral, size_t Size,
    if_integral_integer<Integral>>
constexpr Integral from_big_endian(const data_array<Size>& data) NOEXCEPT
{
    return from_big_array<Integral>(sizeof(Integral), data);
}

template <typename Integral, size_t Size,
    if_integral_integer<Integral>>
constexpr Integral from_little_endian(const data_array<Size>& data) NOEXCEPT
{
    return from_little_array<Integral>(sizeof(Integral), data);
}

template <typename Integral,
    if_integral_integer<Integral>>
VCONSTEXPR Integral from_big_endian(const data_chunk& data) NOEXCEPT
{
    return from_big_chunk<Integral>(sizeof(Integral), data);
}

template <typename Integral,
    if_integral_integer<Integral>>
VCONSTEXPR Integral from_little_endian(const data_chunk& data) NOEXCEPT
{
    return from_little_chunk<Integral>(sizeof(Integral), data);
}

} // namespace system
} // namespace libbitcoin

#endif
