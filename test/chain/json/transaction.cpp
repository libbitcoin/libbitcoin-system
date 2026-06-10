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

BOOST_AUTO_TEST_SUITE(transaction_tests)

BOOST_AUTO_TEST_CASE(transaction__json__conversions__expected)
{
    const std::string_view text
    {
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
    };

    const transaction instance
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
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<transaction>(value) == instance);
}


// mainnet block 840000 coinbase.
// TODO: output script (type/address/asm) parity.
static const auto block840000_coinbase = base16_chunk(
    "0100000000010100000000000000000000000000000000000000000000000000"
    "00000000000000ffffffff600340d10c192f5669614254432f4d696e65642062"
    "792062757a7a3132302f2cfabe6d6d144b553283a6e1a150c9989428c0695e3a"
    "1bef7d482ed1f829bbe25897fd37dc10000000000000001058a4c9000cc3a318"
    "89b38ae08249000000000000ffffffff03fb80e4f2000000001976a914536ffa"
    "992491508dca0354e52f32a3a7a679a53a88ac00000000000000002b6a295253"
    "4b424c4f434b3a52e15efafb3e2cf6dc2fc0e6bde5cb1d7d2143f1e089bd874e"
    "6b7913005fb2a00000000000000000266a24aa21a9ed88601d3d03ccce017fe2"
    "131c4c95a7292e4372983148e62996bb5e2de0e4d1d801200000000000000000"
    "00000000000000000000000000000000000000000000000000000000");

BOOST_AUTO_TEST_CASE(transaction__bitcoind__witness_coinbase_840000__size_and_witness_hash)
{
    const transaction tx{ block840000_coinbase, true };
    BOOST_REQUIRE(tx.is_valid());

    const auto value = json::value_from(bitcoind(tx));
    const auto& object = value.as_object();

    BOOST_REQUIRE_EQUAL(object.at("txid").as_string(),
        "a0db149ace545beabbd87a8d6b20ffd6aa3b5a50e58add49a3d435f898c272cf");

    // Witnessed hash, not the coinbase's null wtxid.
    BOOST_REQUIRE_EQUAL(object.at("hash").as_string(),
        "ba2a2a86efa306684a88b6e0cdb8abf25d9f66e6a851c9b9c67f25640cdacac8");

    BOOST_REQUIRE_EQUAL(object.at("size").to_number<uint64_t>(), 316u);
    BOOST_REQUIRE_EQUAL(object.at("vsize").to_number<uint64_t>(), 289u);
    BOOST_REQUIRE_EQUAL(object.at("weight").to_number<uint64_t>(), 1156u);
    BOOST_REQUIRE_EQUAL(object.at("version").to_number<uint32_t>(), 1u);
    BOOST_REQUIRE_EQUAL(object.at("locktime").to_number<uint32_t>(), 0u);

    const auto& vin = object.at("vin").as_array();
    BOOST_REQUIRE_EQUAL(vin.size(), 1u);
    BOOST_REQUIRE(vin.at(0u).as_object().contains("coinbase"));
    BOOST_REQUIRE(!vin.at(0u).as_object().contains("txid"));

    BOOST_REQUIRE_EQUAL(object.at("vout").as_array().size(), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
