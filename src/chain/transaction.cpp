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

transaction::transaction()
{
}

transaction::transaction(const uint32_t version, const uint32_t locktime,
    const transaction_input_list& inputs,
    const transaction_output_list& outputs)
    : version_(version), locktime_(locktime), inputs_(inputs),
      outputs_(outputs)
{
}

uint32_t transaction::version() const
{
    return version_;
}

void transaction::version(uint32_t version)
{
    version_ = version;
}

uint32_t transaction::locktime() const
{
    return locktime_;
}

void transaction::locktime(uint32_t locktime)
{
    locktime_ = locktime;
}

transaction_input_list& transaction::inputs()
{
    return inputs_;
}

const transaction_input_list& transaction::inputs() const
{
    return inputs_;
}

transaction_output_list& transaction::outputs()
{
    return outputs_;
}

const transaction_output_list& transaction::outputs() const
{
    return outputs_;
}

void transaction::reset()
{
    version_ = 0;
    locktime_ = 0;
    inputs_.clear();
    outputs_.clear();
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

    version_ = read_4_bytes(stream);
    result = !stream.fail();

    if (result)
    {
        uint64_t tx_in_count = read_variable_uint(stream);
        result = !stream.fail();

        for (uint64_t i = 0; (i < tx_in_count) && result; ++i)
        {
            transaction_input input;
            result = input.from_data(stream);
            inputs_.push_back(input);
        }
    }

    if (result)
    {
        uint64_t tx_out_count = read_variable_uint(stream);
        result = !stream.fail();

        for (uint64_t i = 0; (i < tx_out_count) && result; ++i)
        {
            transaction_output output;
            result = output.from_data(stream);
            outputs_.push_back(output);
        }
    }

    if (result)
    {
        locktime_ = read_4_bytes(stream);
        result = !stream.fail();
    }

    if (!result)
        reset();

    return result;
}

data_chunk transaction::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version_);

    serial.write_variable_uint(inputs_.size());

    for (const transaction_input& input: inputs_)
        serial.write_data(input.to_data());

    serial.write_variable_uint(outputs_.size());

    for (const transaction_output& output: outputs_)
        serial.write_data(output.to_data());

    serial.write_4_bytes(locktime_);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

uint64_t transaction::satoshi_size() const
{
    uint64_t tx_size = 8;

    tx_size += variable_uint_size(inputs_.size());

    for (const transaction_input& input: inputs_)
    {
        tx_size += input.satoshi_size();
    }

    tx_size += variable_uint_size(outputs_.size());

    for (const transaction_output& output: outputs_)
    {
        tx_size += output.satoshi_size();
    }

    return tx_size;
}

std::string transaction::to_string() const
{
    std::ostringstream ss;

    ss << "Transaction:\n"
        << "\tversion = " << version_ << "\n"
        << "\tlocktime = " << locktime_ << "\n"
        << "Inputs:\n";

    for (transaction_input input: inputs_)
        ss << input.to_string();

    ss << "Outputs:\n";

    for (transaction_output output: outputs_)
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
    return (inputs_.size() == 1) && inputs_[0].previous_output().is_null();
}

bool transaction::is_final(uint64_t block_height, uint32_t block_time) const
{
    if (locktime_ == 0)
        return true;

    uint32_t max_locktime = block_time;

    if (locktime_ < locktime_threshold)
        max_locktime = static_cast<uint32_t>(block_height);

    if (locktime_ < max_locktime)
        return true;

    for (const transaction_input& tx_input : inputs_)
    {
        if (!tx_input.is_final())
            return false;
    }

    return true;
}

bool transaction::is_locktime_conflict() const
{
    auto locktime_set = locktime_ != 0;

    if (locktime_set)
    {
        for (const auto& input : inputs_)
        {
            if (input.sequence() < max_sequence)
                return false;
        }
    }

    return locktime_set;
}

uint64_t transaction::total_output_value() const
{
    uint64_t total = 0;

    for (const transaction_output& output : outputs_)
        total += output.value();

    return total;
}

} // end chain
} // end libbitcoin
