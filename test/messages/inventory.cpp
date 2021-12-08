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

BOOST_AUTO_TEST_SUITE(inventory_tests)

using namespace bc::system::messages;

BOOST_AUTO_TEST_CASE(inventory__constructor_1__always__invalid)
{
    messages::inventory instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(inventory__constructor_2__always__equals_params)
{
    const messages::inventory_vector::list values =
    {
        messages::inventory_vector(
            inventory::type_id::error,
            {
                {
                    0x44, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
                    0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
                    0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
                    0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
                }
            }
        )
    };

    messages::inventory instance(values);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(values == instance.inventories());
}

BOOST_AUTO_TEST_CASE(inventory__constructor_3__always__equals_params)
{
    messages::inventory_vector::type_id type = messages::inventory_vector::type_id::error;
    auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::inventory_vector::list values =
    {
        messages::inventory_vector(type, hash)
    };

    messages::inventory instance(std::move(values));
    BOOST_REQUIRE(instance.is_valid());
    auto inventories = instance.inventories();
    BOOST_REQUIRE_EQUAL(1u, inventories.size());
    BOOST_REQUIRE(type == inventories[0].type());
    BOOST_REQUIRE(hash == inventories[0].hash());
}

BOOST_AUTO_TEST_CASE(inventory__constructor_4__always__equals_params)
{
    messages::inventory_vector::type_id type = messages::inventory_vector::type_id::error;
    auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const hash_list hashes = { hash };

    messages::inventory instance(hashes, type);
    BOOST_REQUIRE(instance.is_valid());
    auto inventories = instance.inventories();
    BOOST_REQUIRE_EQUAL(1u, inventories.size());
    BOOST_REQUIRE(type == inventories[0].type());
    BOOST_REQUIRE(hash == inventories[0].hash());
}

BOOST_AUTO_TEST_CASE(inventory__constructor_5__always__equals_params)
{
    messages::inventory_vector::type_id type = messages::inventory_vector::type_id::error;
    auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    messages::inventory instance{ { type, hash } };
    BOOST_REQUIRE(instance.is_valid());
    auto inventories = instance.inventories();
    BOOST_REQUIRE_EQUAL(1u, inventories.size());
    BOOST_REQUIRE(type == inventories[0].type());
    BOOST_REQUIRE(hash == inventories[0].hash());
}

BOOST_AUTO_TEST_CASE(inventory__constructor_6__always__equals_params)
{
    messages::inventory_vector::type_id type = messages::inventory_vector::type_id::error;
    auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    const messages::inventory value{ { type, hash } };
    BOOST_REQUIRE(value.is_valid());
    messages::inventory instance(value);
    auto inventories = instance.inventories();
    BOOST_REQUIRE_EQUAL(1u, inventories.size());
    BOOST_REQUIRE(type == inventories[0].type());
    BOOST_REQUIRE(hash == inventories[0].hash());
    BOOST_REQUIRE(value == instance);
}

BOOST_AUTO_TEST_CASE(inventory__constructor_7__always__equals_params)
{
    messages::inventory_vector::type_id type = messages::inventory_vector::type_id::error;
    auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    messages::inventory value{ { type, hash } };
    BOOST_REQUIRE(value.is_valid());
    messages::inventory instance(std::move(value));
    auto inventories = instance.inventories();
    BOOST_REQUIRE_EQUAL(1u, inventories.size());
    BOOST_REQUIRE(type == inventories[0].type());
    BOOST_REQUIRE(hash == inventories[0].hash());
}

BOOST_AUTO_TEST_CASE(inventory__from_data__insufficient_bytes__failure)
{
    static const auto version = version::level::minimum;
    static const data_chunk raw{ 0xab, 0xcd };
    inventory instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(version, raw));
}

BOOST_AUTO_TEST_CASE(inventory__factory_1__valid_input__success)
{
    static const inventory expected
    {
        {
            {
                inventory::type_id::error,
                {
                    {
                        0x44, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
                        0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
                        0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
                        0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
                    }
                }
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    const auto result = inventory::factory(version, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(inventory__factory_2__valid_input__success)
{
    static const inventory expected
    {
        {
            {
                inventory::type_id::transaction,
                {
                    {
                        0x44, 0x9a, 0x0d, 0xee, 0x9a, 0xd5, 0x39, 0xee,
                        0xee, 0x85, 0x0a, 0x3d, 0xee, 0x24, 0xed, 0x0f,
                        0xc3, 0xee, 0x6f, 0x55, 0x7d, 0xee, 0x12, 0x1a,
                        0x37, 0xc0, 0xee, 0x32, 0xf0, 0xd6, 0xee, 0xdf
                    }
                }
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    stream::in::copy istream(data);
    auto result = inventory::factory(version, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(inventory__factory_3__valid_input__success)
{
    static const inventory expected
    {
        {
            {
                inventory::type_id::block,
                {
                    {
                        0x66, 0x9a, 0x0d, 0x24, 0x66, 0xd5, 0x39, 0x89,
                        0xbb, 0x66, 0x0a, 0x3d, 0x79, 0x66, 0xed, 0x0f,
                        0xc3, 0x0d, 0x66, 0x55, 0x7d, 0x71, 0x66, 0x1a,
                        0x37, 0xc0, 0xb0, 0x66, 0xf0, 0xd6, 0x6e, 0x66
                    }
                }
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    read::bytes::copy source(data);
    const auto result = inventory::factory(version, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(inventory__inventories_accessor_1__always__initialized_value)
{
    const messages::inventory_vector::list values =
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    };

    messages::inventory instance(values);
    BOOST_REQUIRE(values == instance.inventories());
}

BOOST_AUTO_TEST_CASE(inventory__inventories_accessor_2__always__initialized_value)
{
    const messages::inventory_vector::list values =
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    };

    const messages::inventory instance(values);
    BOOST_REQUIRE(values == instance.inventories());
}

BOOST_AUTO_TEST_CASE(inventory__inventories_setter_1__roundtrip__success)
{
    const messages::inventory_vector::list values =
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    };

    messages::inventory instance;
    BOOST_REQUIRE(values != instance.inventories());
    instance.set_inventories(values);
    BOOST_REQUIRE(values == instance.inventories());
}

BOOST_AUTO_TEST_CASE(inventory__inventories_setter_2__roundtrip__success)
{
    messages::inventory_vector::list values =
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    };

    messages::inventory instance;
    BOOST_REQUIRE_EQUAL(0u, instance.inventories().size());
    instance.set_inventories(std::move(values));
    BOOST_REQUIRE_EQUAL(1u, instance.inventories().size());
}

BOOST_AUTO_TEST_CASE(inventory__operator_assign_equals__always__matches_equivalent)
{
    const messages::inventory_vector::list elements =
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    };

    messages::inventory instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = messages::inventory(elements);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(elements == instance.inventories());
}

BOOST_AUTO_TEST_CASE(inventory__operator_boolean_equals__duplicates__true)
{
    const messages::inventory expected(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    messages::inventory instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(inventory__operator_boolean_equals__differs__false)
{
    const messages::inventory expected(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    messages::inventory instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(inventory__operator_boolean_not_equals__duplicates__false)
{
    const messages::inventory expected(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    messages::inventory instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(inventory__operator_boolean_not_equals__differs__true)
{
    const messages::inventory expected(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    messages::inventory instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(inventory__count__no_matching_type__zero)
{
    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    BOOST_REQUIRE_EQUAL(0u,
        instance.count(messages::inventory_vector::type_id::block));
}

BOOST_AUTO_TEST_CASE(inventory__count__matching_type__count)
{
    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")),
        messages::inventory_vector(messages::inventory_vector::type_id::block,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"))
    });

    BOOST_REQUIRE_EQUAL(3u,
        instance.count(messages::inventory_vector::type_id::error));
}

BOOST_AUTO_TEST_CASE(inventory__to_hashes__matching_type__empty_list)
{
    const hash_list hashes = {};

    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("1111111111111111111111111111111111111111111111111111111111111111")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("2222222222222222222222222222222222222222222222222222222222222222")),
        messages::inventory_vector(messages::inventory_vector::type_id::block,
            base16_hash("3333333333333333333333333333333333333333333333333333333333333333")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4444444444444444444444444444444444444444444444444444444444444444"))
    });

    hash_list result;
    instance.to_hashes(result, messages::inventory_vector::type_id::transaction);
    BOOST_REQUIRE(hashes == result);
}

BOOST_AUTO_TEST_CASE(inventory__to_hashes__matching_type__hashes)
{
    const hash_list hashes = {
        base16_hash("1111111111111111111111111111111111111111111111111111111111111111"),
        base16_hash("2222222222222222222222222222222222222222222222222222222222222222"),
        base16_hash("4444444444444444444444444444444444444444444444444444444444444444")
    };

    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("1111111111111111111111111111111111111111111111111111111111111111")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("2222222222222222222222222222222222222222222222222222222222222222")),
        messages::inventory_vector(messages::inventory_vector::type_id::block,
            base16_hash("3333333333333333333333333333333333333333333333333333333333333333")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4444444444444444444444444444444444444444444444444444444444444444"))
    });

    hash_list result;
    instance.to_hashes(result, messages::inventory_vector::type_id::error);
    BOOST_REQUIRE(hashes == result);
}

BOOST_AUTO_TEST_CASE(inventory__reduce__matching_type__empty_list)
{
    const messages::inventory_vector::list expected = {};

    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("1111111111111111111111111111111111111111111111111111111111111111")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("2222222222222222222222222222222222222222222222222222222222222222")),
        messages::inventory_vector(messages::inventory_vector::type_id::block,
            base16_hash("3333333333333333333333333333333333333333333333333333333333333333")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4444444444444444444444444444444444444444444444444444444444444444"))
    });

    messages::inventory_vector::list result;
    instance.reduce(result, messages::inventory_vector::type_id::transaction);
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(inventory__reduce__matching_type__matches)
{
    const messages::inventory_vector::list expected = {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("1111111111111111111111111111111111111111111111111111111111111111")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("2222222222222222222222222222222222222222222222222222222222222222")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4444444444444444444444444444444444444444444444444444444444444444"))
    };

    messages::inventory instance(
    {
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("1111111111111111111111111111111111111111111111111111111111111111")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("2222222222222222222222222222222222222222222222222222222222222222")),
        messages::inventory_vector(messages::inventory_vector::type_id::block,
            base16_hash("3333333333333333333333333333333333333333333333333333333333333333")),
        messages::inventory_vector(messages::inventory_vector::type_id::error,
            base16_hash("4444444444444444444444444444444444444444444444444444444444444444"))
    });

    messages::inventory_vector::list result;
    instance.reduce(result, messages::inventory_vector::type_id::error);
    BOOST_REQUIRE_EQUAL(expected.size(), result.size());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_SUITE_END()
