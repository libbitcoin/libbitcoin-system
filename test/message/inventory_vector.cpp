/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::message;

BOOST_AUTO_TEST_SUITE(inventory_vector_tests)

BOOST_AUTO_TEST_CASE(inventory_vector__to_number__error__returns_0)
{
    BOOST_REQUIRE_EQUAL(0u, inventory_vector::to_number(inventory_vector::type_id::error));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_number__none__returns_0)
{
    BOOST_REQUIRE_EQUAL(0u, inventory_vector::to_number(inventory_vector::type_id::none));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_number__transaction__returns_1)
{
    BOOST_REQUIRE_EQUAL(1u, inventory_vector::to_number(inventory_vector::type_id::transaction));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_number__block__returns_2)
{
    BOOST_REQUIRE_EQUAL(2u, inventory_vector::to_number(inventory_vector::type_id::block));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_number__compact_block__returns_4)
{
    BOOST_REQUIRE_EQUAL(4u, inventory_vector::to_number(inventory_vector::type_id::compact_block));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_type__0__returns_error)
{
    BOOST_REQUIRE(inventory_vector::type_id::error == inventory_vector::to_type(0));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_type__1__returns_transaction)
{
    BOOST_REQUIRE(inventory_vector::type_id::transaction == inventory_vector::to_type(1));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_type__2__returns_block)
{
    BOOST_REQUIRE(inventory_vector::type_id::block == inventory_vector::to_type(2));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_type__4__returns_compact_block)
{
    BOOST_REQUIRE(inventory_vector::type_id::compact_block == inventory_vector::to_type(4));
}

BOOST_AUTO_TEST_CASE(inventory_vector__to_type__3__returns_none)
{
    BOOST_REQUIRE(inventory_vector::type_id::none == inventory_vector::to_type(3));
}

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    static const data_chunk raw{ 1 };
    inventory_vector instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(version::level::minimum, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    static const inventory_vector expected
    {
        inventory_vector::type_id::error,
        {
            {
                0x44, 0x9a, 0x0d, 0x24, 0x9a, 0xd5, 0x39, 0x89,
                0xbb, 0x85, 0x0a, 0x3d, 0x79, 0x24, 0xed, 0x0f,
                0xc3, 0x0d, 0x6f, 0x55, 0x7d, 0x71, 0x12, 0x1a,
                0x37, 0xc0, 0xb0, 0x32, 0xf0, 0xd6, 0x6e, 0xdf
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    const auto result = inventory_vector::factory_from_data(version, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    static const inventory_vector expected
    {
        inventory_vector::type_id::transaction,
        {
            {
                0x44, 0x9a, 0x0d, 0xee, 0x9a, 0xd5, 0x39, 0xee,
                0xee, 0x85, 0x0a, 0x3d, 0xee, 0x24, 0xed, 0x0f,
                0xc3, 0xee, 0x6f, 0x55, 0x7d, 0xee, 0x12, 0x1a,
                0x37, 0xc0, 0xee, 0x32, 0xf0, 0xd6, 0xee, 0xdf
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    const auto result = inventory_vector::factory_from_data(version, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    static const inventory_vector expected
    {
        inventory_vector::type_id::block,
        {
            {
                0x66, 0x9a, 0x0d, 0x24, 0x66, 0xd5, 0x39, 0x89,
                0xbb, 0x66, 0x0a, 0x3d, 0x79, 0x66, 0xed, 0x0f,
                0xc3, 0x0d, 0x66, 0x55, 0x7d, 0x71, 0x66, 0x1a,
                0x37, 0xc0, 0xb0, 0x66, 0xf0, 0xd6, 0x6e, 0x66
            }
        }
    };

    static const auto version = version::level::minimum;
    const auto data = expected.to_data(version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = inventory_vector::factory_from_data(version, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version), result.serialized_size(version));
}

BOOST_AUTO_TEST_SUITE_END()
