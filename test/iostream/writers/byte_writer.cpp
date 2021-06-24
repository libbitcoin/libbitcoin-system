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

BOOST_AUTO_TEST_SUITE(byte_writer_tests)

// TODO: test all methods against std::ostringstream
// TODO: test against libbitcoin sinks.

#define BYTE_WRITER_IS_EXHAUSTED

#ifdef BYTE_WRITER_IS_EXHAUSTED

BOOST_AUTO_TEST_CASE(byte_writer__write_byte__empty__expected)
{
    std::ostringstream stream;
    write::bytes::stream writer(stream);
    writer.write_byte('a');
    writer.flush();
    BOOST_REQUIRE_EQUAL(stream.str(), "a");
}

#endif // BYTE_WRITER_IS_EXHAUSTED

BOOST_AUTO_TEST_SUITE_END()
