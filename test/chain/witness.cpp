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

BOOST_AUTO_TEST_SUITE(witness_tests)

BOOST_AUTO_TEST_CASE(witness_test)
{
    // Passing const ref ptr and assigning it to non-const ref pointer only increments the refcount.
    // If the pointer is to a non-const object, the object is mutable under the member pointer.
    auto foo = std::make_shared<data_stack>(data_stack{ data_chunk{ 42 }, data_chunk{ 43 }, data_chunk{ 44 } });
    const chain::witness bar(foo);
    const data_chunk expected1{ 01, 42, 01, 43, 01, 44 };
    BOOST_REQUIRE_EQUAL(bar.to_data(false), expected1);

    // If the pointer is to a non-const object, the container is mutable under the external pointer.
    foo->resize(2);
    const data_chunk expected2{ 01, 42, 01, 43 };
    BOOST_REQUIRE_EQUAL(bar.to_data(false), expected2);

    // So pass const& pointer to const object.
    // This requires modification of from_data so that it assigns a new shared pointer to the member.
    // Elements of the const pointer list can be emitted as const pointers.
    // New lists can be constructed by copying members, but the list cannot be modified.
}

BOOST_AUTO_TEST_SUITE_END()
