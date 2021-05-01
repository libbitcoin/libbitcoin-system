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
#include "../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(stream_tests)

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_empty_stream_returns_true)
{
    std::stringstream stream;
    istream_reader source(stream);
    BOOST_REQUIRE(source.is_exhausted());
}

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_nonempty_stream_returns_false)
{
    std::stringstream stream("abc");
    istream_reader source(stream);
    BOOST_REQUIRE(!source.is_exhausted());
}

BOOST_AUTO_TEST_CASE(peek_byte_nonempty_stream_does_not_advance)
{
    const uint8_t expected = 'a';
    std::stringstream stream("ab");
    istream_reader source(stream);
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE((bool)source);
}

BOOST_AUTO_TEST_CASE(roundtrip_byte)
{
    const uint8_t expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_byte(expected);
    const auto result = source.read_byte();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_little_endian)
{
    const uint16_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_2_bytes_little_endian(expected);
    const auto result = source.read_2_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_little_endian)
{
    const uint32_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_4_bytes_little_endian(expected);
    const auto result = source.read_4_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_little_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_8_bytes_little_endian(expected);
    const auto result = source.read_8_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_big_endian)
{
    const uint16_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_2_bytes_big_endian(expected);
    const auto result = source.read_2_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_big_endian)
{
    const uint32_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_4_bytes_big_endian(expected);
    const auto result = source.read_4_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_big_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_8_bytes_big_endian(expected);
    const auto result = source.read_8_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}


BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_little_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), 1u);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_2_bytes)
{
    const uint64_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_little_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint16_t) + 1);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_little_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint32_t) + 1);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_little_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint64_t) + 1);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), 1u);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_2_bytes)
{
    const uint64_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_big_endian(expected);

    BOOST_REQUIRE(stream.str().length() == sizeof(uint16_t) + 1);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint32_t) + 1);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint64_t) + 1);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

// TODO
BOOST_AUTO_TEST_CASE(roundtrip_stream)
{
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_CASE(roundtrip_data_chunk)
{
    const data_chunk expected
    {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_bytes(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    const auto result = source.read_bytes(expected.size());

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_hash)
{
    const hash_digest expected
    {
        {
            0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
            0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
            0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_hash(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    const auto result = source.read_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_short_hash)
{
    const short_hash expected
    {
        {
            0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79, 0x90, 0xab, 0x62, 0x04,
            0xb5, 0x2c, 0x6a, 0x40, 0xdc, 0x6d, 0xa5, 0xfe
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_short_hash(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    const auto result = source.read_short_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_fixed_string)
{
    const std::string expected = "my string data";

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_string(expected, 10);

    BOOST_REQUIRE_EQUAL(stream.str().length(), 10u);

    const auto result = source.read_string(10);

    BOOST_REQUIRE(expected.substr(0, 10) == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_string)
{
    const std::string expected = "my string data";

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_string(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), (expected.length() + message::variable_uint_size(expected.length())));

    const auto result = source.read_string();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(read_bytes_to_eof)
{
    const data_chunk expected
    {
        {
            0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
            0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
            0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_bytes(expected);

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    const auto result = source.read_bytes();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_SUITE_END()
