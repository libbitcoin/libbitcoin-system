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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(stream_tests)

int read(std::istream& stream)
{
    int value;
    deserialize(value, stream);
    return value;
}

void write(std::ostream& stream, int value)
{
    serialize(stream, value, "fail");
}

// Verify boost input stream behavior with our copy_source.
// peek invalidates *only* an empty source. get invalidates on read past
// end (including on an empty source). peek and get both return zero on an
// empty source (and eof on an invalid source), so always need to use peek
// to test for exhaustion before reading so as to avoid end invalidation.
// These test all of the istream methods utilized by byte_reader (all readers).

BOOST_AUTO_TEST_CASE(stream__eof__always__negative_one)
{
    BOOST_REQUIRE_EQUAL(std::istream::traits_type::eof(), -1);
}

BOOST_AUTO_TEST_CASE(stream__bool__empty__true)
{
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__bool__not_empty__true)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__setstate__failbit__invalid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
}

BOOST_AUTO_TEST_CASE(stream__get__to_end__expected_valid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__bool__get_invalid__eof)
{
    const std::string source;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
}

BOOST_AUTO_TEST_CASE(stream__bool__get_empty__zero_invalid)
{
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.get(), 0x00);
    BOOST_REQUIRE(!istream);
}

BOOST_AUTO_TEST_CASE(stream__get__invalid__eof)
{
    const std::string source;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
}

BOOST_AUTO_TEST_CASE(stream__get__past_end__eof_invalid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
    BOOST_REQUIRE(!istream);
}

BOOST_AUTO_TEST_CASE(stream__peek__empty__zero_invalid)
{
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), 0x00);
    BOOST_REQUIRE(!istream);
}

BOOST_AUTO_TEST_CASE(stream__peek__invalid__eof)
{
    const std::string source;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), -1);
}

BOOST_AUTO_TEST_CASE(stream__peek__past_end__eof_valid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE_EQUAL(istream.peek(), -1);
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__get__peek__expected)
{
    const std::string source{ "+/-" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), '+');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.get(), '+');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), '/');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.get(), '/');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), '-');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.get(), '-');
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__read__empty_zero__valid)
{
    std::vector<char> sink;
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__read__to_end__expected_valid)
{
    const std::string source{ "+/-" };
    std::vector<char> sink(source.size(), 0x00);
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(to_string(sink), source);
}

BOOST_AUTO_TEST_CASE(stream__read__past_end__expected_invalid)
{
    const std::string source{ "+/-" };
    std::vector<char> sink(add1(source.size()), 0x00);
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(to_string(sink).substr(0, source.size()), source);
}

BOOST_AUTO_TEST_CASE(stream__read__invalid__unchanged_invalid)
{
    const std::string source{ "+/-" };
    std::vector<char> sink(source.size(), 0x00);
    const auto expected = sink;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE_EQUAL(sink, expected);
}

BOOST_AUTO_TEST_CASE(stream__seekg__empty_zero__invalid)
{
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.seekg(0, std::ios_base::cur);
    BOOST_REQUIRE(!istream);
}

BOOST_AUTO_TEST_CASE(stream__seekg__non_empty_zero__valid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.seekg(0, std::ios_base::cur);
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__seekg__to_end__valid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.seekg(1, std::ios_base::cur);
    BOOST_REQUIRE(istream);
}

BOOST_AUTO_TEST_CASE(stream__seekg__get__expected)
{
    const std::string source{ "+/-" };
    stream::in::copy istream(source);
    istream.seekg(2, std::ios_base::cur);
    BOOST_REQUIRE_EQUAL(istream.get(), '-');
    BOOST_REQUIRE(istream);
}

// This does not throw?! Empty source behavior is always unique.
BOOST_AUTO_TEST_CASE(stream__seekg__empty_one__invalid)
{
    const std::string source;
    stream::in::copy istream(source);
    istream.seekg(1, std::ios_base::cur);
    istream.seekg(42, std::ios_base::cur);
    BOOST_REQUIRE(!istream);
}

// Throwing does not invalidate the stream!
BOOST_AUTO_TEST_CASE(stream__seekg__past_begin__throws_invalid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.seekg(1, std::ios_base::cur);
    BOOST_REQUIRE_THROW(istream.seekg(-2, std::ios_base::cur), std::ios_base::failure);
    BOOST_REQUIRE(istream);
}

// Throwing does not invalidate the stream!
BOOST_AUTO_TEST_CASE(stream__seekg__past_end__throws_invalid)
{
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_THROW(istream.seekg(2, std::ios_base::cur), std::ios_base::failure);
    BOOST_REQUIRE(istream);
}

// stream::in (copy)
// copy streams are direct.

BOOST_AUTO_TEST_CASE(stream__in__string__expected)
{
    const std::string source{ "42" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

BOOST_AUTO_TEST_CASE(stream__in__array__expected)
{
    const data_array<2> source{ { '4', '2' } };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

BOOST_AUTO_TEST_CASE(stream__in__data__expected)
{
    const data_chunk source{ '4', '2' };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

// stream::out (copy)
// copy streams are direct.

BOOST_AUTO_TEST_CASE(stream__out__copy_string__expected)
{
    std::string sink(2, 0x00);
    stream::out::copy ostream(sink);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(sink, "42");
}

BOOST_AUTO_TEST_CASE(stream__out__array__expected)
{
    data_array<2> sink{ { 0x00, 0x00 } };
    stream::out::copy ostream(sink);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(sink, to_array<2>("42"));
}

BOOST_AUTO_TEST_CASE(stream__out__data__expected)
{
    data_chunk sink(2, 0x00);
    stream::out::copy ostream(sink);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(sink, to_chunk("42"));
}

// stream::out (push)
// push streams are indirect (flush required).

BOOST_AUTO_TEST_CASE(stream__out__into_string__expected)
{
    std::string sink;
    stream::out::text ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, "42");
}

BOOST_AUTO_TEST_CASE(stream__out__into_data__expected)
{
    data_chunk sink;
    stream::out::data ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, to_chunk("42"));
}

BOOST_AUTO_TEST_CASE(stream__out__text__expected)
{
    std::string sink;
    stream::out::text ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, "42");
}

BOOST_AUTO_TEST_CASE(stream__out__push__expected)
{
    data_chunk sink;
    stream::out::data ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, to_chunk("42"));
}

// stream::flip
// flip streams are direct (flush not required).

BOOST_AUTO_TEST_CASE(stream__out__flip__expected)
{
    data_chunk data(2, 0x00);
    stream::flip::copy ostream(data);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(to_string(data), "42");
}

BOOST_AUTO_TEST_SUITE_END()