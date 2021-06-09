/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_IPP
#define LIBBITCOIN_SYSTEM_SERIALIZATION_ENDIAN_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/integer.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// local helpers
// ----------------------------------------------------------------------------

template <typename Integer>
Integer from_big_endian_private(const data_slice& data)
{
    // read msb (forward), shift in the byte (no shift on first)
    // data[0] is most significant
    // { 0x01, 0x02 } => 0x0102
    // 0x0000 << 8 => 0x0000
    // 0x0001 << 8 => 0x0100
    // 0x0000 |= 0x01[0] => 0x0001
    // 0x0100 |= 0x02[1] => 0x0102

    // Guard against insufficient data.
    const auto size = std::min(sizeof(Integer), data.size());

    Integer out = 0;
    for (size_t index = 0; index < size; ++index)
    {
        out <<= byte_bits;
        out |= Integer{ data[index] };
    }

    return out;
}

template <typename Integer>
Integer from_little_endian_private(const data_slice& data)
{
    // read msb (reverse), shift in the byte (no shift on first)
    // data[0] is least significant
    // { 0x01, 0x02 } => 0x0201
    // 0x0000 << 8 => 0x0000
    // 0x0002 << 8 => 0x0200
    // 0x0000 |= 0x02[1] => 0x0002
    // 0x0200 |= 0x01[0] => 0x0201

    // Guard against insufficient data.
    const auto size = std::min(sizeof(Integer), data.size());

    Integer out = 0;
    for (auto index = size; index > 0; --index)
    {
        out <<= byte_bits;
        out |= Integer{ data[index - 1] };
    }

    return out;
}

template <size_t Size, typename Integer>
byte_array<Size> to_big_endian_private(Integer value)
{
    // read and shift out lsb, set byte in reverse order
    // data[0] is most significant
    // 0x0102 => { 0x01, 0x02 }
    // (uint8_t)0x0102 => 0x02[1]
    // (uint8_t)0x0001 => 0x01[0]
    // 0x0102 >> 8 => 0x0001
    // 0x0001 >> 8 => 0x0000

    byte_array<Size> out;
    for (auto& byte: boost::adaptors::reverse(out))
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return out;
}

template <size_t Size, typename Integer>
byte_array<Size> to_little_endian_private(Integer value)
{
    // read and shift out lsb, set byte in forward order
    // data[0] is least significant
    // 0x0102 => { 0x02, 0x01 }
    // (uint8_t)0x0102 => 0x02[0]
    // (uint8_t)0x0001 => 0x01[1]
    // 0x0102 >> 8 => 0x0001
    // 0x0001 >> 8 => 0x0000

    byte_array<Size> out;
    for (auto& byte: out)
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return out;
}

// data <=> integer
// ----------------------------------------------------------------------------

template <typename Integer, if_integer<Integer>>
Integer from_big_endian(const data_slice& data)
{
    return from_big_endian_private<Integer>(data);
}

template <typename Integer, if_integer<Integer>>
Integer from_little_endian(const data_slice& data)
{
    return from_little_endian_private<Integer>(data);
}

template <typename Integer, if_integer<Integer>>
byte_array<sizeof(Integer)> to_big_endian(Integer value)
{
    return to_big_endian_private<sizeof(Integer)>(value);
}

template <typename Integer, if_integer<Integer>>
byte_array<sizeof(Integer)> to_little_endian(Integer value)
{
    return to_little_endian_private<sizeof(Integer)>(value);
}

// stream <=> integer
// ----------------------------------------------------------------------------

template <typename Integer, if_integer<Integer>>
Integer from_big_endian(std::istream& stream)
{
    std::vector<char> buffer(sizeof(Integer));
    stream.read(buffer.data(), buffer.size());
    buffer.resize(stream.gcount());
    return from_big_endian<Integer>(buffer);
}

template <typename Integer, if_integer<Integer>>
Integer from_little_endian(std::istream& stream)
{
    std::vector<char> buffer(sizeof(Integer));
    stream.read(buffer.data(), buffer.size());
    buffer.resize(stream.gcount());
    return from_little_endian<Integer>(buffer);
}

template <typename Integer, if_integer<Integer>>
void to_big_endian(std::ostream& stream, Integer value)
{
    const auto buffer = reinterpret_cast<char*>(to_big_endian(value).data());
    stream.write(buffer, sizeof(Integer));
}

template <typename Integer, if_integer<Integer>>
void to_little_endian(std::ostream& stream, Integer value)
{
    const auto buffer = reinterpret_cast<char*>(to_little_endian(value).data());
    stream.write(buffer, sizeof(Integer));
}

// byte_array <=> uintx_t
// ----------------------------------------------------------------------------

template <size_t Bits, if_byte_aligned<Bits>>
uintx_t<Bits> from_big_endian(const data_slice& data)
{
    return from_big_endian_private<uintx_t<Bits>>(data);
}

template <size_t Bits, if_byte_aligned<Bits>>
uintx_t<Bits> from_little_endian(const data_slice& data)
{
    return from_little_endian_private<uintx_t<Bits>>(data);
}

template <size_t Bits, typename Integer, if_byte_aligned<Bits>>
byte_array<to_bytes(Bits)> to_big_endian(const Integer& value)
{
    return to_big_endian_private<to_bytes(Bits)>(value);
}

template <size_t Bits, typename Integer, if_byte_aligned<Bits>>
byte_array<to_bytes(Bits)> to_little_endian(const Integer& value)
{
    return to_little_endian_private<to_bytes(Bits)>(value);
}

// iterator => integer
// ----------------------------------------------------------------------------
// size is unguarded.

template <typename Integer, typename Iterator, if_integer<Integer>>
Integer from_big_endian_unsafe(const Iterator& data)
{
    const auto slice = data_slice(data, std::next(data, sizeof(Integer)));
    return from_big_endian<Integer>(slice);
}

template <typename Integer, typename Iterator, if_integer<Integer>>
Integer from_little_endian_unsafe(const Iterator& data)
{
    const auto slice = data_slice(data, std::next(data, sizeof(Integer)));
    return from_little_endian<Integer>(slice);
}

} // namespace system
} // namespace libbitcoin

#endif
