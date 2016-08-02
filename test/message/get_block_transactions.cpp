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

bool operator==(const message::get_block_transactions& left,
    const message::get_block_transactions& right)
{
    bool result = (left.block_hash == right.block_hash) &&
        (left.indexes.size() == right.indexes.size());

    for (uint64_t i = 0; (i < left.indexes.size()) && result; ++i)
        result = (left.indexes[i] == right.indexes[i]);

    return result;
}

BOOST_AUTO_TEST_SUITE(get_block_transactions_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    message::get_block_transactions instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::get_block_transactions expected
    {
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        {
            16,
            32,
            37,
            44
        }
    };

    const auto data = expected.to_data();
    const auto result = message::get_block_transactions::factory_from_data(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::get_block_transactions expected
    {
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        {
            16,
            32,
            37,
            44
        }
    };

    const auto data = expected.to_data();
    data_source istream(data);
    auto result = message::get_block_transactions::factory_from_data(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::get_block_transactions expected
    {
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        {
            16,
            32,
            37,
            44
        }
    };

    const auto data = expected.to_data();
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::get_block_transactions::factory_from_data(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_SUITE_END()
