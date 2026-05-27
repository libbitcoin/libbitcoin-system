/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include "../mocks/blocks.hpp"

BOOST_AUTO_TEST_SUITE(witness_tests)

namespace json = boost::json;
using namespace system::chain;

const data_chunk& get_data(bool witness) NOEXCEPT
{
    static const auto data = test::mock_block_c.to_data(witness);
    return data;
}

BOOST_AUTO_TEST_CASE(witness__block__with_witness__parse_round_trips)
{
    constexpr auto witness = true;
    const auto& instance1 = test::mock_block_c;
    const auto& data = get_data(witness);

    const block instance2(data, witness);
    BOOST_REQUIRE(instance1.is_valid());
    BOOST_REQUIRE(instance2.is_valid());
    BOOST_REQUIRE(instance1.hash() == instance2.hash());
    BOOST_REQUIRE(instance1 == instance2);
    BOOST_REQUIRE(instance1.is_segregated());
    BOOST_REQUIRE(instance2.is_segregated());
}

BOOST_AUTO_TEST_CASE(witness__block__genesis_block__arena_parse_round_trips)
{
    constexpr auto witness = true;
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);

    test::reporting_arena<false> arena{};
    stream::in::fast stream(data);
    read::bytes::fast source(stream, &arena);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

BOOST_AUTO_TEST_CASE(witness__block__with_witness__checks)
{
    constexpr auto witness = true;
    test::reporting_arena<false> arena{};
    const auto& data = get_data(witness);
    stream::in::fast stream(data);
    read::bytes::fast source(stream, &arena);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

// TODO:
////BOOST_AUTO_TEST_CASE(witness__block__without_witness__checks)
////{
////    constexpr auto witness = false;
////    test::reporting_arena<false> arena{};
////    const auto& data = get_data(witness);
////    stream::in::fast stream(data);
////    read::bytes::fast source(stream, &arena);
////    const block instance(source, witness);
////    BOOST_REQUIRE(!instance.check());
////}

BOOST_AUTO_TEST_SUITE_END()
