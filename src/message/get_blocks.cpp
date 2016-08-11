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
#include <bitcoin/bitcoin/message/get_blocks.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string get_blocks::command = "getblocks";
const uint32_t get_blocks::version_minimum = version::level::minimum;
const uint32_t get_blocks::version_maximum = version::level::maximum;

get_blocks get_blocks::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    get_blocks instance;
    instance.from_data(version, data);
    return instance;
}

get_blocks get_blocks::factory_from_data(uint32_t version,
    std::istream& stream)
{
    get_blocks instance;
    instance.from_data(version, stream);
    return instance;
}

get_blocks get_blocks::factory_from_data(uint32_t version,
    reader& source)
{
    get_blocks instance;
    instance.from_data(version, source);
    return instance;
}

get_blocks::get_blocks()
  : start_hashes(), stop_hash()
{
}

get_blocks::get_blocks(const hash_list& start, const hash_digest& stop)
  : start_hashes(start), stop_hash(stop)
{
}

get_blocks::get_blocks(hash_list&& start, hash_digest&& stop)
  : start_hashes(std::forward<hash_list>(start)),
    stop_hash(std::forward<hash_digest>(stop))
{
}

bool get_blocks::is_valid() const
{
    return !start_hashes.empty() || (stop_hash != null_hash);
}

void get_blocks::reset()
{
    start_hashes.clear();
    start_hashes.shrink_to_fit();
    stop_hash.fill(0);
}

bool get_blocks::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool get_blocks::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool get_blocks::from_data(uint32_t version, reader& source)
{
    reset();

    // Discard protocol version because it is stupid.
    source.read_4_bytes_little_endian();

    const auto count = source.read_variable_uint_little_endian();
    start_hashes.reserve(count);

    for (uint64_t i = 0; i < count && source; ++i)
        start_hashes.push_back(source.read_hash());

    if (source)
        stop_hash = source.read_hash();
    else
        reset();

    return source;
}

data_chunk get_blocks::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void get_blocks::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void get_blocks::to_data(uint32_t version, writer& sink) const
{
    sink.write_4_bytes_little_endian(version);
    sink.write_variable_uint_little_endian(start_hashes.size());

    for (const auto& start_hash: start_hashes)
        sink.write_hash(start_hash);

    sink.write_hash(stop_hash);
}

uint64_t get_blocks::serialized_size(uint32_t version) const
{
    return 36 + variable_uint_size(start_hashes.size()) +
        hash_size * start_hashes.size();
}

bool operator==(const get_blocks& left, const get_blocks& right)
{
    auto result = (left.start_hashes.size() == right.start_hashes.size());

    for (size_t i = 0; i < left.start_hashes.size() && result; i++)
        result = (left.start_hashes[i] == right.start_hashes[i]);

    return result && left.stop_hash == right.stop_hash;
}

bool operator!=(const get_blocks& left, const get_blocks& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
