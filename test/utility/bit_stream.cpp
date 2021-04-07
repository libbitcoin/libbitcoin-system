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

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <bitcoin/system.hpp>
#include "../overloads.hpp"

// The bit reader/writer operates from high to low order bits.
// It reads and writes high bits first both in the operand and the buffer.
// The bit order is independent of the byte order (big vs. little endian).
// We refer to this as a "high endian" bit stream reader/writer.
// Both big and little byte orderings are supported by the bit reader/writer.
//
// It is often assumed that high order bits are stored on the "left" and low
// on the right, for example the "shift left" and shift right" operator names.
// Below the bit order is displayed high order left and low order right.
//
// The high order bits of 0xaa are written first (high endian):
// ttttf  0xa  0xa pad   <- bit and byte writes
// 11110 1010 1010 000   <- resulting bit buffer
//    0xf5       0x50    <- byte expectations
// [11110101] [01010000] <- resulting byte buffer
//  ttttf   0xaa    000  <- resulting (high endian) bit reader reads
//
// If the bit writer operated from low to high order bits (low endian):
// 010 ftttt pad  0xa 1  <- bit and byte writes
// 010 01111 00 01010 1  <- resulting bit buffer
//    0x4f       0x15    <- byte expectations
// [01001111] [00010101] <- resulting byte buffer
//  ttttf   0xaa    000  <- resulting (low endian) bit reader reads
//
// So when bytes are not aligned, the stream is affected. But when operating
// on byte boundaries, the choice does not affect the buffer.
//    0xaa    tftttftt
// [01010101][10111011]  <- resulting byte buffer in either implementation.
//
// The result is also affected when bit padding is necessary.
//    0xaa    tftttpad
// [01010101][10111000]  <- resulting byte buffer in high endian.
// [01010101][00010111]  <- resulting byte buffer in low endian.
//
// Round trip tests do not see a difference in buffer bit (or byte) ordering,
// so the test cases below test the expected (high endian) buffer as well.

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(bit_stream_tests)

// is_exhausted (bool())

BOOST_AUTO_TEST_CASE(istream_bit_reader__is_exhausted__empty_stream__valid_true)
{
    std::stringstream stream;
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(bit_reader.is_exhausted());
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(istream_bit_reader__is_exhausted__non_empty_stream__valid_false)
{
    std::stringstream stream("abc");
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(!bit_reader.is_exhausted());
    BOOST_REQUIRE(!reader.is_exhausted());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(istream_bit_reader__is_exhausted__read_past_end__invalid_true)
{
    std::stringstream stream;
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    bit_reader.read_bit();
    BOOST_REQUIRE(bit_reader.is_exhausted());
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE(!bit_reader);
    BOOST_REQUIRE(!reader);
    BOOST_REQUIRE(!stream);
}

BOOST_AUTO_TEST_CASE(istream_bit_reader__is_exhausted__read_bit_from_one_byte__reader_exhausted_false)
{
    std::stringstream stream("a");
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    bit_reader.read_bit();
    BOOST_REQUIRE(!bit_reader.is_exhausted());
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// peek_byte

BOOST_AUTO_TEST_CASE(istream_bit_reader__peek_byte__nonempty_stream__expected)
{
    const uint8_t expected = 'a';
    std::stringstream stream("ab");
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), expected);
    BOOST_REQUIRE_EQUAL(bit_reader.peek_byte(), expected);
    BOOST_REQUIRE(!bit_reader.is_exhausted());
    BOOST_REQUIRE(!reader.is_exhausted());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(istream_bit_reader__peek_byte__nonempty_stream__does_not_advance)
{
    std::stringstream stream("a");
    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    reader.peek_byte();
    reader.peek_byte();
    bit_reader.peek_byte();
    bit_reader.peek_byte();
    BOOST_REQUIRE(!bit_reader.is_exhausted());
    BOOST_REQUIRE(!reader.is_exhausted());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_bit (read_bit)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_bit__padded__round_trip)
{
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_bit(false);
    bit_writer.write_bit(false);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_bits (read_bits)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_bits__bits__expected_round_trip)
{
    const uint8_t bits = 32;
    const uint64_t expected = 0x000000aaaaaaaa;
    const data_chunk expected_buffer{ 0xaa, 0xaa, 0xaa, 0xaa };
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bits(expected, bits);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    BOOST_REQUIRE_EQUAL(expected_buffer, buffer);

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_bits(bits), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_bits__not_byte_aligned__expected_round_trip)
{
    const uint8_t bits = 32;
    const uint64_t expected = 0x000000aaaaaaaa;
    const data_chunk expected_buffer{ 0x55, 0x55, 0x55, 0x55, 0x00 };
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bit(false);
    bit_writer.write_bits(expected, bits);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    BOOST_REQUIRE_EQUAL(expected_buffer, buffer);

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE_EQUAL(bit_reader.read_bits(bits), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

// write_byte (read_byte)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_byte__byte_aligned__round_trip)
{
    const uint8_t expected = 0xaa;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_byte(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_byte(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_byte__not_byte_aligned__expected_round_trip)
{
    const data_chunk expected_buffer{ 0xf5, 0x50 };
    const uint8_t expected_numeric = 0xaa;
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_byte(expected_numeric);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    BOOST_REQUIRE_EQUAL(buffer, expected_buffer);

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE_EQUAL(bit_reader.read_byte(), expected_numeric);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

// write_bytes (read_bytes(n))

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_bytes__not_byte_aligned__expected_round_trip)
{
    const uint8_t first_byte = 0xdb;
    const data_chunk expected_buffer{ first_byte, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x00 };
    const data_chunk expected_read{ 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_byte(first_byte);
    bit_writer.write_bit(false);
    bit_writer.write_bytes(expected_read);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    BOOST_REQUIRE_EQUAL(buffer, expected_buffer);

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_byte(), first_byte);
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE_EQUAL(bit_reader.read_bytes(expected_read.size()), expected_read);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_bytes__stringstream__expected_round_trip)
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

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bytes(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_bytes(expected.size()), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_bytes (read_bytes())

BOOST_AUTO_TEST_CASE(ostream_bit_writer__read_bytes__all__expected_length_round_trip)
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

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bytes(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_bytes(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_X_bytes_little_endian (read_X_bytes_little_endian)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_2_bytes_little_endian__byte_aligned__round_trip)
{
    const uint16_t expected = 43707;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_2_bytes_little_endian(expected);

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_2_bytes_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_4_bytes_little_endian__byte_aligned__round_trip)
{
    const uint32_t expected = 2898120443;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_4_bytes_little_endian(expected);

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_4_bytes_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_8_bytes_little_endian__byte_aligned__round_trip)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_8_bytes_little_endian(expected);

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_8_bytes_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_8_bytes_little_endian__not_byte_aligned__round_trip)
{
    const uint64_t expected = 0x0f1e2d3c4b5b6a79;
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_8_bytes_little_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE_EQUAL(bit_reader.read_8_bytes_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

// write_X_bytes_big_endian (read_X_bytes_big_endian)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_2_bytes_big_endian__byte_aligned__round_trip)
{
    const uint16_t expected = 43707;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_2_bytes_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_2_bytes_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_4_bytes_big_endian__byte_aligned__round_trip)
{
    const uint32_t expected = 2898120443;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_4_bytes_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_4_bytes_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_8_bytes_big_endian__byte_aligned__round_trip)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_8_bytes_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_8_bytes_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_8_bytes_big_endian__not_byte_aligned__round_trip)
{
    const uint64_t expected = 0x0f1e2d3c4b5b6a79;
    data_chunk buffer;

    data_sink sink(buffer);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(true);
    bit_writer.write_bit(false);
    bit_writer.write_8_bytes_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(sink);
    bit_writer.flush();
    sink.flush();

    data_source source(buffer);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(bit_reader.read_bit());
    BOOST_REQUIRE(!bit_reader.read_bit());
    BOOST_REQUIRE_EQUAL(bit_reader.read_8_bytes_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(source);
}

// write_variable_little_endian (read_variable_little_endian)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_little_endian__1_byte__round_trip)
{
    const uint64_t expected = 0xaa;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_little_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_little_endian__2_bytes__round_trip)
{
    const uint64_t expected = 43707;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_little_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint16_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_little_endian__4_bytes__round_trip)
{
    const uint64_t expected = 2898120443;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_little_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint32_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_little_endian__8_bytes__round_trip)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_little_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint64_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_little_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_variable_big_endian (read_variable_big_endian)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_big_endian__1_byte__round_trip)
{
    const uint64_t expected = 0xaa;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_big_endian__2_bytes__round_trip)
{
    const uint64_t expected = 43707;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint16_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_big_endian__4_bytes__round_trip)
{
    const uint64_t expected = 2898120443;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint32_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_variable_big_endian__8_bytes__round_trip)
{
    const uint64_t expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_variable_big_endian(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), sizeof(uint64_t) + sizeof(uint8_t));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_variable_big_endian(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_X_hash (read_X_hash)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_hash__value__expected_round_trip)
{
    const hash_digest expected
    {
        {
            0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2,
            0xe4, 0xba, 0x65, 0xa8, 0x36, 0x1f, 0xea, 0x05,
            0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
            0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
        }
    };

    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_hash(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_hash(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_short_hash__value__expected_round_trip)
{
    const short_hash expected
    {
        {
            0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79,
            0x90, 0xab, 0x62, 0x04, 0xb5, 0x2c, 0x6a, 0x40,
            0xdc, 0x6d, 0xa5, 0xfe
        }
    };

    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_short_hash(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_short_hash(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_mini_hash__value__expected_round_trip)
{
    const mini_hash expected
    {
        {
            0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2
        }
    };

    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_mini_hash(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), expected.size());

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_mini_hash(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_string (read_string)

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_string__default_length__expected_length_round_trip)
{
    const std::string expected = "my string data";
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_string(expected);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), (expected.length() + message::variable_uint_size(expected.length())));

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_string(), expected);
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

// write_string(n) (read_string(n))

BOOST_AUTO_TEST_CASE(ostream_bit_writer__write_string__explicit_length__expected_round_trip)
{
    const std::string expected = "my string data";
    std::stringstream stream;

    ostream_writer writer(stream);
    ostream_bit_writer bit_writer(writer);
    bit_writer.write_string(expected, 10);
    BOOST_REQUIRE(bit_writer);
    BOOST_REQUIRE(writer);
    BOOST_REQUIRE(stream);
    bit_writer.flush();
    stream.flush();

    BOOST_REQUIRE_EQUAL(stream.str().length(), 10u);

    istream_reader reader(stream);
    istream_bit_reader bit_reader(reader);
    BOOST_REQUIRE_EQUAL(bit_reader.read_string(10), expected.substr(0, 10));
    BOOST_REQUIRE(bit_reader);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_SUITE_END()
