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
#include "../../test.hpp"

using namespace boost;
using namespace bc::system::chain;

BOOST_AUTO_TEST_SUITE(block_tests)

BOOST_AUTO_TEST_CASE(block__json__conversions__expected)
{
    const std::string_view text
    {
        "{"
            R"("header":)"
            "{"
                R"("hash":"d5b1048b2dcb443dd79a15e54de994fa18620d1d99250f2a4123660c68dea664",)"
                R"("version":42,)"
                R"("previous":"0000000000000000000000000000000000000000000000000000000000000000",)"
                R"("merkle_root":"0000000000000000000000000000000000000000000000000000000000000001",)"
                R"("timestamp":43,)"
                R"("bits":44,)"
                R"("nonce":45)"
            "},"
            R"("transactions":)"
            "["
                "{"
                    R"("hash":"6d74f0162f9c7a3be99cb60cca0c658f3e19fb3462f4c9731d5a0b7495183335",)"
                    R"("version":42,)"
                    R"("inputs":)"
                    "["
                        "{"
                            R"("point":)"
                            "{"
                                R"("hash":"0000000000000000000000000000000000000000000000000000000000000000",)"
                                R"("index":24)"
                            "},"
                            R"("script":"return pick",)"
                            R"("witness":"[242424]",)"
                            R"("sequence":42)"
                        "},"
                        "{"
                            R"("point":)"
                            "{"
                                R"("hash":"0000000000000000000000000000000000000000000000000000000000000001",)"
                                R"("index":42)"
                            "},"
                            R"("script":"return roll",)"
                            R"("witness":"[424242]",)"
                            R"("sequence":24)"
                        "}"
                    "],"
                    R"("outputs":)"
                    "["
                        "{"
                            R"("value":24,)"
                            R"("script":"pick")"
                        "},"
                        "{"
                            R"("value":42,)"
                            R"("script":"roll")"
                        "}"
                    "],"
                    R"("locktime":24)"
                "}"
            "]"
        "}"
    };

    const block instance
    {
        header
        {
            42, null_hash, one_hash, 43, 44, 45
        },
        transactions
        {
            transaction
            {
                42,
                inputs
                {
                    input
                    {
                        point{ null_hash, 24 },
                        script{ { { opcode::op_return }, { opcode::pick } } },
                        witness{ "[242424]" },
                        42
                    },
                    input
                    {
                        point{ one_hash, 42 },
                        script{ { { opcode::op_return }, { opcode::roll } } },
                        witness{ "[424242]" },
                        24
                    }
                },
                outputs
                {
                    output
                    {
                        24,
                        script{ { { opcode::pick } } }
                    },
                    output
                    {
                        42,
                        script{ { { opcode::roll } } }
                    }
                },
                24
            }
        }
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<block>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
