/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// constructors
// ----------------------------------------------------------------------------
    
// protected
template <typename OStream>
bit_writer<OStream>::bit_writer() NOEXCEPT
  : base(),
    byte_(base::pad()),
    offset_(0)
{
}

template <typename OStream>
bit_writer<OStream>::bit_writer(OStream& sink) NOEXCEPT
  : base(sink),
    byte_(base::pad()),
    offset_(0)
{
}

template <typename OStream>
bit_writer<OStream>::~bit_writer() NOEXCEPT
{
    // Derived virtual destructor called before base destructor.
    flusher();
}

// bits
// ----------------------------------------------------------------------------

template <typename OStream>
void bit_writer<OStream>::write_bit(bool value) NOEXCEPT
{
    // Insert bit into byte.
    set_left_into(byte_, offset_++, value);

    // unload resets offset_ to zero.
    if (is_zero(shift()))
        unload();
}

template <typename OStream>
void bit_writer<OStream>::write_bits(uint64_t value, size_t bits) NOEXCEPT
{
    bits = lesser(bc::bits<uint64_t>, bits);

    // 'bits' refers to the count of the rightmost bits in 'value'.
    // Those bits are written from left to right, completing the partial byte,
    // then whole bytes directly, then starting the next partial byte.
    while (!is_zero(bits) && !is_zero(offset_))
        write_bit(get_right(value, --bits));

    for (; bits >= byte_bits; bits -= byte_bits)
    {
        const auto byte = possible_narrow_cast<uint8_t>(
            shift_right(value, bits - byte_bits));
        base::do_write_bytes(&byte, one);
    }

    while (!is_zero(bits))
        write_bit(get_right(value, --bits));
}

// protected overrides
// ----------------------------------------------------------------------------

template <typename OStream>
void bit_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    // Suboptimal because shifts each bit and writes single bytes, but simple.
    for (size_t byte = 0; byte < size; ++byte)
        write_bits(data[byte], byte_bits);
}

template <typename OStream>
void bit_writer<OStream>::do_flush() NOEXCEPT
{
    flusher();
    base::do_flush();
}

// private
// ----------------------------------------------------------------------------

// This is the only byte write and offset change.
template <typename OStream>
void bit_writer<OStream>::unload() NOEXCEPT
{
    base::do_write_bytes(&byte_, one);
    byte_ = base::pad();
    offset_ = 0;
}

template <typename OStream>
void bit_writer<OStream>::flusher() NOEXCEPT
{
    if (!is_zero(offset_))
        unload();
}

template <typename OStream>
uint8_t bit_writer<OStream>::shift() const NOEXCEPT
{
    // If shift is zero then eight bits have been written, so time to dump.
    // If offset is zero then no bits have been written since last dump.
    return byte_bits - offset_;
}

} // namespace system
} // namespace libbitcoin

#endif
