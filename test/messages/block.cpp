/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

using namespace bc::system::messages;

BOOST_AUTO_TEST_SUITE(messages_block_tests)

static const chain::header expected_header
{
    10u,
    base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
    base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
    531234u,
    6523454u,
    68644u
};

static const chain::transactions expected_transactions
{
    chain::transaction(1, 48, chain::inputs{}, {}),
    chain::transaction(2, 32, chain::inputs{}, {}),
    chain::transaction(4, 16, chain::inputs{}, {})
};

BOOST_AUTO_TEST_CASE(block__constructor_1__always__invalid)
{
    block instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(block__constructor_2__always__equals_params)
{
    block instance(expected_header, expected_transactions);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());
}

BOOST_AUTO_TEST_CASE(block__constructor_3__always__equals_params)
{
    chain::header dup_header(expected_header);
    chain::transactions dup_transactions = expected_transactions;
    block instance(std::move(dup_header), std::move(dup_transactions));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());

}

BOOST_AUTO_TEST_CASE(block__constructor_4__always__equals_params)
{
    chain::block value(expected_header, expected_transactions);
    block instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == value);
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());
}

BOOST_AUTO_TEST_CASE(block__constructor_5__always__equals_params)
{
    chain::block value(expected_header, expected_transactions);
    block instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());
}

BOOST_AUTO_TEST_CASE(block__constructor_6__always__equals_params)
{
    block value(expected_header, expected_transactions);
    block instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());
}

BOOST_AUTO_TEST_CASE(block__constructor_7__always__equals_params)
{
    block value(expected_header, expected_transactions);
    block instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected_header == instance.header());
    BOOST_REQUIRE(expected_transactions == instance.transactions());
}

BOOST_AUTO_TEST_CASE(block__operator_assign_equals_1__always__matches_equivalent)
{
    block value(expected_header, expected_transactions);
    BOOST_REQUIRE(value.is_valid());

    messages::block instance;
    BOOST_REQUIRE(!instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.header() == expected_header);
    BOOST_REQUIRE(instance.transactions() == expected_transactions);
}

BOOST_AUTO_TEST_CASE(block__operator_assign_equals_2__always__matches_equivalent)
{
    block value(expected_header, expected_transactions);
    BOOST_REQUIRE(value.is_valid());

    messages::block instance;
    BOOST_REQUIRE(!instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.header() == expected_header);
    BOOST_REQUIRE(instance.transactions() == expected_transactions);
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_equals_1__duplicates__true)
{
    const block expected(expected_header, expected_transactions);
    const messages::block instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_equals_1__differs__false)
{
    const block expected(expected_header, expected_transactions);
    const messages::block instance;
    BOOST_REQUIRE(!(instance == expected));
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_not_equals_1__duplicates__false)
{
    const block expected(expected_header, expected_transactions);
    const messages::block instance(expected);
    BOOST_REQUIRE(!(instance != expected));
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_not_equals_1__differs__true)
{
    const block expected(expected_header, expected_transactions);
    chain::block instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_equals_2__duplicates__true)
{
    const block expected(expected_header, expected_transactions);
    messages::block instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_equals_2__differs__false)
{
    const block expected(expected_header, expected_transactions);
    messages::block instance;
    BOOST_REQUIRE(!(instance == expected));
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_not_equals_2__duplicates__false)
{
    const block expected(expected_header, expected_transactions);
    messages::block instance(expected);
    BOOST_REQUIRE(!( instance != expected));
}

BOOST_AUTO_TEST_CASE(block__operator_boolean_not_equals_2__differs__true)
{
    const block expected(expected_header, expected_transactions);
    messages::block instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()
