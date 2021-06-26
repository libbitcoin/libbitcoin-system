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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_STREAM_READERS_BIT_READER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_READERS_BIT_READER_IPP

#include <bitcoin/system/stream/readers/bit_reader.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/serialization/endian.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// constructors
//-----------------------------------------------------------------------------

template <typename IStream>
bit_reader<IStream>::bit_reader(IStream& source) noexcept
  : byte_reader(source), buffer_(0x00), offset_(byte_bits)
{
}

template <typename IStream>
bit_reader<IStream>::~bit_reader() noexcept
{
    byte_reader::~byte_reader();
}

// bits
//-----------------------------------------------------------------------------

template <typename IStream>
bool bit_reader<IStream>::read_bit() noexcept
{
    if (is_aligned())
        buffer_ = do_read();

    return !is_zero((buffer_ << offset_++) & 0x80);
}

template <typename IStream>
uint64_t bit_reader<IStream>::read_bits(size_t bits) noexcept
{
    uint64_t out = 0;
    while (bits > byte_bits)
        out |= (static_cast<uint64_t>(do_read()) << ((bits -= byte_bits)));

    for (uint8_t bit = 0; bit < bits; ++bit)
        out |= (to_int<uint64_t>(read_bit()) << (bits - add1(bit)));

    return out;
}

// TODO: utilize base class forward seek.
template <typename IStream>
void bit_reader<IStream>::skip_bit(size_t bits) noexcept
{
    while (!is_zero(bits--))
        read_bit();
}

// protected overrides
//-----------------------------------------------------------------------------

template <typename IStream>
uint8_t bit_reader<IStream>::do_peek() noexcept
{
    if (is_aligned())
        return byte_reader::do_peek();

    if (is_zero(offset_))
        return buffer_;

    const uint8_t hi = buffer_ << offset_;
    const uint8_t lo = byte_reader::do_peek() >> shift();
    return hi | lo;
}

template <typename IStream>
uint8_t bit_reader<IStream>::do_read() noexcept
{
    uint8_t byte;
    do_read(&byte, one);
    return byte;
}

template <typename IStream>
void bit_reader<IStream>::do_read(uint8_t* buffer, size_t size) noexcept
{
    if (is_zero(size))
        return;

    if (is_zero(offset_))
    {
        byte_reader::do_read(buffer, sub1(size));
        buffer[sub1(size)] = 0x00;
    }
    else
    {
        byte_reader::do_read(buffer, size);
    }

    if (is_aligned())
        return;

    // Save last byte.
    const auto next = buffer[sub1(size)];

    // Shift all bytes.
    for (auto index = sub1(size); !is_zero(index); --index)
        buffer[index] =
            ((buffer[index] >> shift()) |
            (buffer[sub1(index)] << offset_));

    // Or buffer into first byte.
    buffer[0] = ((buffer[0] >> shift()) | (buffer_ << offset_));

    if (is_zero(offset_))
    {
        align();
        buffer_ = next;
    }
}

template <typename IStream>
void bit_reader<IStream>::do_skip(size_t size) noexcept
{
    skip_bit(to_bits(size));
}

template <typename IStream>
bool bit_reader<IStream>::get_exhausted() const noexcept
{
    return is_aligned() && byte_reader::get_exhausted();
}

template <typename IStream>
bool bit_reader<IStream>::get_valid() const noexcept
{
    return byte_reader::get_valid();
}

template <typename IStream>
void bit_reader<IStream>::set_invalid() noexcept
{
    align();
    byte_reader::set_invalid();
}

// private
//-----------------------------------------------------------------------------

template <typename IStream>
void bit_reader<IStream>::align() noexcept
{
    offset_ = byte_bits;
}

template <typename IStream>
bool bit_reader<IStream>::is_aligned() const noexcept
{
    return is_zero(shift());
}

template <typename IStream>
uint8_t bit_reader<IStream>::shift() const noexcept
{
    return byte_bits - offset_;
}

} // namespace system
} // namespace libbitcoin

#endif
