/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BC_FIXTURE_TEST_SUITE(ofstream_tests, test::directory_setup_fixture)

BC_AUTO_TEST_CASE(ofstream__construct__valid_path__round_trip)
{
    ofstream out(TEST_PATH, std::ofstream::out);
    BC_REQUIRE(out.good());
    BC_REQUIRE(!out.bad());

    const std::string expected{ "libbitcoin" };
    out << expected;
    BC_REQUIRE(out.good());

    out.close();
    BC_REQUIRE(out.good());

    ifstream in(TEST_PATH);
    BC_REQUIRE(in.good());

    std::string line;
    BC_REQUIRE(std::getline(in, line));

    // Not good (no more to read) and not bad (did not fail).
    BC_REQUIRE(!in.good());
    BC_REQUIRE(!in.bad());
    BC_REQUIRE_EQUAL(line, expected);

    in.close();
    BC_REQUIRE(!in.good());
}

BC_AUTO_TEST_SUITE_END()
