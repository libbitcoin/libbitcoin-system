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

// move streams are direct.
BOOST_AUTO_TEST_CASE(stream__in__move__expected)
{
    std::string source{ "42" };
    stream::in::move istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

// from streams are move streams (direct).
BOOST_AUTO_TEST_CASE(stream__in__from_string__expected)
{
    const std::string source{ "42" };
    stream::in::from<std::string> istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

// from streams are move streams (direct).
BOOST_AUTO_TEST_CASE(stream__in__from_array__expected)
{
    const data_array<2> source{ { '4', '2' } };
    stream::in::from<data_array<2>> istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

// copy streams are direct.
BOOST_AUTO_TEST_CASE(stream__in__copy__expected)
{
    const data_chunk source{ '4', '2' };
    stream::in::copy istream(source);
    BOOST_REQUIRE_EQUAL(read(istream), 42);
}

// slab streams are direct (flush not required).
BOOST_AUTO_TEST_CASE(stream__out__slab__expected)
{
    std::string sink(2, 0x00);
    stream::out::slab ostream(sink);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(sink, "42");
}

// text streams are push treams (indirect) (flush required).
BOOST_AUTO_TEST_CASE(stream__out__text__expected)
{
    std::string sink;
    stream::out::text ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(sink, "42");
}

// push streams are indirect (flush required).
BOOST_AUTO_TEST_CASE(stream__out__push__expected)
{
    data_chunk sink;
    stream::out::push ostream(sink);
    write(ostream, 42);
    ostream.flush();
    BOOST_REQUIRE_EQUAL(to_string(sink), "42");
}

// flip streams are direct (flush not required).
BOOST_AUTO_TEST_CASE(stream__out__flip__expected)
{
    data_chunk sink(2, 0x00);
    stream::flip ostream(sink);
    write(ostream, 42);
    BOOST_REQUIRE_EQUAL(to_string(sink), "42");
}

BOOST_AUTO_TEST_SUITE_END()