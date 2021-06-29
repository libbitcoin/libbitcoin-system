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

#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_WRITER_IPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/serialization/endian.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// constructors
//-----------------------------------------------------------------------------

template <typename OStream>
bit_writer<OStream>::bit_writer(OStream& sink) noexcept
  : byte_writer(sink), buffer_(0x00), offset_(0)
{
}

template <typename OStream>
bit_writer<OStream>::~bit_writer() noexcept
{
    if (!is_zero(offset_))
        byte_writer::do_write_bytes(&buffer_, one);

    byte_writer::~byte_writer();
}

// bits
//-----------------------------------------------------------------------------

template <typename OStream>
void bit_writer<OStream>::write_bit(bool value) noexcept
{
    buffer_ |= ((value ? 0x80 : 0x00) >> offset_++);

    if (offset_ == byte_bits)
    {
        byte_writer::do_write_bytes(&buffer_, one);
        buffer_ = 0x00;
        offset_ = 0;
    }
}

template <typename OStream>
void bit_writer<OStream>::write_bits(uint64_t value, size_t bits) noexcept
{
    bits = lesser<size_t>(to_bits(sizeof(uint64_t)), bits);

    for (auto bit = bits; !is_zero(bit); --bit)
        write_bit(!is_zero((value >> sub1(bit)) & to_int<uint64_t>(true)));
}

// protected overrides
//-----------------------------------------------------------------------------

template <typename OStream>
void bit_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) noexcept
{
    // TODO: Suboptimal but simple.
    for (size_t byte = 0; byte < size; ++byte)
        write_bits(data[byte], byte_bits);
}

template <typename OStream>
void bit_writer<OStream>::do_flush() noexcept
{
    if (!is_zero(offset_))
        byte_writer::do_write_bytes(&buffer_, one);

    byte_writer::do_flush();
}

} // namespace system
} // namespace libbitcoin

#endif
