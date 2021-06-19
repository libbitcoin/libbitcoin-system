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
#include <utility>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/collection.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/uintx.hpp>
#include <bitcoin/system/math/divide.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// These conversions are efficient, performing no data copies or reversals.
// array/uintx_t sizes are inferred by type, and vector/uintx by value.
// High order bits are padded when read (from) is insufficient.
// High order bits are ignored when write (to) is insufficient.

// convertors
// ----------------------------------------------------------------------------
// These do the byte-integer translations.

template <typename Integer>
static Integer from_big_endian_private(size_t size,
    const data_slice& data) noexcept
{
    // read msb (forward), shift in the byte (no shift on first)
    // data[0] is most significant
    // { 0x01, 0x02 } => 0x0102
    // 0x0000 << 8 => 0x0000
    // 0x0001 << 8 => 0x0100
    // 0x0000 |= 0x01[0] => 0x0001
    // 0x0100 |= 0x02[1] => 0x0102

    Integer value = 0;
    const auto bytes = std::min(size, data.size());

    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= byte_bits;
        value |= Integer{ data[byte] };
    }

    return value;
}

template <typename Integer>
static Integer from_little_endian_private(size_t size,
    const data_slice& data) noexcept
{
    // read msb (reverse), shift in the byte (no shift on first)
    // data[0] is least significant
    // { 0x01, 0x02 } => 0x0201
    // 0x0000 << 8 => 0x0000
    // 0x0002 << 8 => 0x0200
    // 0x0000 |= 0x02[1] => 0x0002
    // 0x0200 |= 0x01[0] => 0x0201

    Integer value = 0;
    const auto bytes = std::min(size, data.size());

    for (auto byte = bytes; byte > 0; --byte)
    {
        value <<= byte_bits;
        value |= Integer{ data[sub1(byte)] };
    }

    return value;
}

template <typename Data, typename Integer>
static Data to_big_endian_private(Data&& bytes,
    Integer value) noexcept
{
    // read and shift out lsb, set byte in reverse order
    // data[0] is most significant
    // 0x0102 => { 0x01, 0x02 }
    // (uint8_t)0x0102 => 0x02[1]
    // (uint8_t)0x0001 => 0x01[0]
    // 0x0102 >> 8 => 0x0001
    // 0x0001 >> 8 => 0x0000

    for (auto& byte: boost::adaptors::reverse(bytes))
    {
        byte = static_cast<uint8_t>(value);
        value >>= byte_bits;
    }

    return bytes;
}

template <typename Data, typename Integer>
static Data to_little_endian_private(Data&& bytes,
    Integer value) noexcept
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

    return bytes;
}

// allocators
// ----------------------------------------------------------------------------
// These allocate the to-endian outgoing buffer and forward the call.

template <size_t Size, typename Integer>
static byte_array<Size> to_big_endian_private(Integer value) noexcept
{
    byte_array<Size> out;
    return to_big_endian_private(std::move(out), value);
}

template <size_t Size, typename Integer>
static byte_array<Size> to_little_endian_private(Integer value) noexcept
{
    byte_array<Size> out;
    return to_little_endian_private(std::move(out), value);
}

template <typename Integer>
static data_chunk to_big_endian_private(Integer value) noexcept
{
    // Log2 is top bit position, +1 for bit count.
    // Divide bit count by bits per byte (8) and round up.
    // Rounding is required because length is determined by value, not type.
    data_chunk out(no_fill_allocator);
    out.resize(ceilinged_divide(add1(floored_log2(value)), byte_bits));
    return to_little_endian_private(std::move(out), value);
}

template <typename Integer>
static data_chunk to_little_endian_private(Integer value) noexcept
{
    // Log2 is top bit position, +1 for bit count.
    // Divide bit count by bits per byte (8) and round up.
    // Rounding is required because length is determined by value, not type.
    data_chunk out(no_fill_allocator);
    out.resize(ceilinged_divide(add1(floored_log2(value)), byte_bits));
    return to_little_endian_private(std::move(out), value);
}

// data <=> integer
// ----------------------------------------------------------------------------

template <typename Integer, if_integral_integer<Integer>>
Integer from_big_endian(const data_slice& data) noexcept
{
    // Limit sizeof to integral integers.
    return from_big_endian_private<Integer>(sizeof(Integer), data);
}

template <typename Integer, if_integral_integer<Integer>>
Integer from_little_endian(const data_slice& data) noexcept
{
    // Limit sizeof to integral integers.
    return from_little_endian_private<Integer>(sizeof(Integer), data);
}

template <typename Integer, if_integral_integer<Integer>>
byte_array<sizeof(Integer)> to_big_endian(Integer value) noexcept
{
    // Limit sizeof to integral integers.
    return to_big_endian_private<sizeof(Integer)>(value);
}

template <typename Integer, if_integral_integer<Integer>>
byte_array<sizeof(Integer)> to_little_endian(Integer value) noexcept
{
    // Limit sizeof to integral integers.
    return to_little_endian_private<sizeof(Integer)>(value);
}

// stream <=> integer
// ----------------------------------------------------------------------------

template <typename Integer, if_integral_integer<Integer>>
Integer from_big_endian(std::istream& stream) noexcept
{
    // Limit sizeof to integral integers.
    std::vector<char> buffer(sizeof(Integer));
    stream.read(buffer.data(), buffer.size());
    buffer.resize(stream.gcount());
    return from_big_endian<Integer>(buffer);
}

template <typename Integer, if_integral_integer<Integer>>
Integer from_little_endian(std::istream& stream) noexcept
{
    // Limit sizeof to integral integers.
    std::vector<char> buffer(sizeof(Integer));
    stream.read(buffer.data(), buffer.size());
    buffer.resize(stream.gcount());
    return from_little_endian<Integer>(buffer);
}

template <typename Integer, if_integral_integer<Integer>>
void to_big_endian(std::ostream& stream, Integer value) noexcept
{
    // Limit sizeof (in above override) to integral integers.
    const auto buffer = to_big_endian<Integer>(value);
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

template <typename Integer, if_integral_integer<Integer>>
void to_little_endian(std::ostream& stream, Integer value) noexcept
{
    // Limit sizeof (in above override) to integral integers.
    const auto buffer = to_little_endian<Integer>(value);
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

// data => uintx
// uintx => data_chunk
// ----------------------------------------------------------------------------

// TODO: test.
inline uintx from_big_endian(const data_slice& data) noexcept
{
    // Overload for uintx, as from_big_endian<0> reads zero bytes and uintx is
    // a signed type (though otherwise would be declared as uintx_t<0>).
    return from_big_endian_private<uintx>(data.size(), data);
}

// TODO: test.
inline uintx from_little_endian(const data_slice& data) noexcept
{
    // Overload for uintx, as from_big_endian<0> reads zero bytes and uintx is
    // a signed type (though otherwise would be declared as uintx_t<0>).
    return from_little_endian_private<uintx>(data.size(), data);
}

// TODO: test.
template <typename Integer, if_non_integral_integer<Integer>>
data_chunk to_big_endian(const Integer& value) noexcept
{
    // Limit to non-integral integers (integral selects array<-sizeof).
    return to_big_endian_private<Integer>(value);
}

// TODO: test.
template <typename Integer, if_non_integral_integer<Integer>>
data_chunk to_little_endian(const Integer& value) noexcept
{
    // Limit to non-integral integers (integral selects array<-sizeof).
    return to_little_endian_private<Integer>(value);
}

// data => uintx_t<to_bits(Bytes)>
// uintx_t or integer => data<Bytes>
// ----------------------------------------------------------------------------

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_big_endian(const data_slice& data) noexcept
{
    // Explicit uintx_t selection, uintx is specialized above.
    return from_big_endian_private<uintx_t<to_bits(Bytes)>>(Bytes, data);
}

template <size_t Bytes>
uintx_t<to_bits(Bytes)> from_little_endian(const data_slice& data) noexcept
{
    // Explicit uintx_t selection, uintx is specialized above.
    return from_little_endian_private<uintx_t<to_bits(Bytes)>>(Bytes, data);
}

template <size_t Bytes, typename Integer, if_integer<Integer>>
byte_array<Bytes> to_big_endian(const Integer& value) noexcept
{
    // Explicit array selection (from any integer including uintx).
    return to_big_endian_private<Bytes>(value);
}

template <size_t Bytes, typename Integer, if_integer<Integer>>
byte_array<Bytes> to_little_endian(const Integer& value) noexcept
{
    // Explicit array selection (from any integer including uintx).
    return to_little_endian_private<Bytes>(value);
}

// iterator => integer
// ----------------------------------------------------------------------------
// size is unguarded.

template <typename Integer, typename Iterator, if_integral_integer<Integer>>
Integer from_big_endian_unsafe(const Iterator& data) noexcept
{
    // Limit sizeof to integral integers.
    const auto slice = data_slice(data, std::next(data, sizeof(Integer)));
    return from_big_endian<Integer>(slice);
}

template <typename Integer, typename Iterator, if_integral_integer<Integer>>
Integer from_little_endian_unsafe(const Iterator& data) noexcept
{
    // Limit sizeof to integral integers.
    const auto slice = data_slice(data, std::next(data, sizeof(Integer)));
    return from_little_endian<Integer>(slice);
}

} // namespace system
} // namespace libbitcoin

#endif
