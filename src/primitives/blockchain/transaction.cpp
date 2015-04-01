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
#include <bitcoin/bitcoin/primitives/blockchain/transaction.hpp>
#include <sstream>

namespace libbitcoin {

transaction_type::transaction_type()
{
}

transaction_type::transaction_type(const data_chunk& value)
: transaction_type(value.begin(), value.end())
{
}

transaction_type::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version);

    serial.write_variable_uint(inputs.size());

    for (const transaction_input_type& input: inputs)
    {
        serial.write_hash(input.previous_output.hash);
        serial.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = input.script;
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
        serial.write_4_bytes(input.sequence);
    }

    serial.write_variable_uint(outputs.size());

    for (const transaction_output_type& output: outputs)
    {
        serial.write_8_bytes(output.value);
        data_chunk raw_script = output.script;
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
    }

    serial.write_4_bytes(locktime);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

size_t transaction_type::satoshi_size() const
{
    size_t tx_size = 8;

    tx_size += variable_uint_size(inputs.size());

    for (const transaction_input_type& input: inputs)
    {
        tx_size += input.satoshi_size();
    }

    tx_size += variable_uint_size(outputs.size());

    for (const transaction_output_type& output: outputs)
    {
        tx_size += output.satoshi_size();
    }

    return tx_size;
}

std::string transaction_type::to_string() const
{
    std::ostringstream ss;

    ss << "Transaction:\n"
        << "\tversion = " << version << "\n"
        << "\tlocktime = " << locktime << "\n"
        << "Inputs:\n";

    for (transaction_input_type input: inputs)
        ss << input.to_string();

    ss << "Outputs:\n";

    for (transaction_output_type output: outputs)
        ss << output.to_string();

    ss << "\n";

    return ss.str();
}

}
