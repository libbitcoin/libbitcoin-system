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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_ALGORITHM_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_ALGORITHM_IPP

#include <algorithm>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// C++ standard: "The [shift] behavior is undefined if ... greater than or
// equal to the width of the promoted left operand." So we must be careful to
// not shift bytes by byte_bits (8). For this reason we specialize on byte-size
// integers. This also presents a performance optimization for byte conversion,
// which is close to a no-op. Empty vector returns zero and zero returns empty.

// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will produce "1" bits, indefinitely.

// To big/little data.
// ----------------------------------------------------------------------------
// Fills Data to its preallocated size.

// to_big_data()
template <typename Data, typename Integer>
constexpr Data to_big_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        if (!bytes.empty())
            bytes.front() = possible_sign_cast<uint8_t>(value);

        return std::move(bytes);
    }

    for (auto& byte: views_reverse(bytes))
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return std::move(bytes);
}

// to_little_data()
template <typename Data, typename Integer>
constexpr Data to_little_chunk(Data&& bytes, Integer value) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        if (!bytes.empty())
            bytes.front() = possible_sign_cast<uint8_t>(value);

        return std::move(bytes);
    }

    for (auto& byte: bytes)
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return std::move(bytes);
}

// From big data.
// ----------------------------------------------------------------------------
// Shifts all data bytes into Integer.

// Integer from_big_array(data_array)
template <typename Integer, size_t Size>
constexpr Integer from_big_array(const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value{ 0 };

    for (size_t byte = 0; byte < Size; ++byte)
    {
        value <<= byte_bits;
        value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
    }

    return value;
}

// Integer from_big_array<length>(data_array)
template <typename Integer, size_t Size>
constexpr Integer from_big_chunk(size_t size,
    const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
        BC_POP_WARNING()
    }

    return value;
}

// Integer from_big_chunk<length>(data_chunk)
template <typename Integer>
VCONSTEXPR Integer from_big_chunk(size_t size,
    const data_chunk& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
        BC_POP_WARNING()
    }

    return value;
}

// From little data.
// ----------------------------------------------------------------------------
// Shifts all data bytes into Integer.

// Integer from_little_array(data_array)
template <typename Integer, size_t Size>
constexpr Integer from_little_array(const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value{ 0 };

    for (auto byte = Size; byte > 0; --byte)
    {
        value <<= byte_bits;
        value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
    }

    return value;
}

// Integer from_little_array<length>(Integer)
template <typename Integer, size_t Size>
constexpr Integer from_little_chunk(size_t size,
    const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (auto byte = bytes; byte > 0; --byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
        BC_POP_WARNING()
    }

    return value;
}

// Integer from_little_chunk<length>(Integer)
template <typename Integer>
VCONSTEXPR Integer from_little_chunk(size_t size,
    const data_chunk& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
        return data.empty() ? 0 : data.front();

    Integer value(0);
    const auto bytes = std::min(size, data.size());

    for (auto byte = bytes; byte > 0; --byte)
    {
        value <<= byte_bits;

        BC_PUSH_WARNING(USE_GSL_AT)
        value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
        BC_POP_WARNING()
    }

    return value;
}

// To/from integral.
// ----------------------------------------------------------------------------
// Very efficient byteswap, or passthru.

template <typename Integral, if_integral_integer<Integral>>
constexpr Integral native_to_big_end(Integral big) NOEXCEPT
{
    if constexpr (is_little_endian)
        return byteswap(big);
    else
        return big;
}

template <typename Integral, if_integral_integer<Integral>>
constexpr Integral native_to_little_end(Integral little) NOEXCEPT
{
    if constexpr (is_big_endian)
        return byteswap(little);
    else
        return little;
}

template <typename Integral, if_integral_integer<Integral>>
constexpr Integral native_from_big_end(Integral big) NOEXCEPT
{
    return native_to_big_end(big);
}

template <typename Integral, if_integral_integer<Integral>>
constexpr Integral native_from_little_end(Integral little) NOEXCEPT
{
    return native_to_little_end(little);
}

} // namespace system
} // namespace libbitcoin

#endif
