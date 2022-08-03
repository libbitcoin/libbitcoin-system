/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(functions_tests)

// TODO: test all hash wrapper functions.

BOOST_AUTO_TEST_CASE(functions__djb2_hash__ad_hoc__0xe1669c01)
{
    const auto hash = djb2_hash("01234567890abcdefghijklmnopqrstuvwxyz");

    if constexpr (sizeof(size_t) == sizeof(uint32_t))
    {
        BOOST_REQUIRE_EQUAL(hash, 3781598209_u32);
    }
    else
    {
        BOOST_REQUIRE_EQUAL(hash, 9646636626660989953_u64);
    }
}

BOOST_AUTO_TEST_SUITE_END()
