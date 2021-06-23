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

BOOST_AUTO_TEST_SUITE(byte_reader_tests)

// is_exhausted/invalidate/bool()/!

BOOST_AUTO_TEST_CASE(byte_reader__is_exhausted__empty__true)
{
    const data_chunk source;
    stream::in::copy stream;
    byte_reader instance(stream);
    BOOST_REQUIRE(instance.is_exhausted());
    BOOST_REQUIRE_EQUAL((bool)instance, false);
    BOOST_REQUIRE_EQUAL(!instance, true);
}

BOOST_AUTO_TEST_CASE(byte_reader__is_exhausted__not_exhausted__false)
{
    const data_chunk source{ 0x00 };
    stream::in::copy stream;
    byte_reader instance(stream);
    BOOST_REQUIRE(!instance.is_exhausted());
    BOOST_REQUIRE_EQUAL((bool)instance, true);
    BOOST_REQUIRE_EQUAL(!instance, false);
}

BOOST_AUTO_TEST_CASE(byte_reader__is_exhausted__exhausted__true)
{
    const data_chunk source{ 0x00 };
    stream::in::copy stream;
    byte_reader instance(stream);
    instance.skip();
    BOOST_REQUIRE(instance.is_exhausted());
    BOOST_REQUIRE_EQUAL((bool)instance, false);
    BOOST_REQUIRE_EQUAL(!instance, true);
}

BOOST_AUTO_TEST_SUITE_END()
