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
#include "../../test.hpp"
#include "algorithm.hpp"

BOOST_AUTO_TEST_SUITE(merkle)

BOOST_AUTO_TEST_CASE(merkle__hacks2)
{
    using sha_256 = shax::algorithm<sha::h256<>, true, true, true>;
    constexpr auto expected = sha_256::double_hash({ 0 }, { 1 });
    static_assert(sha_256::merkle_root({ { 0 }, { 1 } }) == expected);
    BOOST_CHECK_EQUAL(sha_256::merkle_root({ { 0 }, { 1 } }), expected);
}

BOOST_AUTO_TEST_CASE(merkle__hacks4)
{
    using sha_256 = shax::algorithm<sha::h256<>, true, true, true>;
    constexpr auto expected1 = sha_256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha_256::double_hash({ 2 }, { 3 });
    constexpr auto expected = sha_256::double_hash(expected1, expected2);
    static_assert(sha_256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }) == expected);
    BOOST_CHECK_EQUAL(sha_256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 } }), expected);
}

BOOST_AUTO_TEST_CASE(merkle__hacks8)
{
    using sha_256 = shax::algorithm<sha::h256<>, true, true, true>;
    constexpr auto expected1 = sha_256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha_256::double_hash({ 2 }, { 3 });
    constexpr auto expected3 = sha_256::double_hash({ 4 }, { 5 });
    constexpr auto expected4 = sha_256::double_hash({ 6 }, { 7 });
    constexpr auto expected5 = sha_256::double_hash(expected1, expected2);
    constexpr auto expected6 = sha_256::double_hash(expected3, expected4);
    constexpr auto expected  = sha_256::double_hash(expected5, expected6);
    static_assert(sha_256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 } }) == expected);
    BOOST_CHECK_EQUAL(sha_256::merkle_root({ { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 } }), expected);
}

BOOST_AUTO_TEST_SUITE_END()
