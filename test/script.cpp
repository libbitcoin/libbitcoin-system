/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma warning(push)
#pragma warning(disable: 4996)
// choorucode.com/2010/08/30/visual-c-c4996-warning-on-copy-with-array-parameters
#include <boost/test/unit_test.hpp>
#pragma warning(pop)
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin.hpp>
#include "script_json/vectors.hpp"

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
    return boost::all(hex_part, [](char c) { return std::isxdigit(c); });
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
        BITCOIN_ASSERT(data.size() <= 0xffffffff);
        op.code = opcode::pushdata4;
    }
    op.data = data;
    script_type tmp_script;
    tmp_script.push_operation(op);
    extend_data(raw_script, save_script(tmp_script));
}

bool parse_token(data_chunk& raw_script, std::string token)
{
    boost::algorithm::trim(token);
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
            big_number bignum;
            bignum.set_int64(value);
            push_data(raw_script, bignum.data());
        }
    }
    else if (is_hex_data(token))
    {
        std::string hex_part(token.begin() + 2, token.end());
        data_chunk raw_data = decode_hex(hex_part);
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
    boost::algorithm::trim(format);
    if (format.empty())
        return true;
    std::vector<std::string> tokens;
    boost::split(tokens, format, boost::is_any_of(" "));
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
    if (result_script.operations().empty())
        return false;
    return true;
}

bool run_script(const script_test& test)
{
    script_type input, output;
    if (!parse(input, test.input))
        return false;
    if (!parse(output, test.output))
        return false;
    transaction_type tx;
    //log_debug() << test.input << " -> " << input;
    //log_debug() << test.output << " -> " << output;
    return output.run(input, tx, 0);
}

BOOST_AUTO_TEST_SUITE(script_tests)

BOOST_AUTO_TEST_CASE(script_json_valid)
{
    for (const script_test& test: valid_scripts)
    {
        BOOST_REQUIRE(run_script(test));
    }
}

void ignore_output(log_level,
    const std::string&, const std::string&)
{
}

BOOST_AUTO_TEST_CASE(script_json_invalid)
{
    // Shut up!
    log_fatal().set_output_function(ignore_output);
    for (const script_test& test: invalid_scripts)
    {
        BOOST_REQUIRE(!run_script(test));
    }
}

BOOST_AUTO_TEST_SUITE_END()

