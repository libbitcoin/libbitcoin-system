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
#include <bitcoin/bitcoin/message/block_transactions.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::block_transactions::command = "blocktxn";

block_transactions block_transactions::factory_from_data(
    const data_chunk& data)
{
    block_transactions instance;
    instance.from_data(data);
    return instance;
}

block_transactions block_transactions::factory_from_data(std::istream& stream)
{
    block_transactions instance;
    instance.from_data(stream);
    return instance;
}

block_transactions block_transactions::factory_from_data(reader& source)
{
    block_transactions instance;
    instance.from_data(source);
    return instance;
}

bool block_transactions::is_valid() const
{
    return (block_hash != null_hash);
}

void block_transactions::reset()
{
    block_hash = null_hash;
    transactions.clear();
}

bool block_transactions::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool block_transactions::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool block_transactions::from_data(reader& source)
{
    reset();
    block_hash = source.read_hash();
    auto result = static_cast<bool>(source);

    const auto count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        transactions.emplace_back();
        result = transactions.back().from_data(source);
    }

    if (!result)
        reset();

    return result;
}

data_chunk block_transactions::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void block_transactions::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void block_transactions::to_data(writer& sink) const
{
    sink.write_hash(block_hash);
    sink.write_variable_uint_little_endian(transactions.size());
    for (const auto& element: transactions)
        element.to_data(sink);
}

uint64_t block_transactions::serialized_size() const
{
    uint64_t size = hash_size + variable_uint_size(transactions.size());

    for (const auto& element: transactions)
        size += element.serialized_size();

    return size;
}

} // namspace message
} // namspace libbitcoin
