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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(crc32_tests)

// Bytes of a deterministic sequence, (7 * index + 3) mod 256.
static data_chunk sequence(size_t size)
{
    data_chunk out(size);
    for (size_t index{}; index < size; ++index)
        out[index] = narrow_cast<uint8_t>(index * 7u + 3u);

    return out;
}

struct crc32_length_vector
{
    size_t size;
    uint32_t crc;
};

// Lengths about each word and fold boundary.
const std_vector<crc32_length_vector> crc32_length_vectors
{
    { 15, 0x7c619edc },
    { 16, 0x191f3d9f },
    { 31, 0xd07f9b5b },
    { 32, 0xa10e8695 },
    { 63, 0xb7350c2a },
    { 64, 0xcbd9ecf0 },
    { 65, 0x6d195777 },
    { 79, 0xf8bdaeac },
    { 80, 0x3f42d103 },
    { 100, 0xaa316b09 },
    { 127, 0xefb66daa },
    { 128, 0xbd5d2e01 },
    { 129, 0xd10950af },
    { 1000, 0x17bc2a46 },
    { 5552, 0x25d4ee15 },
    { 5553, 0xa0205ce6 },
    { 6000, 0x7fe05c7b },
    { 70000, 0xe0290e8e }
};

struct crc32_vector
{
    std::string text;
    uint32_t crc;
};

// CRC-32/ISO-HDLC check value and common vectors.
const std_vector<crc32_vector> crc32_vectors
{
    { "", 0x00000000 },
    { "a", 0xe8b7be43 },
    { "abc", 0x352441c2 },
    { "123456789", 0xcbf43926 },
    { "message digest", 0x20159d7f },
    { "The quick brown fox jumps over the lazy dog", 0x414fa339 }
};

BC_AUTO_TEST_CASE(crc32__text__vectors__expected)
{
    for (const auto& vector: crc32_vectors)
    {
        const data_chunk data(vector.text.begin(), vector.text.end());
        BC_REQUIRE_EQUAL(crc32(data), vector.crc);
    }
}

BC_AUTO_TEST_CASE(crc32__zeros__thirty_two_bytes__expected)
{
    const data_array<32> zeros{};
    BC_REQUIRE_EQUAL(crc32(zeros), 0x190a55ad_u32);
}

BC_AUTO_TEST_CASE(crc32__ones__thirty_two_bytes__expected)
{
    data_array<32> ones{};
    ones.fill(0xff);
    BC_REQUIRE_EQUAL(crc32(ones), 0xff6cab0b_u32);
}

BC_AUTO_TEST_CASE(crc32__sequence__length_vectors__expected)
{
    for (const auto& vector: crc32_length_vectors)
    {
        BC_REQUIRE_EQUAL(crc32(sequence(vector.size)), vector.crc);
    }
}

BC_AUTO_TEST_CASE(crc32__continued__split__expected)
{
    const auto data = sequence(70000);
    const data_slice left(data.data(), std::next(data.data(), 12345));
    const data_slice right(std::next(data.data(), 12345), std::next(data.data(), data.size()));
    BC_REQUIRE_EQUAL(crc32(right, crc32(left)), crc32(data));
}

BC_AUTO_TEST_CASE(crc32__empty__continued__unchanged)
{
    BC_REQUIRE_EQUAL(crc32(data_chunk{}, 0x12345678_u32), 0x12345678_u32);
}

BC_AUTO_TEST_SUITE_END()
