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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_INTEGERS_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_INTEGERS_IPP

#include <algorithm>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// C++ standard: "The [shift] behavior is undefined if ... greater than or
// equal to the width of the promoted left operand." So we must be careful to
// not shift bytes by byte_bits (8). For this reason we specialize on byte-size
// integers. This also presents a performance optimization for byte conversion,
// which is close to a no-op. Empty vector returns zero and zero returns empty.

// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will perpetually produce "1" bits.

// To big/little data.
// ----------------------------------------------------------------------------
// Fills Data to its preallocated size.

template <typename Data, typename Integer, if_integer<Integer>>
constexpr Data to_big_data(Data&& data, Integer value) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        if (!data.empty())
            data.front() = possible_sign_cast<uint8_t>(value);

        return std::move(data);
    }
    else
    {
        // Avoid pointer aliasing (byte& loop) using const termination.
        for (auto byte = data.size(); byte > 0; --byte)
        {
            data[sub1(byte)] = possible_narrow_and_sign_cast<uint8_t>(value);
            value >>= byte_bits;
        }

        return std::move(data);
    }
}

template <typename Data, typename Integer, if_integer<Integer>>
constexpr Data to_little_data(Data&& data, Integer value) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        if (!data.empty())
            data.front() = possible_sign_cast<uint8_t>(value);

        return std::move(data);
    }
    else
    {
        // Avoid pointer aliasing (byte& loop) using const termination.
        const auto size = data.size();

        for (auto byte = 0; byte < size; ++byte)
        {
            data[byte] = possible_narrow_and_sign_cast<uint8_t>(value);
            value >>= byte_bits;
        }

        return std::move(data);
    }
}

// From big data.
// ----------------------------------------------------------------------------
// Shifts all data bytes into Integer.

template <typename Integer, size_t Size, if_integer<Integer>>
constexpr Integer from_big_array(const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };

        for (size_t byte = 0; byte < Size; ++byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
        }

        return value;
    }
}

template <typename Integer, size_t Size, if_integer<Integer>>
constexpr Integer from_big_array(size_t length,
    const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };
        const auto bytes = std::min(length, data.size());

        for (size_t byte = 0; byte < bytes; ++byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
        }

        return value;
    }
}

template <typename Integer, if_integer<Integer>>
VCONSTEXPR Integer from_big_chunk(size_t length,
    const data_chunk& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };
        const auto bytes = std::min(length, data.size());

        for (size_t byte = 0; byte < bytes; ++byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[byte]);
        }

        return value;
    }
}

// From little data.
// ----------------------------------------------------------------------------
// Shifts all data bytes into Integer.

template <typename Integer, size_t Size, if_integer<Integer>>
constexpr Integer from_little_array(const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };

        for (auto byte = Size; byte > 0; --byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
        }

        return value;
    }
}

template <typename Integer, size_t Size, if_integer<Integer>>
constexpr Integer from_little_array(size_t length,
    const data_array<Size>& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };
        const auto bytes = std::min(length, data.size());

        for (auto byte = bytes; byte > 0; --byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
        }

        return value;
    }
}

template <typename Integer, if_integer<Integer>>
VCONSTEXPR Integer from_little_chunk(size_t length,
    const data_chunk& data) NOEXCEPT
{
    if constexpr (is_one(sizeof(Integer)))
    {
        return data.empty() ? 0 : data.front();
    }
    else
    {
        Integer value{ 0 };
        const auto bytes = std::min(length, data.size());

        for (auto byte = bytes; byte > 0; --byte)
        {
            value <<= byte_bits;
            value |= possible_narrow_and_sign_cast<Integer>(data[sub1(byte)]);
        }

        return value;
    }
}

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()
BC_POP_WARNING()

#endif
