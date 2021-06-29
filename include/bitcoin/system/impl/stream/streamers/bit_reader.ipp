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

#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_READER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_READER_IPP

#include <bitcoin/system/stream/streamers/bit_reader.hpp>

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
  : byte_reader(source), buffer_(0x00), offset_(0)
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
    if (is_zero(offset_))
    {
        byte_reader::do_read_bytes(&buffer_, one);
        offset_ = byte_bits;
    }

    return !is_zero((buffer_ << (byte_bits - offset_--)) & 0x80);
}

template <typename IStream>
uint64_t bit_reader<IStream>::read_bits(size_t bits) noexcept
{
    uint64_t out = 0;
    bits = lesser<size_t>(to_bits(sizeof(uint64_t)), bits);

    for (auto bit = bits; !is_zero(bit); --bit)
        out |= (to_int<uint64_t>(read_bit()) << sub1(bit));

    return out;
}

template <typename IStream>
void bit_reader<IStream>::skip_bit() noexcept
{
    read_bit();
}

template <typename IStream>
void bit_reader<IStream>::skip_bits(size_t bits) noexcept
{
    read_bits(bits);
}

////template <typename IStream>
////void bit_reader<IStream>::rewind_bit() noexcept
////{
////}
////
////template <typename IStream>
////void bit_reader<IStream>::rewind_bits(size_t bits) noexcept
////{
////}

// protected overrides
//-----------------------------------------------------------------------------

template <typename IStream>
uint8_t bit_reader<IStream>::do_peek_byte() noexcept
{
    if (offset_ == byte_bits)
        return byte_reader::do_peek_byte();

    const uint8_t hi = buffer_ << (byte_bits - offset_);
    const uint8_t lo = byte_reader::do_peek_byte() >> offset_;
    return hi | lo;
}

template <typename IStream>
void bit_reader<IStream>::do_read_bytes(uint8_t* buffer, size_t size) noexcept
{
    // TODO: Suboptimal but simple.
    for (size_t byte = 0; byte < size; ++byte)
        buffer[byte] = static_cast<uint8_t>(read_bits(byte_bits));
}

template <typename IStream>
void bit_reader<IStream>::do_skip_bytes(size_t size) noexcept
{
    // Loop to avoid overflow in multiplying bit_bytes * size.
    for (size_t byte = 0; byte < size; ++byte)
        skip_bits(byte_bits);
}

////template <typename IStream>
////void bit_reader<IStream>::do_rewind_bytes(size_t size) noexcept
////{
////    // Loop to avoid overflow in multiplying bit_bytes * size.
////    for (size_t byte = 0; byte < size; ++byte)
////        rewind_bits(bit_bytes);
////}

template <typename IStream>
bool bit_reader<IStream>::get_exhausted() const noexcept
{
    if (byte_reader::operator!())
        return true;

    return is_zero(offset_) && byte_reader::get_exhausted();
}

} // namespace system
} // namespace libbitcoin

#endif
