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
#include <sstream>

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

constexpr bool is_set(int state, int flag)
{
    return (state & flag) == flag;
}

// Stream state flags (bitmasks).
// no error, standardized.
static_assert(std::istream::goodbit == 0, "");
static_assert(std::istringstream::goodbit == 0, "");
// associated input sequence has reached eof, implementation-defined.
static_assert(std::istream::eofbit == 1, "");
static_assert(std::istringstream::eofbit == 1, "");
// operation failed (formatting or extraction error), implementation-defined.
static_assert(std::istream::failbit == 2, "");
static_assert(std::istringstream::failbit == 2, "");
// irrecoverable stream error, implementation-defined.
static_assert(std::istream::badbit == 4, "");
static_assert(std::istringstream::badbit == 4, "");

// seekg clears eofbit.
// seekg on failure should set failbit, but neither boost nor sstream do this.
// read/get past end should set failbit, boost and sstream do this (even on empty).
// write/put past end should set badbit, boost and sstream do this (even on empty)?
// If eofbit is set, failbit is generally set on all operations.
static_assert(std::istream::traits_type::eof() == -1, "");
static_assert(std::istringstream::traits_type::eof() == -1, "");

// Verify boost input stream behavior with our copy_source and istringstream.
// These test all of the istream methods utilized by byte_reader (all readers).
// Empty behavior is inconsistent within and across implementations.

BOOST_AUTO_TEST_CASE(stream__bool__empty__true)
{
    std::istringstream isstream;
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__bool__not_empty__true)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__rdstate__non_empty__valid)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__setstate__failbit__failbit)
{
    std::istringstream isstream{ "*" };
    isstream.setstate(std::istringstream::failbit);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__get__failbit_empty__failbit)
{
    std::istringstream isstream;
    isstream.setstate(std::istringstream::failbit);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), -1);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__get__failbit_non_empty__failbit)
{
    std::istringstream isstream{ "*" };
    isstream.setstate(std::istringstream::failbit);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), -1);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__get__to_end__expected_valid)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), '*');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__get__empty__inconsistent)
{
    // -1, sets eof and fail, but not bad.
    std::istringstream isstream;
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), -1);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    // zero, sets bad, but not eof or fail.
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.get(), 0x00);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__get__past_end__eofbit_and_failbit)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE_EQUAL(isstream.get(), '*');
    BOOST_REQUIRE_EQUAL(isstream.get(), -1);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE_EQUAL(istream.get(), -1);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__peek__empty__inconsistent)
{
    // -1, eofbit
    std::istringstream isstream;
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.peek(), -1);
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    // 0, badbit
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), 0x00);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__peek__failbit__failbit)
{
    std::istringstream isstream;
    isstream.setstate(std::istringstream::failbit);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE_EQUAL(isstream.peek(), -1);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE_EQUAL(istream.peek(), -1);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__peek__past_end__eofbit)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE_EQUAL(isstream.get(), '*');
    BOOST_REQUIRE_EQUAL(isstream.peek(), -1);
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(istream.get(), '*');
    BOOST_REQUIRE_EQUAL(istream.peek(), -1);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__get__peek__expected)
{
    std::istringstream isstream{ "+/-" };
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.peek(), '+');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), '+');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.peek(), '/');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), '/');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.peek(), '-');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.get(), '-');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

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
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__read__empty_zero__valid)
{
    std::vector<char> ssink;
    std::istringstream isstream;
    BOOST_REQUIRE(isstream);
    isstream.read(ssink.data(), ssink.size());
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    std::vector<char> sink;
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__read__empty_one__inconsistent)
{
    // sets eofbit and failbit
    std::istringstream isstream;
    std::vector<char> ssink(1, 0x00);
    BOOST_REQUIRE(isstream);
    isstream.read(ssink.data(), ssink.size());
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    // sets badbit
    const std::string source;
    std::vector<char> sink(1, 0x00);
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__read__to_end__expected_valid)
{
    const std::string source{ "+/-" };
    std::vector<char> ssink(source.size(), 0x00);
    std::istringstream isstream{ source };
    BOOST_REQUIRE(isstream);
    isstream.read(ssink.data(), ssink.size());
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);
    BOOST_REQUIRE_EQUAL(to_string(ssink), source);

    std::vector<char> sink(source.size(), 0x00);
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
    BOOST_REQUIRE_EQUAL(to_string(sink), source);
}

BOOST_AUTO_TEST_CASE(stream__read__past_end__eofbit_and_failbit)
{
    const std::string source{ "+/-" };
    std::vector<char> ssink(add1(source.size()), 0x00);
    std::istringstream isstream{ source };
    BOOST_REQUIRE(isstream);
    isstream.read(ssink.data(), ssink.size());
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));
    BOOST_REQUIRE_EQUAL(to_string(ssink).substr(0, source.size()), source);

    std::vector<char> sink(add1(source.size()), 0x00);
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
    BOOST_REQUIRE_EQUAL(to_string(sink).substr(0, source.size()), source);
}

BOOST_AUTO_TEST_CASE(stream__read__failbit__failbit)
{
    const std::string source{ "+/-" };
    std::vector<char> ssink(source.size(), 0x00);
    const auto sexpected = ssink;
    std::istringstream isstream{ source };
    isstream.setstate(std::istringstream::failbit);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));
    isstream.read(ssink.data(), ssink.size());
    BOOST_REQUIRE_EQUAL(ssink, sexpected);

    std::vector<char> sink(source.size(), 0x00);
    const auto expected = ssink;
    stream::in::copy istream(source);
    istream.setstate(std::istream::failbit);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
    istream.read(sink.data(), sink.size());
    BOOST_REQUIRE_EQUAL(sink, expected);
}

BOOST_AUTO_TEST_CASE(stream__seekg__empty_zero__inconsistent)
{
    // valid
    std::istringstream isstream;
    BOOST_REQUIRE(isstream);
    isstream.seekg(0, std::istringstream::cur);
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    // failbit
    const std::string source;
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.seekg(0, std::istream::cur);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__seekg__non_empty_zero__valid)
{
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE(isstream);
    isstream.seekg(0, std::istringstream::cur);
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE(istream);
    istream.seekg(0, std::istream::cur);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__seekg__to_end__valid)
{
    std::istringstream isstream{ "*" };
    isstream.seekg(1, std::istringstream::cur);
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "*" };
    stream::in::copy istream(source);
    istream.seekg(1, std::istream::cur);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__seekg__get__expected)
{
    std::istringstream isstream{ "+/-" };
    isstream.seekg(2, std::istringstream::cur);
    BOOST_REQUIRE_EQUAL(isstream.get(), '-');
    BOOST_REQUIRE(isstream);
    BOOST_REQUIRE_EQUAL(isstream.rdstate(), 0);

    const std::string source{ "+/-" };
    stream::in::copy istream(source);
    istream.seekg(2, std::istream::cur);
    BOOST_REQUIRE_EQUAL(istream.get(), '-');
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__seekg__empty_one__failbit)
{
    std::istringstream isstream;
    isstream.seekg(1, std::istringstream::cur);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source;
    stream::in::copy istream(source);
    istream.seekg(1, std::istream::cur);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__seekg__empty_negative_one__failbit)
{
    std::istringstream isstream;
    isstream.seekg(-1, std::istringstream::cur);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    const std::string source;
    stream::in::copy istream(source);
    istream.seekg(-1, std::istream::cur);
    BOOST_REQUIRE(!istream);
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::eofbit));
    BOOST_REQUIRE(is_set(istream.rdstate(), std::istream::failbit));
    BOOST_REQUIRE(!is_set(istream.rdstate(), std::istream::badbit));
}

BOOST_AUTO_TEST_CASE(stream__seekg__past_begin__inconsistent)
{
    // failbit.
    std::istringstream isstream{ "*" };
    isstream.seekg(-2, std::istringstream::cur);
    BOOST_REQUIRE(!isstream);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    // std::istream::failure
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_THROW(istream.seekg(-2, std::istream::cur), std::istream::failure);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
}

BOOST_AUTO_TEST_CASE(stream__seekg__past_end__inconsistent)
{
    // failbit
    std::istringstream isstream{ "*" };
    BOOST_REQUIRE(isstream);
    isstream.seekg(2, std::istringstream::cur);
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::eofbit));
    BOOST_REQUIRE(is_set(isstream.rdstate(), std::istringstream::failbit));
    BOOST_REQUIRE(!is_set(isstream.rdstate(), std::istringstream::badbit));

    // std::istream::failure
    const std::string source{ "*" };
    stream::in::copy istream(source);
    BOOST_REQUIRE_THROW(istream.seekg(2, std::istream::cur), std::istream::failure);
    BOOST_REQUIRE(istream);
    BOOST_REQUIRE_EQUAL(istream.rdstate(), 0);
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