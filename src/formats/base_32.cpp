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
#include <bitcoin/system/formats/base_32.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>

// TODO: expose these from a new <utility/istream.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>

// TODO: expose these from a new <utility/ostream.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {

constexpr char encode[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";
constexpr uint8_t decode[] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    15,   0xff, 10,   17,   21,   20,   26,   30,
    7,    5,    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 29,   0xff, 24,   13,   25,   9,    8,
    23,   0xff, 18,   22,   31,   27,   19,   0xff,
    1,    0,    3,    16,   11,   28,   12,   14,
    6,    4,    2,    0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 29,   0xff, 24,   13,   25,   9,    8,
    23,   0xff, 18,   22,   31,   27,   19,   0xff,
    1,    0,    3,    16,   11,   28,   12,   14,
    6,    4,    2,    0xff, 0xff, 0xff, 0xff, 0xff
};

std::string encode_base32(const data_chunk& data)
{
    size_t index = 0;
    std::string out(data.size(), '\0');

    // The map is 32 elements, expanded byte indexes are bounded.
    for (const auto value: base32_expand(data))
        out[index++] = encode[value];
    
    return out;
}

bool decode_base32(data_chunk& out, const std::string& in)
{
    size_t index = 0;
    data_chunk expanded(in.size());

    // The map is 256 elements, char indexes are bounded.
    for (const uint8_t character: ascii_to_lower(in))
        if (((expanded[index++] = decode[character])) == 0xff)
            return false;

    // Length hasn't been checked, but this handles it.
    return base32_compact(out, expanded);
}

// compact/expand

#if defined(UNDEFINED)

// This is how C developers do it. :P
static bool transform(data_chunk& out, const data_chunk& data, bool expand)
{
    // Compact is a ((n * 5) / 8) operation, ((n * 5) % 8) must be zero.
    if (!expand && (data.size() * 5) % 8 != 0)
        return false;

    const uint32_t to_bits = expand ? 5 : 8;
    const uint32_t from_bits = expand ? 8 : 5;
    const uint32_t to_max = (1 << to_bits) - 1;
    const uint32_t from_max = (1 << ((8 + 5) - 1)) - 1;

    const auto is_valid = [=](uint8_t byte)
        { return expand || byte < (1 << 5); };
    const auto remainder = [=](uint32_t accumulator, uint32_t bits)
        { return (accumulator << (to_bits - bits)) & to_max; };
    const auto shift_in = [=](uint32_t accumulator, uint8_t byte)
        { return ((accumulator << from_bits) | byte) & from_max; };
    const auto shift_out = [](uint32_t acc, uint32_t bits)
        { return acc >> bits; };
    const auto push = [&](uint8_t byte)
        { out.push_back(byte & to_max); };

    uint32_t bits = 0;
    uint32_t accumulator = 0;

    for (const auto byte: data)
    {
        if (!is_valid(byte))
            return false;

        accumulator = shift_in(accumulator, byte);
        bits += from_bits;

        while (bits >= to_bits)
        {
            bits -= to_bits;
            push(shift_out(accumulator, bits));
        }
    }

    // Expand is a ((n * 8) / 5) operation, ((n * 8) % 5) bits are padded.
    BITCOIN_ASSERT(!expand || bits == (data.size() * 8) % 5);

    if (bits > 0)
        push(remainder(accumulator, bits));

    // Expand cannot fail.
    return true;
}

#endif

data_chunk base32_expand(const data_chunk& data)
{
    // The bit reader reads zeros past end until exhaustion check.
    // The bit writer pads bits up to the last whole byte upon flush.
    // This is a ((n * 8) / 5) operation, ((n * 8) % 5) bits are padded.
    data_source source(data);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);

    data_chunk out;
    data_sink sink(out);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);

    // This is how c++ developers do it. :)
    while (!bit_reader.is_exhausted())
        bit_writer.write_bits(bit_reader.read_bits(8), 5);

    bit_writer.flush();
    sink.flush();
    ////transform(out, data, true);
    return out;
}

bool base32_compact(data_chunk& out, const data_chunk& expanded)
{
    // Only a clean read and write on byte boundaries is allowed.
    // This is a ((n * 5) / 8) operation, ((n * 5) % 8) must be zero.
    if ((expanded.size() * 5) % 8 != 0)
        return false;

    // Top 3 bits must be zero for consistency, as they carry no information.
    if (!std::all_of(expanded.begin(), expanded.end(),
        [](uint8_t value) { return value < (1 << 5); }))
        return false;

    data_sink sink(out);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);

    data_source source(expanded);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);

    // This is how c++ developers do it. :)
    while (!bit_reader.is_exhausted())
        bit_writer.write_bits(bit_reader.read_bits(5), 8);

    bit_writer.flush();
    sink.flush();
    ////return transform(out, expanded, false);
    return true;
}

} // namespace system
} // namespace libbitcoin
