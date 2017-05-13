/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>

#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin.hpp>
#include "script.hpp"

using namespace bc;
using namespace bc::chain;
using namespace bc::machine;

bool is_number(const std::string& token)
{
    if (boost::all(token, boost::is_digit()))
        return true;

    // Now check for negative numbers
    if (!boost::starts_with(token, "-"))
        return false;

    std::string numeric_part(token.begin() + 1, token.end());
    return boost::all(numeric_part, boost::is_digit());
}

bool is_hex_data(const std::string& token)
{
    if (!boost::starts_with(token, "0x"))
        return false;

    std::string hex_part(token.begin() + 2, token.end());
    return boost::all(hex_part, [](char c) { return is_base16(c); });
}

bool is_quoted_string(const std::string& token)
{
    if (token.size() < 2)
        return false;

    return boost::starts_with(token, "'") && boost::ends_with(token, "'");
}

bool is_opcode(const std::string& token)
{
    opcode out_code;
    return opcode_from_string(out_code, token);
}

bool is_opx(int64_t value)
{
    return value == -1 || (1 <= value && value <= 16);
}

void push_literal(data_chunk& raw_script, int64_t value)
{
    BITCOIN_ASSERT(is_opx(value));
    switch (value)
    {
        case -1:
            raw_script.push_back(static_cast<uint8_t>(opcode::push_negative_1));
            return;

#define PUSH_X(n) \
        case n: \
            raw_script.push_back(static_cast<uint8_t>(opcode::push_positive_##n)); \
            return;

        PUSH_X(1);
        PUSH_X(2);
        PUSH_X(3);
        PUSH_X(4);
        PUSH_X(5);
        PUSH_X(6);
        PUSH_X(7);
        PUSH_X(8);
        PUSH_X(9);
        PUSH_X(10);
        PUSH_X(11);
        PUSH_X(12);
        PUSH_X(13);
        PUSH_X(14);
        PUSH_X(15);
        PUSH_X(16);
    }
}

void push_data(data_chunk& raw_script, const data_chunk& data)
{
    extend_data(raw_script, operation(data).to_data());
}

static const auto sentinel = "__ENDING__";

bool parse_token(data_chunk& raw_script, data_chunk& raw_hex, std::string token)
{
    boost::trim(token);

    if (token.empty())
        return true;

    if (token == sentinel || !is_hex_data(token))
    {
        extend_data(raw_script, raw_hex);
        raw_hex.clear();
    }

    if (token == sentinel)
        return true;

    opcode out_code;

    if (is_number(token))
    {
        const auto value = boost::lexical_cast<int64_t>(token);

        if (is_opx(value))
        {
            push_literal(raw_script, value);
        }
        else
        {
            number bignum(value);
            push_data(raw_script, bignum.data());
        }
    }
    else if (is_hex_data(token))
    {
        std::string hex_part(token.begin() + 2, token.end());
        data_chunk raw_data;

        if (!decode_base16(raw_data, hex_part))
            return false;

        extend_data(raw_hex, raw_data);
    }
    else if (is_quoted_string(token))
    {
        data_chunk inner_value(token.begin() + 1, token.end() - 1);
        push_data(raw_script, inner_value);
    }
    else if (opcode_from_string(out_code, token))
    {
        // opcode_from_string converts push codes (data) in [hex] format,
        // but that is not currently expected here.
        raw_script.push_back(static_cast<uint8_t>(out_code));
    }
    else
    {
        // see: stackoverflow.com/questions/15192332/boost-message-undefined
        BOOST_TEST_MESSAGE("Token parsing failed with: " << token);
        return false;
    }

    return true;
}

bool parse(script& result_script, std::string format)
{
    boost::trim(format);

    if (format.empty())
        return true;

    data_chunk raw_hex;
    data_chunk raw_script;
    const auto tokens = split(format);
    for (const auto& token: tokens)
        if (!parse_token(raw_script, raw_hex, token))
            return false;

    parse_token(raw_script, raw_hex, sentinel);

    if (!result_script.from_data(raw_script, false))
        return false;

    if (result_script.empty())
        return false;

    return true;
}

transaction new_tx(const script_test& test, uint32_t sequence=0)
{
    script input_script;
    script output_script;

    if (!parse(input_script, test.input))
        return{};

    if (!parse(output_script, test.output))
        return{};

    input input;
    input.set_sequence(sequence);
    input.set_script(std::move(input_script));
    input.previous_output().validation.cache.set_script(std::move(output_script));

    transaction tx;
    tx.set_inputs({ std::move(input) });
    return tx;
}

BOOST_AUTO_TEST_SUITE(script_tests)

BOOST_AUTO_TEST_CASE(script__one_hash__literal__same)
{
    static const auto hash_one = hash_literal("0000000000000000000000000000000000000000000000000000000000000001");
    static const hash_digest one_hash{ { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
    BOOST_REQUIRE(one_hash == hash_one);
}

BOOST_AUTO_TEST_CASE(script__from_data__testnet_119058_invalid_op_codes__success)
{
    const auto raw_script = to_chunk(base16_literal("0130323066643366303435313438356531306633383837363437356630643265396130393739343332353534313766653139316438623963623230653430643863333030326431373463336539306366323433393231383761313037623634373337633937333135633932393264653431373731636565613062323563633534353732653302ae"));

    script parsed;
    BOOST_REQUIRE(parsed.from_data(raw_script, false));
}

BOOST_AUTO_TEST_CASE(script__from_data__parse__success)
{
    const auto raw_script = to_chunk(base16_literal("3045022100ff1fc58dbd608e5e05846a8e6b45a46ad49878aef6879ad1a7cf4c5a7f853683022074a6a10f6053ab3cddc5620d169c7374cd42c1416c51b9744db2c8d9febfb84d01"));

    script parsed;
    BOOST_REQUIRE(parsed.from_data(raw_script, true));
}

BOOST_AUTO_TEST_CASE(script__from_data__to_data__roundtrips)
{
    const auto normal_output_script = to_chunk(base16_literal("76a91406ccef231c2db72526df9338894ccf9355e8f12188ac"));

    script out_script;
    BOOST_REQUIRE(out_script.from_data(normal_output_script, false));

    const auto roundtrip = out_script.to_data(false);
    BOOST_REQUIRE(roundtrip == normal_output_script);
}

BOOST_AUTO_TEST_CASE(script__from_data__to_data_weird__roundtrips)
{
    const auto weird_raw_script = to_chunk(base16_literal(
        "0c49206c69656b20636174732e483045022100c7387f64e1f4"
        "cf654cae3b28a15f7572106d6c1319ddcdc878e636ccb83845"
        "e30220050ebf440160a4c0db5623e0cb1562f46401a7ff5b87"
        "7aa03415ae134e8c71c901534d4f0176519c6375522103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f72103ad6bb76e00d124f07a22680e39debd4dc4bd"
        "b1aa4b893720dd05af3c50560fdd52af67529c63552103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f721025098a1d5a338592bf1e015468ec5a8fafc1f"
        "c9217feb5cb33597f3613a2165e9210360cfabc01d52eaaeb3"
        "976a5de05ff0cfa76d0af42d3d7e1b4c233ee8a00655ed2103"
        "f571540c81fd9dbf9622ca00cfe95762143f2eab6b65150365"
        "bb34ac533160432102bc2b4be1bca32b9d97e2d6fb255504f4"
        "bc96e01aaca6e29bfa3f8bea65d8865855af672103ad6bb76e"
        "00d124f07a22680e39debd4dc4bdb1aa4b893720dd05af3c50"
        "560fddada820a4d933888318a23c28fb5fc67aca8530524e20"
        "74b1d185dbf5b4db4ddb0642848868685174519c6351670068"));

    script weird;
    BOOST_REQUIRE(weird.from_data(weird_raw_script, false));

    const auto roundtrip_result = weird.to_data(false);
    BOOST_REQUIRE(roundtrip_result == weird_raw_script);
}

BOOST_AUTO_TEST_CASE(script__factory_chunk_test)
{
    const auto raw = to_chunk(base16_literal("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));
    const auto instance = script::factory(raw, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__factory_stream_test)
{
    auto raw = to_chunk(base16_literal("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));
    data_source istream(raw);
    auto instance = script::factory(istream, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__factory_reader_test)
{
    auto raw = to_chunk(base16_literal("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));
    data_source istream(raw);
    istream_reader source(istream);
    const auto instance = script::factory(source, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__first_byte_invalid_wire_code__success)
{
    const auto raw = to_chunk(base16_literal(
        "bb566a54e38193e381aee4b896e7958ce381afe496e4babae381abe38288e381"
        "a3e381a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e3"
        "8292e8a8ade38191e381a6e381afe38184e381aae38184"));

    script instance;
    BOOST_REQUIRE(instance.from_data(raw, false));
}

BOOST_AUTO_TEST_CASE(script__from_data__internal_invalid_wire_code__success)
{
    const auto raw = to_chunk(base16_literal(
        "566a54e38193e381aee4b896e7958ce381afe4bb96e4babae381abe38288e381"
        "a3e381a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e3"
        "8292e8a8ade38191e381a6e381afe38184e381aae38184"));

    script instance;
    BOOST_REQUIRE(instance.from_data(raw, false));
}

BOOST_AUTO_TEST_CASE(script__empty__default__true)
{
    script instance;
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(script__empty__empty_operations__true)
{
    script instance(operation::list{});
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(script__empty__non_empty__false)
{
    script instance(script::to_null_data_pattern(data_chunk{ 42u }));
    BOOST_REQUIRE(!instance.empty());
}

BOOST_AUTO_TEST_CASE(script__clear__non_empty__empty)
{
    script instance(script::to_null_data_pattern(data_chunk{ 42u }));
    BOOST_REQUIRE(!instance.empty());

    instance.clear();
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(script__native__block_438513_tx__valid)
{
    ////06:21:05.532171 DEBUG [blockchain] Input validation failed (stack false)
    //// lib         : false
    //// forks       : 62
    //// outpoint    : 8e51d775e0896e03149d585c0655b3001da0c55068b0885139ac6ec34cf76ba0:0
    //// script      : a914faa558780a5767f9e3be14992a578fc1cbcf483087
    //// inpoint     : 6b7f50afb8448c39f4714a73d2b181d3e3233e84670bdfda8f141db668226c54:0
    //// transaction : 0100000001a06bf74cc36eac395188b06850c5a01d00b355065c589d14036e89e075d7518e000000009d483045022100ba555ac17a084e2a1b621c2171fa563bc4fb75cd5c0968153f44ba7203cb876f022036626f4579de16e3ad160df01f649ffb8dbf47b504ee56dc3ad7260af24ca0db0101004c50632102768e47607c52e581595711e27faffa7cb646b4f481fe269bd49691b2fbc12106ad6704355e2658b1756821028a5af8284a12848d69a25a0ac5cea20be905848eb645fd03d3b065df88a9117cacfeffffff0158920100000000001976a9149d86f66406d316d44d58cbf90d71179dd8162dd388ac355e2658

    static const auto input_index = 0u;
    static const auto forks = static_cast<rule_fork>(62);
    static const auto encoded_script = "a914faa558780a5767f9e3be14992a578fc1cbcf483087";
    static const auto encoded_tx = "0100000001a06bf74cc36eac395188b06850c5a01d00b355065c589d14036e89e075d7518e000000009d483045022100ba555ac17a084e2a1b621c2171fa563bc4fb75cd5c0968153f44ba7203cb876f022036626f4579de16e3ad160df01f649ffb8dbf47b504ee56dc3ad7260af24ca0db0101004c50632102768e47607c52e581595711e27faffa7cb646b4f481fe269bd49691b2fbc12106ad6704355e2658b1756821028a5af8284a12848d69a25a0ac5cea20be905848eb645fd03d3b065df88a9117cacfeffffff0158920100000000001976a9149d86f66406d316d44d58cbf90d71179dd8162dd388ac355e2658";

    data_chunk decoded_tx;
    BOOST_REQUIRE(decode_base16(decoded_tx, encoded_tx));

    data_chunk decoded_script;
    BOOST_REQUIRE(decode_base16(decoded_script, encoded_script));

    transaction tx;
    BOOST_REQUIRE(tx.from_data(decoded_tx));

    auto& input = tx.inputs()[input_index];
    auto& prevout = input.previous_output().validation.cache;

    prevout.set_script(script::factory(decoded_script, false));
    BOOST_REQUIRE(prevout.script().is_valid());

    // [3045022100ba555ac17a084e2a1b621c2171fa563bc4fb75cd5c0968153f44ba7203cb876f022036626f4579de16e3ad160df01f649ffb8dbf47b504ee56dc3ad7260af24ca0db01] [00] [632102768e47607c52e581595711e27faffa7cb646b4f481fe269bd49691b2fbc12106ad6704355e2658b1756821028a5af8284a12848d69a25a0ac5cea20be905848eb645fd03d3b065df88a9117cac]
    ////std::cout << prevout.script().to_string(forks) << std::endl;

    // hash160 [faa558780a5767f9e3be14992a578fc1cbcf4830] equal
    ////std::cout << input.script().to_string(forks) << std::endl;

    const auto result = script::verify(tx, input_index, forks);
    BOOST_REQUIRE_EQUAL(result.value(), error::success);
}

BOOST_AUTO_TEST_CASE(script__bip16__valid)
{
    for (const auto& test: valid_bip16_scripts)
    {
        std::string name = test.input + " : " + test.output;
        const auto tx = new_tx(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are valid prior to and after BIP16 activation.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip16_rule) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) == error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip16__invalidated)
{
    for (const auto& test: invalidated_bip16_scripts)
    {
        std::string name = test.input + " : " + test.output;
        const auto tx = new_tx(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are valid prior to BIP16 activation and invalid after.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip16_rule) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) != error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__valid)
{
    for (const auto& test: valid_bip65_scripts)
    {
        std::string name = test.input + " : " + test.output;
        auto tx = new_tx(test, 42);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        tx.set_locktime(99);

        // These are valid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip65_rule) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) == error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__invalid)
{
    for (const auto& test: invalid_bip65_scripts)
    {
        std::string name = test.input + " : " + test.output;
        auto tx = new_tx(test, 42);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        tx.set_locktime(99);

        // These are invalid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip65_rule) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) != error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__invalidated)
{
    for (const auto& test: invalidated_bip65_scripts)
    {
        std::string name = test.input + " : " + test.output;
        auto tx = new_tx(test, 42);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        tx.set_locktime(99);

        // These are valid prior to BIP65 activation and invalid after.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip65_rule) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) != error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__valid)
{
    for (const auto& test: valid_multisig_scripts)
    {
        std::string name = test.input + " : " + test.output;
        auto tx = new_tx(test, 42);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are always valid.
        // These are scripts potentially affected by bip66 (but should not be).
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip66_rule) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) == error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__invalid)
{
    for (const auto& test: invalid_multisig_scripts)
    {
        std::string name = test.input + " : " + test.output;
        const auto tx = new_tx(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are always invalid.
        // These are scripts potentially affected by bip66 (but should not be).
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::bip66_rule) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) != error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__context_free__valid)
{
    for (const auto& test: valid_context_free_scripts)
    {
        std::string name = test.input + " : " + test.output;
        const auto tx = new_tx(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are always valid.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) == error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) == error::success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__context_free__invalid)
{
    for (const auto& test: invalid_context_free_scripts)
    {
        std::string name = test.input + " : " + test.output;
        const auto tx = new_tx(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_REQUIRE_MESSAGE(!tx.inputs().empty(), name);

        // These are always invalid.
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::no_rules) != error::success, name);
        BOOST_CHECK_MESSAGE(script::verify(tx, 0, rule_fork::all_rules) != error::success, name);
    }
}

// These are special tests for checksig.
BOOST_AUTO_TEST_CASE(script__checksig__single__uses_one_hash)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:1
    data_chunk tx_data;
    decode_base16(tx_data, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    transaction parent_tx;
    BOOST_REQUIRE(parent_tx.from_data(tx_data));

    data_chunk distinguished;
    decode_base16(distinguished, "30450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df");

    data_chunk pubkey;
    decode_base16(pubkey, "0275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cb");

    data_chunk script_data;
    decode_base16(script_data, "76a91433cef61749d11ba2adf091a5e045678177fe3a6d88ac");

    script script_code;
    BOOST_REQUIRE(script_code.from_data(script_data, false));

    ec_signature signature;
    static const auto strict = true;
    static const uint32_t input_index = 1;
    BOOST_REQUIRE(parse_signature(signature, distinguished, strict));
    BOOST_REQUIRE(script::check_signature(signature, sighash_algorithm::single, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_CASE(script__checksig__normal__success)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:0
    data_chunk tx_data;
    decode_base16(tx_data, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    transaction parent_tx;
    BOOST_REQUIRE(parent_tx.from_data(tx_data));

    data_chunk distinguished;
    decode_base16(distinguished, "304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27");

    data_chunk pubkey;
    decode_base16(pubkey, "02100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2fe");

    data_chunk script_data;
    decode_base16(script_data, "76a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac");

    script script_code;
    BOOST_REQUIRE(script_code.from_data(script_data, false));

    ec_signature signature;
    static const uint32_t input_index = 0;
    BOOST_REQUIRE(parse_signature(signature, distinguished, true));
    BOOST_REQUIRE(script::check_signature(signature, sighash_algorithm::single, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_CASE(script__create_endorsement__single_input_single_output__expected)
{
    data_chunk tx_data;
    decode_base16(tx_data, "0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970100000000ffffffff01905f0100000000001976a91418c0bd8d1818f1bf99cb1df2269c645318ef7b7388ac00000000");
    transaction new_tx;
    BOOST_REQUIRE(new_tx.from_data(tx_data));

    script prevout_script;
    BOOST_REQUIRE(prevout_script.from_string("dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig"));

    const ec_secret secret = hash_literal("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");

    endorsement out;
    const uint32_t input_index = 0;
    const auto sighash_type = sighash_algorithm::all;
    BOOST_REQUIRE(script::create_endorsement(out, secret, prevout_script, new_tx, input_index, sighash_type));

    const auto result = encode_base16(out);
    const auto expected = "3045022100e428d3cc67a724cb6cfe8634aa299e58f189d9c46c02641e936c40cc16c7e8ed0220083949910fe999c21734a1f33e42fca15fb463ea2e08f0a1bccd952aacaadbb801";
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(script__create_endorsement__single_input_no_output__expected)
{
    data_chunk tx_data;
    decode_base16(tx_data, "0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970000000000ffffffff0000000000");
    transaction new_tx;
    BOOST_REQUIRE(new_tx.from_data(tx_data));

    script prevout_script;
    BOOST_REQUIRE(prevout_script.from_string("dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig"));

    const ec_secret secret = hash_literal("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");

    endorsement out;
    const uint32_t input_index = 0;
    const auto sighash_type = sighash_algorithm::all;
    BOOST_REQUIRE(script::create_endorsement(out, secret, prevout_script, new_tx, input_index, sighash_type));

    const auto result = encode_base16(out);
    const auto expected = "3045022100ba57820be5f0b93a0d5b880fbf2a86f819d959ecc24dc31b6b2d4f6ed286f253022071ccd021d540868ee10ca7634f4d270dfac7aea0d5912cf2b104111ac9bc756b01";
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(script__generate_signature_hash__all__expected)
{
    data_chunk tx_data;
    decode_base16(tx_data, "0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970000000000ffffffff0000000000");
    transaction new_tx;
    BOOST_REQUIRE(new_tx.from_data(tx_data));

    script prevout_script;
    BOOST_REQUIRE(prevout_script.from_string("dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig"));

    endorsement out;
    const uint32_t input_index = 0;
    const auto sighash_type = sighash_algorithm::all;
    const auto sighash = script::generate_signature_hash(new_tx, input_index, prevout_script, sighash_type);
    const auto result = encode_base16(sighash);
    const auto expected = "f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0";
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
