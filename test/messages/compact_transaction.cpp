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

BOOST_AUTO_TEST_SUITE(compact_transaction_tests)

BOOST_AUTO_TEST_CASE(compact_transaction__constructor_1__always__invalid)
{
    messages::compact_transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(compact_transaction__constructor_2__always__equals_params)
{
    uint64_t index = 125u;
    chain::transaction tx(1, 0, chain::inputs{}, {});
    messages::compact_transaction instance(index, tx);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(index == instance.index());
    BOOST_REQUIRE(tx == instance.transaction());
}

BOOST_AUTO_TEST_CASE(compact_transaction__constructor_3__always__equals_params)
{
    uint64_t index = 125u;
    chain::transaction tx(1, 0, chain::inputs{}, {});
    BOOST_REQUIRE(tx.is_valid());
    messages::compact_transaction instance(index, std::move(tx));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(index == instance.index());
    BOOST_REQUIRE(instance.transaction().is_valid());
}

BOOST_AUTO_TEST_CASE(compact_transaction__constructor_4__always__equals_params)
{
    const messages::compact_transaction expected(125u,
        chain::transaction{1, 0, chain::inputs{}, {} });

    messages::compact_transaction instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(compact_transaction__constructor_5__always__equals_params)
{
    messages::compact_transaction expected(125u,
        chain::transaction{1, 0, chain::inputs{}, {} });

    messages::compact_transaction instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(compact_transaction__from_data__insufficient_bytes__failure)
{
    const data_chunk raw{ 1 };
    messages::compact_transaction instance{};
    BOOST_REQUIRE_EQUAL(false, instance.from_data(messages::version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(compact_transaction__factory_1__valid_input__success)
{
    const messages::compact_transaction expected
    {
        16,
        chain::transaction
        {
            1,
            0,
            chain::inputs{},
            {}
        }
    };

    const auto data = expected.to_data(messages::version::level::minimum);
    const auto result = messages::compact_transaction::factory(
        messages::version::level::minimum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(compact_transaction__factory_2__valid_input__success)
{
    const messages::compact_transaction expected
    {
        16,
        chain::transaction
        {
            1,
            0,
            chain::inputs{},
            {}
        }
    };

    const auto data = expected.to_data(messages::version::level::minimum);
    stream::in::copy istream(data);
    const auto result = messages::compact_transaction::factory(
        messages::version::level::minimum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(compact_transaction__factory_3__valid_input__success)
{
    const messages::compact_transaction expected
    {
        16,
        chain::transaction
        {
            1,
            0,
            chain::inputs{},
            {}
        }
    };

    const auto data = expected.to_data(messages::version::level::minimum);
    read::bytes::copy source(data);
    const auto result = messages::compact_transaction::factory(
        messages::version::level::minimum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(compact_transaction__index_accessor__always__initialized_value)
{
    uint64_t index = 634u;
    chain::transaction tx(5, 23, chain::inputs{}, {});
    messages::compact_transaction instance(index, tx);
    BOOST_REQUIRE(index == instance.index());
}

BOOST_AUTO_TEST_CASE(compact_transaction__index_setter__roundtrip__success)
{
    uint64_t index = 634u;
    messages::compact_transaction instance;
    BOOST_REQUIRE(index != instance.index());
    instance.set_index(index);
    BOOST_REQUIRE(index == instance.index());
}

BOOST_AUTO_TEST_CASE(compact_transaction__message_accessor_1__always__initialized_value)
{
    uint64_t index = 634u;
    const chain::transaction tx(5, 23, chain::inputs{}, {});
    messages::compact_transaction instance(index, tx);
    BOOST_REQUIRE(tx == instance.transaction());
}

BOOST_AUTO_TEST_CASE(compact_transaction__message_accessor_2__always__initialized_value)
{
    uint64_t index = 634u;
    const chain::transaction tx(5, 23, chain::inputs{}, {});
    const messages::compact_transaction instance(index, tx);
    BOOST_REQUIRE(tx == instance.transaction());
}

BOOST_AUTO_TEST_CASE(compact_transaction__message_setter_1__roundtrip__success)
{
    const chain::transaction tx(5, 23, chain::inputs{}, {});
    messages::compact_transaction instance;
    BOOST_REQUIRE(tx != instance.transaction());
    instance.set_transaction(tx);
    BOOST_REQUIRE(tx == instance.transaction());
}

BOOST_AUTO_TEST_CASE(compact_transaction__message_setter_2__roundtrip__success)
{
    const chain::transaction duplicate(16, 57, chain::inputs{}, {});
    chain::transaction tx(16, 57, chain::inputs{}, {});
    messages::compact_transaction instance;
    BOOST_REQUIRE(duplicate != instance.transaction());
    instance.set_transaction(std::move(tx));
    BOOST_REQUIRE(duplicate == instance.transaction());
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_assign_equals_1__always__matches_equivalent)
{
    const messages::compact_transaction value
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    BOOST_REQUIRE(value.is_valid());

    messages::compact_transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_assign_equals_2__always__matches_equivalent)
{
    const messages::compact_transaction value
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    BOOST_REQUIRE(value.is_valid());

    messages::compact_transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = value;
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_boolean_equals__duplicates__true)
{
    const messages::compact_transaction expected
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    messages::compact_transaction instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_boolean_equals__differs__false)
{
    const messages::compact_transaction expected
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    messages::compact_transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_boolean_not_equals__duplicates__false)
{
    const messages::compact_transaction expected
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    messages::compact_transaction instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(compact_transaction__operator_boolean_not_equals__differs__true)
{
    const messages::compact_transaction expected
    {
        1234u,
        chain::transaction{ 6u, 10u, chain::inputs{}, {} }
    };

    messages::compact_transaction instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()
