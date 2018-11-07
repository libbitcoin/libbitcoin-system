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
#include <bitcoin/bitcoin/utility/istream_reader.hpp>

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

istream_reader::istream_reader(std::istream& stream)
  : stream_(stream)
{
}

// Context.
//-----------------------------------------------------------------------------

istream_reader::operator bool() const
{
    return (bool)stream_;
}

bool istream_reader::operator!() const
{
    return !stream_;
}

bool istream_reader::is_exhausted() const
{
    return !stream_ || empty();
}

void istream_reader::invalidate()
{
    stream_.setstate(std::istream::failbit);
}

// Hashes.
//-----------------------------------------------------------------------------

hash_digest istream_reader::read_hash()
{
    return read_forward<hash_size>();
}

short_hash istream_reader::read_short_hash()
{
    return read_forward<short_hash_size>();
}

mini_hash istream_reader::read_mini_hash()
{
    return read_forward<mini_hash_size>();
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

uint16_t istream_reader::read_2_bytes_big_endian()
{
    return read_big_endian<uint16_t>();
}

uint32_t istream_reader::read_4_bytes_big_endian()
{
    return read_big_endian<uint32_t>();
}

uint64_t istream_reader::read_8_bytes_big_endian()
{
    return read_big_endian<uint64_t>();
}

uint64_t istream_reader::read_variable_big_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case varint_eight_bytes:
            return read_8_bytes_big_endian();
        case varint_four_bytes:
            return read_4_bytes_big_endian();
        case varint_two_bytes:
            return read_2_bytes_big_endian();
        default:
            return value;
    }
}

size_t istream_reader::read_size_big_endian()
{
    const auto size = read_variable_big_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    invalidate();
    return 0;
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

code istream_reader::read_error_code()
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

uint16_t istream_reader::read_2_bytes_little_endian()
{
    return read_little_endian<uint16_t>();
}

uint32_t istream_reader::read_4_bytes_little_endian()
{
    return read_little_endian<uint32_t>();
}

uint64_t istream_reader::read_8_bytes_little_endian()
{
    return read_little_endian<uint64_t>();
}

uint64_t istream_reader::read_variable_little_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case varint_eight_bytes:
            return read_8_bytes_little_endian();
        case varint_four_bytes:
            return read_4_bytes_little_endian();
        case varint_two_bytes:
            return read_2_bytes_little_endian();
        default:
            return value;
    }
}

size_t istream_reader::read_size_little_endian()
{
    const auto size = read_variable_little_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t)
        return static_cast<size_t>(size);

    invalidate();
    return 0;
}

// Bytes.
//-----------------------------------------------------------------------------

uint8_t istream_reader::peek_byte()
{
    return stream_.peek();
}

uint8_t istream_reader::read_byte()
{
    //// return read_bytes(1)[0];
    return stream_.get();
}


data_chunk istream_reader::read_bytes()
{
    data_chunk out;

    while (!is_exhausted())
        out.push_back(read_byte());

    return out;
}

// Return size is guaranteed.
// This is a memory exhaustion risk if caller does not control size.
data_chunk istream_reader::read_bytes(size_t size)
{
    // TODO: avoid unnecessary default zero fill using
    // the allocator adapter here: stackoverflow.com/a/21028912/1172329.
    data_chunk out(size);

    if (size > 0)
    {
        auto buffer = reinterpret_cast<char*>(out.data());
        stream_.read(buffer, size);
    }

    return out;
}

std::string istream_reader::read_string()
{
    return read_string(read_size_little_endian());
}

// Removes trailing zeros, required for bitcoin string comparisons.
std::string istream_reader::read_string(size_t size)
{
    std::string out;
    out.reserve(size);
    auto terminated = false;

    // Read all size characters, pushing all non-null (may be many).
    for (size_t index = 0; index < size && !empty(); ++index)
    {
        const auto character = read_byte();
        terminated |= (character == string_terminator);

        // Stop pushing characters at the first null.
        if (!terminated)
            out.push_back(character);
    }

    // Reduce the allocation to the number of characters pushed.
    out.shrink_to_fit();
    return out;
}

void istream_reader::skip(size_t size)
{
    // TODO: investigate failure using seekg.
    // Seek the relative size offset from the current position.
    ////stream_.seekg(size, std::ios_base::cur);
    read_bytes(size);
}

// private

bool istream_reader::empty() const
{
    return stream_.peek() == std::istream::traits_type::eof();
}

} // namespace libbitcoin
