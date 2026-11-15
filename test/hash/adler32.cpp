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

BC_AUTO_TEST_SUITE(adler32_tests)

// Bytes of a deterministic sequence, (7 * index + 3) mod 256.
static data_chunk sequence(size_t size)
{
    data_chunk out(size);
    for (size_t index{}; index < size; ++index)
        out[index] = narrow_cast<uint8_t>(index * 7u + 3u);

    return out;
}

struct adler32_length_vector
{
    size_t size;
    uint32_t adler;
};

// Lengths about each word and fold boundary.
const std_vector<adler32_length_vector> adler32_length_vectors
{
    { 15, 0x10c7030d },
    { 16, 0x14400379 },
    { 31, 0x8d8f0d15 },
    { 32, 0x9b800df1 },
    { 63, 0x2c4c1c25 },
    { 64, 0x492d1ce1 },
    { 65, 0x66d11da4 },
    { 79, 0x5032252d },
    { 80, 0x758b2559 },
    { 100, 0xaee02e87 },
    { 127, 0x72113b45 },
    { 128, 0xadd23bc1 },
    { 129, 0xea163c44 },
    { 1000, 0x38adedfc },
    { 5552, 0x19dfcb3f },
    { 5553, 0xe5f1cc12 },
    { 6000, 0x26a8ac6e },
    { 70000, 0x8e7b36c1 }
};

struct adler32_vector
{
    std::string text;
    uint32_t adler;
};

const std_vector<adler32_vector> adler32_vectors
{
    { "", 0x00000001 },
    { "a", 0x00620062 },
    { "abc", 0x024d0127 },
    { "Wikipedia", 0x11e60398 }
};

BC_AUTO_TEST_CASE(adler32__text__vectors__expected)
{
    for (const auto& vector: adler32_vectors)
    {
        const data_chunk data(vector.text.begin(), vector.text.end());
        BC_REQUIRE_EQUAL(adler32(data), vector.adler);
    }
}

BC_AUTO_TEST_CASE(adler32__ones__beyond_reduction__expected)
{
    const data_chunk ones(6000, 0xff);
    BC_REQUIRE_EQUAL(adler32(ones), 0xa49759ea_u32);
}

BC_AUTO_TEST_CASE(adler32__sequence__length_vectors__expected)
{
    for (const auto& vector: adler32_length_vectors)
    {
        BC_REQUIRE_EQUAL(adler32(sequence(vector.size)), vector.adler);
    }
}

BC_AUTO_TEST_CASE(adler32__continued__split__expected)
{
    const auto data = sequence(70000);
    const data_slice left(data.data(), std::next(data.data(), 12345));
    const data_slice right(std::next(data.data(), 12345), std::next(data.data(), data.size()));
    BC_REQUIRE_EQUAL(adler32(right, adler32(left)), adler32(data));
}

BC_AUTO_TEST_CASE(adler32__empty__continued__unchanged)
{
    BC_REQUIRE_EQUAL(adler32(data_chunk{}, 0x12345678_u32), 0x12345678_u32);
}

BC_AUTO_TEST_SUITE_END()
