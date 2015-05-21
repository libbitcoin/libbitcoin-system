/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_OSTREAM_IPP
#define LIBBITCOIN_OSTREAM_IPP

#include <algorithm>
#include <boost/asio/streambuf.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/exceptions.hpp>

namespace libbitcoin {

template <typename T>
void write_big_endian(std::ostream& stream, T value)
{
    byte_array<sizeof(T)> bytes = to_big_endian(value);
    write_bytes<sizeof(T)>(stream, bytes);
}

template <typename T>
void write_little_endian(std::ostream& stream, T value)
{
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
void write_data(std::ostream& stream, T& value)
{
    stream.write(reinterpret_cast<const char*>(value.data()), value.size());
}

template<unsigned N>
void write_bytes(std::ostream& stream, const byte_array<N>& value)
{
    for (unsigned i = 0; i < N; i++)
        write_byte(stream, value[i]);
}

template<unsigned N>
void write_bytes_reverse(std::ostream& stream, const byte_array<N>& value)
{
    for (unsigned i = 0; i < N; i++)
        write_byte(stream, value[N - (i + 1)]);
}

} // libbitcoin

#endif
