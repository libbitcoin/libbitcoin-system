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

BOOST_AUTO_TEST_CASE(interprocess_lock__lock__not_exists__true_created)
{
    BOOST_REQUIRE(!test::exists(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.lock());
    BOOST_REQUIRE(test::exists(TEST_PATH));
}

// The file exists but no lock is held on it.
BOOST_AUTO_TEST_CASE(interprocess_lock__lock__file_exists__true)
{
    BOOST_REQUIRE(test::create(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.lock());
}

// The lock is process-exclusive in linux/macOS, globally in win32.
// This is okay here, but a proper test requires an external process.
BOOST_AUTO_TEST_CASE(interprocess_lock__lock__externally_locked__false)
{
    BOOST_REQUIRE(!test::exists(TEST_PATH));

    interprocess_lock instance1(TEST_PATH);
    interprocess_lock instance2(TEST_PATH);
    BOOST_REQUIRE(instance1.lock());
    BOOST_REQUIRE(test::exists(TEST_PATH));

#ifdef _MSC_VER
    BOOST_REQUIRE(!instance2.lock());
#else
    BOOST_REQUIRE(instance2.lock());
#endif

    BOOST_REQUIRE(instance1.unlock());
    BOOST_REQUIRE(!test::exists(TEST_PATH));

#ifdef _MSC_VER
    BOOST_REQUIRE(instance2.lock());
    BOOST_REQUIRE(test::exists(TEST_PATH));
#else
    BOOST_REQUIRE(!instance2.lock());
    BOOST_REQUIRE(!test::exists(TEST_PATH));
#endif

    BOOST_REQUIRE(instance2.unlock());
    BOOST_REQUIRE(!test::exists(TEST_PATH));
}

BOOST_AUTO_TEST_CASE(interprocess_lock__unlock__not_exists__true)
{
    BOOST_REQUIRE(!test::exists(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.unlock());
}

BOOST_AUTO_TEST_CASE(interprocess_lock__unlock__exists__true_not_deleted)
{
    BOOST_REQUIRE(test::create(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.unlock());
    BOOST_REQUIRE(test::exists(TEST_PATH));
}

BOOST_AUTO_TEST_CASE(interprocess_lock__lock_unlock__not_exists__true_deleted)
{
    BOOST_REQUIRE(!test::exists(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.lock());
    BOOST_REQUIRE(instance.unlock());
    BOOST_REQUIRE(!test::exists(TEST_PATH));
}

BOOST_AUTO_TEST_CASE(interprocess_lock__lock_unlock__exists__true_deleted)
{
    BOOST_REQUIRE(test::create(TEST_PATH));

    interprocess_lock instance(TEST_PATH);
    BOOST_REQUIRE(instance.lock());
    BOOST_REQUIRE(instance.unlock());
    BOOST_REQUIRE(!test::exists(TEST_PATH));
}

BOOST_AUTO_TEST_SUITE_END()
