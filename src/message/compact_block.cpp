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
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string compact_block::command = "cmpctblock";
const uint32_t compact_block::version_minimum = version::level::bip152;
const uint32_t compact_block::version_maximum = version::level::bip152;

compact_block compact_block::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    compact_block instance;
    instance.from_data(version, data);
    return instance;
}

compact_block compact_block::factory_from_data(uint32_t version,
    std::istream& stream)
{
    compact_block instance;
    instance.from_data(version, stream);
    return instance;
}

compact_block compact_block::factory_from_data(uint32_t version,
    reader& source)
{
    compact_block instance;
    instance.from_data(version, source);
    return instance;
}

bool compact_block::is_valid() const
{
    return header.is_valid() && !short_ids.empty() && !transactions.empty();
}

void compact_block::reset()
{
    header.reset();
    nonce = 0;
    short_ids.clear();
    short_ids.shrink_to_fit();
    transactions.clear();
    transactions.shrink_to_fit();
}

bool compact_block::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool compact_block::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool compact_block::from_data(uint32_t version, reader& source)
{
    reset();

    auto insufficient_version = (version < compact_block::version_minimum);
    auto result = header.from_data(source, false);
    nonce = source.read_8_bytes_little_endian();
    const auto short_ids_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    if (result)
        short_ids.reserve(short_ids_count);

    for (uint64_t i = 0; (i < short_ids_count) && result; ++i)
    {
        short_ids.push_back(source.read_mini_hash());
        result = static_cast<bool>(source);
    }

    const auto transaction_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    if (result)
    {
        transactions.resize(transaction_count);

        for (auto& transaction: transactions)
        {
            result = transaction.from_data(version, source);

            if (!result)
                break;
        }
    }

    if (!result || insufficient_version)
        reset();

    return result && !insufficient_version;
}

data_chunk compact_block::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void compact_block::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void compact_block::to_data(uint32_t version, writer& sink) const
{
    header.to_data(sink, false);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_variable_uint_little_endian(short_ids.size());
    for (const auto& element: short_ids)
        sink.write_mini_hash(element);

    sink.write_variable_uint_little_endian(transactions.size());
    for (const auto& element: transactions)
        element.to_data(version, sink);
}

uint64_t compact_block::serialized_size(uint32_t version) const
{
    uint64_t size = chain::header::satoshi_fixed_size_without_transaction_count() +
        variable_uint_size(short_ids.size()) + (short_ids.size() * 6) +
        variable_uint_size(transactions.size()) + 8;

    for (const auto& tx: transactions)
        size += tx.serialized_size(version);

    return size;
}

} // namspace message
} // namspace libbitcoin
