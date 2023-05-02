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

using iostream_chunk = istream<data_chunk>;
using seekdir = iostream_chunk::seekdir;
using pos_type = iostream_chunk::pos_type;
using iostate = iostream_chunk::iostate;
auto chunk = base16_chunk("01020304050607080900");

BOOST_AUTO_TEST_CASE(iostream__setstate__goodbit__goodbit)
{
    iostream_chunk stream{ {} };
    stream.setstate(iostate::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__tellg__initial__zero_goodbit)
{
    const iostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_SUITE_END()