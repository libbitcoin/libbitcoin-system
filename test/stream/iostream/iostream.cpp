/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(iostream_tests)

using iostream_chunk = iostream<data_chunk>;

// setstate/clear

BOOST_AUTO_TEST_CASE(iostream__setstate__goodbit__goodbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.setstate(iostream_chunk::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__eofbit__eofbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    stream.setstate(iostream_chunk::eofbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::eofbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__failbit__failbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.setstate(iostream_chunk::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::failbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__badbit__badbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.setstate(iostream_chunk::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__badbit_failbit__badbit_failbit)
{
    constexpr auto badfail = iostream_chunk::failbit | iostream_chunk::badbit;

    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    stream.setstate(iostream_chunk::badbit);
    stream.setstate(iostream_chunk::failbit);
    BOOST_REQUIRE(stream.rdstate() == badfail);

    stream.clear(iostream_chunk::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);

    stream.clear(iostream_chunk::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::failbit);

    stream.clear(iostream_chunk::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_SUITE_END()
