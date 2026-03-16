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

block block_instance()
{
    static const block instance
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

    return instance;
}

BOOST_AUTO_TEST_CASE(block__json__native__expected)
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

    const auto& instance = block_instance();
    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<block>(value) == instance);
}

BOOST_AUTO_TEST_CASE(block__json__bitcoind__expected)
{
    const std::string_view text
    {
        "{"
            R"("hash":"d5b1048b2dcb443dd79a15e54de994fa18620d1d99250f2a4123660c68dea664",)"
            R"("size":209,)"
            R"("strippedsize":197,)"
            R"("weight":800,)"
            R"("version":42,)"
            R"("versionHex":"0000002a",)"
            R"("merkleroot":"0000000000000000000000000000000000000000000000000000000000000001",)"
            R"("time":43,)"
            R"("nonce":45,)"
            R"("bits":"0000002c",)"
            R"("difficulty":1.0279680609929991E73,)"
            R"("nTx":1)"
        "}"
    };

    const auto& instance = block_instance();
    const auto value = json::value_from(bitcoind(instance));
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_CASE(block__json__bitcoind_hashed__expected)
{
    const std::string_view text
    {
        "{"
            R"("hash":"d5b1048b2dcb443dd79a15e54de994fa18620d1d99250f2a4123660c68dea664",)"
            R"("size":209,)"
            R"("strippedsize":197,)"
            R"("weight":800,)"
            R"("version":42,)"
            R"("versionHex":"0000002a",)"
            R"("merkleroot":"0000000000000000000000000000000000000000000000000000000000000001",)"
            R"("time":43,)"
            R"("nonce":45,)"
            R"("bits":"0000002c",)"
            R"("difficulty":1.0279680609929991E73,)"
            R"("nTx":1,)"
            R"("tx":["6d74f0162f9c7a3be99cb60cca0c658f3e19fb3462f4c9731d5a0b7495183335"])"
        "}"
    };

    const auto& instance = block_instance();
    const auto value = json::value_from(bitcoind_hashed(instance));
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_CASE(block__json__bitcoind_embedded__expected)
{
    const std::string_view text
    {
        "{"
            R"("hash":"d5b1048b2dcb443dd79a15e54de994fa18620d1d99250f2a4123660c68dea664",)"
            R"("size":209,)"
            R"("strippedsize":197,)"
            R"("weight":800,)"
            R"("version":42,)"
            R"("versionHex":"0000002a",)"
            R"("merkleroot":"0000000000000000000000000000000000000000000000000000000000000001",)"
            R"("time":43,)"
            R"("nonce":45,)"
            R"("bits":"0000002c",)"
            R"("difficulty":1.0279680609929991E73,)"
            R"("nTx":1,)"
            R"("tx":["2a000000000102000000000000000000000000000000000000000000000000000000000000000018000000026a792a00000001000000000000000000000000000000000000000000000000000000000000002a000000026a7a1800000002180000000000000001792a00000000000000017a0103242424010342424218000000"])"
        "}"
    };

    const auto& instance = block_instance();
    const auto value = json::value_from(bitcoind_embedded(instance));
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_CASE(block__json__bitcoind_verbose__expected)
{
    const std::string_view text
    {
        "{"
            R"("hash":"d5b1048b2dcb443dd79a15e54de994fa18620d1d99250f2a4123660c68dea664",)"
            R"("size":209,)"
            R"("strippedsize":197,)"
            R"("weight":800,)"
            R"("version":42,)"
            R"("versionHex":"0000002a",)"
            R"("merkleroot":"0000000000000000000000000000000000000000000000000000000000000001",)"
            R"("time":43,)"
            R"("nonce":45,)"
            R"("bits":"0000002c",)"
            R"("difficulty":1.0279680609929991E73,)"
            R"("nTx":1,)"
            R"("tx":)"
            "["
                "{"
                    R"("hex":"2a000000000102000000000000000000000000000000000000000000000000000000000000000018000000026a792a00000001000000000000000000000000000000000000000000000000000000000000002a000000026a7a1800000002180000000000000001792a00000000000000017a0103242424010342424218000000",)"
                    R"("txid":"6d74f0162f9c7a3be99cb60cca0c658f3e19fb3462f4c9731d5a0b7495183335",)"
                    R"("hash":"d6650355f9f42540f512cfe6c55829636e7a1b92a8893a4d48e1dec362ad62ff",)"
                    R"("size":116,)"
                    R"("vsize":119,)"
                    R"("weight":476,)"
                    R"("version":42,)"
                    R"("locktime":24,)"
                    R"("vin":)"
                    "["
                        "{"
                            R"("txid":"0000000000000000000000000000000000000000000000000000000000000000",)"
                            R"("vout":24,)"
                            R"("scriptSig":)"
                            "{"
                                R"("asm":"return pick",)"
                                R"("hex":"6a79")"
                            "},"
                            R"("sequence":42,)"
                            R"("txinwitness":["242424"])"
                        "},"
                        "{"
                            R"("txid":"0000000000000000000000000000000000000000000000000000000000000001",)"
                            R"("vout":42,)"
                            R"("scriptSig":)"
                            "{"
                                R"("asm":"return roll",)"
                                R"("hex":"6a7a")"
                            "},"
                            R"("sequence":24,)"
                            R"("txinwitness":["424242"])"
                        "}"
                    "],"
                    R"("vout":)"
                    "["
                        "{"
                            R"("value":2.4E-7,)"
                            R"("scriptPubKey":)"
                            "{"
                                R"("asm":"pick",)"
                                R"("hex":"79",)"
                                R"("type":"nonstandard",)"
                                R"("addresses":["fab04811b1d0379bf78c2a41902368c200d675788e4bff8c88ff543836e4fca1"])"
                            "},"
                            R"("n":0)"
                        "},"
                        "{"
                            R"("value":4.2E-7,)"
                            R"("scriptPubKey":)"
                            "{"
                                R"("asm":"roll",)"
                                R"("hex":"7a",)"
                                R"("type":"nonstandard",)"
                                R"("addresses":["067bd624c5840ed0d5b65597bafcde68f07fa9d87d3b43292b3199e49a514e59"])"
                            "},"
                            R"("n":1)"
                        "}"
                    "]"
                "}"
            "]"
        "}"
    };

    const auto& instance = block_instance();
    const auto value = json::value_from(bitcoind_verbose(instance));
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_SUITE_END()
