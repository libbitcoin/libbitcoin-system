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

#include "siphash.hpp"

#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc::system;

uint64_t read_uint64(const data_chunk& data)
{
    data_source istream(data);
    istream_reader source(istream);
    return source.read_8_bytes_little_endian();
}

BOOST_AUTO_TEST_SUITE(siphash_tests)

BOOST_AUTO_TEST_CASE(siphash_hash_test)
{
    const data_chunk raw_key = to_chunk(
        base16_literal("000102030405060708090a0b0c0d0e0f"));

    const data_chunk message = to_chunk(
        base16_literal("000102030405060708090a0b0c0d0e"));

    uint64_t expected = 0xa129ca6149be45e5;

    data_source key_stream(raw_key);
    istream_reader key_source(key_stream);
    uint64_t k0 = key_source.read_8_bytes_little_endian();
    uint64_t k1 = key_source.read_8_bytes_little_endian();

    BOOST_REQUIRE_EQUAL(siphash(k0, k1, message), expected);
}

BOOST_AUTO_TEST_CASE(siphash_1__tests)
{
    byte_array<16> raw_key;
    BOOST_REQUIRE(decode_base16(raw_key, raw_test_key));

    for (const auto& result: siphash_hash_tests)
    {
        data_chunk data, raw_expected;
        BOOST_REQUIRE(decode_base16(data, result.message));
        BOOST_REQUIRE(decode_base16(raw_expected, result.result));
        uint64_t expected = read_uint64(raw_expected);
        BOOST_REQUIRE_EQUAL(siphash(raw_key, data), expected);
    }
}

BOOST_AUTO_TEST_CASE(siphash_2__tests)
{
    data_chunk raw_key;
    BOOST_REQUIRE(decode_base16(raw_key, raw_test_key));
    data_source key_stream(raw_key);
    istream_reader key_source(key_stream);
    const uint64_t k0 = key_source.read_8_bytes_little_endian();
    const uint64_t k1 = key_source.read_8_bytes_little_endian();

    for (const auto& result: siphash_hash_tests)
    {
        data_chunk data, raw_expected;
        BOOST_REQUIRE(decode_base16(data, result.message));
        BOOST_REQUIRE(decode_base16(raw_expected, result.result));
        uint64_t expected = read_uint64(raw_expected);
        BOOST_REQUIRE_EQUAL(siphash(k0, k1, data), expected);
    }
}

BOOST_AUTO_TEST_SUITE_END()
