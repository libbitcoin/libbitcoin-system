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
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

inventory_vector inventory_vector::factory_from_data(const data_chunk& data)
{
    inventory_vector instance;
    instance.from_data(data);
    return instance;
}

inventory_vector inventory_vector::factory_from_data(std::istream& stream)
{
    inventory_vector instance;
    instance.from_data(stream);
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

bool inventory_vector::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool inventory_vector::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint32_t raw_type = read_4_bytes(stream);
    type = inventory_type_from_number(raw_type);
    hash = read_hash(stream);
    result = stream;

    if (!result)
        reset();

    return result;
}

data_chunk inventory_vector::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    uint32_t raw_type = inventory_type_to_number(type);
    serial.write_4_bytes(raw_type);
    serial.write_hash(hash);
    return result;
}

uint64_t inventory_vector::satoshi_size() const
{
    return inventory_vector::satoshi_fixed_size();
}

uint64_t inventory_vector::satoshi_fixed_size()
{
    return 36;
}

} // end message
} // end libbitcoin
