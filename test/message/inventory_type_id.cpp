/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(inventory_type_id_tests)

BOOST_AUTO_TEST_CASE(inventory_type_to_number_error_returns_0)
{
    BOOST_REQUIRE_EQUAL(0u,
        message::inventory_type_to_number(message::inventory_type_id::error));
}

BOOST_AUTO_TEST_CASE(inventory_type_to_number_none_returns_0)
{
    BOOST_REQUIRE_EQUAL(0u,
        message::inventory_type_to_number(message::inventory_type_id::none));
}

BOOST_AUTO_TEST_CASE(inventory_type_to_number_transaction_returns_1)
{
    BOOST_REQUIRE_EQUAL(1u,
        message::inventory_type_to_number(message::inventory_type_id::transaction));
}

BOOST_AUTO_TEST_CASE(inventory_type_to_number_block_returns_2)
{
    BOOST_REQUIRE_EQUAL(2u,
        message::inventory_type_to_number(message::inventory_type_id::block));
}

BOOST_AUTO_TEST_CASE(inventory_type_from_number_0_returns_error)
{
    BOOST_REQUIRE(message::inventory_type_id::error == message::inventory_type_from_number(0u));
}

BOOST_AUTO_TEST_CASE(inventory_type_from_number_1_returns_transaction)
{
    BOOST_REQUIRE(message::inventory_type_id::transaction == message::inventory_type_from_number(1u));
}

BOOST_AUTO_TEST_CASE(inventory_type_from_number_2_returns_block)
{
    BOOST_REQUIRE(message::inventory_type_id::block == message::inventory_type_from_number(2u));
}

BOOST_AUTO_TEST_CASE(inventory_type_from_number_other_returns_none)
{
    BOOST_REQUIRE(message::inventory_type_id::none == message::inventory_type_from_number(3u));
}

BOOST_AUTO_TEST_SUITE_END()
