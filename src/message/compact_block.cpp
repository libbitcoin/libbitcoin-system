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
#include <bitcoin/bitcoin/message/compact_block.hpp>

#include <initializer_list>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::compact_block::command = "cmpctblock";

compact_block compact_block::factory_from_data(const data_chunk& data)
{
    compact_block instance;
    instance.from_data(data);
    return instance;
}

compact_block compact_block::factory_from_data(std::istream& stream)
{
    compact_block instance;
    instance.from_data(stream);
    return instance;
}

compact_block compact_block::factory_from_data(reader& source)
{
    compact_block instance;
    instance.from_data(source);
    return instance;
}

bool compact_block::is_valid() const
{
    return header.is_valid();
}

void compact_block::reset()
{
    header.reset();
    nonce = 0;
    short_ids.clear();
    transactions.clear();
}

bool compact_block::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool compact_block::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool compact_block::from_data(reader& source)
{
    reset();
    auto result = header.from_data(source, false);

    nonce = source.read_8_bytes_little_endian();
    const auto short_ids_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    for (uint64_t i = 0; (i < short_ids_count) && result; ++i)
    {
        short_ids.push_back(source.read_mini_hash());
        result = static_cast<bool>(source);
    }

    const auto transaction_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    for (uint64_t i = 0; (i < transaction_count) && result; ++i)
    {
        transactions.emplace_back();
        result = transactions.back().from_data(source);
    }

    if (!result)
        reset();

    return result;
}

data_chunk compact_block::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void compact_block::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void compact_block::to_data(writer& sink) const
{
    header.to_data(sink, false);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_variable_uint_little_endian(short_ids.size());
    for (const auto& element: short_ids)
        sink.write_mini_hash(element);

    sink.write_variable_uint_little_endian(transactions.size());
    for (const auto& element: transactions)
        element.to_data(sink);
}

uint64_t compact_block::serialized_size() const
{
    uint64_t size = chain::header::satoshi_fixed_size_without_transaction_count() +
        variable_uint_size(short_ids.size()) + (short_ids.size() * 6) +
        variable_uint_size(transactions.size()) + 8;

    for (const auto& tx: transactions)
        size += tx.serialized_size();

    return size;
}

} // namspace message
} // namspace libbitcoin
