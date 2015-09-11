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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/get_blocks.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::get_blocks::command = "getblocks";

get_blocks get_blocks::factory_from_data(const data_chunk& data)
{
    get_blocks instance;
    instance.from_data(data);
    return instance;
}

get_blocks get_blocks::factory_from_data(std::istream& stream)
{
    get_blocks instance;
    instance.from_data(stream);
    return instance;
}

get_blocks get_blocks::factory_from_data(reader& source)
{
    get_blocks instance;
    instance.from_data(source);
    return instance;
}

bool get_blocks::is_valid() const
{
    return !start_hashes.empty() || (hash_stop != null_hash);
}

void get_blocks::reset()
{
    start_hashes.clear();
    hash_stop.fill(0);
}

bool get_blocks::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool get_blocks::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool get_blocks::from_data(reader& source)
{
    reset();

    // Discard protocol version because it is stupid
    source.read_4_bytes_little_endian();

    // Note: changed to uint64_t to preclude possible loss of data.
    uint64_t count = source.read_variable_uint_little_endian();

    for (uint64_t i = 0; (i < count) && source; ++i)
        start_hashes.push_back(source.read_hash());

    if (source)
        hash_stop = source.read_hash();

    if (!source)
        reset();

    return source;
}

data_chunk get_blocks::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void get_blocks::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void get_blocks::to_data(writer& sink) const
{
    sink.write_4_bytes_little_endian(protocol_version);
    sink.write_variable_uint_little_endian(start_hashes.size());

    for (hash_digest start_hash : start_hashes)
        sink.write_hash(start_hash);

    sink.write_hash(hash_stop);
}

uint64_t get_blocks::serialized_size() const
{
    return 36 + variable_uint_size(start_hashes.size()) +
        hash_size * start_hashes.size();
}

bool operator==(const get_blocks& left, const get_blocks& right)
{
    auto result = (left.start_hashes.size() == right.start_hashes.size());
    for (size_t i = 0; (i < left.start_hashes.size()) && result; i++)
        result = (left.start_hashes[i] == right.start_hashes[i]);

    return result && (left.hash_stop == right.hash_stop);
}

bool operator!=(const get_blocks& left, const get_blocks& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
