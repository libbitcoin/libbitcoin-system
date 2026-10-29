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

BOOST_AUTO_TEST_SUITE(crc32_tests)

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

BOOST_AUTO_TEST_CASE(crc32__text__vectors__expected)
{
    for (const auto& vector: crc32_vectors)
    {
        const data_chunk data(vector.text.begin(), vector.text.end());
        BOOST_REQUIRE_EQUAL(crc32(data), vector.crc);
    }
}

BOOST_AUTO_TEST_CASE(crc32__zeros__thirty_two_bytes__expected)
{
    const data_array<32> zeros{};
    BOOST_REQUIRE_EQUAL(crc32(zeros), 0x190a55ad_u32);
}

BOOST_AUTO_TEST_CASE(crc32__ones__thirty_two_bytes__expected)
{
    data_array<32> ones{};
    ones.fill(0xff);
    BOOST_REQUIRE_EQUAL(crc32(ones), 0xff6cab0b_u32);
}

BOOST_AUTO_TEST_SUITE_END()
