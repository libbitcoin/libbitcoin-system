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
    stream::out::push<std::string> ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, "42");
}

BOOST_AUTO_TEST_CASE(stream__out__into_data__expected)
{
    data_chunk sink;
    stream::out::push<data_chunk> ostream(sink);
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