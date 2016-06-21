/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>

#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(resource_lock_tests)

BOOST_AUTO_TEST_CASE(resource_lock__duplicate_locks)
{
    resource_lock main("foo");
    BOOST_REQUIRE(main.lock());
    std::thread thread(
        []()
        {
            resource_lock duplicate("foo");
            BOOST_REQUIRE(duplicate.lock());
            BOOST_REQUIRE(duplicate.unlock());
        });
    thread.join();
    main.unlock();
}

BOOST_AUTO_TEST_SUITE_END()

