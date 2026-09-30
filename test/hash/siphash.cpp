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

// Sponsored in part by Digital Contract Design, LLC

#include "../test.hpp"
#include "siphash.hpp"

// This is a lot of tests, not suprisingly very fast execution.

BOOST_AUTO_TEST_SUITE(siphash_tests)

BOOST_AUTO_TEST_CASE(siphash__hash__test_key__expected)
{
    half_hash hash{};
    BOOST_REQUIRE(decode_base16(hash, hash_test_key));

    constexpr auto expected = 0xa129ca6149be45e5;
    constexpr auto message = base16_array("000102030405060708090a0b0c0d0e");
    BOOST_REQUIRE_EQUAL(siphash(hash, message), expected);
}

BOOST_AUTO_TEST_CASE(siphash__words__test_key__expected)
{
    half_hash hash{};
    BOOST_REQUIRE(decode_base16(hash, hash_test_key));

    constexpr auto message = base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
    const siphash_words words{ 0x0706050403020100, 0x0f0e0d0c0b0a0908, 0x1716151413121110, 0x1f1e1d1c1b1a1918 };
    BOOST_REQUIRE_EQUAL(siphash(to_siphash_key(hash), words), siphash(hash, message));
}

// 29 rows exercise 512, 256 and 128 bit lanes and the integral remainder.
BOOST_AUTO_TEST_CASE(siphash__columns__rows__expected_by_row)
{
    half_hash hash{};
    BOOST_REQUIRE(decode_base16(hash, hash_test_key));
    const auto key = to_siphash_key(hash);

    constexpr size_t rows = 29;
    std_array<std_array<uint64_t, rows>, 4> words{};
    for (size_t row{}; row < rows; ++row)
        for (size_t column{}; column < words.size(); ++column)
            words[column][row] = (row << 8) + column;

    std_array<uint64_t, rows> out{};
    const siphash_columns columns{ words[0], words[1], words[2], words[3] };
    siphash(out, key, columns);

    for (size_t row{}; row < rows; ++row)
        BOOST_REQUIRE_EQUAL(out[row], siphash(key, siphash_words{ words[0][row], words[1][row], words[2][row], words[3][row] }));
}

BOOST_AUTO_TEST_CASE(siphash__columns__empty__unchanged)
{
    const siphash_columns columns{};
    std::span<uint64_t> out{};
    siphash(out, siphash_key{}, columns);
    BOOST_REQUIRE(out.empty());
}

BOOST_AUTO_TEST_CASE(siphash__hash__vectors__expected)
{
    half_hash hash{};
    BOOST_REQUIRE(decode_base16(hash, hash_test_key));

    for (const auto& result: siphash_hash_tests)
    {
        data_chunk data;
        BOOST_REQUIRE(decode_base16(data, result.message));

        data_chunk encoded_expected;
        BOOST_REQUIRE(decode_base16(encoded_expected, result.result));

        const auto expected = from_little_endian<uint64_t>(encoded_expected);
        BOOST_REQUIRE_EQUAL(siphash(hash, data), expected);
    }
}

BOOST_AUTO_TEST_CASE(siphash__key__vectors__expected)
{
    half_hash hash{};
    BOOST_REQUIRE(decode_base16(hash, hash_test_key));

    const auto key = to_siphash_key(hash);

    for (const auto& result: siphash_hash_tests)
    {
        data_chunk data;
        BOOST_REQUIRE(decode_base16(data, result.message));

        data_chunk encoded_expected;
        BOOST_REQUIRE(decode_base16(encoded_expected, result.result));

        const auto expected = from_little_endian<uint64_t>(encoded_expected);
        BOOST_REQUIRE_EQUAL(siphash(key, data), expected);
    }
}

BOOST_AUTO_TEST_SUITE_END()
