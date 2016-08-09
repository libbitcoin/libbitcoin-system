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
#include <bitcoin/bitcoin/message/inventory_vector.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

inventory_vector inventory_vector::factory_from_data(const uint32_t version,
    const data_chunk& data)
{
    inventory_vector instance;
    instance.from_data(version, data);
    return instance;
}

inventory_vector inventory_vector::factory_from_data(const uint32_t version,
    std::istream& stream)
{
    inventory_vector instance;
    instance.from_data(version, stream);
    return instance;
}

inventory_vector inventory_vector::factory_from_data(const uint32_t version,
    reader& source)
{
    inventory_vector instance;
    instance.from_data(version, source);
    return instance;
}

bool inventory_vector::is_valid() const
{
    return (type != inventory_type_id::error) || (hash != null_hash);
}

void inventory_vector::reset()
{
    type = inventory_type_id::error;
    hash.fill(0);
}

bool inventory_vector::from_data(const uint32_t version,
    const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool inventory_vector::from_data(const uint32_t version,
    std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool inventory_vector::from_data(const uint32_t version,
    reader& source)
{
    reset();
    uint32_t raw_type = source.read_4_bytes_little_endian();
    type = inventory_type_from_number(raw_type);
    hash = source.read_hash();
    bool result = static_cast<bool>(source);
    if (!result)
        reset();

    return result;
}

data_chunk inventory_vector::to_data(const uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void inventory_vector::to_data(const uint32_t version,
    std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void inventory_vector::to_data(const uint32_t version,
    writer& sink) const
{
    uint32_t raw_type = inventory_type_to_number(type);
    sink.write_4_bytes_little_endian(raw_type);
    sink.write_hash(hash);
}

uint64_t inventory_vector::serialized_size(const uint32_t version) const
{
    return inventory_vector::satoshi_fixed_size(version);
}

uint64_t inventory_vector::satoshi_fixed_size(const uint32_t version)
{
    return 36;
}

bool operator==(const inventory_vector& left, const inventory_vector& right)
{
    return (left.hash == right.hash) && (left.type == right.type);
}

bool operator!=(const inventory_vector& left, const inventory_vector& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
