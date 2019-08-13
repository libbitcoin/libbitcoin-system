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

#include <bitcoin/system/math/siphash.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>

namespace libbitcoin {
namespace system {

using data_slice_source = stream_source<data_slice>;

const uint64_t i0 = 0x736f6d6570736575;
const uint64_t i1 = 0x646f72616e646f6d;
const uint64_t i2 = 0x6c7967656e657261;
const uint64_t i3 = 0x7465646279746573;
const uint64_t finalization = 0xff;
const uint32_t max_encoded_byte_count = 256;

// NOTE: C++20 provides std::rotl which could replace this functionality.
uint64_t inline rotate_left(uint64_t& value, uint8_t shift)
{
    return (uint64_t)((value << shift) | (value >> (64 - shift)));
}

void inline sipround(uint64_t& v0, uint64_t& v1, uint64_t& v2, uint64_t& v3)
{
    v0 += v1;
    v2 += v3;
    v1 = rotate_left(v1, 13);
    v3 = rotate_left(v3, 16);
    v1 ^= v0;
    v3 ^= v2;

    v0 = rotate_left(v0, 32);

    v2 += v1;
    v0 += v3;
    v1 = rotate_left(v1, 17);
    v3 = rotate_left(v3, 21);
    v1 ^= v2;
    v3 ^= v0;

    v2 = rotate_left(v2, 32);
}

void inline compression_round(uint64_t& v0, uint64_t& v1, uint64_t& v2, uint64_t& v3, uint64_t word)
{
    v3 ^= word;
    sipround(v0, v1, v2, v3);
    sipround(v0, v1, v2, v3);
    v0 ^= word;
}

uint64_t siphash(byte_array<16> key, const data_slice& message)
{
    data_slice_source istream(key);
    istream_reader source(istream);
    uint64_t k0 = source.read_8_bytes_little_endian();
    uint64_t k1 = source.read_8_bytes_little_endian();
    return siphash(k0, k1, message);
}

uint64_t siphash(uint64_t k0, uint64_t k1, const data_slice& message)
{
    uint64_t v0 = k0 ^ i0;
    uint64_t v1 = k1 ^ i1;
    uint64_t v2 = k0 ^ i2;
    uint64_t v3 = k1 ^ i3;

    uint64_t byte_count = message.size();
    data_slice_source istream(message);
    istream_reader source(istream);

    for (uint64_t i = 8; i <= byte_count; i += 8)
    {
        uint64_t word = source.read_8_bytes_little_endian();
        compression_round(v0, v1, v2, v3, word);
    }

    uint64_t last_word = 0;

    if (byte_count % 8 > 0)
        last_word = source.read_8_bytes_little_endian();

    last_word ^= ((byte_count % max_encoded_byte_count) << 56);
    compression_round(v0, v1, v2, v3, last_word);

    v2 ^= finalization;
    sipround(v0, v1, v2, v3);
    sipround(v0, v1, v2, v3);
    sipround(v0, v1, v2, v3);
    sipround(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

} // namespace system
} // namespace libbitcoin
