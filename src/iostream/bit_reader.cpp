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

#include <bitcoin/system/iostream/readers/bit_reader.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/serialization/endian.hpp>

namespace libbitcoin {
namespace system {

bit_reader::bit_reader(std::istream& source) noexcept
  : byte_reader(source), buffer_(0x00), offset_(byte_bits)
{
}

bit_reader::~bit_reader() noexcept
{
    byte_reader::~byte_reader();
}

// bits
//-----------------------------------------------------------------------------

bool bit_reader::read_bit() noexcept
{
    if (is_aligned())
        buffer_ = do_read();

    return !is_zero((buffer_ << offset_++) & 0x80);
}

uint64_t bit_reader::read_bits(uint8_t bits) noexcept
{
    if (is_zero(bits))
        return 0;

    auto read_bits = lesser<uint8_t>(to_bits(sizeof(uint64_t)), bits);

    uint64_t out = 0;
    while (read_bits > byte_bits)
        out |= (static_cast<uint64_t>(do_read()) << ((read_bits -= byte_bits)));

    for (uint8_t index = 0; index < read_bits; ++index)
        out |= (to_int<uint64_t>(read_bit()) << (read_bits - add1(index)));

    return out;
}

// TODO: implement forward seek.
void bit_reader::skip_bits(size_t bits) noexcept
{
    while (!is_zero(bits--))
        read_bit();
}

// protected overrides
//-----------------------------------------------------------------------------

uint8_t bit_reader::do_peek() noexcept
{
    if (is_aligned())
        return byte_reader::do_peek();

    if (is_zero(offset_))
        return buffer_;

    const uint8_t hi = buffer_ << offset_;
    const uint8_t lo = byte_reader::do_peek() >> shift();
    return hi | lo;
}

uint8_t bit_reader::do_read() noexcept
{
    uint8_t byte;
    do_read(&byte, one);
    return byte;
}

void bit_reader::do_read(uint8_t* buffer, size_t size) noexcept
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

void bit_reader::do_skip(size_t size) noexcept
{
    skip_bits(to_bits(size));
}

bool bit_reader::get_valid() const noexcept
{
    return byte_reader::get_valid();
}

bool bit_reader::get_exhausted() const noexcept
{
    return is_aligned() && byte_reader::get_exhausted();
}

void bit_reader::set_invalid() noexcept
{
    align();
    byte_reader::set_invalid();
}

// private
//-----------------------------------------------------------------------------

void bit_reader::align() noexcept
{
    offset_ = byte_bits;
}

bool bit_reader::is_aligned() const noexcept
{
    return is_zero(shift());
}

uint8_t bit_reader::shift() const noexcept
{
    return byte_bits - offset_;
}

} // namespace system
} // namespace libbitcoin
