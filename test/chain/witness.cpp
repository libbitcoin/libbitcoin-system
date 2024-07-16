/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(witness_tests)

namespace json = boost::json;
using namespace system::chain;

const block& get_block() NOEXCEPT
{
    constexpr auto root = base16_hash("1bf6a4281aca254af9dafb696dc1b9ce47d9f0785ebd4622fa0e28104fe3df44");
    constexpr uint32_t block_version{ 0 };
    constexpr hash_digest previous_block_hash{ 1 };
    constexpr hash_digest merkle_root{ root };
    constexpr uint32_t timestamp{ 3 };
    constexpr uint32_t bits{ 4 };
    constexpr uint32_t nonce{ 5 };
    constexpr uint32_t tx_version{ 6 };
    constexpr uint32_t locktime{ 7 };
    constexpr uint32_t sequence{ 8 };
    constexpr uint64_t value{ 9 };
    constexpr hash_digest point_hash{ 9 };
    constexpr uint32_t point_index{ 9 };
    constexpr bool minimal{ true };

    static const block instance
    {
        header // ptr<header>
        {
            block_version,
            previous_block_hash,
            merkle_root,
            timestamp,
            bits,
            nonce
        },
        transactions // ptr<vector<tx>>
        {
            // coinbase
            transaction  // first tx
            {
                tx_version,
                inputs
                {
                    input // exactly one input
                    {
                        point // null point
                        {
                            null_hash,
                            point::null_index
                        },
                        script{ operations{ operation{ opcode::nop1 }, operation{ opcode::nop2 } } }, // minumum 2 bytes
                        sequence
                    }
                },
                outputs
                {
                    output // at least one output
                    {
                        value,  // no more than reward
                        script{}
                    }
                },
                locktime
            },
            transaction // ptr<tx>
            {
                tx_version,
                inputs // ptr<vector<ptr<input>>>
                {
                    input // ptr<input>
                    {
                        point // ptr<point>
                        {
                            point_hash,
                            point_index
                        },
                        script // ptr<script>
                        {
                            operations // vector<operation>
                            {
                                operation
                                {
                                    opcode::nop3  // ptr<nullptr>
                                },
                                operation
                                {
                                    data_chunk{}, // ptr<data_chunk>
                                    minimal
                                }
                            }
                        },
                        witness // ptr<witness>
                        {
                            data_stack // vector<ptr<data_chunk>>
                            {
                                data_chunk{}, // ptr<data_chunk>
                                data_chunk{}  // ptr<data_chunk>
                            }
                        },
                        sequence
                    }
                },
                outputs // vector<ptr<output>>
                {
                    output // ptr<output>
                    {
                        value,
                        script // ptr<script>
                        {
                            operations // vector<operation>
                            {
                                operation
                                {
                                    opcode::nop4  // ptr<nullptr>
                                },
                                operation
                                {
                                    data_chunk{}, // ptr<data_chunk>
                                    minimal
                                }
                            }
                        }
                    }
                },
                locktime
            }
        }
    };

    return instance;
}

const data_chunk& get_data(bool witness) NOEXCEPT
{
    static const auto data = get_block().to_data(witness);
    return data;
}

BOOST_AUTO_TEST_CASE(witness__block__with_witness__parse_round_trips)
{
    constexpr auto witness = true;
    const auto& instance1 = get_block();
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

BOOST_AUTO_TEST_CASE(witness__block__without_witness__checks)
{
    constexpr auto witness = false;
    test::reporting_arena<false> arena{};
    const auto& data = get_data(witness);
    stream::in::fast stream(data);
    read::bytes::fast source(stream, &arena);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

// json
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(witness___json__conversions__expected)
{
    const std::string text
    {
        "\"[424344] [454647] [48494a]\""
    };

    const chain::witness instance
    {
        data_stack
        {
            { 0x42, 0x43, 0x44 },
            { 0x45, 0x46, 0x47 },
            { 0x48, 0x49, 0x4a }
        }
    };

    const auto value = json::value_from(instance);

    // cannot parse a value, must be an object.
    ////BOOST_REQUIRE(json::parse(text) == value);
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::witness>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
