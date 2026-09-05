/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BOOST_AUTO_TEST_SUITE(script_json_tests)

BOOST_AUTO_TEST_CASE(script__json__conversions__expected)
{
    const std::string_view text
    {
        R"("pick roll return")"
    };

    const script instance
    {
        operations
        {
            { opcode::pick },
            { opcode::roll },
            { opcode::op_return }
        }
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<script>(value) == instance);
}

// bitcoind

static const std::string signature =
    "3044022001"
    "11111111111111111111111111111111111111111111111111111111111111"
    "022002"
    "22222222222222222222222222222222222222222222222222222222222222";
static const std::string public_key =
    "02abababababababababababababababababababababababababababababababab";
static const std::string p2pkh_sign = "47" + signature + "01" + "21" + public_key;
static const std::string return_sign = "6a47" + signature + "01";

BOOST_AUTO_TEST_CASE(script__json__bitcoind__expected)
{
    const std::string_view text
    {
        R"({"asm":"OP_PICK OP_ROLL OP_RETURN","hex":"797a6a","type":"nonstandard"})"
    };

    const script instance
    {
        operations
        {
            { opcode::pick },
            { opcode::roll },
            { opcode::op_return }
        }
    };

    const auto value = json::value_from(bitcoind(instance));
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_CASE(script__json__bitcoind_flags__fork_resolved)
{
    const script instance{ operations{ { opcode::checklocktimeverify } } };
    const auto inactive = json::value_from(bitcoind(instance, flags::no_rules));
    const auto active = json::value_from(bitcoind(instance, flags::bip65_rule));
    BOOST_REQUIRE_EQUAL(inactive.at("asm").as_string(), "OP_NOP2");
    BOOST_REQUIRE_EQUAL(active.at("asm").as_string(), "OP_CHECKLOCKTIMEVERIFY");
}

BOOST_AUTO_TEST_CASE(script__json__bitcoind_signature__sighash_decoded)
{
    data_chunk data{};
    BOOST_REQUIRE(decode_base16(data, p2pkh_sign));

    const script instance(data, false);
    const auto value = json::value_from(bitcoind_signature(instance));
    const auto& object = value.as_object();
    const auto expected = signature + "[ALL] " + public_key;
    BOOST_REQUIRE_EQUAL(object.at("asm").as_string(), expected);
    BOOST_REQUIRE_EQUAL(object.at("hex").as_string(), p2pkh_sign);
    BOOST_REQUIRE(!object.contains("type"));
}

BOOST_AUTO_TEST_CASE(script__json__bitcoind_signature_op_return__not_decoded)
{
    data_chunk data{};
    BOOST_REQUIRE(decode_base16(data, return_sign));

    const script instance(data, false);
    const auto value = json::value_from(bitcoind_signature(instance));
    const auto expected = "OP_RETURN " + signature + "01";
    BOOST_REQUIRE_EQUAL(value.at("asm").as_string(), expected);
}

BOOST_AUTO_TEST_CASE(script__json__bitcoind_output__not_decoded)
{
    data_chunk data{};
    BOOST_REQUIRE(decode_base16(data, p2pkh_sign));

    const script instance(data, false);
    const auto value = json::value_from(bitcoind(instance));
    const auto expected = signature + "01 " + public_key;
    BOOST_REQUIRE_EQUAL(value.at("asm").as_string(), expected);
    BOOST_REQUIRE_EQUAL(value.at("type").as_string(), "nonstandard");
}

BOOST_AUTO_TEST_SUITE_END()
