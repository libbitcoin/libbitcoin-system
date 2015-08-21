/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <ctype.h>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "script.hpp"

using namespace bc;

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
    return boost::all(hex_part, [](char c) { return isxdigit(c); });
}

bool is_quoted_string(const std::string& token)
{
    if (token.size() < 2)
        return false;
    return boost::starts_with(token, "'") && boost::ends_with(token, "'");
}

chain::opcode token_to_opcode(const std::string& token)
{
    std::string lower_token = token;
    boost::algorithm::to_lower(lower_token);
    return chain::string_to_opcode(lower_token);
}

bool is_opcode(const std::string& token)
{
    return token_to_opcode(token) != chain::opcode::bad_operation;
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
            raw_script.push_back(static_cast<uint8_t>(chain::opcode::negative_1));
            return;

#define PUSH_X(n) \
        case n: \
            raw_script.push_back(static_cast<uint8_t>(chain::opcode::op_##n)); \
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
    chain::opcode code;

    // pushdata1 = 76
    if (data.empty())
        code = chain::opcode::zero;
    else if (data.size() < 76)
        code = chain::opcode::special;
    else if (data.size() <= 0xff)
        code = chain::opcode::pushdata1;
    else if (data.size() <= 0xffff)
        code = chain::opcode::pushdata2;
    else
    {
        BOOST_REQUIRE_LE(data.size(), 0xffffffffu);
        code = chain::opcode::pushdata4;
    }

    chain::script tmp_script;
    tmp_script.operations.push_back(chain::operation{ code, data });
    data_chunk raw_tmp_script = tmp_script.to_data(false);
    extend_data(raw_script, raw_tmp_script);
}

bool parse_token(data_chunk& raw_script, std::string token)
{
    boost::trim(token);
    // skip this
    if (token.empty())
        return true;
    static data_chunk hex_raw;
    if (token == "ENDING" || !is_hex_data(token))
    {
        if (!hex_raw.empty())
        {
            extend_data(raw_script, hex_raw);
            hex_raw.resize(0);
        }
    }
    if (token == "ENDING")
    {
        // Do nothing...
    }
    else if (is_number(token))
    {
        int64_t value = boost::lexical_cast<int64_t>(token);
        if (is_opx(value))
            push_literal(raw_script, value);
        else
        {
            script_number bignum(value);
            push_data(raw_script, bignum.data());
        }
    }
    else if (is_hex_data(token))
    {
        std::string hex_part(token.begin() + 2, token.end());
        data_chunk raw_data;
        if (!decode_base16(raw_data, hex_part))
            return false;
        extend_data(hex_raw, raw_data);
    }
    else if (is_quoted_string(token))
    {
        data_chunk inner_value(token.begin() + 1, token.end() - 1);
        push_data(raw_script, inner_value);
    }
    else if (is_opcode(token))
    {
        chain::opcode tokenized_opcode = token_to_opcode(token);
        raw_script.push_back(static_cast<uint8_t>(tokenized_opcode));
    }
    else
    {
        log_error() << "Token parsing failed with: " << token;
        return false;
    }
    return true;
}

bool parse(chain::script& result_script, std::string format)
{
    boost::trim(format);
    if (format.empty())
        return true;
    const auto tokens = split(format);
    data_chunk raw_script;
    for (const auto& token: tokens)
        if (!parse_token(raw_script, token))
            return false;
    parse_token(raw_script, "ENDING");

    if (!result_script.from_data(raw_script, false, chain::script::parse_mode::strict))
        return false;

    if (result_script.operations.empty())
        return false;

    return true;
}

bool run_script(const script_test& test)
{
    chain::script input, output;

    if (!parse(input, test.input))
        return false;

    if (!parse(output, test.output))
        return false;

    chain::transaction tx;
    //log_debug() << test.input << " -> " << input;
    //log_debug() << test.output << " -> " << output;
    return chain::script::verify(input, output, tx, 0);
}

void ignore_output(log_level,
    const std::string&, const std::string&)
{
}

BOOST_AUTO_TEST_SUITE(script_tests)

BOOST_AUTO_TEST_CASE(from_data_fails_parse)
{
    data_chunk raw_script = to_data_chunk(base16_literal(
        "3045022100ff1fc58dbd608e5e05846a8e6b45a46ad49878aef6879ad1a7cf4c"
        "5a7f853683022074a6a10f6053ab3cddc5620d169c7374cd42c1416c51b9744d"
        "b2c8d9febfb84d01"));

    chain::script psc;
    BOOST_REQUIRE_EQUAL(false, psc.from_data(raw_script, true,
        chain::script::parse_mode::strict));
}

BOOST_AUTO_TEST_CASE(from_data_to_data_roundtrip)
{
    data_chunk normal_output_script = to_data_chunk(base16_literal(
        "76a91406ccef231c2db72526df9338894ccf9355e8f12188ac"));

    chain::script out_scr;
    BOOST_REQUIRE(out_scr.from_data(normal_output_script, false,
        chain::script::parse_mode::raw_data_fallback));

    data_chunk roundtrip = out_scr.to_data(false);
    BOOST_REQUIRE(roundtrip == normal_output_script);
}

BOOST_AUTO_TEST_CASE(from_data_to_data_roundtrip_weird)
{
    data_chunk weird_raw_script = to_data_chunk(base16_literal(
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

    chain::script weird;
    BOOST_REQUIRE(weird.from_data(weird_raw_script, false,
        chain::script::parse_mode::raw_data_fallback));

    data_chunk roundtrip_result = weird.to_data(false);
    BOOST_REQUIRE(roundtrip_result == weird_raw_script);
}

BOOST_AUTO_TEST_CASE(json_valid)
{
    for (const script_test& test: valid_scripts)
    {
        BOOST_REQUIRE(run_script(test));
    }
}

BOOST_AUTO_TEST_CASE(json_invalid)
{
    // Shut up!
    log_fatal().set_output_function(ignore_output);
    size_t count = 0;

    for (const script_test& test: invalid_scripts)
    {
        BOOST_REQUIRE(!run_script(test));
        count++;
    }
}

BOOST_AUTO_TEST_CASE(script_checksig_uses_one_hash)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:1
    data_chunk raw_tx;
    BOOST_REQUIRE(decode_base16(raw_tx, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000"));

    chain::transaction parent_tx;
    BOOST_REQUIRE(parent_tx.from_data(raw_tx));

    uint32_t input_index = 1;
    data_chunk signature;
    decode_base16(signature, "30450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03");

    data_chunk pubkey;
    decode_base16(pubkey, "0275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cb");

    data_chunk raw_script;
    decode_base16(raw_script, "76a91433cef61749d11ba2adf091a5e045678177fe3a6d88ac");

    chain::script script_code;
    BOOST_REQUIRE(script_code.from_data(raw_script, false,
        chain::script::parse_mode::raw_data_fallback));

    BOOST_REQUIRE(chain::script::check_signature(
        signature, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_CASE(script_checksig_normal)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:0
    data_chunk raw_tx;
    decode_base16(raw_tx, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");

    chain::transaction parent_tx;
    BOOST_REQUIRE(parent_tx.from_data(raw_tx));

    uint32_t input_index = 0;
    data_chunk signature;
    decode_base16(signature, "304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c2703");

    data_chunk pubkey;
    decode_base16(pubkey, "02100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2fe");

    data_chunk raw_script;
    decode_base16(raw_script, "76a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac");

    chain::script script_code;
    BOOST_REQUIRE_EQUAL(true, script_code.from_data(raw_script, false,
        chain::script::parse_mode::raw_data_fallback));

    BOOST_REQUIRE(chain::script::check_signature(
        signature, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_CASE(is_raw_data_operations_size_not_equal_one_returns_false)
{
    chain::script instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_raw_data());
}

BOOST_AUTO_TEST_CASE(is_raw_data_code_not_equal_raw_data_returns_false)
{
    chain::script instance;
    instance.operations.emplace_back();
    instance.operations.back().code = chain::opcode::vernotif;
    BOOST_REQUIRE_EQUAL(false, instance.is_raw_data());
}

BOOST_AUTO_TEST_CASE(is_raw_data_returns_true)
{
    chain::script instance;
    instance.operations.emplace_back();
    instance.operations.back().code = chain::opcode::raw_data;
    BOOST_REQUIRE_EQUAL(true, instance.is_raw_data());
}

BOOST_AUTO_TEST_CASE(factory_from_data_chunk_test)
{
    auto raw = to_data_chunk(base16_literal(
        "76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));

    auto instance = chain::script::factory_from_data(
        raw, false, chain::script::parse_mode::strict);

    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(factory_from_data_stream_test)
{
    auto raw = to_data_chunk(base16_literal(
        "76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));
    boost::iostreams::stream<byte_source<data_chunk>> istream(raw);

    auto instance = chain::script::factory_from_data(
        istream, false, chain::script::parse_mode::strict);

    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(factory_from_data_reader_test)
{
    auto raw = to_data_chunk(base16_literal(
        "76a914fc7b44566256621affb1541cc9d59f08336d276b88ac"));
    boost::iostreams::stream<byte_source<data_chunk>> istream(raw);
    istream_reader source(istream);

    auto instance = chain::script::factory_from_data(
        source, false, chain::script::parse_mode::strict);

    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_SUITE_END()
