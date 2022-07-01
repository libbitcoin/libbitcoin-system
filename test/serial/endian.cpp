/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
/// DELETECSTDINT
#include <sstream>
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(endian_tests)

#define ENDIAN_BYTE
#define ENDIAN_NEGATIVE
#define ENDIAN_CHUNK
#define ENDIAN_ARRAY
#define ENDIAN_VARIABLE_SIZE
#define ENDIAN_FIXED_SIZE
#define ENDIAN_STREAM_INTEGER
#define ENDIAN_ARRAY_UINTX
#define ENDIAN_ITERATOR_INTEGER
#define ENDIAN_SYMMETRIC_ROUND_TRIPS
#define ENDIAN_ASYMMETRIC_ROUND_TRIPS
#define ENDIAN_PARALLEL

#ifdef ENDIAN_BYTE

// single byte (specialized)

BOOST_AUTO_TEST_CASE(from_big_endian__single_byte__chunk__expected)
{
    const data_chunk data{ 0x42, 0xff };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint8_t>(data), data[0]);
}

BOOST_AUTO_TEST_CASE(from_little_endian__single_byte__chunk___expected)
{
    const data_chunk data{ 0x42, 0xff };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint8_t>(data), data[0]);
}

BOOST_AUTO_TEST_CASE(to_big_endian__single_byte__array__expected)
{
    const data_array<1> data{ { 0x42 } };
    BOOST_REQUIRE_EQUAL(to_big_endian(data[0]), data);
}

BOOST_AUTO_TEST_CASE(to_little_endian__single_byte__array__expected)
{
    const data_array<1> data{ { 0x42 } };
    BOOST_REQUIRE_EQUAL(to_little_endian(data[0]), data);
}

#endif // ENDIAN_BYTE

#ifdef ENDIAN_NEGATIVE

// negatives

BOOST_AUTO_TEST_CASE(to_big_endian__negative__full__expected)
{
    const data_array<mini_hash_size> full_mini_hash{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_big_endian<mini_hash_size>(-1), full_mini_hash);
}

BOOST_AUTO_TEST_CASE(to_big_endian__negative__partial__expected)
{
    const data_array<mini_hash_size> half_mini_big{ { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 } };
    BOOST_REQUIRE_EQUAL(to_big_endian<mini_hash_size>(mask_right(-1, 24)), half_mini_big);
}

BOOST_AUTO_TEST_CASE(to_little_endian__negative__full__expected)
{
    const data_array<mini_hash_size> full_mini_hash{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_little_endian<mini_hash_size>(-1), full_mini_hash);
}

BOOST_AUTO_TEST_CASE(to_little_endian__negative__partial__expected)
{
    const data_array<mini_hash_size> half_mini_little{ { 0x00, 0x00, 0x00, 0xff, 0xff, 0xff } };
    BOOST_REQUIRE_EQUAL(to_little_endian<mini_hash_size>(mask_right(-1, 24)), half_mini_little);
}

#endif // ENDIAN_NEGATIVE

#ifdef ENDIAN_CHUNK

const auto chunk8 = base16_chunk("0102030405060708");

BOOST_AUTO_TEST_CASE(endian__to_endian_chunk_size__uint64_t__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian_size(from_little_endian<uint64_t>(chunk8)), reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(to_little_endian_size(from_big_endian<uint64_t>(chunk8)), reverse_copy(chunk8));
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian_size__excess__expected_capacity)
{
    constexpr auto excess = 42u;
    const auto chunk = to_big_endian_size(from_little_endian<uint64_t>(chunk8), excess);
    BOOST_REQUIRE_EQUAL(chunk, reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(chunk.size() + excess, chunk.capacity());
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian_size__excess__expected_capacity)
{
    constexpr auto excess = 42u;
    const auto chunk = to_little_endian_size(from_big_endian<uint64_t>(chunk8), excess);
    BOOST_REQUIRE_EQUAL(chunk, reverse_copy(chunk8));
    BOOST_REQUIRE_EQUAL(chunk.size() + excess, chunk.capacity());
}


#endif // ENDIAN_CHUNK

#ifdef ENDIAN_ARRAY

const auto array8 = base16_array("0102030405060708");

BOOST_AUTO_TEST_CASE(endian__to_endian_array_size__uint64_t__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian_size<8>(from_little_endian<uint64_t>(array8)), reverse_copy(array8));
    BOOST_REQUIRE_EQUAL(to_little_endian_size<8>(from_big_endian<uint64_t>(array8)), reverse_copy(array8));
}

#endif // ENDIAN_ARRAY

#ifdef ENDIAN_VARIABLE_SIZE

constexpr uint32_t value32 = 0x01020304;
const data_chunk data_forward{ 0x01, 0x02, 0x03, 0x04 };
const data_chunk data_reverse{ 0x04, 0x03, 0x02, 0x01 };

// matched data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__chunk_to_integer__expected)
{
    BOOST_REQUIRE(from_big_endian<uintx>(data_forward) == value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__chunk_to_integer__expected)
{
    BOOST_REQUIRE(from_little_endian<uintx>(data_reverse) == value32);
}

// These are now restricted by bit_width in a type constraint.

////BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_chunk__expected)
////{
////    BOOST_REQUIRE_EQUAL(to_big_endian<uintx>(value32), data_forward);
////}

////BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_chunk__expected)
////{
////    BOOST_REQUIRE_EQUAL(to_little_endian<uintx>(value32), data_reverse);
////}

// short data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__chunk_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(data_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__chunk_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(data_reverse), value32);
}

// long data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__chunk_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(data_forward), value32 >> 16u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__chunk_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(data_reverse), value32 & 0x0000ffffu);
}

#endif // ENDIAN_VARIABLE_SIZE

#ifdef ENDIAN_FIXED_SIZE

const data_array<4> array_forward{ { 0x01, 0x02, 0x03, 0x04 } };
const data_array<4> array_reverse{ { 0x04, 0x03, 0x02, 0x01 } };

// matched data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_big_endian<uint32_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_little_endian<uint32_t>(array_reverse), value32);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_array__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian(value32), array_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_array__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian(value32), array_reverse);
}

// short data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(array_forward), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_short__expected)
{
    // 4 bytes of data for an 8 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(array_reverse), value32);
}

// long data

BOOST_AUTO_TEST_CASE(endian__from_big_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_big_endian<uint16_t>(array_forward), value32 >> 16u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__array_to_integer_long__expected)
{
    // 4 bytes of data for an 2 byte integer.
    BOOST_REQUIRE_EQUAL(from_little_endian<uint16_t>(array_reverse), value32 & 0x0000ffffu);
}

#endif // ENDIAN_FIXED_SIZE

#ifdef ENDIAN_STREAM_INTEGER

const auto alpha_forward = "BITCOIN!";
const auto alpha_reverse = "!NIOCTIB";
constexpr uint64_t value_alpha64 = 0x424954434f494e21;

BOOST_AUTO_TEST_CASE(endian__from_big_endian__stream_to_integer__expected)
{
    std::stringstream stream{ alpha_forward };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(stream), value_alpha64);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__stream_to_integer__expected)
{
    std::stringstream stream{ alpha_reverse };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(stream), value_alpha64);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_stream__expected)
{
    std::stringstream stream;
    to_big_endian(stream, value_alpha64);
    BOOST_REQUIRE_EQUAL(stream.str(), alpha_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_stream__expected)
{
    std::stringstream stream;
    to_little_endian(stream, value_alpha64);
    BOOST_REQUIRE_EQUAL(stream.str(), alpha_reverse);
}

#endif // ENDIAN_STREAM_INTEGER

#ifdef ENDIAN_ARRAY_UINTX

BOOST_AUTO_TEST_CASE(endian__from_big_endian__data_to_uintx__expected)
{
    // Populating uintx8
    BOOST_REQUIRE_EQUAL(uintx_from_big_endian_chunk<1>(data_forward), value32 >> 24u);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__data_to_uintx__expected)
{
    // Populating uintx800
    BOOST_REQUIRE_EQUAL(uintx_from_little_endian_chunk<100>(data_reverse), value32);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__uintx_to_data__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<4>(value32), array_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__uintx_to_data__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<4>(value32), array_reverse);
}

#endif // ENDIAN_ARRAY_UINTX

#ifdef ENDIAN_SYMMETRIC_ROUND_TRIPS

const mini_hash bytes6 = base16_array(
    "010203040506");
const quarter_hash bytes8 = base16_array(
    "0102030405060708");
const half_hash bytes16 = base16_array(
    "01020304050607080102030405060708");
const short_hash bytes20 = base16_array(
    "0102030405060708010203040506070801020304");
const hash_digest bytes32 = base16_array(
    "0102030405060708010203040506070801020304050607080102030405060708");
const long_hash bytes64 = base16_array(
    "0102030405060708010203040506070801020304050607080102030405060708"
    "0102030405060708010203040506070801020304050607080102030405060708");

// This is not a predefined hash size or uintx_t type.
const data_array<128> bytes128 = base16_array(
    "0102030405060708010203040506070801020304050607080102030405060708"
    "0102030405060708010203040506070801020304050607080102030405060708"
    "0102030405060708010203040506070801020304050607080102030405060708"
    "0102030405060708010203040506070801020304050607080102030405060708");

// big endian

BOOST_AUTO_TEST_CASE(endian__round_trip__uint48_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<6>(uintx_from_big_endian_array<6>(bytes6)), bytes6);
}

// This is uintx64 not uint64_t!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<8>(uintx_from_big_endian_array<8>(bytes8)), bytes8);
}

// This is uint64_t not uintx64!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_t_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian(from_big_endian<uint64_t>(bytes8)), bytes8);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint128_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<16>(uintx_from_big_endian_array<16>(bytes16)), bytes16);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint160_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<20>(uintx_from_big_endian_array<20>(bytes20)), bytes20);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint256_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<32>(uintx_from_big_endian_array<32>(bytes32)), bytes32);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint512_big_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<64>(uintx_from_big_endian_array<64>(bytes64)), bytes64);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint1024_big_endian__expected)
{
    // Reading more bytes than provided.
    BOOST_REQUIRE_EQUAL(to_big_endian<128>(uintx_from_big_endian_array<128>(bytes128)), bytes128);
}

// little endian

BOOST_AUTO_TEST_CASE(endian__round_trip__uint48_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<6>(uintx_from_little_endian_array<6>(bytes6)), bytes6);
}

// This is uintx64 not uint64_t!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<8>(uintx_from_little_endian_array<8>(bytes8)), bytes8);
}

// This is uint64_t not uintx64!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_t_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian(from_little_endian<uint64_t>(bytes8)), bytes8);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint128_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<16>(uintx_from_little_endian_array<16>(bytes16)), bytes16);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint160_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<20>(uintx_from_little_endian_array<20>(bytes20)), bytes20);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint256_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<32>(uintx_from_little_endian_array<32>(bytes32)), bytes32);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint512_little_endian__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_endian<64>(uintx_from_little_endian_array<64>(bytes64)), bytes64);
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint1024_little_endian__expected)
{
    // Reading more bytes than provided.
    BOOST_REQUIRE_EQUAL(to_little_endian<128>(uintx_from_little_endian_array<128>(bytes128)), bytes128);
}

#endif // ENDIAN_SYMMETRIC_ROUND_TRIPS

#ifdef ENDIAN_ASYMMETRIC_ROUND_TRIPS

// big endian

BOOST_AUTO_TEST_CASE(endian__round_trip__uint48__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<6>(uintx_from_little_endian_array<6>(bytes6)), reverse_copy(bytes6));
    BOOST_REQUIRE_EQUAL(to_little_endian<6>(uintx_from_big_endian_array<6>(bytes6)), reverse_copy(bytes6));
}

// This is uintx64 not uint64_t!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<8>(uintx_from_little_endian_array<8>(bytes8)), reverse_copy(bytes8));
    BOOST_REQUIRE_EQUAL(to_little_endian<8>(uintx_from_big_endian_array<8>(bytes8)), reverse_copy(bytes8));
}

// This is uint64_t not uintx64!
BOOST_AUTO_TEST_CASE(endian__round_trip__uint64_t__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian(from_little_endian<uint64_t>(bytes8)), reverse_copy(bytes8));
    BOOST_REQUIRE_EQUAL(to_little_endian(from_big_endian<uint64_t>(bytes8)), reverse_copy(bytes8));
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint128__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<16>(uintx_from_little_endian_array<16>(bytes16)), reverse_copy(bytes16));
    BOOST_REQUIRE_EQUAL(to_little_endian<16>(uintx_from_big_endian_array<16>(bytes16)), reverse_copy(bytes16));
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint160__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<20>(uintx_from_little_endian_array<20>(bytes20)), reverse_copy(bytes20));
    BOOST_REQUIRE_EQUAL(to_little_endian<20>(uintx_from_big_endian_array<20>(bytes20)), reverse_copy(bytes20));
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint256__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<32>(uintx_from_little_endian_array<32>(bytes32)), reverse_copy(bytes32));
    BOOST_REQUIRE_EQUAL(to_little_endian<32>(uintx_from_big_endian_array<32>(bytes32)), reverse_copy(bytes32));
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint512__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_endian<64>(uintx_from_little_endian_array<64>(bytes64)), reverse_copy(bytes64));
    BOOST_REQUIRE_EQUAL(to_little_endian<64>(uintx_from_big_endian_array<64>(bytes64)), reverse_copy(bytes64));
}

BOOST_AUTO_TEST_CASE(endian__round_trip__uint1024__expected)
{
    // Reading more bytes than provided.
    BOOST_REQUIRE_EQUAL(to_big_endian<128>(uintx_from_little_endian_array<128>(bytes128)), reverse_copy(bytes128));
    BOOST_REQUIRE_EQUAL(to_little_endian<128>(uintx_from_big_endian_array<128>(bytes128)), reverse_copy(bytes128));
}

#endif // ENDIAN_ASYMMETRIC_ROUND_TRIPS

#ifdef ENDIAN_ITERATOR_INTEGER

BOOST_AUTO_TEST_CASE(endian__from_big_endian_unchecked__iterator_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_big_endian_unchecked<uint32_t>(data_forward.begin()), value32);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian_unchecked__iterator_to_integer__expected)
{
    BOOST_REQUIRE_EQUAL(from_little_endian_unchecked<uint32_t>(data_reverse.begin()), value32);
}

#endif // ENDIAN_ITERATOR_INTEGER

#ifdef ENDIAN_PARALLEL

// parallel conversions of integer arrays

BOOST_AUTO_TEST_CASE(from_big_endian__parallel__always__expected)
{
    uint32_t to[4];
    const uint8_t from[16] =
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x00
    };

    from_big_endian<4>(to, from);
    BOOST_REQUIRE_EQUAL(to[0], 0x01020304u);
    BOOST_REQUIRE_EQUAL(to[1], 0x05060708u);
    BOOST_REQUIRE_EQUAL(to[2], 0x090a0b0cu);
    BOOST_REQUIRE_EQUAL(to[3], 0x0d0e0f00u);
}

BOOST_AUTO_TEST_CASE(from_little_endian__parallel__always__expected)
{
    uint32_t to[4];
    const uint8_t from[16] =
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x00
    };

    from_little_endian<4>(to, from);
    BOOST_REQUIRE_EQUAL(to[0], 0x04030201u);
    BOOST_REQUIRE_EQUAL(to[1], 0x08070605u);
    BOOST_REQUIRE_EQUAL(to[2], 0x0c0b0a09u);
    BOOST_REQUIRE_EQUAL(to[3], 0x000f0e0du);
}

BOOST_AUTO_TEST_CASE(to_big_endian__parallel__always__expected)
{
    uint8_t to[16];
    const uint32_t from[4] =
    {
        0x01020304,
        0x05060708,
        0x090a0b0c,
        0x0d0e0f00
    };

    to_big_endian<4>(to, from);

    BOOST_REQUIRE_EQUAL(to[0], 0x01u);
    BOOST_REQUIRE_EQUAL(to[1], 0x02u);
    BOOST_REQUIRE_EQUAL(to[2], 0x03u);
    BOOST_REQUIRE_EQUAL(to[3], 0x04u);

    BOOST_REQUIRE_EQUAL(to[4], 0x05u);
    BOOST_REQUIRE_EQUAL(to[5], 0x06u);
    BOOST_REQUIRE_EQUAL(to[6], 0x07u);
    BOOST_REQUIRE_EQUAL(to[7], 0x08u);

    BOOST_REQUIRE_EQUAL(to[8], 0x09u);
    BOOST_REQUIRE_EQUAL(to[9], 0x0au);
    BOOST_REQUIRE_EQUAL(to[10], 0x0bu);
    BOOST_REQUIRE_EQUAL(to[11], 0x0cu);

    BOOST_REQUIRE_EQUAL(to[12], 0x0du);
    BOOST_REQUIRE_EQUAL(to[13], 0x0eu);
    BOOST_REQUIRE_EQUAL(to[14], 0x0fu);
    BOOST_REQUIRE_EQUAL(to[15], 0x00u);
}

BOOST_AUTO_TEST_CASE(to_little_endian__parallel__always__expected)
{
    uint8_t to[16];
    const uint32_t from[4] =
    {
        0x01020304,
        0x05060708,
        0x090a0b0c,
        0x0d0e0f00
    };

    to_little_endian<4>(to, from);

    BOOST_REQUIRE_EQUAL(to[0], 0x04u);
    BOOST_REQUIRE_EQUAL(to[1], 0x03u);
    BOOST_REQUIRE_EQUAL(to[2], 0x02u);
    BOOST_REQUIRE_EQUAL(to[3], 0x01u);

    BOOST_REQUIRE_EQUAL(to[4], 0x08u);
    BOOST_REQUIRE_EQUAL(to[5], 0x07u);
    BOOST_REQUIRE_EQUAL(to[6], 0x06u);
    BOOST_REQUIRE_EQUAL(to[7], 0x05u);

    BOOST_REQUIRE_EQUAL(to[8], 0x0cu);
    BOOST_REQUIRE_EQUAL(to[9], 0x0bu);
    BOOST_REQUIRE_EQUAL(to[10], 0x0au);
    BOOST_REQUIRE_EQUAL(to[11], 0x09u);

    BOOST_REQUIRE_EQUAL(to[12], 0x00u);
    BOOST_REQUIRE_EQUAL(to[13], 0x0fu);
    BOOST_REQUIRE_EQUAL(to[14], 0x0eu);
    BOOST_REQUIRE_EQUAL(to[15], 0x0du);
}

#endif // ENDIAN_BYTE

BOOST_AUTO_TEST_SUITE_END()
