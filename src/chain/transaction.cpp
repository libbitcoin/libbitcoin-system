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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/transaction.hpp>

#include <numeric>
#include <sstream>
#include <utility>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/input.hpp>
#include <bitcoin/bitcoin/chain/output.hpp>
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

// default constructors

transaction::transaction()
  : version(0), locktime(0), hash_(nullptr)
{
}

transaction::transaction(const transaction& other)
  : transaction(other.version, other.locktime, other.inputs, other.outputs)
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    const input::list& inputs, const output::list& outputs)
  : version(version),
    locktime(locktime),
    inputs(inputs),
    outputs(outputs),
    hash_(nullptr)
{
}

transaction::transaction(transaction&& other)
  : transaction(other.version, other.locktime, 
        std::forward<input::list>(other.inputs),
        std::forward<output::list>(other.outputs))
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    input::list&& inputs, output::list&& outputs)
  : version(version),
    locktime(locktime),
    inputs(std::forward<input::list>(inputs)),
    outputs(std::forward<output::list>(outputs)),
    hash_(nullptr)
{
}

transaction& transaction::operator=(transaction&& other)
{
    version = other.version;
    locktime = other.locktime;
    inputs = std::move(other.inputs);
    outputs = std::move(other.outputs);
    return *this;
}

// TODO: eliminate blockchain transaction copies and then delete this.
transaction& transaction::operator=(const transaction& other)
{
    version = other.version;
    locktime = other.locktime;
    inputs = other.inputs;
    outputs = other.outputs;
    return *this;
}

bool transaction::is_valid() const
{
    return (version != 0) || (locktime != 0) || !inputs.empty() ||
        !outputs.empty();
}

void transaction::reset()
{
    version = 0;
    locktime = 0;
    inputs.clear();
    inputs.shrink_to_fit();
    outputs.clear();
    outputs.shrink_to_fit();

    mutex_.lock();
    hash_.reset();
    mutex_.unlock();
}

bool transaction::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool transaction::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool transaction::from_data(reader& source)
{
    reset();
    version = source.read_4_bytes_little_endian();
    auto result = static_cast<bool>(source);

    if (result)
    {
        const auto tx_in_count = source.read_variable_uint_little_endian();
        result = source;

        if (result)
        {
            inputs.resize(tx_in_count);

            for (auto& input: inputs)
            {
                result = input.from_data(source);

                if (!result)
                    break;
            }
        }
    }

    if (result)
    {
        const auto tx_out_count = source.read_variable_uint_little_endian();
        result = source;

        if (result)
        {
            outputs.resize(tx_out_count);

            for (auto& output: outputs)
            {
                result = output.from_data(source);

                if (!result)
                    break;
            }
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
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());

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

    for (const auto& input: inputs)
        input.to_data(sink);

    sink.write_variable_uint_little_endian(outputs.size());

    for (const auto& output: outputs)
        output.to_data(sink);

    sink.write_4_bytes_little_endian(locktime);
}

uint64_t transaction::serialized_size() const
{
    uint64_t tx_size = 8;
    tx_size += variable_uint_size(inputs.size());
    for (const auto& input: inputs)
        tx_size += input.serialized_size();

    tx_size += variable_uint_size(outputs.size());
    for (const auto& output: outputs)
        tx_size += output.serialized_size();

    return tx_size;
}

std::string transaction::to_string(uint32_t flags) const
{
    std::ostringstream value;
    value << "Transaction:\n"
        << "\tversion = " << version << "\n"
        << "\tlocktime = " << locktime << "\n"
        << "Inputs:\n";

    for (const auto input: inputs)
        value << input.to_string(flags);

    value << "Outputs:\n";
    for (const auto output: outputs)
        value << output.to_string(flags);

    value << "\n";
    return value.str();
}

hash_digest transaction::hash() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (!hash_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        hash_.reset(new hash_digest(bitcoin_hash(to_data())));
        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    hash_digest hash = *hash_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
}

hash_digest transaction::hash(uint32_t sighash_type) const
{
    auto serialized = to_data();
    extend_data(serialized, to_little_endian(sighash_type));
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

    auto max_locktime = block_time;

    if (locktime < locktime_threshold)
        max_locktime = static_cast<uint32_t>(block_height);

    if (locktime < max_locktime)
        return true;

    for (const auto& tx_input: inputs)
        if (!tx_input.is_final())
            return false;

    return true;
}

bool transaction::is_locktime_conflict() const
{
    auto locktime_set = locktime != 0;

    if (locktime_set)
        for (const auto& input: inputs)
            if (input.sequence < max_input_sequence)
                return false;

    return locktime_set;
}

uint64_t transaction::total_output_value() const
{
    const auto value = [](uint64_t total, const output& output)
    {
        return total + output.value;
    };

    return std::accumulate(outputs.begin(), outputs.end(), uint64_t(0), value);
}

} // namspace chain
} // namspace libbitcoin
