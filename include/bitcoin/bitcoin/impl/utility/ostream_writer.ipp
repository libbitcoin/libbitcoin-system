/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_OSTREAM_WRITER_IPP
#define LIBBITCOIN_OSTREAM_WRITER_IPP

#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

template <unsigned Size>
void ostream_writer::write_forward(const byte_array<Size>& value)
{
    const auto size = value.size();
    if (size > 0)
        stream_.write(reinterpret_cast<const char*>(value.data()), size);
}

template <unsigned Size>
void ostream_writer::write_reverse(const byte_array<Size>& value)
{
    for (unsigned i = 0; i < Size; i++)
        write_byte(value[Size - (i + 1)]);
}

template <typename Integer>
void ostream_writer::write_big_endian(Integer value)
{
    byte_array<sizeof(Integer)> bytes = to_big_endian(value);
    write_forward<sizeof(Integer)>(bytes);
}

template <typename Integer>
void ostream_writer::write_little_endian(Integer value)
{
    byte_array<sizeof(Integer)> bytes = to_little_endian(value);
    write_forward<sizeof(Integer)>(bytes);
}

} // libbitcoin

#endif
