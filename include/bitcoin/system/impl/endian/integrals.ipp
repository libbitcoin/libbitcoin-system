/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_INTEGRALS_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_INTEGRALS_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// These are euqally-efficient generalizations of the common practice of
// shifting with or-ing to construct integers from bytes, and shifting with
// masking to obtain bytes from integers. The byte<offset> template performs
// integer byte extraction (zero offset is low order byte). These avoid byte
// casting in order to support constexpr, though byte_cast is more efficient.

namespace libbitcoin {
namespace system {

// to_big/to_little (by reference, implicit, offsettable).
// ----------------------------------------------------------------------------
// TODO: if_lesser<safe_add(Offset, sub1(sizeof(Integral))), Size>>

template <size_t Offset, typename Byte, size_t Size, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr void to_big(std_array<Byte, Size>& data, Integral value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    if constexpr (sizeof(Integral) == sizeof(uint64_t))
    {
        data[0 + Offset] = byte<7>(value);
        data[1 + Offset] = byte<6>(value);
        data[2 + Offset] = byte<5>(value);
        data[3 + Offset] = byte<4>(value);
        data[4 + Offset] = byte<3>(value);
        data[5 + Offset] = byte<2>(value);
        data[6 + Offset] = byte<1>(value);
        data[7 + Offset] = byte<0>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint32_t))
    {
        data[0 + Offset] = byte<3>(value);
        data[1 + Offset] = byte<2>(value);
        data[2 + Offset] = byte<1>(value);
        data[3 + Offset] = byte<0>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint16_t))
    {
        data[0 + Offset] = byte<1>(value);
        data[1 + Offset] = byte<0>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint8_t))
    {
        data[0 + Offset] = byte<0>(value);
    }
    BC_POP_WARNING()
}

template <size_t Offset, typename Byte, size_t Size, typename Integral,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr void to_little(std_array<Byte, Size>& data, Integral value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    if constexpr (sizeof(Integral) == sizeof(uint64_t))
    {
        data[0 + Offset] = byte<0>(value);
        data[1 + Offset] = byte<1>(value);
        data[2 + Offset] = byte<2>(value);
        data[3 + Offset] = byte<3>(value);
        data[4 + Offset] = byte<4>(value);
        data[5 + Offset] = byte<5>(value);
        data[6 + Offset] = byte<6>(value);
        data[7 + Offset] = byte<7>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint32_t))
    {
        data[0 + Offset] = byte<0>(value);
        data[1 + Offset] = byte<1>(value);
        data[2 + Offset] = byte<2>(value);
        data[3 + Offset] = byte<3>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint16_t))
    {
        data[0 + Offset] = byte<0>(value);
        data[1 + Offset] = byte<1>(value);
    }

    if constexpr (sizeof(Integral) == sizeof(uint8_t))
    {
        data[0 + Offset] = byte<0>(value);
    }
    BC_POP_WARNING()
}

//  from_big/from_little (by reference, implicit, offsettable).
// ----------------------------------------------------------------------------

template <size_t Offset, typename Integral, typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr void from_big(Integral& value,
    const std_array<Byte, Size>& data) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    if constexpr (sizeof(Integral) == sizeof(uint64_t))
    {
        value =
            (wide_cast<uint64_t>(data[0 + Offset]) << 56) |
            (wide_cast<uint64_t>(data[1 + Offset]) << 48) |
            (wide_cast<uint64_t>(data[2 + Offset]) << 40) |
            (wide_cast<uint64_t>(data[3 + Offset]) << 32) |
            (wide_cast<uint64_t>(data[4 + Offset]) << 24) |
            (wide_cast<uint64_t>(data[5 + Offset]) << 16) |
            (wide_cast<uint64_t>(data[6 + Offset]) <<  8) |
            (wide_cast<uint64_t>(data[7 + Offset]) <<  0);
    }

    if constexpr (sizeof(Integral) == sizeof(uint32_t))
    {
        value =
            (wide_cast<uint32_t>(data[0 + Offset]) << 24) |
            (wide_cast<uint32_t>(data[1 + Offset]) << 16) |
            (wide_cast<uint32_t>(data[2 + Offset]) <<  8) |
            (wide_cast<uint32_t>(data[3 + Offset]) <<  0);
    }

    if constexpr (sizeof(Integral) == sizeof(uint16_t))
    {
        value = depromote<uint16_t>(
            (wide_cast<uint16_t>(data[0 + Offset]) << 8) |
            (wide_cast<uint16_t>(data[1 + Offset]) << 0));
    }

    if constexpr (sizeof(Integral) == sizeof(uint8_t))
    {
        value = data[0 + Offset];
    }
    BC_POP_WARNING()
}

template <size_t Offset, typename Integral, typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr void from_little(Integral& value,
    const std_array<Byte, Size>& data) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    if constexpr (sizeof(Integral) == sizeof(uint64_t))
    {
        value =
            (wide_cast<uint64_t>(data[0 + Offset]) <<  0) |
            (wide_cast<uint64_t>(data[1 + Offset]) <<  8) |
            (wide_cast<uint64_t>(data[2 + Offset]) << 16) |
            (wide_cast<uint64_t>(data[3 + Offset]) << 24) |
            (wide_cast<uint64_t>(data[4 + Offset]) << 32) |
            (wide_cast<uint64_t>(data[5 + Offset]) << 40) |
            (wide_cast<uint64_t>(data[6 + Offset]) << 48) |
            (wide_cast<uint64_t>(data[7 + Offset]) << 56);
    }

    if constexpr (sizeof(Integral) == sizeof(uint32_t))
    {
        value =
            (wide_cast<uint32_t>(data[0 + Offset]) <<  0) |
            (wide_cast<uint32_t>(data[1 + Offset]) <<  8) |
            (wide_cast<uint32_t>(data[2 + Offset]) << 16) |
            (wide_cast<uint32_t>(data[3 + Offset]) << 24);
    }

    if constexpr (sizeof(Integral) == sizeof(uint16_t))
    {
        value = depromote<uint16_t>(
            (wide_cast<uint16_t>(data[0 + Offset]) << 0) |
            (wide_cast<uint16_t>(data[1 + Offset]) << 8));
    }

    if constexpr (sizeof(Integral) == sizeof(uint8_t))
    {
        value = data[0 + Offset];
    }
    BC_POP_WARNING()
}

// from_big/from_little (by value, explicit, offsettable).
// ----------------------------------------------------------------------------

template<typename Integral, size_t Offset, typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr Integral from_big(const std_array<Byte, Size>& data) NOEXCEPT
{
    Integral out{};
    from_big<Offset>(out, data);
    return out;
}

template<typename Integral, size_t Offset, typename Byte, size_t Size,
    if_one_byte<Byte>,
    if_integral_integer<Integral>>
constexpr Integral from_little(const std_array<Byte, Size>& data) NOEXCEPT
{
    Integral out{};
    from_little<Offset>(out, data);
    return out;
}

// to_big/to_little (by value, implicit).
// ----------------------------------------------------------------------------

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_big(Integral value) NOEXCEPT
{
    data_array<sizeof(Integral)> out{};
    to_big<zero>(out, value);
    return out;
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_little(Integral value) NOEXCEPT
{
    data_array<sizeof(Integral)> out{};
    to_little<zero>(out, value);
    return out;
}

// from_big/from_little (by value, implicit).
// ----------------------------------------------------------------------------
// place1 avoids ambiguity with explicit byte argument for other overload.

template<place1, typename Byte, size_t Size,
    if_one_byte<Byte >>
constexpr unsigned_type<Size> from_big(
    const std_array<Byte, Size>& data) NOEXCEPT
{
    unsigned_type<Size> out{};
    from_big<zero>(out, data);
    return out;
}

template<place1, typename Byte, size_t Size,
    if_one_byte<Byte>>
constexpr unsigned_type<Size> from_little(
    const std_array<Byte, Size>& data) NOEXCEPT
{
    unsigned_type<Size> out{};
    from_little<zero>(out, data);
    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
