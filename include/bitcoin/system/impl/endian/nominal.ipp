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

// TODO: split into integral and non-integral, use bytecasting for integrals.

template <typename Integer,
    if_one_byte<Integer> = true>
inline Integer from_big_chunk(size_t, const data_slice& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Integer,
    if_one_byte<Integer> = true>
inline Integer from_little_chunk(size_t, const data_slice& data) NOEXCEPT
{
    return data.empty() ? 0 : data.front();
}

template <typename Data, typename Integer,
    if_one_byte<Integer> = true>
constexpr Data to_big(Data&& bytes, Integer value) NOEXCEPT
{
    if (!bytes.empty())
        bytes.front() = static_cast<uint8_t>(value);

    return std::move(bytes);
}

template <typename Data, typename Integer,
    if_one_byte<Integer> = true>
constexpr Data to_little(Data&& bytes, Integer value) NOEXCEPT
{
    if (!bytes.empty())
        bytes.front() = static_cast<uint8_t>(value);

    return std::move(bytes);
}

// integer convertors
// ----------------------------------------------------------------------------

// C++ standard: "Right-shift on signed integral types is an arithmetic right
// shift, which performs sign-extension". In other words, repeatedly shifting
// -1 of any integer width will produce "1" bits, indefinitely.

// for constexpr uintx_t.
template <typename Integer, size_t Size,
    if_non_integral_integer<Integer> = true>
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
template <typename Integer, size_t Size,
    if_non_integral_integer<Integer> = true>
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

template <typename Integer,
    if_not_one_byte<Integer> = true>
inline Integer from_big_chunk(size_t size, const data_slice& data) NOEXCEPT
{
    // read msb (forward), shift in the byte (no shift on first)
    // data[0] is most significant
    // { 0x01, 0x02 } => 0x0102
    // 0x0000 << 8 => 0x0000
    // 0x0001 << 8 => 0x0100
    // 0x0000 |= 0x01[0] => 0x0001
    // 0x0100 |= 0x02[1] => 0x0102

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

template <typename Integer,
    if_not_one_byte<Integer> = true>
inline Integer from_little_chunk(size_t size, const data_slice& data) NOEXCEPT
{
    // read msb (reverse), shift in the byte (no shift on first)
    // data[0] is least significant
    // { 0x01, 0x02 } => 0x0201
    // 0x0000 << 8 => 0x0000
    // 0x0002 << 8 => 0x0200
    // 0x0000 |= 0x02[1] => 0x0002
    // 0x0200 |= 0x01[0] => 0x0201

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

template <typename Data, typename Integer,
    if_not_one_byte<Integer> = true>
constexpr Data to_big(Data&& bytes, Integer value) NOEXCEPT
{
    // read and shift out lsb, set byte in reverse order
    // data[0] is most significant
    // 0x0102 => { 0x01, 0x02 }
    // (uint8_t)0x0102 => 0x02[1]
    // (uint8_t)0x0001 => 0x01[0]
    // 0x0102 >> 8 => 0x0001
    // 0x0001 >> 8 => 0x0000

    for (auto& byte: views_reverse(bytes))
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return std::move(bytes);
}

template <typename Data, typename Integer,
    if_not_one_byte<Integer> = true>
constexpr Data to_little(Data&& bytes, Integer value) NOEXCEPT
{
    // read and shift out lsb, set byte in forward order
    // data[0] is least significant
    // 0x0102 => { 0x02, 0x01 }
    // (uint8_t)0x0102 => 0x02[0]
    // (uint8_t)0x0001 => 0x01[1]
    // 0x0102 >> 8 => 0x0001
    // 0x0001 >> 8 => 0x0000

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

// TODO: reduce locals to four implementations, for all integer.
// TODO: templatize for any Data, allowing public wrappers to constrain.
// TODO: all can be constexpr and called from constexpr or otherwise.
// TODO: expect single byte shift to be precluded by integral bytecasting.

template <size_t Size>
constexpr unsigned_type<Size> from_big_endian(
    const data_array<Size>& data) NOEXCEPT
{
    return from_big_chunk<unsigned_type<Size>>(Size, data);
}

template <size_t Size>
constexpr unsigned_type<Size> from_little_endian(
    const data_array<Size>& data) NOEXCEPT
{
    return from_little_chunk<unsigned_type<Size>>(Size, data);
}

// integral   from_big|little_endian(data_slice)
// data_array   to_big|little_endian(integral)

template <typename Integral,
    if_integral_integer<Integral>>
constexpr Integral from_big_endian(const data_slice& data) NOEXCEPT
{
    return from_big_chunk<Integral>(sizeof(Integral), data);
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr Integral from_little_endian(const data_slice& data) NOEXCEPT
{
    return from_little_chunk<Integral>(sizeof(Integral), data);
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_big_endian(Integral value) NOEXCEPT
{
    return to_big(data_array<sizeof(Integral)>{}, value);
}

template <typename Integral,
    if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)> to_little_endian(Integral value) NOEXCEPT
{
    return to_little(data_array<sizeof(Integral)>{}, value);
}

} // namespace system
} // namespace libbitcoin

#endif
