/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

uint8_t read_byte(std::istream& stream)
{
    uint8_t result;

    stream.read(reinterpret_cast<char*>(&result), sizeof result);

    return result;
}

uint16_t read_2_bytes(std::istream& stream)
{
    return read_little_endian<uint16_t>(stream);
}

uint32_t read_4_bytes(std::istream& stream)
{
    return read_little_endian<uint32_t>(stream);
}

uint64_t read_8_bytes(std::istream& stream)
{
    return read_little_endian<uint64_t>(stream);
}

uint64_t read_variable_uint(std::istream& stream)
{
    uint8_t length = read_byte(stream);

    if (length < 0xfd)
        return length;
    else if (length == 0xfd)
        return read_2_bytes(stream);
    else if (length == 0xfe)
        return read_4_bytes(stream);

    // length should be 0xff
    return read_8_bytes(stream);
}

data_chunk read_data(std::istream& stream, uint64_t n_bytes)
{
    data_chunk raw_bytes(n_bytes);

    for (uint64_t i = 0; i < n_bytes; ++i)
        raw_bytes[i] = read_byte(stream);

    return raw_bytes;
}

data_chunk read_all_data(std::istream& stream)
{
    data_chunk raw_bytes;

    while (stream && (stream.peek() != std::istream::traits_type::eof()))
        raw_bytes.push_back(read_byte(stream));

    return raw_bytes;
}

hash_digest read_hash(std::istream& stream)
{
    return read_bytes<hash_size>(stream);
}

short_hash read_short_hash(std::istream& stream)
{
    return read_bytes<short_hash_size>(stream);
}

std::string read_fixed_string(std::istream& stream, uint64_t len)
{
    data_chunk string_bytes = read_data(stream, len);
    std::string result(string_bytes.begin(), string_bytes.end());
    // Removes trailing 0s... Needed for string comparisons
    return result.c_str();
}

std::string read_string(std::istream& stream)
{
    uint64_t string_size = read_variable_uint(stream);
    return read_fixed_string(stream, string_size);
}

} // namespace libbitcoin

