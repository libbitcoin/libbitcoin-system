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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(serializer_tests)

BOOST_AUTO_TEST_CASE(roundtrip_serialize_deserialize)
{
    data_chunk data(1 + 2 + 4 + 8 + 4 + 4 + 3 + 7);
    auto writer = make_unsafe_serializer(data.begin());
    writer.write_byte(0x80);
    writer.write_2_bytes_little_endian(0x8040);
    writer.write_4_bytes_little_endian(0x80402010);
    writer.write_8_bytes_little_endian(0x8040201011223344);
    writer.write_4_bytes_big_endian(0x80402010);
    writer.write_variable_little_endian(1234);
    writer.write_bytes(to_chunk(to_little_endian<uint32_t>(0xbadf00d)));
    writer.write_string("hello");

    auto reader = make_safe_deserializer(data.begin(), data.end());
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 0x80u);
    BOOST_REQUIRE_EQUAL(reader.read_2_bytes_little_endian(), 0x8040u);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_little_endian(), 0x80402010u);
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_little_endian(), 0x8040201011223344u);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_big_endian(), 0x80402010u);
    BOOST_REQUIRE_EQUAL(reader.read_variable_little_endian(), 1234u);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(reader.read_bytes(4).begin()), 0xbadf00du);
    BOOST_REQUIRE_EQUAL(reader.read_string(), "hello");
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 0u);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(deserializer_exhaustion)
{
    data_chunk data(42);
    auto reader = make_safe_deserializer(data.begin(), data.end());
    reader.read_bytes(42);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_empty_stream_returns_true)
{
    data_chunk data(0);
    auto source = make_safe_deserializer(data.begin(), data.end());
    BOOST_REQUIRE(source.is_exhausted());
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_nonempty_stream_returns_false)
{
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    BOOST_REQUIRE(!source.is_exhausted());
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(peek_byte_nonempty_stream_does_not_advance)
{
    const uint8_t expected = 0x42;
    data_chunk data({ expected, 0x00 });
    auto source = make_safe_deserializer(data.begin(), data.end());
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE_EQUAL(source.peek_byte(), expected);
    BOOST_REQUIRE((bool)source);
}

BOOST_AUTO_TEST_CASE(roundtrip_byte)
{
    const uint8_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_byte(expected);
    const auto result = source.read_byte();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_error_code)
{
    const code expected(error::futuristic_timestamp);
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_error_code(expected);
    const auto result = source.read_error_code();

    BOOST_REQUIRE_EQUAL(expected, result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_little_endian)
{
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_2_bytes_little_endian(expected);
    const auto result = source.read_2_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);

}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_little_endian)
{
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_4_bytes_little_endian(expected);
    const auto result = source.read_4_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_little_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_8_bytes_little_endian(expected);
    const auto result = source.read_8_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_big_endian)
{
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_2_bytes_big_endian(expected);
    const auto result = source.read_2_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_big_endian)
{
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_4_bytes_big_endian(expected);
    const auto result = source.read_4_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_big_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_8_bytes_big_endian(expected);
    const auto result = source.read_8_bytes_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}


BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_2_bytes)
{
    const uint64_t expected = 43707;
    data_chunk data(3);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    const auto result = source.read_variable_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_2_bytes)
{
    const uint64_t expected = 43707;
    data_chunk data(sizeof(uint16_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    const auto result = source.read_variable_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_data_chunk)
{
    static const data_chunk expected
    {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_bytes(expected);

    const auto result = source.read_bytes(expected.size());

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_hash)
{
    static const hash_digest expected
    {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_hash(expected);

    const auto result = source.read_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_short_hash)
{
    static const short_hash expected
    {
        0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79, 0x90, 0xab, 0x62, 0x04,
        0xb5, 0x2c, 0x6a, 0x40 , 0xdc, 0x6d, 0xa5, 0xfe
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_short_hash(expected);

    const auto result = source.read_short_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_mini_hash)
{
    static const mini_hash expected
    {
        0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_mini_hash(expected);

    const auto result = source.read_mini_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_fixed_string)
{
    const std::string expected = "my string data";

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_string(expected, 10);

    const auto result = source.read_string(10);

    BOOST_REQUIRE(expected.substr(0, 10) == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_string)
{
    const std::string expected = "my string data";

    data_chunk data((expected.length() + message::variable_uint_size(expected.length())));
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_string(expected);

    const auto result = source.read_string();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(read_bytes_to_eof)
{
    static const data_chunk expected
    {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_bytes(expected);

    const auto result = source.read_bytes();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE((bool)sink);
    BOOST_REQUIRE((bool)source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_SUITE_END()
