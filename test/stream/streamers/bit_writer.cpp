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
#include "../../test.hpp"
#include <sstream>

BC_AUTO_TEST_SUITE(stream_tests)

#define BIT_WRITER_CONTEXT
#define BIT_WRITER_BIG_ENDIAN
#define BIT_WRITER_LITTLE_ENDIAN
#define BIT_WRITER_BYTES
#define BIT_WRITER_STRINGS

#ifdef BIT_WRITER_CONTEXT

// bool

BC_AUTO_TEST_CASE(bit_writer__bool__stream_default__true)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__bool__not_empty__true)
{
    std::ostringstream stream{ "*" };
    write::bits::ostream writer(stream);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__bool__stream_end__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    write::bits::ostream writer(stream);
    BC_REQUIRE(writer);
}

// !bool

BC_AUTO_TEST_CASE(bit_writer__not_bool__stream_invalid__true)
{
    std::stringstream stream{ "*" };
    stream.get();
    stream.get();
    write::bits::ostream writer(stream);
    BC_REQUIRE(!writer);
}

// get_write_position

BC_AUTO_TEST_CASE(bit_writer__get_write_position___stream_end__expected)
{
    std::stringstream stream{};
    write::bits::ostream writer(stream);
    BC_REQUIRE_EQUAL(writer.get_write_position(), 0u);
    writer.write_byte('*');
    BC_REQUIRE_EQUAL(writer.get_write_position(), 1u);
    writer.write_byte('*');
    BC_REQUIRE_EQUAL(writer.get_write_position(), 2u);
    BC_REQUIRE(writer);
}

// flush

BC_AUTO_TEST_CASE(bit_writer__flush__push_sink_empty__valid)
{
    data_chunk sink;
    write::bits::data writer(sink);
    writer.flush();
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__flush__empty_sstream__valid)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.flush();
    BC_REQUIRE(writer);
}

// Test with our push_sink, since sstream always flushes on read.
BC_AUTO_TEST_CASE(bit_writer__flush__push_sink_not_empty__expected)
{
    data_chunk sink;
    write::bits::data writer(sink);
    writer.write_byte('*');
    BC_REQUIRE(sink.empty());
    writer.flush();
    BC_REQUIRE(!sink.empty());
    BC_REQUIRE_EQUAL(sink[0], '*');
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__flush__not_empty_sstream__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_byte('*');
    writer.flush();
    BC_REQUIRE_EQUAL(stream.str(), "*");
    BC_REQUIRE(writer);
}

#endif // BIT_WRITER_CONTEXT

#ifdef BIT_WRITER_BIG_ENDIAN

// write_big_endian

BC_AUTO_TEST_CASE(bit_writer__write_big_endian__uint8_t__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_big_endian<uint8_t>(0x42);
    const std::string expected{ 0x42 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_big_endian__size_t__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_big_endian<size_t>(0x42);
    std::string expected(sizeof(size_t), 0x00);
    expected.back() = 0x42;
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_2_bytes_big_endian

BC_AUTO_TEST_CASE(bit_writer__write_2_bytes_big_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_2_bytes_big_endian(0x42);
    const std::string expected{ 0x00, 0x42 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_4_bytes_big_endian

BC_AUTO_TEST_CASE(bit_writer__write_4_bytes_big_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_4_bytes_big_endian(0x42);
    const std::string expected{ 0x00, 0x00, 0x00, 0x42 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_8_bytes_big_endian

BC_AUTO_TEST_CASE(bit_writer__write_8_bytes_big_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_8_bytes_big_endian(0x42);
    const std::string expected{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

#endif // BIT_WRITER_BIG_ENDIAN

#ifdef BIT_WRITER_LITTLE_ENDIAN

// write_little_endian

BC_AUTO_TEST_CASE(bit_writer__write_little_endian__uint8_t__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_little_endian<uint8_t>(0x42);
    const std::string expected{ 0x42 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_little_endian__size_t__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_little_endian<size_t>(0x42);
    std::string expected(sizeof(size_t), 0x00);
    expected.front() = 0x42;
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_2_bytes_little_endian

BC_AUTO_TEST_CASE(bit_writer__write_2_bytes_little_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_2_bytes_little_endian(0x42);
    const std::string expected{ 0x42, 0x00 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_4_bytes_little_endian

BC_AUTO_TEST_CASE(bit_writer__write_4_bytes_little_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_4_bytes_little_endian(0x42);
    const std::string expected{ 0x42, 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_8_bytes_little_endian

BC_AUTO_TEST_CASE(bit_writer__write_8_bytes_little_endian__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_8_bytes_little_endian(0x42);
    const std::string expected{ 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_variable

BC_AUTO_TEST_CASE(bit_writer__write_variable__one_byte__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_variable(0x08);
    const std::string expected{ 0x08 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_variable__two_bytes__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_variable(0x0708);
    const std::string expected{ (char)varint_two_bytes, 0x08, 0x07 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_variable__four_bytes__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_variable(0x05060708);
    const std::string expected{ (char)varint_four_bytes, 0x08, 0x07, 0x06, 0x05 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_variable__eight_bytes__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_variable(0x0102030405060708);
    const std::string expected{ (char)varint_eight_bytes, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_error_code

BC_AUTO_TEST_CASE(bit_writer__write_error_code__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_error_code(error::double_spend);
    const std::string expected{ (char)error::double_spend, 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

#endif // BIT_WRITER_LITTLE_ENDIAN

#ifdef BIT_WRITER_BYTES

// write

BC_AUTO_TEST_CASE(bit_writer__write__empty__empty)
{
    std::istringstream in;
    std::stringstream out;
    write::bits::ostream writer(out);
    BC_REQUIRE(writer.write(in));
    BC_REQUIRE(out.str().empty());
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write__value__expected)
{
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    std::istringstream in{ expected };
    std::stringstream out;
    write::bits::ostream writer(out);
    BC_REQUIRE(writer.write(in));
    BC_REQUIRE_EQUAL(out.str(), expected);
    BC_REQUIRE(writer);
}

// write_byte

BC_AUTO_TEST_CASE(bit_writer__write_byte__always__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_byte(0x42);
    BC_REQUIRE_EQUAL(stream.str().front(), 0x42);
    BC_REQUIRE(writer);
}

// write_bytes1

BC_AUTO_TEST_CASE(bit_writer__write_bytes1__empty__empty)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_bytes({});
    BC_REQUIRE(stream.str().empty());
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_bytes1__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_bytes(expected);
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_bytes2

BC_AUTO_TEST_CASE(bit_writer__write_bytes2__empty__empty)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    data_chunk data;
    writer.write_bytes(data.data(), data.size());
    BC_REQUIRE(stream.str().empty());
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_bytes2__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_bytes(data_slice(expected).data(), expected.size());
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

#endif // BIT_WRITER_BYTES

#ifdef BIT_WRITER_STRINGS

// write_string

BC_AUTO_TEST_CASE(bit_writer__write_string__one_byte__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    constexpr auto size = to_half(varint_two_bytes);
    const std::string expected(size, '*');
    writer.write_string(expected);
    BC_REQUIRE_EQUAL(stream.str(), std::string{ "\x7e" } + expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_string__two_bytes__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    constexpr auto size = varint_two_bytes;
    std::string expected(size, '*');
    writer.write_string(expected);
    expected.insert(expected.cbegin(), 0x00);
    expected.insert(expected.cbegin(), '\xfd');
    expected.insert(expected.cbegin(), '\xfd');
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

////// Too much memory allocation required to test.
////BC_AUTO_TEST_CASE(bit_writer__write_string__four_bytes__expected)
////{
////    std::ostringstream stream;
////    write::bits::ostream writer(stream);
////    const auto size = add1<uint32_t>(max_uint16);
////    std::string expected(size, '*');
////    writer.write_string(expected);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x01);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), '\xfe');
////    BC_REQUIRE(writer);
////}
////
////// Too much memory allocation required to test.
////BC_AUTO_TEST_CASE(bit_writer__write_string__eight_bytes__expected)
////{
////    std::ostringstream stream;
////    write::bits::ostream writer(stream);
////    const auto size = add1<uint64_t>(max_uint32);
////    std::string expected(size, '*');
////    writer.write_string(expected);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x01);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), 0x00);
////    expected.insert(expected.begin(), '\xff');
////    BC_REQUIRE(writer);
////}

// write_string_buffer

BC_AUTO_TEST_CASE(bit_writer__write_string_buffer__empty__empty)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_string_buffer({}, zero);
    BC_REQUIRE(stream.str().empty());
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_string_buffer__value__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_string_buffer(expected.data(), expected.size());
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

// write_line

BC_AUTO_TEST_CASE(bit_writer__write_line__default__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_line();
    BC_REQUIRE_EQUAL(stream.str(), "\r\n");
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_line__empty_default__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_line("");
    BC_REQUIRE_EQUAL(stream.str(), "\r\n");
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_line__empty_empty__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    writer.write_line("", "");
    BC_REQUIRE(stream.str().empty());
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_line__value_empty__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_line(expected, "");
    BC_REQUIRE_EQUAL(stream.str(), expected);
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_line__value_explicit__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_line(expected, "|");
    BC_REQUIRE_EQUAL(stream.str(), expected + "|");
    BC_REQUIRE(writer);
}

BC_AUTO_TEST_CASE(bit_writer__write_line__value_default__expected)
{
    std::ostringstream stream;
    write::bits::ostream writer(stream);
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz" };
    writer.write_line(expected);
    BC_REQUIRE_EQUAL(stream.str(), expected + "\r\n");
    BC_REQUIRE(writer);
}

#endif // BIT_WRITER_STRINGS

BC_AUTO_TEST_SUITE_END()
