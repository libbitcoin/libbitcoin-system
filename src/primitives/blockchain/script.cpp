/*
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
#include <bitcoin/bitcoin/primitives/blockchain/script.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>

namespace libbitcoin {

script_type::script_type()
{
}

script_type::script_type(const data_chunk& value, bool allow_raw_data_fallback)
{
    deserialize(value, allow_raw_data_fallback);
}

script_type::script_type(const operation& op)
{
    operations.push_back(op);
}

script_type::script_type(const std::string& human_readable)
{
    std::vector<std::string> tokens;

    boost::split(tokens, human_readable, boost::is_any_of(" "),
        boost::token_compress_on);

    bool clear = false;

    for (auto token = tokens.begin(); (token != tokens.end()); ++token)
    {
        operation op;

        if (*token == "[")
        {
            data_chunk data;

            if (!decode_base16(data, *++token))
            {
                clear = true;
                break;
            }

            if (data.empty() || *++token != "]")
            {
                clear = true;
                break;
            }

            op.code = data_to_opcode(data);

            if (op.code == opcode::bad_operation)
            {
                clear = true;
                break;
            }

            op.data = data;
        }
        else
        {
            op.code = string_to_opcode(*token);
        }

        operations.push_back(op);
    }

    if (clear)
    {
        operations.clear();
    }
}

script_type::operator const data_chunk() const
{
    data_chunk result;

    if ((operations.size() > 0) && (operations[0].code == opcode::raw_data))
    {
        result = operations[0];
    }
    else
    {
        for (const operation& op: operations)
        {
            data_chunk raw_op = op;
            extend_data(result, raw_op);
        }
    }

    return result;
}

size_t script_type::satoshi_size() const
{
    size_t size = 0;

    if (operations.size() > 0 && (operations[0].code == opcode::raw_data))
    {
        size = operations[0].satoshi_size();
    }
    else
    {
        for (const operation& op: operations)
        {
            size += op.satoshi_size();
        }
    }

    return size;
}

std::string script_type::to_string() const
{
    std::ostringstream ss;

    for (auto it = operations.begin(); it != operations.end(); ++it)
    {
        if (it != operations.begin())
            ss << " ";

        ss << (*it).to_string();
    }

    return ss.str();
}

payment_type script_type::type() const
{
    if (is_pubkey_type(operations))
        return payment_type::pubkey;
    if (is_pubkey_hash_type(operations))
        return payment_type::pubkey_hash;
    if (is_script_hash_type(operations))
        return payment_type::script_hash;
    if (is_stealth_info_type(operations))
        return payment_type::stealth_info;
    if (is_multisig_type(operations))
        return payment_type::multisig;
    if (is_pubkey_hash_sig_type(operations))
        return payment_type::pubkey_hash_sig;
    if (is_script_code_sig_type(operations))
        return payment_type::script_code_sig;
    return payment_type::non_standard;
}

void script_type::deserialize(data_chunk raw_script,
    bool allow_raw_data_fallback)
{
    try
    {
        parse(raw_script);
    }
    catch (end_of_stream eos)
    {
        if (!allow_raw_data_fallback)
        {
            throw eos;
        }

        // recognize as raw data
        operations.clear();
        operation op;
        op.code = opcode::raw_data;
        op.data = to_data_chunk(raw_script);
        operations.push_back(op);
    }
}

void script_type::parse(data_chunk raw_script)
{
    auto deserial = make_deserializer(raw_script.begin(), raw_script.end());

    while (deserial.iterator() != raw_script.end())
    {
        operation op(deserial);
        operations.push_back(op);
    }
}

}
