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

BOOST_AUTO_TEST_SUITE(istream_tests)

using istream_chunk = istream<data_chunk>;
using seekdir = istream_chunk::seekdir;
using pos_type = istream_chunk::pos_type;
const auto chunk = base16_chunk("01020304050607080900");

BOOST_AUTO_TEST_CASE(istream__tellg__empty__zero)
{
    const istream_chunk stream{ {} };
    BOOST_REQUIRE(is_zero(stream.tellg()));
}

BOOST_AUTO_TEST_CASE(istream__tellg__default__zero)
{
    const istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellg()));
}

BOOST_AUTO_TEST_CASE(istream__seekg__zero__tellg_zero)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(0, seekdir::beg).tellg()));
}

BOOST_AUTO_TEST_SUITE_END()