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
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

transaction transaction::factory_from_data(const data_chunk& data)
{
    transaction instance;
    instance.from_data(data);
    return instance;
}

transaction transaction::factory_from_data(std::istream& stream)
{
    transaction instance;
    instance.from_data(stream);
    return instance;
}

transaction transaction::factory_from_data(reader& source)
{
    transaction instance;
    instance.from_data(source);
    return instance;
}

bool transaction::is_valid() const
{
    return (version != 0) ||
        (locktime != 0) ||
        !inputs.empty() ||
        !outputs.empty();
}

void transaction::reset()
{
    version = 0;
    locktime = 0;
    inputs.clear();
    outputs.clear();
}

bool transaction::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool transaction::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool transaction::from_data(reader& source)
{
    bool result = true;

    reset();

    version = source.read_4_bytes_little_endian();
    result = source;

    if (result)
    {
        uint64_t tx_in_count = source.read_variable_uint_little_endian();
        result = source;

        for (uint64_t i = 0; (i < tx_in_count) && result; ++i)
        {
            inputs.emplace_back();
            result = inputs.back().from_data(source);
        }
    }

    if (result)
    {
        uint64_t tx_out_count = source.read_variable_uint_little_endian();
        result = source;

        for (uint64_t i = 0; (i < tx_out_count) && result; ++i)
        {
            outputs.emplace_back();
            result = outputs.back().from_data(source);
        }
    }

    if (result)
    {
        locktime = source.read_4_bytes_little_endian();
        result = source;
    }

    if (!result)
        reset();

    return result;
}

data_chunk transaction::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());

    return data;
}

void transaction::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void transaction::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(version);

    sink.write_variable_uint_little_endian(inputs.size());

    for (const transaction_input& input: inputs)
        input.to_data(sink);

    sink.write_variable_uint_little_endian(outputs.size());

    for (const transaction_output& output: outputs)
        output.to_data(sink);

    sink.write_4_bytes_little_endian(locktime);
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
