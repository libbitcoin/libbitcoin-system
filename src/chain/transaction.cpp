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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

void transaction::reset()
{
    version = 0;
    locktime = 0;
    inputs.clear();
    outputs.clear();
}

bool transaction::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool transaction::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    version = read_4_bytes(stream);
    result = stream;

    if (result)
    {
        uint64_t tx_in_count = read_variable_uint(stream);
        result = stream;

        for (uint64_t i = 0; (i < tx_in_count) && result; ++i)
        {
            inputs.emplace_back();
            result = inputs.back().from_data(stream);
        }
    }

    if (result)
    {
        uint64_t tx_out_count = read_variable_uint(stream);
        result = stream;

        for (uint64_t i = 0; (i < tx_out_count) && result; ++i)
        {
            outputs.emplace_back();
            result = outputs.back().from_data(stream);
        }
    }

    if (result)
    {
        locktime = read_4_bytes(stream);
        result = stream;
    }

    if (!result)
        reset();

    return result;
}

data_chunk transaction::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version);

    serial.write_variable_uint(inputs.size());

    for (const transaction_input& input: inputs)
        serial.write_data(input.to_data());

    serial.write_variable_uint(outputs.size());

    for (const transaction_output& output: outputs)
        serial.write_data(output.to_data());

    serial.write_4_bytes(locktime);

    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

uint64_t transaction::satoshi_size() const
{
    uint64_t tx_size = 8;

    tx_size += variable_uint_size(inputs.size());

    for (const transaction_input& input: inputs)
        tx_size += input.satoshi_size();

    tx_size += variable_uint_size(outputs.size());

    for (const transaction_output& output: outputs)
        tx_size += output.satoshi_size();

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
    return bitcoin_hash(to_data());
}

hash_digest transaction::hash(uint32_t hash_type_code) const
{
    data_chunk serialized = to_data();
    extend_data(serialized, to_little_endian(hash_type_code));
    return bitcoin_hash(serialized);
}

bool transaction::is_coinbase() const
{
    return (inputs.size() == 1) && inputs[0].previous_output.is_null();
}

bool transaction::is_final(uint64_t block_height, uint32_t block_time) const
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
