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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(external_ptr_tests)

BOOST_AUTO_TEST_CASE(external_ptr__construct__default__unassigned)
{
    const external_ptr<bool> empty{};
    BOOST_REQUIRE(!empty);
}

BOOST_AUTO_TEST_SUITE_END()


// stdlib object sizes are subjsct to implementation (including debug builds).
#if defined(HAVE_MSC) && defined(NDEBUG)

// std::unique_ptr owns storage, deleting the referenced object on destruct.
static_assert(sizeof(std::unique_ptr<std_vector<uint8_t>>) == 1 * sizeof(size_t));

// std::shared_ptr owns storage, deleting the referenced object on destruct.
static_assert(sizeof(std::shared_ptr<std_vector<uint8_t>>) == 2 * sizeof(size_t));

// std::weak_ptr doubles storage cost and is overly restrictive.
static_assert(sizeof(std::weak_ptr<std_vector<uint8_t>>) == 2 * sizeof(size_t));

// external_ptr is safer and more flexible than a raw pointer, at same cost.
static_assert(sizeof(external_ptr<std_vector<uint8_t>>) == 1 * sizeof(size_t));

#endif // HAVE_MSC && NDEBUG