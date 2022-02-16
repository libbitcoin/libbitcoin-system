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

struct ofstream_tests_setup_fixture
{
    ofstream_tests_setup_fixture()
    {
        test::remove(TEST_NAME);
    }

    ~ofstream_tests_setup_fixture()
    {
        test::remove(TEST_NAME);
    }
};

BOOST_FIXTURE_TEST_SUITE(ofstream_tests, ofstream_tests_setup_fixture)

BOOST_AUTO_TEST_CASE(ofstream__construct__valid_path__round_trip)
{
    ofstream out(TEST_NAME, std::ifstream::out);
    BOOST_REQUIRE(!out.bad());

    out << "libbitcoin";
    out.close();
    BOOST_REQUIRE(!out.bad());

    ifstream in(TEST_NAME);
    BOOST_REQUIRE(!in.bad());

    std::string line;
    BOOST_REQUIRE(std::getline(in, line));

    in.close();
    BOOST_REQUIRE(!in.bad());
}

BOOST_AUTO_TEST_SUITE_END()
