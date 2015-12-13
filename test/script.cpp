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
#include "script.hpp"

#include <ctype.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

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

opcode token_to_opcode(const std::string& token)
{
    std::string lower_token = token;
    boost::algorithm::to_lower(lower_token);
    return string_to_opcode(lower_token);
}

bool is_opcode(const std::string& token)
{
    return token_to_opcode(token) != opcode::bad_operation;
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
            raw_script.push_back(static_cast<uint8_t>(opcode::negative_1));
            return;

#define PUSH_X(n) \
        case n: \
            raw_script.push_back(static_cast<uint8_t>(opcode::op_##n)); \
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
    operation op;
    // pushdata1 = 76
    if (data.empty())
        op.code = opcode::zero;
    else if (data.size() < 76)
        op.code = opcode::special;
    else if (data.size() <= 0xff)
        op.code = opcode::pushdata1;
    else if (data.size() <= 0xffff)
        op.code = opcode::pushdata2;
    else
    {
        BOOST_REQUIRE_LE(data.size(), 0xffffffffu);
        op.code = opcode::pushdata4;
    }
    op.data = data;
    script_type script;
    script.push_operation(op);
    extend_data(raw_script, save_script(script));
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
        opcode tokenized_opcode = token_to_opcode(token);
        raw_script.push_back(static_cast<uint8_t>(tokenized_opcode));
    }
    else
    {
        log_error() << "Token parsing failed with: " << token;
        return false;
    }
    return true;
}

bool parse(script_type& result_script, std::string format)
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

    try
    {
        result_script = parse_script(raw_script);
    }
    catch (end_of_stream)
    {
        return false;
    }

    return !result_script.operations().empty();
}

bool run_script(const script_test& test, const transaction_type& tx,
    uint32_t context)
{
    script_type input;
    script_type output;

    if (!parse(input, test.input))
        return false;

    if (!parse(output, test.output))
        return false;

    ////std::cout << test.input << " -> " << input << std::endl;
    ////std::cout << test.output << " -> " << output << std::endl;

    return output.run(input, tx, 0, context);
}

void ignore_output(log_level, const std::string&, const std::string&)
{
}

BOOST_AUTO_TEST_SUITE(script_tests)

// Valid pay-to-script-hash scripts are valid regardless of context,
// however after bip16 activation the scripts have additional constraints.
BOOST_AUTO_TEST_CASE(script_bip16_valid)
{
    transaction_type tx;
    for (const auto& test: valid_bip16_scripts)
    {
        // These are valid prior to and after BIP16 activation.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::bip16_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::all_enabled), test.description);
    }
}

BOOST_AUTO_TEST_CASE(script_bip16_invalidated)
{
    transaction_type tx;
    for (const auto& test: invalidated_bip16_scripts)
    {
        // These are valid prior to BIP16 activation and invalid after.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::bip16_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::all_enabled), test.description);
    }
}

// Prior to bip65 activation op_nop2 always returns true, but after it becomes a locktime comparer.
BOOST_AUTO_TEST_CASE(script_bip65_valid)
{
    transaction_type tx;
    tx.locktime = 500000042;
    transaction_input_type input;
    input.sequence = 42;
    tx.inputs.push_back(input);

    for (const auto& test: valid_bip65_scripts)
    {
        // These are valid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::bip65_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::all_enabled), test.description);
    }
}

BOOST_AUTO_TEST_CASE(script_bip65_invalid)
{
    transaction_type tx;
    tx.locktime = 99;
    transaction_input_type input;
    input.sequence = 42;
    tx.inputs.push_back(input);

    for (const auto& test: invalid_bip65_scripts)
    {
        // These are invalid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::bip65_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::all_enabled), test.description);
    }
}

BOOST_AUTO_TEST_CASE(script_bip65_invalidated)
{
    transaction_type tx;
    tx.locktime = 99;
    transaction_input_type input;
    input.sequence = 42;
    tx.inputs.push_back(input);

    for (const auto& test : invalidated_bip65_scripts)
    {
        // These are valid prior to BIP65 activation and invalid after.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::bip65_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::all_enabled), test.description);
    }
}

// These are scripts potentially affected by bip66 (but should not be).
BOOST_AUTO_TEST_CASE(script_multisig_valid)
{
    transaction_type tx;
    for (const auto& test: valid_multisig_scripts)
    {
        // These are always valid.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::bip66_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::all_enabled), test.description);
    }
}

// These are scripts potentially affected by bip66 (but should not be).
BOOST_AUTO_TEST_CASE(script_multisig_invalid)
{
    transaction_type tx;
    for (const auto& test: invalid_multisig_scripts)
    {
        // These are always invalid.
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::bip66_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::all_enabled), test.description);
    }
}

BOOST_AUTO_TEST_CASE(script_context_free_valid)
{
    transaction_type tx;
    for (const auto& test: valid_context_free_scripts)
    {
        // These are always valid.
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(run_script(test, tx, script_context::all_enabled), test.description);
    }
}

BOOST_AUTO_TEST_CASE(script_context_free_invalid)
{
    transaction_type tx;
    for (const auto& test: invalid_context_free_scripts)
    {
        // These are always invalid.
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::none_enabled), test.description);
        BOOST_CHECK_MESSAGE(!run_script(test, tx, script_context::all_enabled), test.description);
    }
}

// These are special tests for checksig.
BOOST_AUTO_TEST_CASE(script_checksig_uses_one_hash)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:1
    data_chunk txdat;
    decode_base16(txdat, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    transaction_type parent_tx;
    satoshi_load(txdat.begin(), txdat.end(), parent_tx);
    uint32_t input_index = 1;

    data_chunk distinguished;
    decode_base16(distinguished, "30450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df");
    data_chunk pubkey;
    decode_base16(pubkey, "0275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cb");

    script_type script_code;
    data_chunk rawscr;
    decode_base16(rawscr, "76a91433cef61749d11ba2adf091a5e045678177fe3a6d88ac");
    script_code = parse_script(rawscr);

    ec_signature signature;
    BOOST_REQUIRE(parse_signature(signature, distinguished, false));
    BOOST_REQUIRE(check_ec_signature(signature, sighash::single, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_CASE(script_checksig_normal)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:0
    data_chunk txdat;
    decode_base16(txdat, "0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    transaction_type parent_tx;
    satoshi_load(txdat.begin(), txdat.end(), parent_tx);
    uint32_t input_index = 0;

    data_chunk distinguished;
    decode_base16(distinguished, "304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27");
    data_chunk pubkey;
    decode_base16(pubkey, "02100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2fe");

    script_type script_code;
    data_chunk rawscr;
    decode_base16(rawscr, "76a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac");
    script_code = parse_script(rawscr);

    ec_signature signature;
    BOOST_REQUIRE(parse_signature(signature, distinguished, false));
    BOOST_REQUIRE(check_ec_signature(signature, sighash::single, pubkey, script_code, parent_tx, input_index));
}

BOOST_AUTO_TEST_SUITE_END()
