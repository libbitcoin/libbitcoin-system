/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

#ifndef LIBBITCOIN_SYSTEM_ISTREAM_BIT_READER_IPP
#define LIBBITCOIN_SYSTEM_ISTREAM_BIT_READER_IPP

#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/endian.hpp>

namespace libbitcoin {
namespace system {

template <size_t Size>
byte_array<Size> istream_bit_reader::read_forward()
{
	auto data = reader_.read_bytes(Size);
	byte_array<Size> out = to_array<Size>(data);
    return out;
}

template <size_t Size>
byte_array<Size> istream_bit_reader::read_reverse()
{
    byte_array<Size> out;

    for (size_t index = 0; index < Size; ++index)
        out[Size - (index + 1u)] = read_byte();

    return out;
}

template <typename Integer, typename>
Integer istream_bit_reader::read_big_endian()
{
    Integer out = 0;

    for (size_t index = sizeof(Integer); index > 0u && !is_exhausted(); --index)
    {
        auto value = read_byte();
        out |= static_cast<Integer>(value) << (8u * (index - 1u));
    }

    return out;
}

template <typename Integer, typename>
Integer istream_bit_reader::read_little_endian()
{
    Integer out = 0;

    for (size_t index = 0; index < sizeof(Integer) && !is_exhausted(); ++index)
    {
        auto value = read_byte();
        out |= static_cast<Integer>(value) << (8u * index);
    }

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
