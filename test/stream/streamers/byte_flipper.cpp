/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "../../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(stream_tests)

// Failed get/peek reads are populated with 0x00 by the reader.
constexpr uint8_t pad = 0x00;

// Exact copy of the byte_reader tests, replaced by flip::bits::iostream.
#define BYTE_FLIPPER_CONTEXT
#define BYTE_FLIPPER_BIG_ENDIAN
#define BYTE_FLIPPER_LITTLE_ENDIAN
#define BYTE_FLIPPER_BYTES
#define BYTE_FLIPPER_BYTES
#define BYTE_FLIPPER_STRINGS
#define BYTE_FLIPPER_READER
#define BYTE_FLIPPER_WRITER

#ifdef BYTE_FLIPPER_READER

#ifdef BYTE_FLIPPER_CONTEXT

// bool

BOOST_AUTO_TEST_CASE(byte_flipper__bool__stream_default__true)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__bool__not_empty__true)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__bool__stream_end__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader);
}

// !bool

BOOST_AUTO_TEST_CASE(byte_flipper__not_bool__stream_invalid__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    stream.get();
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(!reader);
}

// is_exhausted

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__default__true)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__not_empty__false)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(!reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__end__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__invalid__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    stream.get();
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__valid__does_not_change_state)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    const auto state = stream.rdstate();
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE_EQUAL(stream.rdstate(), state);
    BOOST_REQUIRE(stream);
}

BOOST_AUTO_TEST_CASE(byte_flipper__is_exhausted__invalid__does_not_change_state)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    stream.get();
    stream.get();
    const auto state = stream.rdstate();
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE_EQUAL(stream.rdstate(), state);
    BOOST_REQUIRE(!stream);
}

// invalidate

BOOST_AUTO_TEST_CASE(byte_flipper__invalidate__empty__exhausts_reader)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    reader.invalidate();
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__invalidate__empty__invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    reader.invalidate();
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__invalidate__not_empty__stream_invalid)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    reader.invalidate();
    BOOST_REQUIRE(!stream);
}

// skip

BOOST_AUTO_TEST_CASE(byte_flipper__skip__default_empty__invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__skip__default_single__exhausted)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__skip__end__exhausted)
{
    const auto size = 42;
    std::stringstream stream{ std::string(size, 0x00) };
    flip::bytes::iostream reader(stream);
    reader.skip(size);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(byte_flipper__skip__past_end__invalid)
{
    const auto size = 42;
    std::stringstream stream{ std::string(size, 0x00) };
    flip::bytes::iostream reader(stream);
    reader.skip(add1(size));
    BOOST_REQUIRE(!reader);
}

// rewind

BOOST_AUTO_TEST_CASE(byte_flipper__rewind__default_single__expected)
{
    std::stringstream stream{ "*" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), '*');
    BOOST_REQUIRE(reader.is_exhausted());
    reader.rewind();
    BOOST_REQUIRE(!reader.is_exhausted());
    BOOST_REQUIRE_EQUAL(reader.read_byte(), '*');
    BOOST_REQUIRE(reader.is_exhausted());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__rewind__skip_to_end_rewind_to_middle__expected)
{
    const auto size = 42;
    const std::string value(size, 0x00);
    std::stringstream stream{ value + "*" + value };
    flip::bytes::iostream reader(stream);
    reader.skip(size);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), '*');
    reader.skip(size);
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
    reader.rewind(add1(size));
    BOOST_REQUIRE_EQUAL(reader.read_byte(), '*');
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__rewind__past_begin__invalid)
{
    const auto size = 42;
    std::stringstream stream{ std::string(size, 0x00) };
    flip::bytes::iostream reader(stream);
    reader.skip(size);
    reader.rewind(add1(size));
    BOOST_REQUIRE(!reader);
}

#endif // BYTE_FLIPPER_CONTEXT

#ifdef BYTE_FLIPPER_BIG_ENDIAN

// read_big_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_big_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_big_endian<size_t>(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_big_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_big_endian<uint64_t>(), 0x0807060504030201);
    BOOST_REQUIRE(reader);
}

// read_2_bytes_big_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_2_bytes_big_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_2_bytes_big_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_2_bytes_big_endian__full__expected)
{
    const std::string value{ 0x08, 0x07 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_2_bytes_big_endian(), 0x0807);
    BOOST_REQUIRE(reader);
}

// read_4_bytes_big_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_4_bytes_big_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_big_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_4_bytes_big_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_big_endian(), 0x08070605);
    BOOST_REQUIRE(reader);
}

// read_8_bytes_big_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_8_bytes_big_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_big_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_8_bytes_big_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_big_endian(), 0x0807060504030201);
    BOOST_REQUIRE(reader);
}

#endif // BYTE_FLIPPER_BIG_ENDIAN

#ifdef BYTE_FLIPPER_LITTLE_ENDIAN

// read_little_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_little_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_little_endian<size_t>(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_little_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_little_endian<size_t>(), 0x0102030405060708);
    BOOST_REQUIRE(reader);
}

// read_2_bytes_little_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_2_bytes_little_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_2_bytes_little_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_2_bytes_little_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_2_bytes_little_endian(), 0x0708);
    BOOST_REQUIRE(reader);
}

// read_4_bytes_little_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_4_bytes_little_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_little_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_4_bytes_little_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_4_bytes_little_endian(), 0x05060708);
    BOOST_REQUIRE(reader);
}

// read_8_bytes_little_endian

BOOST_AUTO_TEST_CASE(byte_flipper__read_8_bytes_little_endian__empty__zero)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_little_endian(), 0u);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_8_bytes_little_endian__full__expected)
{
    const std::string value{ 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_little_endian(), 0x0102030405060708);
    BOOST_REQUIRE(reader);
}

// read_size/read_variable

BOOST_AUTO_TEST_CASE(byte_flipper__read_variable_size__one_byte__expected)
{
    const std::string value{ 0x08 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_size(), 0x08);
    reader.rewind(1);
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 0x08);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_variable_size__two_bytes__expected)
{
    const std::string value{ (char)varint_two_bytes, 0x08, 0x07 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_size(), 0x0708);
    reader.rewind(3);
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 0x0708);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_variable_size__four_bytes__expected)
{
    const std::string value{ (char)varint_four_bytes, 0x08, 0x07, 0x06, 0x05 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_size(), 0x05060708);
    reader.rewind(5);
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 0x05060708);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_variable_size__eight_bytes__expected)
{
    const std::string value{ (char)varint_eight_bytes, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 0x0102030405060708);
    reader.rewind(9);

    if (sizeof(size_t) < sizeof(uint64_t))
    {
        BOOST_REQUIRE_EQUAL(reader.read_size(), 0);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(reader.read_size(), 0x0102030405060708);
    }

    BOOST_REQUIRE(reader);
}


// read_error_code

BOOST_AUTO_TEST_CASE(byte_flipper__read_error_code__empty__default_invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_error_code(), error::success);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_error_code__value__expected)
{
    const char code = error::insufficient_work;
    const std::string value{ code, 0x00, 0x00, 0x00, '*' };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_error_code().value(), code);
    BOOST_REQUIRE(reader);
}

#endif // BYTE_FLIPPER_LITTLE_ENDIAN

#ifdef BYTE_FLIPPER_BYTES

// read_forward

BOOST_AUTO_TEST_CASE(byte_flipper__read_forward__empty__pad_invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_forward<1>()[0], pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_forward__not_empty__expected)
{
    const data_array<4> expected{ 0x01, 0x02, 0x03, 0x04 };
    std::stringstream stream{ "*" + to_string(expected) + "*" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE_EQUAL(reader.read_forward<4>(), expected);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_forward__past_end__padded_invalid)
{
    const data_array<4> expected{ 0x01, 0x02, pad, pad };
    std::stringstream stream{ to_string(expected).substr(0, 2) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_forward<4>(), expected);
    BOOST_REQUIRE(!reader);
}

// read_reverse

BOOST_AUTO_TEST_CASE(byte_flipper__read_reverse__empty__pad_invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_reverse<1>()[0], pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_reverse__not_empty__expected)
{
    const data_array<4> value{ 0x01, 0x02, 0x03, 0x04 };
    const data_array<4> expected{ 0x04, 0x03, 0x02, 0x01 };
    std::stringstream stream{ "*" + to_string(value) + "*" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE_EQUAL(reader.read_reverse<4>(), expected);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_reverse__past_end__padded_invalid)
{
    const data_array<2> value{ 0x01, 0x02 };
    const data_array<4> expected{ pad, pad, 0x02, 0x01 };
    std::stringstream stream{ to_string(value) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_reverse<4>(), expected);
    BOOST_REQUIRE(!reader);
}

// read_mini_hash

BOOST_AUTO_TEST_CASE(byte_flipper__read_mini_hash__empty__null_hash)
{
    std::stringstream stream{ to_string(null_hash) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_mini_hash(), null_mini_hash);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_mini_hash__value__expected)
{
    const mini_hash expected{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    std::stringstream stream{ to_string(expected) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_mini_hash(), expected);
    BOOST_REQUIRE(reader);
}

// read_short_hash

BOOST_AUTO_TEST_CASE(byte_flipper__read_short_hash__value__expected)
{
    const short_hash expected
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a
    };

    std::stringstream stream{ to_string(expected) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_short_hash(), expected);
    BOOST_REQUIRE(reader);
}

// read_hash

BOOST_AUTO_TEST_CASE(byte_flipper__read_hash__value__expected)
{
    const hash_digest expected
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02
    };

    std::stringstream stream{ to_string(expected) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_hash(), expected);
    BOOST_REQUIRE(reader);
}

// read_long_hash

BOOST_AUTO_TEST_CASE(byte_flipper__read_long_hash__value__expected)
{
    const long_hash expected
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x01, 0x02, 0x03, 0x04
    };

    std::stringstream stream{ to_string(expected) };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_long_hash(), expected);
    BOOST_REQUIRE(reader);
}

// peek_byte

BOOST_AUTO_TEST_CASE(byte_flipper__peek_byte__empty__pad_invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__peek_byte__end__pad_invalid)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.skip(3);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__peek_byte__not_empty__expected_not_advanced)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), 'a');
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), 'a');
    BOOST_REQUIRE_EQUAL(reader.peek_byte(), 'a');
    BOOST_REQUIRE(reader);
}

// read_byte

BOOST_AUTO_TEST_CASE(byte_flipper__read_byte__empty__pad_invalid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_byte__end__pad_invalid)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.skip(3);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), pad);
    BOOST_REQUIRE(!reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_byte__not_empty__expected_advanced)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 'a');
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 'b');
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 'c');
    BOOST_REQUIRE(reader);
}

// read_bytes0

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes0__empty__empty_valid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.read_bytes().empty());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes0__end__empty_valid)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.skip(3);
    BOOST_REQUIRE(reader.read_bytes().empty());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes0__not_empty__expected)
{
    std::stringstream stream{ "*abc" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE_EQUAL(reader.read_bytes(), to_chunk("abc"));
    BOOST_REQUIRE(reader);
}

// read_bytes1

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes1__empty_zero__empty_valid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.read_bytes(0).empty());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes1__end_zero__empty_valid)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.skip(3);
    BOOST_REQUIRE(reader.read_bytes(0).empty());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes1__to_end__expected)
{
    std::stringstream stream{ "*abc" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE_EQUAL(reader.read_bytes(3), to_chunk("abc"));
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes1__middle__expected)
{
    std::stringstream stream{ "*abc*" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    BOOST_REQUIRE_EQUAL(reader.read_bytes(3), to_chunk("abc"));
    BOOST_REQUIRE_EQUAL(stream.get(), '*');
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes1__past_end__expected_invalid)
{
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_bytes(6), to_chunk("abc\0\0\0"));
    BOOST_REQUIRE(!reader);
}

// read_bytes2

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes2__empty_zero__empty_valid)
{
    data_chunk sink;
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    reader.read_bytes(sink.data(), 0);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes2__end_zero__empty_valid)
{
    data_chunk sink;
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.skip(3);
    reader.read_bytes(sink.data(), 0);
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes2__to_end__expected)
{
    data_chunk sink(3, 0x00);
    std::stringstream stream{ "*abc" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    reader.read_bytes(sink.data(), sink.size());
    BOOST_REQUIRE_EQUAL(sink, to_chunk("abc"));
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes2__middle__expected)
{
    data_chunk sink(3, 0x00);
    std::stringstream stream{ "*abc*" };
    flip::bytes::iostream reader(stream);
    reader.skip();
    reader.read_bytes(sink.data(), sink.size());
    BOOST_REQUIRE_EQUAL(sink, to_chunk("abc"));
    BOOST_REQUIRE_EQUAL(stream.get(), '*');
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_bytes2__past_end__expected_invalid)
{
    data_chunk sink(6, 0x00);
    std::stringstream stream{ "abc" };
    flip::bytes::iostream reader(stream);
    reader.read_bytes(sink.data(), sink.size());
    BOOST_REQUIRE_EQUAL(sink, to_chunk("abc\0\0\0"));
    BOOST_REQUIRE(!reader);
}

#endif // BYTE_FLIPPER_BYTES

#ifdef BYTE_FLIPPER_STRINGS

// read_string0

BOOST_AUTO_TEST_CASE(byte_flipper__read_string0__one_byte__expected)
{
    const std::string value{ 0x03, 'a', 'b', 'c' };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(), "abc");
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_string0__two_bytes__expected)
{
    const std::string value{ (char)varint_two_bytes, 0x03, 0x00, 'a', 'b', 'c' };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(), "abc");
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_string0__four_bytes__expected)
{
    const std::string value{ (char)varint_four_bytes, 0x03, 0x00, 0x00, 0x00, 'a', 'b', 'c' };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(), "abc");
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_string0__eight_bytes__expected)
{
    const std::string value{ (char)varint_eight_bytes, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'a', 'b', 'c' };
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(), "abc");
}

// read_string1

BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__zero__empty_valid)
{
    std::stringstream stream;
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE(reader.read_string(0).empty());
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__partial__expected_valid)
{
    const std::string value{ "abcdefghij" };
    const auto length = to_half(value.length());
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(length), value.substr(0, length));
    BOOST_REQUIRE(reader);
}

BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__full__expected_valid)
{
    const std::string value{ "abcdefghij" };
    const auto length = value.length();
    std::stringstream stream{ value + "*" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(length), value);
    BOOST_REQUIRE_EQUAL(stream.get(), '*');
    BOOST_REQUIRE(reader);
}

// The full number of bytes are read, but not past end.
BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__past_end__truncated_valid)
{
    const std::string value{ "abcdefghij" };
    const auto length = value.length();
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(add1(length)), value);
    BOOST_REQUIRE(reader);
}

// The full number of bytes are read, and the string is terminated at null.
BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__full_embedded_null__truncated_exhausted)
{
    const std::string value{ "abcdef\0hij" };
    const auto length = value.length();
    std::stringstream stream{ value };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(length), "abcdef");
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

// The full number of bytes are read, and the string is terminated at null.
BOOST_AUTO_TEST_CASE(byte_flipper__read_string1__partial_embedded_null__truncated_not_exhausted)
{
    const std::string value{ "abcdef\0hij" };
    const auto length = value.length();
    std::stringstream stream{ value + "*" };
    flip::bytes::iostream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_string(length), "abcdef");
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(!reader.is_exhausted());
    BOOST_REQUIRE_EQUAL(stream.get(), '*');
}

#endif // BYTE_FLIPPER_STRINGS

#endif // BYTE_FLIPPER_READER

#ifdef BYTE_FLIPPER_WRITER

// TODO:

#endif // BYTE_FLIPPER_WRITER

BOOST_AUTO_TEST_SUITE_END()
