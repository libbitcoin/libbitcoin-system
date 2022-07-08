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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_NOMINAL_IPP

#include <algorithm>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// byte convertors
// ----------------------------------------------------------------------------

// C++ standard: "The [shift] behavior is undefined if ... greater than or
// equal to the width of the promoted left operand." So we must be careful to
// not shift bytes by byte_bits (8). For this reason we specialize on byte-size
// integers. This also presents a performance optimization for byte conversion,
// which is close to a no-op. Empty vector returns zero and zero returns empty.

template <typename Integer, if_one_byte<Integer> = true>
VCONSTEXPR Integer from_big_chunk(size_t, const data_chunk& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Integer, size_t Size, if_one_byte<Integer> = true>
constexpr Integer from_big_chunk(size_t, const data_array<Size>& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Integer, if_one_byte<Integer> = true>
VCONSTEXPR Integer from_little_chunk(size_t, const data_chunk& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Integer, size_t Size, if_one_byte<Integer> = true>
constexpr Integer from_little_chunk(size_t, const data_array<Size>& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Data, typename Integer, if_one_byte<Integer> = true>
constexpr Data to_big_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    if (!bytes.empty()) { bytes.front() = static_cast<uint8_t>(value); }
    return std::move(bytes);
}

template <typename Data, typename Integer, if_one_byte<Integer> = true>
constexpr Data to_little_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    if (!bytes.empty()) { bytes.front() = static_cast<uint8_t>(value); }
    return std::move(bytes);
}

// integer convertors
// ----------------------------------------------------------------------------

// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will produce "1" bits, indefinitely.

// for constexpr uintx_t.
template <typename Integer, size_t Size, if_non_integral_integer<Integer> = true>
constexpr Integer from_big_array(const data_array<Size>& data) NOEXCEPT
{
    Integer value{ 0 };

    for (size_t byte = 0; byte < Size; ++byte)
    {
        value <<= byte_bits;
        value |= static_cast<Integer>(data[byte]);
    }

    return value;
}

// for constexpr uintx_t.
template <typename Integer, size_t Size, if_non_integral_integer<Integer> = true>
constexpr Integer from_little_array(const data_array<Size>& data) NOEXCEPT
{
    Integer value{ 0 };

    for (auto byte = Size; byte > 0; --byte)
    {
        value <<= byte_bits;
        value |= static_cast<Integer>(data[sub1(byte)]);
    }

    return value;
}

template <typename Integer, if_not_one_byte<Integer> = true>
VCONSTEXPR Integer from_big_chunk(size_t size, const data_chunk& data) NOEXCEPT
{
    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= static_cast<Integer>(data[byte]);
        BC_POP_WARNING()
    }

    return value;
}

template <typename Integer, size_t Size, if_not_one_byte<Integer> = true>
constexpr Integer from_big_chunk(size_t size, const data_array<Size>& data) NOEXCEPT
{
    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= static_cast<Integer>(data[byte]);
        BC_POP_WARNING()
    }

    return value;
}

template <typename Integer, if_not_one_byte<Integer> = true>
VCONSTEXPR Integer from_little_chunk(size_t size, const data_chunk& data) NOEXCEPT
{
    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (auto byte = bytes; byte > 0; --byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= static_cast<Integer>(data[sub1(byte)]);
        BC_POP_WARNING()
    }

    return value;
}

template <typename Integer, size_t Size, if_not_one_byte<Integer> = true>
constexpr Integer from_little_chunk(size_t size, const data_array<Size>& data) NOEXCEPT
{
    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (auto byte = bytes; byte > 0; --byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= static_cast<Integer>(data[sub1(byte)]);
        BC_POP_WARNING()
    }

    return value;
}

template <typename Data, typename Integer, if_not_one_byte<Integer> = true>
constexpr Data to_big_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    for (auto& byte: views_reverse(bytes))
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return std::move(bytes);
}

template <typename Data, typename Integer, if_not_one_byte<Integer> = true>
constexpr Data to_little_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    for (auto& byte: bytes)
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return std::move(bytes);
}

// data => integral
// integral => byte_array
// ----------------------------------------------------------------------------

template <size_t Size>
constexpr unsigned_type<Size> from_big_endian(const data_array<Size>& data) NOEXCEPT
{
    return from_big_chunk<unsigned_type<Size>>(Size, data);
}

template <size_t Size>
constexpr unsigned_type<Size> from_little_endian(const data_array<Size>& data) NOEXCEPT
{
    return from_little_chunk<unsigned_type<Size>>(Size, data);
}

// integral   from_big|little_endian(data_slice)
// data_array   to_big|little_endian(integral)
// ----------------------------------------------------------------------------

template <typename Integral, if_integral_integer<Integral>>
VCONSTEXPR Integral from_big_endian(const data_chunk& data) NOEXCEPT
{
    // Variably sized non-array format, stay with native/constexpr.
    return from_big_chunk<Integral>(sizeof(Integral), data);
}

template <typename Integral, if_integral_integer<Integral>>
VCONSTEXPR Integral from_little_endian(const data_chunk& data) NOEXCEPT
{
    // Variably sized non-array format, stay with native/constexpr.
    return from_little_chunk<Integral>(sizeof(Integral), data);
}

template <typename Integral, size_t Size, if_integral_integer<Integral>>
constexpr Integral from_big_endian(const data_array<Size>& data) NOEXCEPT
{

    if (std::is_constant_evaluated())
    {
        return from_big_chunk<Integral>(sizeof(Integral), data);
    }
    else
    {
        return from_big_chunk<Integral>(sizeof(Integral), data);
        ////return native_from_big_end(byte_cast(data));
    }
}

template <typename Integral, size_t Size, if_integral_integer<Integral>>
constexpr Integral from_little_endian(const data_array<Size>& data) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return from_little_chunk<Integral>(sizeof(Integral), data);
    }
    else
    {
        return from_little_chunk<Integral>(sizeof(Integral), data);
        ////return native_from_little_end(byte_cast(data));
    }
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_big_endian(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return to_big_chunk(data_array<sizeof(Integral)>{}, value);
    }
    else
    {
        // inline
        return byte_cast(native_to_big_end(value));
    }
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_little_endian(Integral value) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        return to_little_chunk(data_array<sizeof(Integral)>{}, value);
    }
    else
    {
        // inline
        return byte_cast(native_to_little_end(value));
    }
}

} // namespace system
} // namespace libbitcoin

#endif
