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

#include <iostream>
#include <boost/filesystem.hpp>

struct interprocess_lock_setup_fixture
{
    interprocess_lock_setup_fixture()
    {
        BOOST_REQUIRE(test::clear(test::directory));
    }

    ~interprocess_lock_setup_fixture()
    {
        BOOST_REQUIRE(test::clear(test::directory));
    }
};

BOOST_FIXTURE_TEST_SUITE(interprocess_lock_tests, interprocess_lock_setup_fixture)

// TODO: verify delete result code.

BOOST_AUTO_TEST_CASE(interprocess_lock__todo__todo__todo)
{
    ////interprocess_lock instance("interprocess_lock");
    ////BOOST_REQUIRE(instance.lock());
    ////BOOST_REQUIRE(instance.unlock());
}

BOOST_AUTO_TEST_SUITE_END()
