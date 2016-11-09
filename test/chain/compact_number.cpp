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

#include <cstdint>
#include <bitcoin/bitcoin.hpp>

BOOST_AUTO_TEST_SUITE(compact_number_tests)

using namespace bc;
using namespace bc::chain;

BOOST_AUTO_TEST_CASE(compact_number__constructor1__0__expected)
{
    const auto expected = 0u;
    compact_number compact(expected);
    BOOST_REQUIRE(!compact.is_overflowed());
    BOOST_REQUIRE_EQUAL(compact.normal(), expected);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__42__expected)
{
    const auto expected = 42u;
    compact_number compact(expected);
    BOOST_REQUIRE(!compact.is_overflowed());
    BOOST_REQUIRE_EQUAL(compact.normal(), expected);
}

BOOST_AUTO_TEST_SUITE_END()
