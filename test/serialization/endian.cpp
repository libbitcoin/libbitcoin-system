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
#include <cstdint>
#include <sstream>
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(endian_tests)

#define TODO_TESTS true
#define ENDIAN_DATA_INTEGER
#define ENDIAN_STREAM_INTEGER
#define ENDIAN_ARRAY_UINTX
#define ENDIAN_ITERATOR_INTEGER
#define ENDIAN_ROUND_TRIPS

#ifdef ENDIAN_DATA_INTEGER

// matched data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__data_to_integer__expected)
{
    uint32_t value = 0x01020304;
    data_chunk data{ 0x01, 0x02, 0x03, 0x04 };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint32_t>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__data_to_integer__expected)
{
    uint32_t value = 0x01020304;
    data_chunk data{ 0x04, 0x03, 0x02, 0x01 };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint32_t>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_data__expected)
{
    uint32_t value = 0x01020304;
    byte_array<4> data{ { 0x01, 0x02, 0x03, 0x04 } };
    BOOST_REQUIRE_EQUAL(to_big_endian(value), data);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_data__expected)
{
    uint32_t value = 0x01020304;
    byte_array<4> data{ { 0x04, 0x03, 0x02, 0x01 } };
    BOOST_REQUIRE_EQUAL(to_little_endian(value), data);
}

// short data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__data_to_integer_short__expected)
{
    uint64_t value = 0x0000000001020304;
    data_chunk data{ 0x01, 0x02, 0x03, 0x04 };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__data_to_integer_short__expected)
{
    uint64_t value = 0x0000000001020304;
    data_chunk data{ 0x04, 0x03, 0x02, 0x01 };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(data), value);
}

// long data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__data_to_integer_long__expected)
{
    uint16_t value = 0x0102;
    data_chunk data{ 0x01, 0x02, 0x03, 0x04 };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__data_to_integer_long__expected)
{
    uint16_t value = 0x0304;
    data_chunk data{ 0x04, 0x03, 0x02, 0x01 };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(data), value);
}

#endif // ENDIAN_DATA_INTEGER

#ifdef ENDIAN_STREAM_INTEGER

BOOST_AUTO_TEST_CASE(endian__from_big_endian__stream_to_integer__expected)
{
    uint64_t value = 0x0102030405060708;
    std::string data{ "\x01\x02\x03\x04\x05\x06\x07\x08" };
    std::stringstream stream;
    stream << data;
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(stream), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__stream_to_integer__expected)
{
    uint64_t value = 0x0102030405060708;
    std::string data{ "\x08\x07\x06\x05\x04\x03\x02\x01" };
    std::stringstream stream;
    stream << data;
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(stream), value);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_stream__expected)
{
    uint64_t value = 0x0102030405060708;
    std::string data{ "\x01\x02\x03\x04\x05\x06\x07\x08" };
    std::stringstream stream;
    to_big_endian(stream, value);
    BOOST_REQUIRE_EQUAL(stream.str(), data);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_stream__expected)
{
    uint64_t value = 0x0102030405060708;
    std::string data{ "\x08\x07\x06\x05\x04\x03\x02\x01" };
    std::stringstream stream;
    to_little_endian(stream, value);
    BOOST_REQUIRE_EQUAL(stream.str(), data);
}

#endif // ENDIAN_STREAM_INTEGER

#ifdef ENDIAN_ARRAY_UINTX

BOOST_AUTO_TEST_CASE(endian__from_big_endian__data_to_uintx__expected)
{
    uint256_t value = 0x01020304;
    data_chunk data{ 0x01, 0x02, 0x03, 0x04 };
    BOOST_REQUIRE_EQUAL(from_big_endian<256>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__data_to_uintx__expected)
{
    uint256_t value = 0x01020304;
    data_chunk data{ 0x04, 0x03, 0x02, 0x01 };
    BOOST_REQUIRE_EQUAL(from_little_endian<256>(data), value);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__uintx_to_data__expected)
{
    uint256_t value = 0x01020304;
    byte_array<4> data{ { 0x01, 0x02, 0x03, 0x04 } };
    BOOST_REQUIRE_EQUAL(to_big_endian<32>(value), data);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__uintx_to_data__expected)
{
    uint256_t value = 0x01020304;
    byte_array<32> data{ { 0x04, 0x03, 0x02, 0x01 } };
    BOOST_REQUIRE_EQUAL(to_little_endian<256>(value), data);
}

#endif // ENDIAN_ARRAY_UINTX

#ifdef ENDIAN_ROUND_TRIPS

// big endian

BOOST_AUTO_TEST_CASE(endian__round_trip__uint48_big_endian__expected)
{
    const mini_hash data = base16_array("010203040506");
    BOOST_REQUIRE_EQUAL(to_big_endian<48>(from_big_endian<48>(data)), data);
}

// This is uintx_t<64> not uint64_t!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_big_endian__expected)
{
    const quarter_hash data = base16_array("0102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian<64>(from_big_endian<64>(data)), data);
}

// This is uint64_t not uintx_t<64>!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_t_big_endian__expected)
{
    const quarter_hash data = base16_array("0102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian(from_big_endian<uint64_t>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint128_big_endian__expected)
{
    const half_hash data = base16_array("01020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian<128>(from_big_endian<128>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint160_big_endian__expected)
{
    const short_hash data = base16_array("0102030405060708010203040506070801020304");
    BOOST_REQUIRE_EQUAL(to_big_endian<160>(from_big_endian<160>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint256_big_endian__expected)
{
    const hash_digest data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian<256>(from_big_endian<256>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint512_big_endian__expected)
{
    const long_hash data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian<512>(from_big_endian<512>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint1024_big_endian__expected)
{
    // This is not a predefined uintx_t.
    const byte_array<128> data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_big_endian<1024>(from_big_endian<1024>(data)), data);
}

// little endian

BOOST_AUTO_TEST_CASE(endian__round_trip__uint48_little_endian__expected)
{
    const mini_hash data = base16_array("010203040506");
    BOOST_REQUIRE_EQUAL(to_little_endian<48>(from_little_endian<48>(data)), data);
}

// This is uintx_t<64> not uint64_t!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_little_endian__expected)
{
    const quarter_hash data = base16_array("0102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian<64>(from_little_endian<64>(data)), data);
}

// This is uint64_t not uintx_t<64>!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_t_little_endian__expected)
{
    const quarter_hash data = base16_array("0102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian(from_little_endian<uint64_t>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint128_little_endian__expected)
{
    const half_hash data = base16_array("01020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian<128>(from_little_endian<128>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint160_little_endian__expected)
{
    const short_hash data = base16_array("0102030405060708010203040506070801020304");
    BOOST_REQUIRE_EQUAL(to_little_endian<160>(from_little_endian<160>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint256_little_endian__expected)
{
    const hash_digest data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian<256>(from_little_endian<256>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint512_little_endian__expected)
{
    const long_hash data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian<512>(from_little_endian<512>(data)), data);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint1024_little_endian__expected)
{
    // This is not a predefined uintx_t.
    const byte_array<128> data = base16_array(
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708"
        "0102030405060708010203040506070801020304050607080102030405060708");
    BOOST_REQUIRE_EQUAL(to_little_endian<1024>(from_little_endian<1024>(data)), data);
}

#endif // ENDIAN_ROUND_TRIPS

#ifdef ENDIAN_ITERATOR_INTEGER

BOOST_AUTO_TEST_CASE(endian__from_big_endian_unsafe__iterator_to_integer__expected)
{
    auto a = '0';
    uint32_t value = 0x01020304;
    data_chunk data{ 0x01, 0x02, 0x03, 0x04 };
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(data.begin()), value);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian_unsafe__iterator_to_integer__expected)
{
    uint32_t value = 0x01020304;
    data_chunk data{ 0x04, 0x03, 0x02, 0x01 };
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(data.begin()), value);
}

#endif // ENDIAN_ITERATOR_INTEGER

#ifdef DISABLED

BOOST_AUTO_TEST_CASE(endian__from_big_endian__insufficient_data__stream_failure)
{
    const data_chunk data{ 0xff };
    data_source stream(data);
    BOOST_REQUIRE(!stream.eof());

    const auto value = from_big_endian<uint64_t>(stream);
    BOOST_REQUIRE(!stream);
    BOOST_REQUIRE(stream.eof());
    BOOST_REQUIRE(value != 0u);
}

BOOST_AUTO_TEST_CASE(endian__from_big_endian__eof__stream_partial_read)
{
    const uint8_t content = 0xff;
    const uint32_t expected = 0xff000000;
    std::stringstream stream;
    stream.put(content);
    BOOST_REQUIRE(!stream.eof());

    const auto value = from_big_endian<uint32_t>(stream);
    BOOST_REQUIRE_EQUAL(value, expected);
    BOOST_REQUIRE(stream.fail());
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(endian__from_big_endian__valid__expected)
{
    static const uint64_t expected = 0xfc65d1a2;
    std::stringstream stream;
    auto bytes = to_big_endian(expected);
    const auto action = [&stream](uint8_t& value) { stream.put(value); };
    std::for_each(bytes.begin(), bytes.end(), action);
    const auto value = from_big_endian<uint64_t>(stream);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__insufficient_data__stream_failure)
{
    static const data_chunk data{ 0xff };
    data_source stream(data);
    BOOST_REQUIRE(!stream.eof());

    const auto value = from_little_endian<uint64_t>(stream);
    BOOST_REQUIRE(!stream);
    BOOST_REQUIRE(stream.eof());
    BOOST_REQUIRE(value != 0u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__eof__stream_partial_read)
{
    static const uint8_t content = 0xff;
    const auto expected = 0x000000ff;
    std::stringstream stream;
    stream.put(content);
    BOOST_REQUIRE(!stream.eof());

    const auto value = from_little_endian<uint32_t>(stream);
    BOOST_REQUIRE_EQUAL(value, expected);
    BOOST_REQUIRE(stream.fail());
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__valid__expected)
{
    static const uint64_t expected = 0xfc65d1a2;
    std::stringstream stream;
    auto bytes = to_little_endian(expected);
    const auto action = [&stream](uint8_t& value) { stream.put(value); };
    std::for_each(bytes.begin(), bytes.end(), action);
    const auto value = from_little_endian<uint64_t>(stream);
    BOOST_REQUIRE(stream);
    BOOST_REQUIRE_EQUAL(value, expected);
}

// non-stream tests

BOOST_AUTO_TEST_CASE(endian__from_little_endian_unsafe__one_byte__expected)
{
    static const uint8_t expected = 0xff;
    static const auto bytes = data_chunk{ expected };
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint8_t>(bytes.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__from_big_endian_unsafe__one_byte__expected)
{
    static const uint8_t expected = 0xff;
    static const auto bytes = data_chunk{ expected };
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint8_t>(bytes.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip32__little_to_big__expected)
{
    static const uint32_t expected = 0x075bcd15;
    auto little_endian = to_little_endian<uint32_t>(expected);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(little_endian.begin()), expected);

    std::reverse(little_endian.begin(), little_endian.end());
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(little_endian.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip32__big_to_little__expected)
{
    static const uint32_t expected = 0x075bcd15;
    auto big_endian = to_big_endian<uint32_t>(expected);
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(big_endian.begin()), expected);

    std::reverse(big_endian.begin(), big_endian.end());
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(big_endian.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip32__big_to_big__expected)
{
    static const uint32_t expected = 0x075bcd15;
    const auto big_endian = to_big_endian<uint32_t>(expected);
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(big_endian.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip32__little_to_little__expected)
{
    static const uint32_t expected = 0x075bcd15;
    const auto little_endian = to_little_endian<uint32_t>(expected);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(little_endian.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip64__little_to_little__expected)
{
    static const uint64_t expected = 0x1122334455667788;
    const auto little_endian = to_little_endian<uint64_t>(expected);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint64_t>(little_endian.begin()), expected);
}

BOOST_AUTO_TEST_CASE(endian__round_trip64__big_to_big__expected)
{
    static const uint64_t expected = 0x1122334455667788;
    const auto big_endian = to_big_endian<uint64_t>(expected);
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint64_t>(big_endian.begin()), expected);
}

#endif // DISABLED

BOOST_AUTO_TEST_SUITE_END()
