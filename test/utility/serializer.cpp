/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(serializer_tests)

BOOST_AUTO_TEST_CASE(roundtrip_serialize_deserialize)
{
    data_chunk data(1 + 2 + 4 + 8 + 4 + 4 + 3 + 7);
    auto s = make_serializer(data.begin());
    s.write_byte(0x80);
    s.write_2_bytes_little_endian(0x8040);
    s.write_4_bytes_little_endian(0x80402010);
    s.write_8_bytes_little_endian(0x8040201011223344);
    s.write_big_endian<uint32_t>(0x80402010);
    s.write_variable_uint_little_endian(1234);
    s.write_data(to_data_chunk(to_little_endian<uint32_t>(0xbadf00d)));
    s.write_string("hello");
    auto ds = make_deserializer(data.begin(), s.iterator());
    BOOST_REQUIRE_EQUAL(ds.read_byte(), 0x80u);
    BOOST_REQUIRE_EQUAL(ds.read_2_bytes_little_endian(), 0x8040u);
    BOOST_REQUIRE_EQUAL(ds.read_4_bytes_little_endian(), 0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_8_bytes_little_endian(), 0x8040201011223344u);
    BOOST_REQUIRE_EQUAL(ds.read_big_endian<uint32_t>(),  0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_variable_uint_little_endian(), 1234u);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(
        ds.read_data(4).begin()), 0xbadf00du);
    BOOST_REQUIRE_EQUAL(ds.read_string(), "hello");
    BOOST_REQUIRE_THROW(ds.read_byte(), end_of_stream);
}

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_empty_stream_returns_true)
{
    data_chunk data(0);
    auto source = make_deserializer(data.begin(), data.end());
    BOOST_REQUIRE_EQUAL(true, source.is_exhausted());
}

BOOST_AUTO_TEST_CASE(is_exhausted_initialized_nonempty_stream_returns_false)
{
    data_chunk data(1);
    auto source = make_deserializer(data.begin(), data.end());
    BOOST_REQUIRE_EQUAL(false, source.is_exhausted());
}

BOOST_AUTO_TEST_CASE(roundtrip_byte)
{
    const uint8_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_byte(expected);
    uint8_t result = source.read_byte();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_little_endian)
{
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_2_bytes_little_endian(expected);
    uint16_t result = source.read_2_bytes_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_little_endian)
{
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_4_bytes_little_endian(expected);
    uint32_t result = source.read_4_bytes_little_endian();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_little_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_8_bytes_little_endian(expected);
    uint64_t result = source.read_8_bytes_little_endian();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_2_bytes_big_endian)
{
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_2_bytes_big_endian(expected);
    uint16_t result = source.read_2_bytes_big_endian();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_4_bytes_big_endian)
{
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_4_bytes_big_endian(expected);
    uint32_t result = source.read_4_bytes_big_endian();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_8_bytes_big_endian)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_8_bytes_big_endian(expected);
    uint64_t result = source.read_8_bytes_big_endian();

    BOOST_REQUIRE(expected == result);

    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}


BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_little_endian(expected);

    uint64_t result = source.read_variable_uint_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_2_bytes)
{
    const uint64_t expected = 43707;
    data_chunk data(3);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_little_endian(expected);

    uint64_t result = source.read_variable_uint_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_little_endian(expected);

    uint64_t result = source.read_variable_uint_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_little_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_little_endian(expected);

    uint64_t result = source.read_variable_uint_little_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_1_byte)
{
    const uint64_t expected = 0xAA;
    data_chunk data(1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_big_endian(expected);

    uint64_t result = source.read_variable_uint_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_2_bytes)
{
    const uint64_t expected = 43707;
    data_chunk data(sizeof(uint16_t) + 1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_big_endian(expected);

    uint64_t result = source.read_variable_uint_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_4_bytes)
{
    const uint64_t expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_big_endian(expected);

    uint64_t result = source.read_variable_uint_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_variable_uint_big_endian_8_bytes)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_variable_uint_big_endian(expected);

    uint64_t result = source.read_variable_uint_big_endian();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_data_chunk)
{
    const data_chunk expected = {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_data(expected);

    data_chunk result = source.read_data(expected.size());

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_data_pointer_array)
{
    const data_chunk expected = {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_data(expected.data(), expected.size());

    data_chunk result(expected.size());
    source.read_data(reinterpret_cast<uint8_t*>(result.data()), result.size());

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_hash)
{
    const hash_digest expected = {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_hash(expected);

    hash_digest result = source.read_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_short_hash)
{
    const short_hash expected = {
        0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79, 0x90, 0xab, 0x62, 0x04,
        0xb5, 0x2c, 0x6a, 0x40 , 0xdc, 0x6d, 0xa5, 0xfe
    };

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_short_hash(expected);

    short_hash result = source.read_short_hash();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_fixed_string)
{
    const std::string expected = "my string data";

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_fixed_string(expected, 10);

    std::string result = source.read_fixed_string(10);

    BOOST_REQUIRE(expected.substr(0, 10) == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(roundtrip_string)
{
    const std::string expected = "my string data";

    data_chunk data((expected.length() + variable_uint_size(expected.length())));
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_string(expected);

    std::string result = source.read_string();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_CASE(read_data_to_eof)
{
    const data_chunk expected = {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_deserializer(data.begin(), data.end());
    auto sink = make_serializer(data.begin());

    sink.write_data(expected);

    data_chunk result = source.read_data_to_eof();

    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE(sink);
    BOOST_REQUIRE(source);
    BOOST_REQUIRE_EQUAL(false, !sink);
    BOOST_REQUIRE_EQUAL(false, !source);
}

BOOST_AUTO_TEST_SUITE_END()
