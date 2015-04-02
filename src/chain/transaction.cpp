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
#include <bitcoin/bitcoin/chain/transaction.hpp>

#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace chain {

transaction::transaction()
{
}

transaction::transaction(const data_chunk& value)
: transaction(value.begin(), value.end())
{
}

transaction::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version);

    serial.write_variable_uint(inputs.size());

    for (const transaction_input& input: inputs)
    {
        serial.write_hash(input.previous_output.hash);
        serial.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = input.script;
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
        serial.write_4_bytes(input.sequence);
    }

    serial.write_variable_uint(outputs.size());

    for (const transaction_output& output: outputs)
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

size_t transaction::satoshi_size() const
{
    size_t tx_size = 8;

    tx_size += variable_uint_size(inputs.size());

    for (const transaction_input& input: inputs)
    {
        tx_size += input.satoshi_size();
    }

    tx_size += variable_uint_size(outputs.size());

    for (const transaction_output& output: outputs)
    {
        tx_size += output.satoshi_size();
    }

    return tx_size;
}

std::string transaction::to_string() const
{
    std::ostringstream ss;

    ss << "Transaction:\n"
        << "\tversion = " << version << "\n"
        << "\tlocktime = " << locktime << "\n"
        << "Inputs:\n";

    for (transaction_input input: inputs)
        ss << input.to_string();

    ss << "Outputs:\n";

    for (transaction_output output: outputs)
        ss << output.to_string();

    ss << "\n";

    return ss.str();
}

hash_digest transaction::hash() const
{
    data_chunk serialized = *this;
    return bitcoin_hash(serialized);
}

hash_digest transaction::hash(uint32_t hash_type_code) const
{
    data_chunk serialized = *this;
    extend_data(serialized, to_little_endian(hash_type_code));
    return bitcoin_hash(serialized);
}

bool transaction::is_coinbase() const
{
    return (inputs.size() == 1) && inputs[0].previous_output.is_null();
}

bool transaction::is_final(size_t block_height, uint32_t block_time) const
{
    if (locktime == 0)
        return true;

    uint32_t max_locktime = block_time;

    if (locktime < locktime_threshold)
        max_locktime = static_cast<uint32_t>(block_height);

    if (locktime < max_locktime)
        return true;

    for (const transaction_input& tx_input : inputs)
    {
        if (!tx_input.is_final())
            return false;
    }

    return true;
}

bool transaction::is_locktime_conflict() const
{
    auto locktime_set = locktime != 0;

    if (locktime_set)
    {
        for (const auto& input : inputs)
        {
            if (input.sequence < max_sequence)
                return false;
        }
    }

    return locktime_set;
}

uint64_t transaction::total_output_value() const
{
    uint64_t total = 0;

    for (const transaction_output& output : outputs)
        total += output.value;

    return total;
}

} // end chain
} // end libbitcoin
